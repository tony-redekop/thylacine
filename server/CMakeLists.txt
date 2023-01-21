cmake_minimum_required(VERSION 3.14)

set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)

project(thylacine)

add_subdirectory(src)
add_subdirectory(test)

target_link_options(runtest PRIVATE -Wall -Wextra -ldl)
target_include_directories(runtest PRIVATE include)