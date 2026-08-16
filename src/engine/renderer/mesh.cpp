#include <engine/renderer/mesh.h>

#include <bgfx/bgfx.h>

#include <cstdint>
#include <memory>
#include <span>
#include <stdexcept>
#include <utility>

#include "mesh_state.h"
#include "vertex_layout.h"

namespace engine::renderer {

Mesh::State::State(std::span<const Vertex> vertices, std::span<const std::uint16_t> indices)
    : index_count(static_cast<std::uint32_t>(indices.size())) {
    const bgfx::Memory* vertex_memory =
        bgfx::copy(vertices.data(), static_cast<std::uint32_t>(vertices.size_bytes()));
    vertex_buffer = bgfx::createVertexBuffer(vertex_memory, vertex_layout());

    const bgfx::Memory* index_memory =
        bgfx::copy(indices.data(), static_cast<std::uint32_t>(indices.size_bytes()));
    index_buffer = bgfx::createIndexBuffer(index_memory);

    if (!bgfx::isValid(vertex_buffer) || !bgfx::isValid(index_buffer)) {
        throw std::runtime_error("mesh buffer creation failed");
    }
}

Mesh::State::~State() {
    if (bgfx::isValid(index_buffer)) {
        bgfx::destroy(index_buffer);
    }

    if (bgfx::isValid(vertex_buffer)) {
        bgfx::destroy(vertex_buffer);
    }
}

Mesh::Mesh(std::unique_ptr<State> state) : state_(std::move(state)) {}

Mesh::~Mesh() = default;

std::shared_ptr<Mesh> create_mesh(std::span<const Vertex> vertices,
                                  std::span<const std::uint16_t> indices) {
    auto state = std::make_unique<Mesh::State>(vertices, indices);

    return std::shared_ptr<Mesh>(new Mesh(std::move(state)));
}

}  // namespace engine::renderer
