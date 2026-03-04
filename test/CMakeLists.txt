cmake_minimum_required(VERSION 3.10)
project(HelloWorldFMU_Tests C)

# Add test executable
add_executable(test_fmu test_fmu.c)

# Link against dl (dynamic library loader) on Unix
if(UNIX AND NOT APPLE)
    target_link_libraries(test_fmu PRIVATE dl m)
elseif(APPLE)
    target_link_libraries(test_fmu PRIVATE dl)
endif()

# Set output directory
set_target_properties(test_fmu PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
)

message(STATUS "Test executable: ${CMAKE_BINARY_DIR}/test/test_fmu")
