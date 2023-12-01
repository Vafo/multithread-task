cmake_minimum_required(VERSION 3.15)

project(Multithread)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(Concurrency_compiler_flags INTERFACE)
target_compile_features(Concurrency_compiler_flags INTERFACE cxx_std_11)

# Include src folders
add_subdirectory(src)
add_subdirectory(test)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.3.0
)

FetchContent_MakeAvailable(Catch2)

add_executable(concurrency_app main.cpp)

target_link_libraries(concurrency_app concurrency_impl Concurrency_compiler_flags)

