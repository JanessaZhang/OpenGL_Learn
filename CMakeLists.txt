cmake_minimum_required(VERSION 3.21)
 
project(my_project)
 
find_package(OpenGL REQUIRED)
add_library(glad glad/src/glad.c)
target_include_directories(glad  PUBLIC glad/include)
 
set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl") 
 
add_executable(main main.cpp)
target_link_libraries(main glfw ${GL_LIBRARY} glad)
