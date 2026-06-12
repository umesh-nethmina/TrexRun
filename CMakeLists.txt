cmake_minimum_required(VERSION 3.10)
project(DinosaurGame)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(DinosaurGame main.cpp)

target_include_directories(DinosaurGame PRIVATE ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIR})
target_link_libraries(DinosaurGame ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
