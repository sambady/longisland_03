#pragma once

#include <bgfx/bgfx.h>
#include <engine/renderer/mesh.h>

#include <cstdint>
#include <span>

namespace engine::renderer {

/// Хендлы буферов меша.
///
/// Внутренний заголовок подсистемы: типы bgfx наружу не выходят.
struct Mesh::State {
    State(std::span<const Vertex> vertices, std::span<const std::uint16_t> indices);
    ~State();

    State(const State&) = delete;
    State& operator=(const State&) = delete;
    State(State&&) = delete;
    State& operator=(State&&) = delete;

    bgfx::VertexBufferHandle vertex_buffer = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle index_buffer = BGFX_INVALID_HANDLE;
    std::uint32_t index_count = 0;
};

}  // namespace engine::renderer
