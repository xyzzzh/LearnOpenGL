cmake_minimum_required(VERSION 3.22)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 14)

link_directories(/usr/local/Cellar/glfw/3.3.7/lib)

include_directories(/Users/xyz/include/glad/include)
include_directories(/Users/xyz/include/glm/)
include_directories(/Users/xyz/include)
include_directories(/usr/local/Cellar/glfw/3.3.7/include)

add_executable(LearnOpenGL
        /Users/xyz/include/glad/src/glad.c
        /Users/xyz/include/stb_image.h
        main.cpp
        /Users/xyz/include/shader_m.h
        /Users/xyz/include/camera.h
        )

add_executable(hello_window 01_hello_window/window.cpp
        /Users/xyz/include/glad/src/glad.c
        /Users/xyz/include/stb_image.h
        /Users/xyz/include/shader_m.h
        /Users/xyz/include/camera.h
        )

target_link_libraries(
        LearnOpenGL
        glfw
)

target_link_libraries(
        hello_window
        glfw
)