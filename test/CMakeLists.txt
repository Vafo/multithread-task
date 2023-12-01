set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(concurrency_test_suite OBJECT thread_test.cpp mutex_test.cpp condition_var_test.cpp)
target_link_libraries(concurrency_test_suite Catch2::Catch2)
target_link_libraries(concurrency_test_suite concurrency_impl compiler_flags)

# Work around to create executable
add_executable(concurrency_test $<TARGET_OBJECTS:concurrency_test_suite>)

# Catch2
target_link_libraries(concurrency_test concurrency_test_suite Catch2::Catch2WithMain)

# Output to build dir
set_target_properties(concurrency_test 
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)