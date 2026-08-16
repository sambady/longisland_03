#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

namespace engine::platform {

/// Размер в пикселях кадрового буфера.
///
/// На дисплеях с масштабированием отличается от логического размера окна:
/// рендеринг ведётся в пикселях, поэтому подсистема сообщает именно их.
struct PixelSize {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
};

/// Что произошло с окном за время обработки событий.
struct WindowEvents {
    /// Пользователь запросил закрытие окна.
    bool close_requested = false;

    /// Размер кадрового буфера изменился; `Window::pixel_size` вернёт новый.
    bool resized = false;
};

/// Окно операционной системы.
///
/// Владеет окном и подключением к оконной подсистеме: разрушение закрывает окно.
/// Создаётся через `create_window`.
class Window {
public:
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    /// Обрабатывает накопленные события операционной системы.
    ///
    /// Вызывается раз в кадр. Не блокирует.
    WindowEvents poll_events();

    [[nodiscard]] PixelSize pixel_size() const;

    /// Нативный хендл окна для графического API.
    ///
    /// На Windows — `HWND`. Тип платформы наружу не раскрывается: потребитель
    /// передаёт указатель графическому API, не разыменовывая его.
    [[nodiscard]] void* native_handle() const;

private:
    friend std::shared_ptr<Window> create_window(std::string_view title, std::uint32_t width,
                                                 std::uint32_t height);

    struct State;

    explicit Window(std::unique_ptr<State> state);

    std::unique_ptr<State> state_;
};

/// Создаёт окно заданного размера.
///
/// Размер задаётся в логических единицах; фактический размер кадрового буфера
/// сообщает `Window::pixel_size`. Бросает `std::runtime_error`, если окно
/// создать не удалось.
std::shared_ptr<Window> create_window(std::string_view title, std::uint32_t width,
                                      std::uint32_t height);

}  // namespace engine::platform
