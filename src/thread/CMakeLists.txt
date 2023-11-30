set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(thread_impl thread.cpp)

target_include_directories(thread_impl PUBLIC .)

target_link_libraries(thread_impl PUBLIC function_impl)

# Link pthread
target_link_libraries(thread_impl PUBLIC pthread compiler_flags)