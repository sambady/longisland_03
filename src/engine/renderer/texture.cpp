#include <engine/renderer/texture.h>

#include <bgfx/bgfx.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <stdexcept>
#include <utility>

#include "texture_state.h"

namespace engine::renderer {
namespace {

constexpr std::size_t kBytesPerPixel = 4;

}  // namespace

Texture::State::State(std::uint16_t width, std::uint16_t height,
                      std::span<const std::byte> pixels) {
    const bgfx::Memory* memory =
        bgfx::copy(pixels.data(), static_cast<std::uint32_t>(pixels.size_bytes()));

    handle = bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::BGRA8,
                                   BGFX_SAMPLER_POINT, memory);

    if (!bgfx::isValid(handle)) {
        throw std::runtime_error("texture creation failed");
    }
}

Texture::State::~State() {
    if (bgfx::isValid(handle)) {
        bgfx::destroy(handle);
    }
}

Texture::Texture(std::unique_ptr<State> state) : state_(std::move(state)) {}

Texture::~Texture() = default;

std::shared_ptr<Texture> create_texture(std::uint16_t width, std::uint16_t height,
                                        std::span<const std::byte> pixels) {
    const std::size_t expected = static_cast<std::size_t>(width) * height * kBytesPerPixel;
    if (pixels.size() != expected) {
        throw std::invalid_argument("texture pixel data size does not match its dimensions");
    }

    auto state = std::make_unique<Texture::State>(width, height, pixels);

    return std::shared_ptr<Texture>(new Texture(std::move(state)));
}

}  // namespace engine::renderer
