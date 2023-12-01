set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(util_impl INTERFACE)

target_include_directories(util_impl INTERFACE .)

target_link_libraries(util_impl INTERFACE function_impl)
target_link_libraries(util_impl INTERFACE compiler_flags)