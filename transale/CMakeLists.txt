cmake_minimum_required(VERSION 3.9)
project(myTranslate)

set(CMAKE_C_STANDARD 99)
#set(libraries "${libraries} mySimpleComputer" PARENT_SCOPE)

include_directories("../")
include_directories("../simpleComputer/")
add_library(myTranslate asm.c basic.c rpn.c)
target_include_directories(myTranslate PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})