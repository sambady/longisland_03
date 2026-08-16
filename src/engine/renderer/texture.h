#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

namespace engine::renderer {

/// Изображение на устройстве.
///
/// Владеет текстурой: разрушение освобождает её.
class Texture {
public:
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&&) = delete;
    Texture& operator=(Texture&&) = delete;

private:
    friend class Renderer;
    friend std::shared_ptr<Texture> create_texture(std::uint16_t width, std::uint16_t height,
                                                   std::span<const std::byte> pixels);

    struct State;

    explicit Texture(std::unique_ptr<State> state);

    std::unique_ptr<State> state_;
};

/// Загружает изображение на устройство.
///
/// Пиксели идут построчно, по четыре байта на пиксель в порядке BGRA. Данные
/// копируются, поэтому диапазон переживать вызов не обязан. Бросает
/// `std::invalid_argument`, если размер диапазона не соответствует размерам
/// изображения, и `std::runtime_error`, если текстуру создать не удалось.
std::shared_ptr<Texture> create_texture(std::uint16_t width, std::uint16_t height,
                                        std::span<const std::byte> pixels);

}  // namespace engine::renderer
