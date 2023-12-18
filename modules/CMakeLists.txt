set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(function EXCLUDE_FROM_ALL)

add_library(module_function ALIAS function_impl)


