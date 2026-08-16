#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace engine::renderer {
struct Vertex;
}  // namespace engine::renderer

namespace client {

/// Геометрия и изображение, заданные в коде.
///
/// Временная замена конвейеру ассетов: проверяет путь от данных до экрана, пока
/// формата готовых ассетов и загрузчика нет. Уходит вместе с их появлением.
struct TestContent {
    std::vector<engine::renderer::Vertex> vertices;
    std::vector<std::uint16_t> indices;

    std::uint16_t texture_width = 0;
    std::uint16_t texture_height = 0;
    std::vector<std::byte> texture_pixels;
};

/// Куб с текстурой-шахматкой.
TestContent make_test_content();

}  // namespace client
