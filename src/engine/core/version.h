#pragma once

#include <string_view>

namespace engine::core {

/// Версия сборки runtime.
std::string_view version() noexcept;

}  // namespace engine::core
