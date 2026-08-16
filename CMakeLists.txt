cmake_minimum_required(VERSION 3.28)

# Формат отладочной информации MSVC задаётся переменной, а не флагом в CMAKE_CXX_FLAGS.
cmake_policy(SET CMP0141 NEW)

project(longisland
    VERSION 0.1.0
    DESCRIPTION "MMORPG client runtime"
    LANGUAGES CXX C  # C нужен SDL3
)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Configure into a separate directory.")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Один compile_commands.json на корень — им пользуются clangd, clang-tidy и IDE.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# Отладочная информация — в объектные файлы, а не в общий на таргет .pdb.
#
# sccache не умеет восстанавливать из кэша побочную запись в .pdb, поэтому при
# параллельной сборке cl.exe падает с C1041 даже с /FS. С /Z7 общего файла нет:
# каждый объектный файл самодостаточен, и кэш работает.
#
# Относится к формату отладочных данных, а не к правилам проекта, поэтому
# действует и на зависимости.
if(CMAKE_CXX_COMPILER_LAUNCHER MATCHES "sccache|ccache")
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>")
endif()

include(CompilerWarnings)
include(Sanitizers)
include(Subsystem)

# Корень исходников. Он же — корень include-путей: заголовок подсистемы
# подключается с префиксом слоя, #include <engine/core/version.h>.
set(LONGISLAND_SOURCE_ROOT "${PROJECT_SOURCE_DIR}/src")

include(deps/sdl3)
include(deps/bgfx)

add_subdirectory(src)
