cmake_minimum_required(VERSION 3.9)
project(myTerm)

set(CMAKE_C_STANDARD 99)
#set(libraries "${libraries} myTerm" PARENT_SCOPE)

add_library(myTerm myTerm.c)
target_include_directories(myTerm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})