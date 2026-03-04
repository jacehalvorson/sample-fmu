cmake_minimum_required(VERSION 3.10)
project(HelloWorldFMU C)

# Detect if cross-compiling
if(DEFINED CMAKE_TOOLCHAIN_FILE)
  message(STATUS "Cross-compiling with toolchain: ${CMAKE_TOOLCHAIN_FILE}")
endif()

# FMU library
add_library(HelloWorld SHARED sources/fmu.c)

# Set output directory for binaries
set_target_properties(HelloWorld PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/binaries"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/binaries"
)

# Platform-specific settings
if(WIN32)
    # Windows specific configuration
    set_target_properties(HelloWorld PROPERTIES SUFFIX ".dll")
    message(STATUS "Building for Windows")
elseif(APPLE)
    # macOS specific configuration
    set_target_properties(HelloWorld PROPERTIES SUFFIX ".dylib")
    message(STATUS "Building for macOS")
elseif(UNIX AND NOT APPLE)
    # Linux/Unix/RTOS configuration
    set_target_properties(HelloWorld PROPERTIES SUFFIX ".so")
    
    # Check if this is an ARM platform (NXP i.MX95 variant)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm*" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
        message(STATUS "Building for ARM platform: ${CMAKE_SYSTEM_PROCESSOR}")
        
        # Add RTOS detection
        if(DEFINED FREERTOS_DIR)
            message(STATUS "FreeRTOS detected at: ${FREERTOS_DIR}")
            target_include_directories(HelloWorld PRIVATE ${FREERTOS_DIR}/include)
            add_compile_definitions(FMU_RTOS_FREERTOS=1)
        elseif(DEFINED QNX_BASE)
            message(STATUS "QNX detected at: ${QNX_BASE}")
            add_compile_definitions(FMU_RTOS_QNX=1)
        endif()
    else()
        message(STATUS "Building for Linux x86: ${CMAKE_SYSTEM_PROCESSOR}")
    endif()
endif()

# Include directories
target_include_directories(HelloWorld PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Compiler flags for shared library
if(MSVC)
    target_compile_options(HelloWorld PRIVATE /W4)
else()
    # GCC/Clang compatible flags
    target_compile_options(HelloWorld PRIVATE -Wall -Wextra -fPIC)
    
    # RTOS-specific flags
    if(DEFINED ARM_RTOS)
        target_compile_options(HelloWorld PRIVATE -mcpu=${ARM_RTOS})
    endif()
endif()

# Optional math library (Linux)
if(UNIX AND NOT APPLE)
    target_link_libraries(HelloWorld PRIVATE m)
endif()

# Print final configuration info
message(STATUS "=== HelloWorld FMU Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Output directory: ${CMAKE_BINARY_DIR}/binaries")
message(STATUS "=====================================")

