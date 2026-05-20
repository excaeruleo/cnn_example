cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})

# cnnlib: CNN support library
# note: currently static until we have correct symbol export on Windows
add_library(cnnlib STATIC functions.cpp mnist.cpp utils.cpp)
target_link_libraries(cnnlib PRIVATE ZLIB::ZLIB)
# note: header interface is currently a mess and Boost dependency is transitive
target_link_libraries(
    cnnlib PUBLIC
    Boost::headers Boost::serialization Boost::YML-archive
)

# on Windows we need DLLs to be picked up in the correct build directory, which
# in our case, should be PROJECT_BINARY_DIR. we make the DLL copying a one-time
# process by having it be done before cnnlib is built vs. using POST_BUILD
if(WIN32)
    add_custom_target(
        cnnlib_dlls
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:cnnlib> $<TARGET_FILE_DIR:cnnlib>
    )
    add_dependencies(cnnlib cnnlib_dlls)
endif()
