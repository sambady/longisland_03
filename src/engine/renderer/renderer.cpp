#include <engine/renderer/renderer.h>

#include <bgfx/bgfx.h>

// Варианты шейдеров, которые не компилируются:
//
// DXIL (s_6_0) — компилятор HLSL выдаёт предупреждение внутри собственного кода,
//   а shaderc вызывается с --Werror. Direct3D 12 вернётся вместе с этим профилем.
// WGSL — WebGPU в целевых платформах не значится.
//
// Именно макросы: их читает препроцессор bgfx/embedded_shader.h.
// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define BGFX_PLATFORM_SUPPORTS_DXIL 0
#define BGFX_PLATFORM_SUPPORTS_WGSL 0
// NOLINTEND(cppcoreguidelines-macro-usage)

#include <bgfx/embedded_shader.h>
#include <bx/math.h>
#include <engine/renderer/camera.h>
#include <engine/renderer/mesh.h>
#include <engine/renderer/texture.h>

#include <array>
#include <cstdint>
#include <memory>
#include <span>
#include <stdexcept>
#include <utility>

#include "mesh_state.h"
#include "texture_state.h"

// Скомпилированные шейдеры: по варианту на графический API. Имена массивов
// задаёт bgfx_compile_shaders, макрос BGFX_EMBEDDED_SHADER их и ожидает.
#include "shaders/dxbc/fs_mesh.sc.bin.h"
#include "shaders/dxbc/vs_mesh.sc.bin.h"
#include "shaders/essl/fs_mesh.sc.bin.h"
#include "shaders/essl/vs_mesh.sc.bin.h"
#include "shaders/glsl/fs_mesh.sc.bin.h"
#include "shaders/glsl/vs_mesh.sc.bin.h"
#include "shaders/spirv/fs_mesh.sc.bin.h"
#include "shaders/spirv/vs_mesh.sc.bin.h"

namespace engine::renderer {
namespace {

/// Единственная область вывода: кадр целиком очищается и предъявляется.
constexpr bgfx::ViewId kMainView = 0;

constexpr std::uint32_t kClearColor = 0x303030FF;

/// Матрица 4×4 в порядке, ожидаемом графическим API.
using Matrix = std::array<float, 16>;

/// Шейдеры, встроенные в исполняемый файл, по одному варианту на графический API.
const std::array<bgfx::EmbeddedShader, 3> kEmbeddedShaders = {{
    BGFX_EMBEDDED_SHADER(vs_mesh),
    BGFX_EMBEDDED_SHADER(fs_mesh),
    BGFX_EMBEDDED_SHADER_END(),
}};

/// Владение библиотекой: инициализация парная выключению.
///
/// Тип отдельный от `State`, чтобы `bgfx::shutdown` вызывался только при
/// удавшейся инициализации.
class Library {
public:
    Library(void* native_window_handle, std::uint32_t width, std::uint32_t height) {
        bgfx::Init init;
        init.type = bgfx::RendererType::Count;  // выбор графического API за bgfx
        init.resolution.width = width;
        init.resolution.height = height;
        init.resolution.reset = BGFX_RESET_VSYNC;
        init.platformData.nwh = native_window_handle;

        if (!bgfx::init(init)) {
            throw std::runtime_error("bgfx initialization failed");
        }
    }

    ~Library() {
        bgfx::shutdown();
    }

