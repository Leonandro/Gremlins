cmake_minimum_required(VERSION 3.5)
project (cache_manager VERSION 1.0.0 LANGUAGES CXX )


#--------------------------------
# This is for old cmake versions
set (CMAKE_CXX_STANDARD 11)
#--------------------------------


#=== Driver target ===

add_executable( driver_tests "src/driver_code.cpp" )
add_executable( tests_data "src/test_data_integrity.cpp" )
add_executable( tests_list "src/test_list_integrity.cpp" )