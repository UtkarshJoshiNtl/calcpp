cmake_minimum_required(VERSION 3.20)

project(CalcCpp VERSION 0.1.0 LANGUAGES CXX)

add_executable(CalcCpp WIN32
    src/main.cpp
    src/gui/main_window.cpp
    src/operator/expression_evaluator.cpp
)

target_include_directories(CalcCpp PRIVATE src)

target_compile_features(CalcCpp PRIVATE cxx_std_17)

target_compile_definitions(CalcCpp PRIVATE UNICODE _UNICODE)

if (MSVC)
    target_compile_options(CalcCpp PRIVATE /W4 /permissive-)
else()
    target_compile_options(CalcCpp PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_link_libraries(CalcCpp PRIVATE user32 gdi32)

option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_CLI "Build command-line frontend" ON)
option(BUILD_SDL_GUI "Build SDL2 GUI frontend" OFF)

if (BUILD_CLI)
    add_executable(calc_cli
        src/cli/cli_main.cpp
        src/operator/expression_evaluator.cpp
    )
    target_include_directories(calc_cli PRIVATE src)
    target_compile_features(calc_cli PRIVATE cxx_std_17)
    if (MSVC)
        target_compile_options(calc_cli PRIVATE /W4 /permissive-)
    else()
        target_compile_options(calc_cli PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

if (BUILD_SDL_GUI)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    if (SDL2_FOUND AND SDL2_TTF_FOUND)
        add_executable(CalcCppSDL
            src/gui_sdl/main_sdl.cpp
            src/operator/expression_evaluator.cpp
        )
        target_include_directories(CalcCppSDL PRIVATE src ${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})
        target_link_libraries(CalcCppSDL PRIVATE ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})
        target_compile_features(CalcCppSDL PRIVATE cxx_std_17)
    else()
        message(WARNING "SDL2 or SDL2_ttf not found; skipping SDL GUI target")
    endif()
endif()
if (BUILD_TESTS)
    add_executable(calc_tests
        tests/expression_evaluator_tests.cpp
        src/operator/expression_evaluator.cpp
    )
    target_include_directories(calc_tests PRIVATE src)
    target_compile_features(calc_tests PRIVATE cxx_std_17)
    if (MSVC)
        target_compile_options(calc_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(calc_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()
