# cnn_example

Implement a simple neural network to recognize handwritten numbers.

## Building

[CMake] >= 3.21 and a C++17 compiler is required to build the project. 

[CMake]: https://cmake.org/cmake/help/latest/index.html

For Unix:

<!-- Debug or default config by not setting CMAKE_BUILD_TYPE -->

```shell
cmake -S . -B build && cmake --build build -j
```

For Windows:

<!-- Debug config by using Debug instead -->

[CTest] tests can be run to validate the build.

[CTest]: https://cmake.org/cmake/help/latest/manual/ctest.1.html

For Unix: 

```shell
ctest --test-dir build -j$(nproc)
```

For Windows:

```shell
ctest --test-dir build_win64 -C Release -j%NUMBER_OF_PROCESSORS%
```
