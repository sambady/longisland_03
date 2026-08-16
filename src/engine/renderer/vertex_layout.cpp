#include "vertex_layout.h"

#include <bgfx/bgfx.h>

namespace engine::renderer {

bgfx::VertexLayout vertex_layout() {
    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    return layout;
}

}  // namespace engine::renderer
