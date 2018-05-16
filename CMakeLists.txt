cmake_minimum_required(VERSION 3.9)
project(sComputer)

set(CMAKE_C_STANDARD 99)
#set(libraries "")
add_executable(main main.c interface.c cpu.c)
#change include : cpu.c

add_subdirectory(term)
add_subdirectory(bigChars)
add_subdirectory(simpleComputer)
add_subdirectory(readKey)
add_subdirectory(transale)
target_link_libraries(main myTerm myBigChars mySimpleComputer myReadKey)