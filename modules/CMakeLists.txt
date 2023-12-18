set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


if(NOT TARGET function_impl)
    add_subdirectory(function EXCLUDE_FROM_ALL)
endif()

if(NOT TARGET module_function)
    add_library(module_function ALIAS function_impl)
endif()

