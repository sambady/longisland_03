#pragma once

#include <cstdint>
#include <memory>
#include <span>

namespace engine::renderer {

/// Вершина статического меша.
struct Vertex {
    float x = 0.0F;
    float y = 0.0F;
    float z = 0.0F;

    float u = 0.0F;
    float v = 0.0F;
};

/// Геометрия на устройстве.
///
/// Владеет буферами вершин и индексов: разрушение освобождает их.
class Mesh {
public:
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(Mesh&&) = delete;

private:
    friend class Renderer;
    friend std::shared_ptr<Mesh> create_mesh(std::span<const Vertex> vertices,
                                             std::span<const std::uint16_t> indices);

    struct State;

    explicit Mesh(std::unique_ptr<State> state);

    std::unique_ptr<State> state_;
};

/// Загружает геометрию на устройство.
///
/// Данные копируются на устройство, поэтому переданные диапазоны переживать
/// вызов не обязаны. Бросает `std::runtime_error`, если буферы создать не
/// удалось.
std::shared_ptr<Mesh> create_mesh(std::span<const Vertex> vertices,
                                  std::span<const std::uint16_t> indices);

}  // namespace engine::renderer
