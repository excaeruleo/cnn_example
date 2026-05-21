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

# C++ standard requirements
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# enable testing in top level
include(CTest)

# ensure all build outputs go in top-level directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# find Boost
# note: compatibility with older versions of Boost
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.30)
    cmake_policy(PUSH)
    cmake_policy(SET CMP0167 OLD)
endif()
# on Windows require static libraries due to automatic compilation with
# BOOST_<COMPONENT>_STATIC_LINK being used
if(WIN32)
    set(Boost_USE_STATIC_LIBS ON)
endif()
# TODO: need to ensure that Boost.Iostreams has been built with zlib support
# otherwise using boost/iostreams/filter/zlib.hpp gives linker errors
find_package(Boost 1.71 REQUIRED COMPONENTS iostreams serialization)
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.30)
    cmake_policy(POP)
endif()
message(STATUS "Boost version: ${Boost_VERSION}")
# indicate if static libraries are being used
if(Boost_USE_STATIC_LIBS)
    message(STATUS "Boost libraries: Static")
else()
    message(STATUS "Boost libraries: Shared")
endif()

# create Boost.YML-archive IMPORTED target
add_library(Boost::YML-archive IMPORTED INTERFACE)
target_include_directories(
    Boost::YML-archive INTERFACE
    "${PROJECT_SOURCE_DIR}/third_party/boost-archive-yml"
)
# MSVC needs iso646.h included to support alternative operator representations
# and __INCLUDE_LEVEL__ is defined to 1 to prevent Boost.YML-archive from
# defining its own main() entry point on Windows
if(MSVC)
    target_compile_definitions(Boost::YML-archive INTERFACE __INCLUDE_LEVEL__=1)
    target_compile_options(Boost::YML-archive INTERFACE /FIiso646.h)
endif()

# find OpenCV
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

# find X11
find_package(X11)
if(X11_FOUND)
    message(STATUS "X version: 11")
else()
    message(STATUS "X version: None")
endif()

# 1/0 variable for Windows detection since WIN32 is not always defined
if(WIN32)
    set(EXCCNN_WIN32 1)
else()
    set(EXCCNN_WIN32 0)
endif()

# set compile options
if(MSVC)
    add_compile_options(/W4)
else()
    # TODO: consider if -Wextra and -frtti are necessary
    # TODO: may also want -g during Release for debug info
    add_compile_options(-Wall $<$<CONFIG:Release>:-O3>)
endif()

add_subdirectory(src)
add_subdirectory(test)
