cmake_minimum_required(VERSION 3.21)

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
# FIXME: DEBUG should have a project prefix, e.g. EXCCNN_. we can also have
# this be an INTERFACE target instead with this macro definition
option(USE_DEBUG "Enter debug mode" OFF)
if(USE_DEBUG)
    add_definitions(-DDEBUG)
    message(STATUS "Debug mode: ON")
else()
    message(STATUS "Debug mode: OFF")
endif()

# enable testing in top level
include(CTest)

# find Boost
# note: compatibility with older versions of Boost
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.30)
    cmake_policy(PUSH)
    cmake_policy(SET CMP0167 OLD)
endif()
find_package(
    Boost 1.71 REQUIRED
    COMPONENTS filesystem iostreams serialization system
)
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.30)
    cmake_policy(POP)
endif()
message(STATUS "Boost version: ${Boost_VERSION}")

# find OpenCV
# TODO: user should set this themselves
# SET("OpenCV_DIR" "/usr/include/opencv4")
# note: we don't actually use it much here so not required
find_package(OpenCV 4.2)
if(OpenCV_FOUND)
    message(STATUS "OpenCV version: ${OpenCV_VERSION}")
else()
    message(STATUS "OpenCV version: None")
endif()

# find zlib
# note: currently only used for reading MNIST pixel data from gzip files
find_package(ZLIB 1.2 REQUIRED)
if(ZLIB_FOUND)
    message(STATUS "zlib version: ${ZLIB_VERSION}")
else()
    message(STATUS "zlib version: None")
endif()

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
