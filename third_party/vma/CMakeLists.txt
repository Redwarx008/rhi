cmake_minimum_required(VERSION 3.16)

project(vma)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 14)

add_library(vma INTERFACE)

target_include_directories(vma INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})