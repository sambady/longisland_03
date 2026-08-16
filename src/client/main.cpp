#include <engine/core/version.h>
#include <engine/platform/window.h>
#include <engine/renderer/camera.h>
#include <engine/renderer/renderer.h>

#include <cstdint>
#include <cstdio>
#include <exception>
#include <string>

namespace {

constexpr std::uint32_t kInitialWidth = 1280;
constexpr std::uint32_t kInitialHeight = 720;

}  // namespace

int main() {
    try {
        const std::string title = std::string("longisland ") + std::string(engine::core::version());
        const auto window = engine::platform::create_window(title, kInitialWidth, kInitialHeight);

        const engine::platform::PixelSize size = window->pixel_size();
        const auto renderer =
            engine::renderer::create_renderer(window->native_handle(), size.width, size.height);

        const engine::renderer::Camera camera;

        while (true) {
            const engine::platform::WindowEvents events = window->poll_events();

            if (events.close_requested) {
                break;
            }

            if (events.resized) {
                const engine::platform::PixelSize current = window->pixel_size();
                renderer->resize(current.width, current.height);
            }

            renderer->render(camera);
        }
    } catch (const std::exception& error) {
        std::fputs(error.what(), stderr);
        return 1;
    }

    return 0;
}
