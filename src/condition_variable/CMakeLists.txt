set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(condition_var_impl condition_variable.cpp)

target_include_directories(condition_var_impl PUBLIC .)

# Link mutex
target_link_libraries(condition_var_impl PUBLIC mutex_impl util_impl)
# Link pthread
target_link_libraries(condition_var_impl PUBLIC pthread Concurrency_compiler_flags)