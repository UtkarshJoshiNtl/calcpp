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
