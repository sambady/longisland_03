#include <engine/platform/window.h>

#include <SDL3/SDL.h>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace engine::platform {
namespace {

/// Владение подсистемой видео SDL: инициализация парная выключению.
class VideoSubsystem {
public:
    VideoSubsystem() {
        if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
            throw std::runtime_error(std::string("SDL video init failed: ") + SDL_GetError());
        }
    }

    ~VideoSubsystem() {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    VideoSubsystem(const VideoSubsystem&) = delete;
    VideoSubsystem& operator=(const VideoSubsystem&) = delete;
    VideoSubsystem(VideoSubsystem&&) = delete;
    VideoSubsystem& operator=(VideoSubsystem&&) = delete;
};

}  // namespace

struct Window::State {
    State() = default;

    ~State() {
        if (handle != nullptr) {
            SDL_DestroyWindow(handle);
        }
    }

    State(const State&) = delete;
    State& operator=(const State&) = delete;
    State(State&&) = delete;
    State& operator=(State&&) = delete;

    VideoSubsystem video;
    SDL_Window* handle = nullptr;
};

Window::Window(std::unique_ptr<State> state) : state_(std::move(state)) {}

Window::~Window() = default;

WindowEvents Window::poll_events() {
    WindowEvents events;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                events.close_requested = true;
                break;

            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                if (event.window.windowID == SDL_GetWindowID(state_->handle)) {
                    events.close_requested = true;
                }
                break;

            // Размер кадрового буфера, а не логический размер окна: на дисплее
            // с масштабированием они расходятся.
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                if (event.window.windowID == SDL_GetWindowID(state_->handle)) {
                    events.resized = true;
                }
                break;

            default:
                break;
        }
    }

    return events;
}

PixelSize Window::pixel_size() const {
    int width = 0;
    int height = 0;
    SDL_GetWindowSizeInPixels(state_->handle, &width, &height);

    return PixelSize{.width = static_cast<std::uint32_t>(width),
                     .height = static_cast<std::uint32_t>(height)};
}

void* Window::native_handle() const {
    const SDL_PropertiesID properties = SDL_GetWindowProperties(state_->handle);

    // Свойство зависит от платформы: на X11 и Wayland хендл берётся по другому
    // имени. Ветвление появится здесь, когда появится вторая целевая платформа.
    return SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
}

std::shared_ptr<Window> create_window(std::string_view title, std::uint32_t width,
                                      std::uint32_t height) {
    auto state = std::make_unique<Window::State>();

    const std::string window_title(title);
    state->handle = SDL_CreateWindow(window_title.c_str(), static_cast<int>(width),
                                     static_cast<int>(height), SDL_WINDOW_RESIZABLE);

    if (state->handle == nullptr) {
        throw std::runtime_error(std::string("SDL window creation failed: ") + SDL_GetError());
    }

    return std::shared_ptr<Window>(new Window(std::move(state)));
}

}  // namespace engine::platform
