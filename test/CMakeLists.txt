set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(multithread_test thread_test.cpp)

target_link_libraries(multithread_test multithread_impl compiler_flags)
# Catch2
target_link_libraries(multithread_test Catch2::Catch2WithMain)

# Output to build dir
set_target_properties(multithread_test 
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)