cmake_minimum_required(VERSION 3.9)
project(myBigChars)

set(CMAKE_C_STANDARD 99)
#set(libraries "${libraries} myBigChars" PARENT_SCOPE)

include_directories("../term/")
add_library(myBigChars myBigChars.c)
target_include_directories(myBigChars PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})