cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# cnnlib: CNN support library
add_library(cnnlib SHARED functions.cpp mnist.cpp utils.cpp)
target_include_directories(cnnlib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(cnnlib PRIVATE ZLIB::ZLIB)
# note: header interface is currently a mess and Boost dependency is transitive
target_link_libraries(
    cnnlib PUBLIC
    Boost::headers Boost::serialization Boost::YML-archive
)
# use d postfix for Debug builds + ensure symbols exported in shared build
set_target_properties(
    cnnlib PROPERTIES
    DEBUG_POSTFIX d
    DEFINE_SYMBOL EXCCNN_BUILD_DLL
)

##
# Obtain the list of ZLIB::ZLIB DLLs.
#
# The list may be empty if only static libraries are available. This function
# copes with FindZLIB.cmake causing IMPORTED_LOCATION_<CONFIG> to be set not to
# the DLL, but to the import library, on Windows.
#
# Arguments:
#
#   output              Output list variable of ZLIB::ZLIB DLLs
#   CONFIGS config...   Configurations, e.g. Debug, Release
#
function(exccnn_zlib_dlls output)
    # get configs to handle
    cmake_parse_arguments(ARG "" "" "CONFIGS" ${ARGN})
    if(NOT ARG_CONFIGS)
        message(FATAL_ERROR "CONFIGS required")
    endif()
    # for each config
    foreach(config ${ARG_CONFIGS})
        # need uppercase config
        string(TOUPPER "${config}" config_upper)
        # get IMPORTED_LOCATION for config
        get_target_property(
            zlib_dll ZLIB::ZLIB
            IMPORTED_LOCATION_${config_upper}
        )
        # if not a DLL, need to reassemble e.g. bin/z[d].dll from lib/z[d].lib
        cmake_path(GET zlib_dll EXTENSION zlib_dll_ext)
        if(NOT zlib_dll_ext STREQUAL ".dll")
            cmake_path(GET zlib_dll STEM zlib_dll_stem)
            cmake_path(GET zlib_dll PARENT_PATH zlib_dll)
            cmake_path(GET zlib_dll PARENT_PATH zlib_dll)
            cmake_path(APPEND zlib_dll "bin" "${zlib_dll_stem}.dll")
        endif()
        # if the path exists append to list of zlib DLLs to copy
        if(EXISTS "${zlib_dll}")
            list(APPEND zlib_dlls "${zlib_dll}")
        endif()
    endforeach()
    # set and return
    set(${output} ${zlib_dlls} PARENT_SCOPE)
endfunction()

# on Windows we need DLLs to be picked up in the correct build directory, which
# in our case, should be PROJECT_BINARY_DIR. we make the DLL copying a one-time
# process by having it be done before cnnlib is built vs. using POST_BUILD
if(WIN32)
    # on Windows, IMPORTED_lOCATION_<CONFIG> may not have the correct DLL
    # location, e.g. the result from FindZLIB.cmake. in this case, we manually
    # extract the Debug and Release DLLs as necessary
    exccnn_zlib_dlls(zlib_dlls CONFIGS Debug Release)
    # define custom copy command
    add_custom_target(
        cnnlib_dlls
        # note: to ensure that even if $<TARGET_RUNTIME_DLLS:cnnlib> and
        # zlib_dlls are both empty that we don't get an error, we copy an extra
        # file (the README.md of all things), to the build directory
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${PROJECT_SOURCE_DIR}/README.md
                ${zlib_dlls}
                $<TARGET_RUNTIME_DLLS:cnnlib> $<TARGET_FILE_DIR:cnnlib>
        COMMENT "Copying cnnlib runtime DLLs + README"
        VERBATIM
        COMMAND_EXPAND_LISTS
    )
    add_dependencies(cnnlib cnnlib_dlls)
endif()
