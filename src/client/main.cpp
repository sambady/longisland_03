#include <engine/core/version.h>
#include <engine/platform/window.h>
#include <engine/renderer/camera.h>
#include <engine/renderer/mesh.h>
#include <engine/renderer/renderer.h>
#include <engine/renderer/texture.h>

#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <string>

#include "test_content.h"

namespace {

constexpr std::uint32_t kInitialWidth = 1280;
constexpr std::uint32_t kInitialHeight = 720;

/// Матрица поворота вокруг оси Y, по столбцам.
std::array<float, 16> rotation_about_y(float radians) {
    const float sine = std::sin(radians);
    const float cosine = std::cos(radians);

    return {cosine, 0.0F, -sine,  0.0F,   //
            0.0F,   1.0F, 0.0F,   0.0F,   //
            sine,   0.0F, cosine, 0.0F,   //
            0.0F,   0.0F, 0.0F,   1.0F};  //
}

}  // namespace

int main() {
    try {
        const std::string title = std::string("longisland ") + std::string(engine::core::version());
        const auto window = engine::platform::create_window(title, kInitialWidth, kInitialHeight);

        const engine::platform::PixelSize size = window->pixel_size();
        const auto renderer =
            engine::renderer::create_renderer(window->native_handle(), size.width, size.height);

        const client::TestContent content = client::make_test_content();
        const auto mesh = engine::renderer::create_mesh(content.vertices, content.indices);
        const auto texture = engine::renderer::create_texture(
            content.texture_width, content.texture_height, content.texture_pixels);

        const engine::renderer::Camera camera;
        const auto started = std::chrono::steady_clock::now();

        while (true) {
            const engine::platform::WindowEvents events = window->poll_events();

            if (events.close_requested) {
                break;
            }

            if (events.resized) {
                const engine::platform::PixelSize current = window->pixel_size();
                renderer->resize(current.width, current.height);
            }

            const std::chrono::duration<float> elapsed = std::chrono::steady_clock::now() - started;

            renderer->begin_frame(camera);
            renderer->draw(*mesh, *texture, rotation_about_y(elapsed.count()));
            renderer->end_frame();
        }
    } catch (const std::exception& error) {
        std::fputs(error.what(), stderr);
        return 1;
    }

    return 0;
}
