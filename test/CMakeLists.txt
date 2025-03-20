#How to build robust software and make software development pleasant
# 1. Set up a flexible build system.
# 2. Set up unit tests for your code.
# 3. Set up a test frame to constantly check your changes.
# 4. Set up revision control.
# 5. cmake and git make the process managable.
cmake_minimum_required(VERSION 3.5)
project(cnn_tests)

option(USE_DEBUG "Enter debug mode" OFF)
if (USE_DEBUG)
  add_definitions(-DDEBUG)
endif()

list( APPEND cnn_testinput
  testinput/test.png
  testinput/simple_nn.yaml
  testinput/train-images-idx3-ubyte.gz
  testinput/train-labels-idx1-ubyte.gz
  testinput/t10k-images-idx3-ubyte.gz
  testinput/t10k-labels-idx1-ubyte.gz 
)

# Prepare a list of sources files under test
file( GLOB SOURCE_FILES ./*.cpp )

# Create an empty list for executables
set(cnn_testlist)

# Iterate over each source file
foreach(SOURCE_FILE ${SOURCE_FILES})

    # Get the base name of the source file (without extension)
    get_filename_component(BASE_NAME ${SOURCE_FILE} NAME_WE)

    # Add an executable target for each source file
    add_executable(${BASE_NAME} ${SOURCE_FILE})

    # Append the executable name to the list
    list(APPEND cnn_testlist ${BASE_NAME})

endforeach()

# Print the list of executables
message("List of executables: ${cnn_testlist}")

# Copy the list of executables to list of tests
set(cnn_testexec ${cnn_testlist})

# Make the testinput directory under test
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testinput)
# Soft link all the testinput files
message(CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
message(CMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")
message(CMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}")

foreach(FILENAME ${cnn_testinput})
	execute_process( COMMAND ${CMAKE_COMMAND} -E create_symlink
		${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}
		${CMAKE_CURRENT_BINARY_DIR}/${FILENAME} )
endforeach(FILENAME)

enable_testing()
foreach(FILENAME ${cnn_testlist})
  add_test(
    NAME ${FILENAME}
    COMMAND ${FILENAME}
  )
endforeach(FILENAME)
# No need to check the opencv test all the time.
# add_test(
# NAME test_opencv
# COMMAND test_opencv "testinput/test.png"
# )
# add_test(
# NAME test_file_exists
# COMMAND test_file_exists "test_layer.cpp non_existent.cpp"
# )
include_directories(${CNN_HEADERS_DIR} ${YAML_HEADERS_DIR} /usr/include/opencv4)
link_directories(${CMAKE_BINARY_DIR}/src .)

set(EXECUTABLE_OUTPUT_PATH ../bin)
set(BOOST_LINK_FLAG boost_serialization boost_system boost_filesystem boost_iostreams z)
set(CNNLIB_LINK_FLAG cnnlib)
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(X_LINK_FLAG X11 Xext)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(X_LINK_FLAG gdi32)
endif()

#message(OpenCV_LIBS="${OpenCV_LIBS}")

# https://stackoverflow.com/questions/14306642/adding-multiple-executables-in-cmake
foreach( testsourcefile ${SOURCE_FILES} )
    # Cut off the file extension and directory path
    get_filename_component( testname ${testsourcefile} NAME_WE )
    #add_executable( ${testname} ${testsourcefile} )
    # Make sure YourLib is linked to each app
    # target_link_libraries( ${testname} YourLib )
    target_link_libraries( ${testname} ${CNNLIB_LINK_FLAG} ${OpenCV_LIBS} ${X_LINK_FLAG} ${BOOST_LINK_FLAG} )
endforeach( testsourcefile ${SOURCE_FILES} )

install(TARGETS ${cnn_testexec} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../bin)

#file(COPY testinput DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

