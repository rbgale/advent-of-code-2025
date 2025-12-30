cmake_minimum_required(VERSION 4.2.1)

#set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON CACHE BOOL "" FORCE)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_CXX_STDLIB_MODULES_JSON "/opt/homebrew/Cellar/llvm/21.1.8/lib/c++/libc++.modules.json")
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
set(CMAKE_CXX_MODULE_STD ON)

project(aoc_2025 CXX)

#add_library(aoc_common STATIC common.cpp)

add_compile_options(-stdlib=libc++)
add_link_options(-stdlib=libc++)

add_executable(day1.1 day1/day1.1.cpp)
add_executable(day1.2 day1/day1.2.cpp)

add_executable(day2.1 day2/day2.1.cpp)
add_executable(day2.2 day2/day2.2.cpp)

add_executable(day3.1 day3/day3.1.cpp)
add_executable(day3.2 day3/day3.2.cpp)

add_executable(day4.1 day4/day4.1.cpp)
add_executable(day4.2 day4/day4.2.cpp)

add_executable(day5.1 day5/day5.1.cpp)
add_executable(day5.2 day5/day5.2.cpp)
#target_link_libraries(day1 PRIVATE aoc_common)
