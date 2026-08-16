#pragma once

#include <bgfx/bgfx.h>
#include <engine/renderer/texture.h>

#include <cstddef>
#include <cstdint>
#include <span>

namespace engine::renderer {

/// Хендл текстуры.
///
/// Внутренний заголовок подсистемы: типы bgfx наружу не выходят.
struct Texture::State {
    State(std::uint16_t width, std::uint16_t height, std::span<const std::byte> pixels);
    ~State();

    State(const State&) = delete;
    State& operator=(const State&) = delete;
    State(State&&) = delete;
    State& operator=(State&&) = delete;

    bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;
};

}  // namespace engine::renderer
