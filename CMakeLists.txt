cmake_minimum_required(VERSION 3.12)
project(Pybind11Example)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-O3 -Wall)

include_directories(
    /opt/homebrew/Cellar/eigen/3.4.0_1/include/eigen3
    /usr/local/include
)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)

pybind11_add_module(example MODULE module.cpp orderbook.cpp orderbook_python_funcs.cpp sidebook.cpp sidebook_python_funcs.cpp)

if(APPLE)
    set_target_properties(example PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif()

execute_process(
    COMMAND "${Python_EXECUTABLE}" -c "from distutils.sysconfig import get_config_var; print(get_config_var('EXT_SUFFIX'))"
    OUTPUT_VARIABLE PYTHON_EXT_SUFFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set_target_properties(example PROPERTIES OUTPUT_NAME "orderbook")
set_target_properties(example PROPERTIES SUFFIX "${PYTHON_EXTENSION_SUFFIX}")
set_target_properties(example PROPERTIES PREFIX "")