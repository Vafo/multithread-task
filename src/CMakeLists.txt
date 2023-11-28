set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(thread)

add_library(multithread_impl INTERFACE)

target_link_libraries(multithread_impl INTERFACE thread_impl)

target_link_libraries(multithread_impl INTERFACE compiler_flags)