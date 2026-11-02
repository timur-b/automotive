cmake_minimum_required(VERSION 3.20)

project(automotive
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(libraries/embedded-input)
add_subdirectory(libraries/automotive-core)
add_subdirectory(tests/host)

