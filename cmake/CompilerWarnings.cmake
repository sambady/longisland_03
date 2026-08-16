# Предупреждения как интерфейсный таргет, а не как CMAKE_CXX_FLAGS.
#
# Глобальные флаги применились бы и к коду зависимостей, полученному через
# FetchContent. Чужой код собирается со своими настройками; наши строгие
# предупреждения к нему не относятся.

add_library(project_warnings INTERFACE)
add_library(longisland::warnings ALIAS project_warnings)

set(LONGISLAND_WARNINGS_AS_ERRORS ON CACHE BOOL "Treat compiler warnings as errors")

if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(project_warnings INTERFACE /W4)

    if(LONGISLAND_WARNINGS_AS_ERRORS)
        target_compile_options(project_warnings INTERFACE /WX)
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # clang-cl понимает предупреждения Clang поверх MSVC-совместимого набора.
        # /permissive- и /Zc:* ему не нужны: он и так следует стандарту.
        target_compile_options(project_warnings INTERFACE
            -Wextra
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wpedantic
            -Wconversion
            -Wsign-conversion
            -Wdouble-promotion
            -Wformat=2
            -Wimplicit-fallthrough
        )
    else()
        target_compile_options(project_warnings INTERFACE
            /permissive-      # строгое соответствие стандарту
            /Zc:__cplusplus   # иначе __cplusplus сообщает C++98
            /Zc:preprocessor  # препроцессор по стандарту
        )
    endif()
else()
    target_compile_options(project_warnings INTERFACE
        -Wall
        -Wextra
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wdouble-promotion
        -Wformat=2
        -Wimplicit-fallthrough
    )

    if(LONGISLAND_WARNINGS_AS_ERRORS)
        target_compile_options(project_warnings INTERFACE -Werror)
    endif()
endif()
