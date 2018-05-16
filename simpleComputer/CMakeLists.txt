cmake_minimum_required(VERSION 3.9)
project(mySimpleComputer)

set(CMAKE_C_STANDARD 99)
#set(libraries "${libraries} mySimpleComputer" PARENT_SCOPE)

include_directories("../term/")
add_library(mySimpleComputer mySimpleComputer.c)
target_include_directories(mySimpleComputer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})