include(bgfxToolUtils)

# Шейдеры компилируются в заголовки и встраиваются в исполняемый файл: у клиента
# нет зависимости от расположения файлов. Чтение с диска появится вместе с
# конвейером ассетов.
#
# Профили ограничены Direct3D 11 и Vulkan — теми, что bgfx выбирает на целевой
# платформе. Профиль s_6_0 исключён: компилятор HLSL выдаёт на нём предупреждение
# внутри собственного кода, а shaderc вызывается с --Werror.
bgfx_compile_shaders(
    TYPE VERTEX
    SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vs_mesh.sc
    VARYING_DEF ${CMAKE_CURRENT_SOURCE_DIR}/shaders/varying.def.sc
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders
    INCLUDE_DIRS ${bgfx_SOURCE_DIR}/bgfx/src
    PROFILES s_5_0 spirv 120 100_es
    AS_HEADERS
    OUT_FILES_VAR VERTEX_SHADER_HEADERS
)

bgfx_compile_shaders(
    TYPE FRAGMENT
    SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fs_mesh.sc
    VARYING_DEF ${CMAKE_CURRENT_SOURCE_DIR}/shaders/varying.def.sc
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders
    INCLUDE_DIRS ${bgfx_SOURCE_DIR}/bgfx/src
    PROFILES s_5_0 spirv 120 100_es
    AS_HEADERS
    OUT_FILES_VAR FRAGMENT_SHADER_HEADERS
)

# bgfx.cmake подключается через add_subdirectory и не объявляет алиасов с
# пространством имён: таргеты называются bgfx и bx. bx линкуется явно, потому
# что реализация считает матрицы его средствами, а сам bgfx держит его приватно.
longisland_add_subsystem(renderer
    LAYER engine
    SOURCES
        renderer.cpp
        mesh.cpp
        texture.cpp
        vertex_layout.cpp
        ${VERTEX_SHADER_HEADERS}
        ${FRAGMENT_SHADER_HEADERS}
    PRIVATE_DEPS
        bgfx
        bx
)

# Сгенерированные заголовки шейдеров лежат в каталоге сборки.
target_include_directories(engine_renderer PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
