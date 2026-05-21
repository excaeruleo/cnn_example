# cnn_example

Implement a simple neural network to recognize handwritten numbers.

## Building

[CMake] >= 3.21 and a C++17 compiler is required to build the project as well
as the following:

* [Boost] >= 1.71.0
* [zlib] >= 1.2.1

[CMake]: https://cmake.org/cmake/help/latest/index.html
[Boost]: https://www.boost.org/
[zlib]: https://zlib.net/

In particular, the non-header-only [Boost.Iostreams] and [Boost.Serialization]
libraries are required, as well as their transitive dependencies. This means
that if using a partial Boost installation, it's best to follow the advice on
using [individual Boost modules]. Furthermore, since this project uses the
[zlib compression filter], when building the Boost libraries, Boost.Iostreams
must be built with zlib enabled so `zlib.cpp` gets compiled.

[Boost.Iostreams]: https://www.boost.org/doc/libs/latest/libs/iostreams/doc/index.html
[Boost.Serialization]: https://www.boost.org/doc/libs/latest/libs/serialization/doc/index.html
[individual Boost modules]: https://www.boost.org/doc/user-guide/getting-started.html#_individual_modules
[zlib compression filter]: https://www.boost.org/doc/libs/latest/boost/iostreams/filter/zlib.hpp

On Unix-like systems, you can generally get a full Boost installation using the
native package manager, e.g. APT, where Boost.Iostreams will already have zlib
support as there should be a system zlib installed. However, on Windows, you
will also need to build zlib, and ensure that Boost.Iostreams is linked against
your local installation. If building a Boost release with CMake support on
Windows, this means that you must also configure with `ZLIB_ROOT` set and/or
`BOOST_IOSTREAMS_ENABLE_ZLIB=ON` to ensure Boost.Iostreams is built with zlib
support. Otherwise, when building this project, you will see linker errors
complaining about zlib-related Boost.Iostreams symbols not being resolved.

To build the Release configuration on Unix (default is Debug), use:

<!-- Debug or default config by not setting CMAKE_BUILD_TYPE -->

```shell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
```

To build the Release configuration on Windows, use:

<!-- Debug config by using Debug instead -->

```shell
cmake -S . -B build_win64 -A x64 && cmake --build build_win64 --config Release -j
```

Of course, `Boost_ROOT` and `ZLIB_ROOT` should be configured appropriately.

## Testing

[CTest] tests can be run to validate the build.

[CTest]: https://cmake.org/cmake/help/latest/manual/ctest.1.html

For Unix:

```shell
ctest --test-dir build -j$(nproc)
```

For the Release configuration on Windows:

```shell
ctest --test-dir build_win64 -C Release -j%NUMBER_OF_PROCESSORS%
```
