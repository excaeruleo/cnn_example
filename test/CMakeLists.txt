cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# How to build robust software and make software development pleasant:
#
# 1. Set up a flexible build system.
# 2. Set up unit tests for your code.
# 3. Set up a test framework to constantly check your changes.
# 4. Set up revision control.
# 5. CMake and Git make the process managable.
#

# ensure test programs can be run from build directory without copying test
# input files. we do this by creating symlinks from PROJECT_SOURCE_DIR to
# PROJECT_BINARY_DIR for each of the input files in testinput we care about
list(
    APPEND test_input_files
    testinput/train-images-idx3-ubyte.gz
    testinput/train-labels-idx1-ubyte.gz
    testinput/t10k-images-idx3-ubyte.gz
    testinput/t10k-labels-idx1-ubyte.gz
)
# custom target for the symlink creation + initial directory creation
add_custom_target(exccnn_testinput_symlinks ALL)
add_custom_target(
    exccnn_testinput_build_dir
    COMMAND ${CMAKE_COMMAND} -E make_directory
            ${PROJECT_BINARY_DIR}/test/testinput
    VERBATIM
)
# extra dependency in case test_input_files is empty
add_dependencies(exccnn_testinput_symlinks exccnn_testinput_build_dir)
# add custom target to create symlink for each input file
foreach(input_file ${test_input_files})
    # mangle name appropriately
    string(REGEX REPLACE "[/.-]+" "_" target_suffix "${input_file}")
    # add target to create symlink
    # note: on Windows elevated privileges are required to create symbolic
    # links so we zwork around this by creating a hard link instead
    add_custom_target(
        exccnn_symlink_${target_suffix}
        COMMAND ${CMAKE_COMMAND} -E create_$<IF:${EXCCNN_WIN32},hard,sym>link
                ${CMAKE_CURRENT_SOURCE_DIR}/${input_file}
                ${PROJECT_BINARY_DIR}/test/${input_file}
        BYPRODUCTS "${PROJECT_BINARY_DIR}/test/${input_file}"
        COMMENT "Creating link for ${input_file}"
        VERBATIM
    )
    # ensure target is run only when test/testinput directory is created
    add_dependencies(exccnn_symlink_${target_suffix} exccnn_testinput_build_dir)
    # ensure that exccnn_testinput_symlinks depends on the rule
    add_dependencies(exccnn_testinput_symlinks exccnn_symlink_${target_suffix})
endforeach()

##
# Create a simple test executable that runs in the top-level binary directory.
#
# The test looks for ${target}.cpp, creates the executable target ${target},
# and adds a CTest test ${target} that runs in ${PROJECT_BINARY_DIR}. Any extra
# libraries are linked as PRIVATE using target_link_libraries().
#
# Additional test properties need to be done manually.
#
# Arguments:
#   target                  Executable target
#   [LIBRARIES libs...]     Library targets for target_link_libraries()
#
function(exccnn_add_test target)
    cmake_parse_arguments(ARG "" "" "LIBRARIES" ${ARGN})
    # create test executable using libraries
    add_executable(${target} ${target}.cpp)
    if(ARG_LIBRARIES)
        target_link_libraries(${target} PRIVATE ${ARG_LIBRARIES})
    endif()
    # basic test
    add_test(
        NAME ${target}
        COMMAND ${target}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )
endfunction()

# test_boost_ver: prints the Boost version
exccnn_add_test(test_boost_ver LIBRARIES Boost::headers)

# test_cnn2: simple neural network test
exccnn_add_test(
    test_cnn2 LIBRARIES
    Boost::serialization Boost::YML-archive cnnlib
)
# test_cnn2 produces the trained2.yml used by test_cnn2_import
set_tests_properties(test_cnn2 PROPERTIES FIXTURES_SETUP trained2_yml)

# test_cnn2_import: simple neural network test using test_cnn2 YML model output
exccnn_add_test(
    test_cnn2_import LIBRARIES
    Boost::serialization Boost::YML-archive cnnlib
)
# test_cnn2_import uses the trained2.yml from test_cnn2
set_tests_properties(test_cnn2_import PROPERTIES FIXTURES_REQUIRED trained2_yml)

# test_cnn3: another simple neural network test
exccnn_add_test(
    test_cnn3 LIBRARIES
    Boost::serialization Boost::YML-archive cnnlib
)
# test_cnn3 produces trained3.yml
set_tests_properties(test_cnn3 PROPERTIES FIXTURES_SETUP trained3_yml)

