#include <engine/core/version.h>

#include <cstdio>
#include <exception>
#include <iostream>
#include <string_view>

int main() {
    try {
        const std::string_view version = engine::core::version();
        std::cout << "longisland client " << version << '\n';
    } catch (const std::exception& error) {
        std::fputs(error.what(), stderr);
        return 1;
    }

    return 0;
}
