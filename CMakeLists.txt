cmake_minimum_required(VERSION 3.16)
project(orderbook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)

include(FetchContent)

FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
)
FetchContent_MakeAvailable(eigen)

FetchContent_Declare(
    boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG boost-1.86.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(boost)


find_package(pybind11 REQUIRED)


set(SOURCE_FILES
    orderbook_python_funcs.cpp
    orderbook.cpp
    sidebook.cpp
    sidebook_python_funcs.cpp
    module.cpp
)

pybind11_add_module(orderbook MODULE ${SOURCE_FILES})


target_include_directories(orderbook PRIVATE
    ${boost_SOURCE_DIR}        
    ${boost_BINARY_DIR}        
    ${eigen_SOURCE_DIR}        
)

target_link_libraries(orderbook PRIVATE)