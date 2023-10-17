# cmake -DCMAKE_BUILD_TYPE=Release ..
cmake_minimum_required(VERSION 3.12)
project(Metrics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
add_subdirectory("tests")

#########################################
add_library(Metrics INTERFACE)
target_include_directories(
    Metrics
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#########################################
add_executable(Benchmark
    ./Benchmark.cpp
    ./elapsed.hpp
)
target_link_libraries(Benchmark
    Metrics
)
target_compile_options(Benchmark
    PRIVATE
    -Wall
    -Weffc++
    -Wextra
)

add_executable(ShowSize
    ./ShowSize.cpp
)
target_link_libraries(ShowSize
    Metrics
)
target_compile_options(ShowSize
    PRIVATE
    -Wall
    -Weffc++
    -Wextra
)