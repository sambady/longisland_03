#pragma once

#include <bgfx/bgfx.h>

namespace engine::renderer {

/// Раскладка `Vertex` для графического API.
///
/// Внутренний заголовок подсистемы: тип bgfx наружу не выходит.
bgfx::VertexLayout vertex_layout();

}  // namespace engine::renderer
