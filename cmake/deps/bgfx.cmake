# bgfx — абстракция графического API.
#
# Собственная сборка bgfx использует genie, поэтому подключается обёртка
# bgfx.cmake. В её репозитории bx, bimg и bgfx подключены как подмодули git,
# и клон по тегу оставил бы их каталоги пустыми. Архив релиза содержит все три.

include(FetchContent)

set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BGFX_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BGFX_INSTALL OFF CACHE BOOL "" FORCE)
set(BGFX_CONFIG_VIDEO OFF CACHE BOOL "" FORCE)

# Нужен только компилятор шейдеров: остальные инструменты (texturec, geometryc,
# geometryv) появятся вместе с конвейером ассетов.
set(BGFX_BUILD_TOOLS ON CACHE BOOL "" FORCE)
set(BGFX_BUILD_TOOLS_SHADER ON CACHE BOOL "" FORCE)
set(BGFX_BUILD_TOOLS_TEXTURE OFF CACHE BOOL "" FORCE)
set(BGFX_BUILD_TOOLS_GEOMETRY OFF CACHE BOOL "" FORCE)
# bin2c превращает скомпилированный шейдер в заголовок: шейдеры встраиваются
# в исполняемый файл, а не читаются с диска.
set(BGFX_BUILD_TOOLS_BIN2C ON CACHE BOOL "" FORCE)

FetchContent_Declare(bgfx
    URL https://github.com/bkaradzic/bgfx.cmake/releases/download/v1.153.9398-566/bgfx.cmake.v1.153.9398-566.tar.gz
    URL_HASH SHA256=e03e1fe7e2aaba6a3c7807f5aeb5c6d78d0feb158944f42100485cb9fcf11a47
    EXCLUDE_FROM_ALL
    SYSTEM
)

FetchContent_MakeAvailable(bgfx)

# bgfxToolUtils даёт bgfx_compile_shaders — компиляцию шейдеров как часть сборки.
list(APPEND CMAKE_MODULE_PATH "${bgfx_SOURCE_DIR}/cmake")
