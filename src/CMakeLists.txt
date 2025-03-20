cmake_minimum_required(VERSION 3.5)
project(cnn)

#add_subdirectory(division)
set(SOURCE_FILES utils.cpp mnist.cpp functions.cpp)

add_library(cnnlib SHARED ${SOURCE_FILES})
target_link_libraries(cnnlib)
MESSAGE(STATUS "LINK LIBRARIES PATH: ${CMAKE_CURRENT_BINARY_DIR}")
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../lib)
install(TARGETS cnnlib DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../lib)

#add_executable(divider ${SOURCE_FILES})
#target_link_libraries(divider division)
#install(TARGETS divider DESTINATION ${DIVISIBLE_INSTALL_BIN_DIR})
