#pragma once

namespace engine::renderer {

/// Точка или направление в мировом пространстве.
struct Vector3 {
    float x = 0.0F;
    float y = 0.0F;
    float z = 0.0F;
};

/// Положение наблюдателя и параметры перспективной проекции.
///
/// Соотношение сторон задаётся кадровым буфером, а не камерой: его сообщает
/// подсистема окна, поэтому здесь его нет.
struct Camera {
    Vector3 position{.x = 0.0F, .y = 0.0F, .z = -5.0F};
    Vector3 target{.x = 0.0F, .y = 0.0F, .z = 0.0F};
    Vector3 up{.x = 0.0F, .y = 1.0F, .z = 0.0F};

    /// Вертикальный угол обзора в градусах.
    float field_of_view = 60.0F;

    float near_plane = 0.1F;
    float far_plane = 1000.0F;
};

}  // namespace engine::renderer
