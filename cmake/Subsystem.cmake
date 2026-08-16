# Объявление подсистемы.
#
# Подсистема — статическая библиотека. Заголовки и реализация лежат рядом,
# в src/<layer>/<subsystem>/. Публикуется корень слоя, поэтому потребитель
# подключает заголовок как #include <engine/renderer/renderer.h>.
#
#   longisland_add_subsystem(renderer
#       LAYER engine
#       SOURCES renderer.cpp
#       PUBLIC_DEPS engine::core
#       PRIVATE_DEPS bgfx::bgfx
#   )
#
# Создаёт таргет engine_renderer и алиас engine::renderer.
#
# PUBLIC_DEPS попадают в публичный API и передаются потребителям.
# PRIVATE_DEPS видны только реализации — так сторонняя библиотека не протекает
# в заголовки (CLAUDE.md, раздел 1).

set(LONGISLAND_LAYERS engine runtime game)

function(longisland_add_subsystem name)
    cmake_parse_arguments(ARG
        ""
        "LAYER"
        "SOURCES;PUBLIC_DEPS;PRIVATE_DEPS"
        ${ARGN}
    )

    if(NOT ARG_LAYER)
        message(FATAL_ERROR "longisland_add_subsystem(${name}): LAYER is required.")
    endif()

    if(NOT ARG_LAYER IN_LIST LONGISLAND_LAYERS)
        message(FATAL_ERROR
            "longisland_add_subsystem(${name}): unknown layer '${ARG_LAYER}'. "
            "Expected one of: ${LONGISLAND_LAYERS}")
    endif()

    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "longisland_add_subsystem(${name}): SOURCES is required.")
    endif()

    set(target "${ARG_LAYER}_${name}")

    add_library(${target} STATIC ${ARG_SOURCES})
    add_library(${ARG_LAYER}::${name} ALIAS ${target})

    # Публикуется корень src/: заголовок подключается с префиксом слоя и
    # подсистемы — #include <engine/core/version.h>.
    target_include_directories(${target} PUBLIC "${LONGISLAND_SOURCE_ROOT}")

    target_link_libraries(${target}
        PUBLIC ${ARG_PUBLIC_DEPS}
        PRIVATE
            ${ARG_PRIVATE_DEPS}
            longisland::warnings
            longisland::sanitizers
    )

    _longisland_check_layer_dependencies(${target} ${ARG_LAYER}
        "${ARG_PUBLIC_DEPS}" "${ARG_PRIVATE_DEPS}")
endfunction()


# Направление зависимостей: game → runtime → engine → библиотеки.
# Ссылка вверх по слоям — ошибка конфигурации, а не замечание на ревью.
function(_longisland_check_layer_dependencies target layer public_deps private_deps)
    list(FIND LONGISLAND_LAYERS ${layer} own_rank)

    foreach(dep IN LISTS public_deps private_deps)
        # Интересуют только зависимости между слоями: engine::core, runtime::world.
        if(NOT dep MATCHES "^([a-z]+)::")
            continue()
        endif()

        set(dep_layer "${CMAKE_MATCH_1}")
        list(FIND LONGISLAND_LAYERS ${dep_layer} dep_rank)

        if(dep_rank EQUAL -1)
            continue()  # сторонняя библиотека
        endif()

        if(dep_rank GREATER own_rank)
            message(FATAL_ERROR
                "${target} depends on ${dep}: layer '${layer}' must not depend on "
                "layer '${dep_layer}'. Dependencies point downwards only "
                "(game -> runtime -> engine).")
        endif()
    endforeach()
endfunction()
