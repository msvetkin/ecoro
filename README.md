[![linux](https://github.com/msvetkin/ecoro/actions/workflows/linux.yml/badge.svg)](https://github.com/msvetkin/ecoro/actions/workflows/linux.yml)
[![macos](https://github.com/msvetkin/ecoro/actions/workflows/macos.yml/badge.svg)](https://github.com/msvetkin/ecoro/actions/workflows/macos.yml)
[![windows](https://github.com/msvetkin/ecoro/actions/workflows/windows.yml/badge.svg)](https://github.com/msvetkin/ecoro/actions/workflows/windows.yml)

ecoro is a research project to find an easy way use coroutines in daily c++ life.

# Requirements

A recent compiler that supports C++20 or later. ecoro is known to work
with the following compilers:

* GCC, 10.x and later
* Clang, 10.x and later
* MSVC 19.29 and later

# Add to project

Clone as a submodule. Then add like this:

```cmake
add_subdirectory(src)
```

Link your target(s) against it:
```cmake
target_link_libraries(my_target PRIVATE ecoro)
```

## Developer build

You need to have GTest installed for testing.

#### The default building:

```shell
cmake -G "Ninja" -B build
cmake --build build
ctest
```

#### CMake presets:

vcpkg presets uses "Ninja Multi-Config" generator and try to auto-detect vcpkg.cmake location.
If auto-dectection does not work then you can use the standard way of vcpkg integration via
`-DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake`.

```shell
cmake --preset vcpkg-<clang/gcc/msvc>
cmake --build --preset vcpkg-<clang/gcc/msvc>
ctest --preset vcpkg-<clang/gcc/msvc>
```
