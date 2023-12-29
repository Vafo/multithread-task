set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(thread_impl thread.cpp)

target_include_directories(thread_impl PUBLIC .)

target_link_libraries(thread_impl PUBLIC module_function util_impl concurrency_common)

# Link pthread
target_link_libraries(thread_impl PUBLIC pthread Concurrency_compiler_flags)