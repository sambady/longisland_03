#include <engine/renderer/renderer.h>

#include <bgfx/bgfx.h>
#include <bx/math.h>
#include <engine/renderer/camera.h>

#include <array>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <utility>

namespace engine::renderer {
namespace {

/// Единственная область вывода: кадр целиком очищается и предъявляется.
constexpr bgfx::ViewId kMainView = 0;

constexpr std::uint32_t kClearColor = 0x303030FF;

/// Матрица 4×4 в порядке, ожидаемом графическим API.
using Matrix = std::array<float, 16>;

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

}  // namespace

struct Renderer::State {
    State(void* native_window_handle, std::uint32_t initial_width, std::uint32_t initial_height)
        : library(native_window_handle, initial_width, initial_height),
          width(initial_width),
          height(initial_height) {}

    Library library;
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

void Renderer::render(const Camera& camera) {
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

    bgfx::frame();
}

std::shared_ptr<Renderer> create_renderer(void* native_window_handle, std::uint32_t width,
                                          std::uint32_t height) {
    auto state = std::make_unique<Renderer::State>(native_window_handle, width, height);

    bgfx::setViewClear(kMainView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, kClearColor, 1.0F, 0);

    return std::shared_ptr<Renderer>(new Renderer(std::move(state)));
}

}  // namespace engine::renderer
