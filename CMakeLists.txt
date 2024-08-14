cmake_minimum_required(VERSION 3.29)

project(Mithril)

enable_language(CXX)
enable_language(C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "" FORCE)

add_subdirectory(Mithril)
add_subdirectory(Sandbox)
