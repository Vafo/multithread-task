set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(function_impl INTERFACE)

target_include_directories(function_impl INTERFACE .)

target_link_libraries(function_impl INTERFACE Concurrency_compiler_flags)