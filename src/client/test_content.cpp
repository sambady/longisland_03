#include "test_content.h"

#include <engine/renderer/mesh.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace client {
namespace {

using engine::renderer::Vertex;

constexpr float kHalfSize = 1.0F;

constexpr std::uint16_t kTextureSize = 64;
constexpr std::uint16_t kCheckerCell = 8;

constexpr std::size_t kFaceCount = 6;
constexpr std::size_t kVerticesPerFace = 4;

/// Вершины куба: по четыре подряд на грань, против часовой стрелки снаружи.
///
/// У каждой грани свои вершины: общие углы имеют разные текстурные координаты,
/// поэтому переиспользовать их между гранями нельзя.
///
/// Поля вершины перечислены позиционно: это таблица числовых данных, и
/// designated initializers сделали бы её нечитаемой.
// NOLINTBEGIN(modernize-use-designated-initializers)
constexpr std::array<Vertex, kFaceCount * kVerticesPerFace> kVertices = {{
    // Передняя, -Z
    {-kHalfSize, -kHalfSize, -kHalfSize, 0.0F, 1.0F},
    {kHalfSize, -kHalfSize, -kHalfSize, 1.0F, 1.0F},
    {kHalfSize, kHalfSize, -kHalfSize, 1.0F, 0.0F},
    {-kHalfSize, kHalfSize, -kHalfSize, 0.0F, 0.0F},
    // Задняя, +Z
    {kHalfSize, -kHalfSize, kHalfSize, 0.0F, 1.0F},
    {-kHalfSize, -kHalfSize, kHalfSize, 1.0F, 1.0F},
    {-kHalfSize, kHalfSize, kHalfSize, 1.0F, 0.0F},
    {kHalfSize, kHalfSize, kHalfSize, 0.0F, 0.0F},
    // Левая, -X
    {-kHalfSize, -kHalfSize, kHalfSize, 0.0F, 1.0F},
    {-kHalfSize, -kHalfSize, -kHalfSize, 1.0F, 1.0F},
    {-kHalfSize, kHalfSize, -kHalfSize, 1.0F, 0.0F},
    {-kHalfSize, kHalfSize, kHalfSize, 0.0F, 0.0F},
    // Правая, +X
    {kHalfSize, -kHalfSize, -kHalfSize, 0.0F, 1.0F},
    {kHalfSize, -kHalfSize, kHalfSize, 1.0F, 1.0F},
    {kHalfSize, kHalfSize, kHalfSize, 1.0F, 0.0F},
    {kHalfSize, kHalfSize, -kHalfSize, 0.0F, 0.0F},
    // Нижняя, -Y
    {-kHalfSize, -kHalfSize, kHalfSize, 0.0F, 1.0F},
    {kHalfSize, -kHalfSize, kHalfSize, 1.0F, 1.0F},
    {kHalfSize, -kHalfSize, -kHalfSize, 1.0F, 0.0F},
    {-kHalfSize, -kHalfSize, -kHalfSize, 0.0F, 0.0F},
    // Верхняя, +Y
    {-kHalfSize, kHalfSize, -kHalfSize, 0.0F, 1.0F},
    {kHalfSize, kHalfSize, -kHalfSize, 1.0F, 1.0F},
    {kHalfSize, kHalfSize, kHalfSize, 1.0F, 0.0F},
    {-kHalfSize, kHalfSize, kHalfSize, 0.0F, 0.0F},
}};

// NOLINTEND(modernize-use-designated-initializers)

std::vector<std::byte> make_checker_texture() {
    constexpr std::size_t kBytesPerPixel = 4;

    std::vector<std::byte> pixels;
    pixels.reserve(static_cast<std::size_t>(kTextureSize) * kTextureSize * kBytesPerPixel);

    for (std::uint16_t y = 0; y < kTextureSize; ++y) {
        for (std::uint16_t x = 0; x < kTextureSize; ++x) {
            const bool light = ((x / kCheckerCell) + (y / kCheckerCell)) % 2 == 0;
            const auto value = static_cast<std::byte>(light ? 0xD0 : 0x40);

            // Порядок BGRA.
            pixels.push_back(value);
            pixels.push_back(value);
            pixels.push_back(value);
            pixels.push_back(static_cast<std::byte>(0xFF));
        }
    }

    return pixels;
}

}  // namespace

TestContent make_test_content() {
    TestContent content;

    content.vertices.assign(kVertices.begin(), kVertices.end());

    for (std::size_t face = 0; face < kFaceCount; ++face) {
        const auto base = static_cast<std::uint16_t>(face * kVerticesPerFace);

        // Два треугольника на грань.
        content.indices.push_back(base);
        content.indices.push_back(static_cast<std::uint16_t>(base + 1));
        content.indices.push_back(static_cast<std::uint16_t>(base + 2));

        content.indices.push_back(base);
        content.indices.push_back(static_cast<std::uint16_t>(base + 2));
        content.indices.push_back(static_cast<std::uint16_t>(base + 3));
    }

    content.texture_width = kTextureSize;
    content.texture_height = kTextureSize;
    content.texture_pixels = make_checker_texture();

    return content;
}

}  // namespace client
