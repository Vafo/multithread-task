set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(thread)
add_subdirectory(mutex)
add_subdirectory(condition_variable)
add_subdirectory(function)
add_subdirectory(restop_source)
add_subdirectory(util)

add_library(concurrency_impl INTERFACE)

target_link_libraries(concurrency_impl INTERFACE
    thread_impl
    mutex_impl
    condition_var_impl
    function_impl
    restop_source_impl
)

target_link_libraries(concurrency_impl INTERFACE Concurrency_compiler_flags)