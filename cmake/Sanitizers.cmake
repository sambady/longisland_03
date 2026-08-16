# Санитайзеры как интерфейсный таргет: включаются пресетом, применяются к нашему коду.

add_library(project_sanitizers INTERFACE)
add_library(longisland::sanitizers ALIAS project_sanitizers)

set(LONGISLAND_SANITIZE_ADDRESS OFF CACHE BOOL "Enable AddressSanitizer")
set(LONGISLAND_SANITIZE_UNDEFINED OFF CACHE BOOL "Enable UndefinedBehaviorSanitizer")

set(_sanitizers "")

if(LONGISLAND_SANITIZE_ADDRESS)
    list(APPEND _sanitizers address)
endif()

if(LONGISLAND_SANITIZE_UNDEFINED)
    if(MSVC AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(FATAL_ERROR "UndefinedBehaviorSanitizer requires Clang; MSVC does not implement it.")
    endif()
    list(APPEND _sanitizers undefined)
endif()

if(_sanitizers)
    list(JOIN _sanitizers "," _sanitizer_list)

    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        # ASan на Windows не поддерживает отладочную runtime-библиотеку (/MDd):
        # https://github.com/llvm/llvm-project/issues/59168
        # Пресет asan выбирает MultiThreadedDLL; проверяем это здесь, чтобы
        # несовместимая конфигурация падала на этапе configure, а не компиляции.
        if(CMAKE_MSVC_RUNTIME_LIBRARY MATCHES "Debug")
            message(FATAL_ERROR
                "AddressSanitizer on Windows requires a release runtime library. "
                "Set CMAKE_MSVC_RUNTIME_LIBRARY to MultiThreadedDLL.")
        endif()

        target_compile_options(project_sanitizers INTERFACE -fsanitize=${_sanitizer_list})

        if(LONGISLAND_SANITIZE_ADDRESS)
            # CMake вызывает lld-link напрямую, минуя драйвер clang, поэтому
            # -fsanitize= до линковки не доходит и runtime приходится указывать явно.
            # Каталог берём у самого компилятора: он зависит от версии LLVM.
            execute_process(
                COMMAND "${CMAKE_CXX_COMPILER}" /clang:-print-resource-dir
                OUTPUT_VARIABLE _clang_resource_dir
                OUTPUT_STRIP_TRAILING_WHITESPACE
                COMMAND_ERROR_IS_FATAL ANY
            )
            cmake_path(SET _clang_runtime_dir NORMALIZE "${_clang_resource_dir}/lib/windows")

            string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" _arch)
            if(_arch MATCHES "^(amd64|x86_64)$")
                set(_asan_arch "x86_64")
            elseif(_arch MATCHES "^arm64$")
                set(_asan_arch "aarch64")
            else()
                message(FATAL_ERROR "AddressSanitizer: unsupported architecture '${CMAKE_SYSTEM_PROCESSOR}'.")
            endif()

            target_link_directories(project_sanitizers INTERFACE "${_clang_runtime_dir}")
            target_link_libraries(project_sanitizers INTERFACE
                clang_rt.asan_dynamic-${_asan_arch}.lib
                clang_rt.asan_dynamic_runtime_thunk-${_asan_arch}.lib
            )
            # ASan подключается динамически: DLL должна лежать рядом с исполняемым файлом.
            file(COPY "${_clang_runtime_dir}/clang_rt.asan_dynamic-${_asan_arch}.dll"
                 DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
        endif()
    else()
        target_compile_options(project_sanitizers INTERFACE
            -fsanitize=${_sanitizer_list}
            -fno-omit-frame-pointer
        )
        target_link_options(project_sanitizers INTERFACE -fsanitize=${_sanitizer_list})
    endif()
endif()
