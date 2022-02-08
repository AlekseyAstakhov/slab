cmake_minimum_required(VERSION 3.5)

project(slab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tests tests/tests.cpp)

add_executable(simple_example examples/simple.cpp)
add_executable(owning_example examples/owning.cpp)
add_executable(iterator_example examples/iterator.cpp)
add_executable(polymorph_example examples/polymorph.cpp)
