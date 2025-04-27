cmake_minimum_required(VERSION 3.20)

SET(ProjectName HTTP-server)

project(${ProjectName} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

add_compile_options(
    -Wall
    -Wno-format
    -Wno-unused-function
    -Wno-maybe-uninitialized
    -fpermissive
    -lws2_32
)

file(GLOB sources main.cpp src/*cpp)
add_executable(${ProjectName}
    ${sources}
)

target_include_directories(${ProjectName} PRIVATE inc)

#target_link_libraries(${ProjectName} )
