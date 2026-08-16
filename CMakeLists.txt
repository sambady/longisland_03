cmake_minimum_required(VERSION 3.28)

project(longisland
    VERSION 0.1.0
    DESCRIPTION "MMORPG client runtime"
    LANGUAGES CXX
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

include(CompilerWarnings)
include(Sanitizers)
include(Subsystem)

add_subdirectory(engine)
add_subdirectory(runtime)
add_subdirectory(client)