    Library(const Library&) = delete;
    Library& operator=(const Library&) = delete;
    Library(Library&&) = delete;
    Library& operator=(Library&&) = delete;
};

/// Собирает программу из шейдеров, встроенных в исполняемый файл.
///
/// Встроенные шейдеры избавляют клиента от зависимости от расположения файлов.
/// Загрузка с диска появится вместе с конвейером ассетов.
bgfx::ProgramHandle load_mesh_program() {
    const bgfx::RendererType::Enum renderer = bgfx::getRendererType();

    const bgfx::ShaderHandle vertex =
        bgfx::createEmbeddedShader(kEmbeddedShaders.data(), renderer, "vs_mesh");
    const bgfx::ShaderHandle fragment =
        bgfx::createEmbeddedShader(kEmbeddedShaders.data(), renderer, "fs_mesh");

    if (!bgfx::isValid(vertex) || !bgfx::isValid(fragment)) {
        throw std::runtime_error("shader creation failed");
    }

    // Программа принимает владение шейдерами и уничтожит их вместе с собой.
    const bgfx::ProgramHandle program = bgfx::createProgram(vertex, fragment, true);
    if (!bgfx::isValid(program)) {
        throw std::runtime_error("shader program creation failed");
    }

    return program;
}

}  // namespace

struct Renderer::State {
    State(void* native_window_handle, std::uint32_t initial_width, std::uint32_t initial_height)
        : library(native_window_handle, initial_width, initial_height),
          program(load_mesh_program()),
          albedo_sampler(bgfx::createUniform("s_albedo", bgfx::UniformType::Sampler)),
          width(initial_width),
          height(initial_height) {
        if (!bgfx::isValid(albedo_sampler)) {
            throw std::runtime_error("sampler uniform creation failed");
        }
    }

    ~State() {
        if (bgfx::isValid(albedo_sampler)) {
            bgfx::destroy(albedo_sampler);
        }

        if (bgfx::isValid(program)) {
            bgfx::destroy(program);
        }
    }

    State(const State&) = delete;
    State& operator=(const State&) = delete;
    State(State&&) = delete;
    State& operator=(State&&) = delete;

    Library library;
    bgfx::ProgramHandle program;
    bgfx::UniformHandle albedo_sampler;
    std::uint32_t width;
    std::uint32_t height;
};

Renderer::Renderer(std::unique_ptr<State> state) : state_(std::move(state)) {}

Renderer::~Renderer() = default;

void Renderer::resize(std::uint32_t width, std::uint32_t height) {
    state_->width = width;
    state_->height = height;

    // Меняется размер кадрового буфера, а не окна: окном распоряжается платформа.
    bgfx::reset(width, height, BGFX_RESET_VSYNC);
}

void Renderer::begin_frame(const Camera& camera) {
    const float aspect_ratio =
        static_cast<float>(state_->width) / static_cast<float>(state_->height);

    Matrix view{};
    bx::mtxLookAt(view.data(), bx::Vec3{camera.position.x, camera.position.y, camera.position.z},
                  bx::Vec3{camera.target.x, camera.target.y, camera.target.z},
                  bx::Vec3{camera.up.x, camera.up.y, camera.up.z});

    Matrix projection{};
    bx::mtxProj(projection.data(), camera.field_of_view, aspect_ratio, camera.near_plane,
                camera.far_plane, bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewTransform(kMainView, view.data(), projection.data());
    bgfx::setViewRect(kMainView, 0, 0, static_cast<std::uint16_t>(state_->width),
                      static_cast<std::uint16_t>(state_->height));

    // Область без вызовов отрисовки обрабатывается только по явному запросу.
    bgfx::touch(kMainView);
}

void Renderer::draw(const Mesh& mesh, const Texture& texture,
                    std::span<const float, 16> transform) {
    bgfx::setTransform(transform.data());

    bgfx::setVertexBuffer(0, mesh.state_->vertex_buffer);
    bgfx::setIndexBuffer(mesh.state_->index_buffer);
    bgfx::setTexture(0, state_->albedo_sampler, texture.state_->handle);

    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z |
                   BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_CULL_CW);

    bgfx::submit(kMainView, state_->program);
}

// Метод не статический намеренно: кадр принадлежит контексту, и вызов вне
// объекта смысла не имеет.
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Renderer::end_frame() {
    bgfx::frame();
}

std::shared_ptr<Renderer> create_renderer(void* native_window_handle, std::uint32_t width,
                                          std::uint32_t height) {
    auto state = std::make_unique<Renderer::State>(native_window_handle, width, height);

    bgfx::setViewClear(kMainView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, kClearColor, 1.0F, 0);

    return std::shared_ptr<Renderer>(new Renderer(std::move(state)));
}

}  // namespace engine::renderer
