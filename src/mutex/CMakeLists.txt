set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(mutex_impl mutex.cpp)

target_include_directories(mutex_impl PUBLIC .)
target_link_libraries(mutex_impl PUBLIC util_impl)

# Link pthread
target_link_libraries(mutex_impl PUBLIC pthread Concurrency_compiler_flags)