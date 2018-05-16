cmake_minimum_required(VERSION 3.9)
project(myReadKey)

set(CMAKE_C_STANDARD 99)
#set(libraries "${libraries} mySimpleComputer" PARENT_SCOPE)

add_library(myReadKey myReadKey.c)
target_include_directories(myReadKey PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})