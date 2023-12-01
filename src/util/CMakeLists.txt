set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(util_impl INTERFACE)

target_include_directories(util_impl INTERFACE .)

target_link_libraries(util_impl INTERFACE function_impl)
target_link_libraries(util_impl INTERFACE Concurrency_compiler_flags)