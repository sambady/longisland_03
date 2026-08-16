add_executable(client main.cpp)

target_link_libraries(client
    PRIVATE
        engine::core
        engine::platform
        engine::renderer
        longisland::warnings
        longisland::sanitizers
)
