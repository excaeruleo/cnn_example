cmake_minimum_required(VERSION 3.5)

project(cnn-example)

# determine if multi-config generator
get_property(EXCCNN_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(EXCCNN_MULTI_CONFIG)
    message(STATUS "Build config: Multi")
else()
    # set default build type to Debug if not specified for single-config
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
    endif()
    message(STATUS "Build config: ${CMAKE_BUILD_TYPE}")
endif()

# C++ standard requirements
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Debug option
option(USE_DEBUG "Enter debug mode" OFF)
if(USE_DEBUG)
    add_definitions(-DDEBUG)
    message(STATUS "Debug mode: ON")
else()
    message(STATUS "Debug mode: OFF")
endif()

# enable testing in top level
include(CTest)

# Rest of your existing configuration
SET("OpenCV_DIR" "/usr/include/opencv4")
find_package(Boost REQUIRED COMPONENTS serialization system filesystem iostreams)
find_package(OpenCV REQUIRED)
find_package(ZLIB REQUIRED)

MESSAGE(STATUS "Include dirs ${OpenCV_INCLUDE_DIRS}")
MESSAGE(STATUS "LINK LIBRARIES ${OpenCV_LIBS}")

# set compile options
if(MSVC)
    add_compile_options(/W4)
else()
    # TODO: consider if -Wextra and -frtti are necessary
    # TODO: may also want -g during Release for debug info
    add_compile_options(-Wall $<$<CONFIG:Release>:-O3>)
endif()

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(DIVISIBLE_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(DIVISIBLE_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(DIVISIBLE_INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)

set(YAML_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src/boost-archive-yaml)
set(CNN_HEADERS_DIR  ${PROJECT_SOURCE_DIR}/include)

include_directories(${DIVISIBLE_INSTALL_INCLUDE_DIR})
include_directories(${DIVISION_HEADERS_DIR})

add_subdirectory(src)
add_subdirectory(test)
