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

# on Windows we need DLLs to be picked up in the correct build directory, which
# in our case, should be PROJECT_BINARY_DIR. we make the DLL copying a one-time
# process by having it be done before cnnlib is built vs. using POST_BUILD
if(WIN32)
    add_custom_target(
        cnnlib_dlls
        # HACK: seems like the IMPORTED_LOCATION_<CONFIG> property of ZLIB::ZLIB
        # can be incorrect even if finding zlib in CONFIG mode (e.g. via
        # ZLIBConfig.cmake), so we copy an extra file (the README.md of all
        # things) to ensure that that there is at least one file copied
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${PROJECT_SOURCE_DIR}/README.md
                $<TARGET_RUNTIME_DLLS:cnnlib> $<TARGET_FILE_DIR:cnnlib>
        COMMENT "Copying cnnlib runtime DLLs + README"
        VERBATIM
        COMMAND_EXPAND_LISTS
    )
    add_dependencies(cnnlib cnnlib_dlls)
endif()
