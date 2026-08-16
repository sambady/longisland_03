#include <engine/core/version.h>

#include <string_view>

namespace engine::core {

std::string_view version() noexcept {
    return "0.1.0";
}

}  // namespace engine::core
