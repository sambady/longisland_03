# bgfx.cmake подключается через add_subdirectory и не объявляет алиасов с
# пространством имён: таргеты называются bgfx и bx. bx линкуется явно, потому
# что реализация считает матрицы его средствами, а сам bgfx держит его приватно.
longisland_add_subsystem(renderer
    LAYER engine
    SOURCES
        renderer.cpp
    PRIVATE_DEPS
        bgfx
        bx
)
