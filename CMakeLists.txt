cmake_minimum_required(VERSION 3.10)

project(maze4d)

cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)


add_executable(${PROJECT_NAME}
    glad/src/glad.c
    maze4d/Cube.cpp
    maze4d/Field.cpp
    maze4d/Game.cpp
    maze4d/main.cpp
    maze4d/Maze.cpp
    maze4d/Utils.cpp
)
include_directories(SYSTEM glad/include maze4d)

target_link_libraries(${PROJECT_NAME} gcc_s c glfw OpenGL::GL ${CMAKE_DL_LIBS})