# test_cnn4: another simple neural network test
exccnn_add_test(
    test_cnn4 LIBRARIES
    Boost::serialization Boost::YML-archive cnnlib
)
# test_cnn4 produces trained4.yml
set_tests_properties(test_cnn4 PROPERTIES FIXTURES_SETUP trained4_yml)

# test_cnn5: neural network training test on 1000 images from MNIST
exccnn_add_test(
    test_cnn5 LIBRARIES
    Boost::serialization Boost::YML-archive cnnlib
)
# test_cnn5 produces trained5.yml
set_tests_properties(test_cnn5 PROPERTIES FIXTURES_SETUP trained5_yml)

# test_cnn6: larger neural network test on 1000 images from MNIST
# note: not run with CTest since it's pretty slow
# note: test_cnn6 produces trained_mnist.yml
add_executable(test_cnn6 test_cnn6.cpp)
target_link_libraries(
    test_cnn6 PRIVATE
    Boost::serialization Boost::YML-archive cnnlib
)

# test_compression: Boost.Serialize struct compression test
# note: need additional zlib link due to use of boost/iostreams/filter/zlib.hpp
exccnn_add_test(
    test_compression LIBRARIES
    Boost::iostreams Boost::serialization ZLIB::ZLIB
)
# test_compression produces compressed_data.dat
set_tests_properties(
    test_compression PROPERTIES
    FIXTURES_SETUP compressed_data_dat
)

# test_connection: test the CNN connection class
# FIXME: test just includes connection.hpp and returns zero
exccnn_add_test(test_connection LIBRARIES cnnlib)

# test_connector: test the CNN connector class
# FIXME: test just includes connector.hpp and returns zero
exccnn_add_test(test_connector LIBRARIES cnnlib)

# test_file_exists: test if several files exist
# note: kind of silly test but this has been revamped to work correctly
add_executable(test_file_exists test_file_exists.cpp)
# use some of the existing source files as inputs
add_test(
    NAME test_file_exists
    COMMAND test_file_exists test_cnn2.cpp test_cnn3.cpp test_cnn4.cpp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# test_fr_read_linux: test writing a screenshot of the current screen
if(X11_FOUND AND X11_XShm_FOUND)
    exccnn_add_test(test_fr_read_linux LIBRARIES X11::X11 X11::Xext)
    # skip test if X display is not available or if X server doesn't support
    # the MIT-SHM extension required for this test to work correctly
    set_property(
        TEST test_fr_read_linux PROPERTY
        SKIP_REGULAR_EXPRESSION "Unable to open X display"
                                "X server does not support MIT-SHM extension"
    )
else()
    message(STATUS "Skipping test_fr_read_linux (X11 and XShm required)")
endif()

# test_ifassign: conditional assignment test
# TODO: consider removing as it's not really relevant
exccnn_add_test(test_ifassign)

# test_inheritance: test class member function inhertiance
# TODO: consider removing as it's not really relevant
exccnn_add_test(test_inheritance)

# test_layer: test the CNN layer class
exccnn_add_test(test_layer LIBRARIES cnnlib)

# test_map_archive: test std::unordered_map archive with Boost.Serialization
exccnn_add_test(test_map_archive LIBRARIES Boost::headers Boost::serialization)

# test_matrix: Boost.Numeric uBLAS matrix functionality test
exccnn_add_test(test_matrix LIBRARIES Boost::headers cnnlib)

# test_neuron: test CNN neuron class
# FIXME: test just includes neuron.hpp and returns zero
exccnn_add_test(test_neuron LIBRARIES cnnlib)

# test_optimizer: test CNN optimizer class
# note: mostly a smoke test of construction and doesn't do much
exccnn_add_test(test_optimizer LIBRARIES cnnlib)

# test_rand: test the stdlib.h rand() function
# TODO: consider removing as it's not really relevant
exccnn_add_test(test_rand)

# test_screencap: capture the screen and save it to a PPM file
# on Windows need gdi32 and user32 libraries
if(WIN32)
    exccnn_add_test(test_screencap LIBRARIES gdi32 user32)
# otherwise we use X11 for non-Windows platforms
elseif(X11_FOUND)
    exccnn_add_test(test_screencap LIBRARIES X11::X11)
    # skip test if X display is not available
    set_property(
        TEST test_screencap PROPERTY
        SKIP_REGULAR_EXPRESSION "Unable to open X display"
    )
else()
    message(STATUS "Skipping test_screencap (X11 required)")
endif()

# test_weights: test CNN weights construction + some uBLAS stuff
# TODO: probably want to write a more meaningful test here
exccnn_add_test(test_weights LIBRARIES Boost::headers cnnlib)

# unordered_map: test use of unordered_map for use as a sparse array
exccnn_add_test(unordered_map)
