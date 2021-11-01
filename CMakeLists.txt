cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(ecoro
        DESCRIPTION "easy coroutine library"
        LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)


if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT ANDROID)
  add_compile_options(-stdlib=libc++)
  add_link_options(-stdlib=libc++ -lc++abi)
endif()

add_subdirectory(src)
add_subdirectory(examples)

enable_testing()
add_subdirectory(tests)
