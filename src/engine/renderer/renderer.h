#pragma once

#include <cstdint>
#include <memory>

namespace engine::renderer {

struct Camera;

/// Графический контекст и отправка кадров на устройство.
///
/// Владеет графическим API: разрушение освобождает его ресурсы. Создаётся
/// через `create_renderer`.
class Renderer {
public:
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    /// Согласует размер кадрового буфера с размером окна.
    ///
    /// Вызывается, когда окно сообщило об изменении размера.
    void resize(std::uint32_t width, std::uint32_t height);

    /// Отправляет кадр на устройство.
    ///
    /// Камера задаёт матрицы вида и проекции для этого кадра.
    void render(const Camera& camera);

private:
    friend std::shared_ptr<Renderer> create_renderer(void* native_window_handle,
                                                     std::uint32_t width, std::uint32_t height);

    struct State;

    explicit Renderer(std::unique_ptr<State> state);

    std::unique_ptr<State> state_;
};

/// Создаёт графический контекст для окна.
///
/// `native_window_handle` — хендл окна, полученный у подсистемы платформы.
/// Размер задаётся в пикселях кадрового буфера. Бросает `std::runtime_error`,
/// если графический API инициализировать не удалось.
std::shared_ptr<Renderer> create_renderer(void* native_window_handle, std::uint32_t width,
                                          std::uint32_t height);

}  // namespace engine::renderer
