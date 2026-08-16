# bgfx — абстракция графического API.
#
# Собственная сборка bgfx использует genie, поэтому подключается обёртка
# bgfx.cmake. В её репозитории bx, bimg и bgfx подключены как подмодули git,
# и клон по тегу оставил бы их каталоги пустыми. Архив релиза содержит все три.

include(FetchContent)

set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BGFX_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BGFX_INSTALL OFF CACHE BOOL "" FORCE)
# Компилятор шейдеров понадобится, когда появится первый шейдер.
set(BGFX_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BGFX_CONFIG_VIDEO OFF CACHE BOOL "" FORCE)

FetchContent_Declare(bgfx
    URL https://github.com/bkaradzic/bgfx.cmake/releases/download/v1.153.9398-566/bgfx.cmake.v1.153.9398-566.tar.gz
    URL_HASH SHA256=e03e1fe7e2aaba6a3c7807f5aeb5c6d78d0feb158944f42100485cb9fcf11a47
    EXCLUDE_FROM_ALL
    SYSTEM
)

FetchContent_MakeAvailable(bgfx)
