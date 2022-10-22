cmake_minimum_required(VERSION 3.22)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 14)
set(include_dir /Volumes/ZHITAI/include)
link_directories(/usr/local/Cellar/glfw/3.3.7/lib)

include_directories(${include_dir})
include_directories(${include_dir}/glad/include)
include_directories(${include_dir}/glm/)
include_directories(/usr/local/Cellar/glfw/3.3.7/include)

find_package(assimp REQUIRED) # 添加assimp

add_executable(LearnOpenGL main.cpp
        ${include_dir}/glad/src/glad.c
        ${include_dir}/stb_image.h
        ${include_dir}/shader_m.h
        ${include_dir}/camera.h
        ${include_dir}/mesh.h
        ${include_dir}/model.h
        )

target_link_libraries(
        LearnOpenGL
        glfw
        assimp::assimp
)

add_executable(hello_window 01_hello_window/window.cpp
        ${include_dir}/glad/src/glad.c
        )

target_link_libraries(
        hello_window
        glfw
)

add_executable(hello_triangle 02_hello_triangle/hello_triangle.cpp
        ${include_dir}/glad/src/glad.c
        )

target_link_libraries(
        hello_triangle
        glfw
)

add_executable(shaders 03_shaders/shaders.cpp
        ${include_dir}/glad/src/glad.c
        ${include_dir}/shader_m.h
        )

target_link_libraries(
        shaders
        glfw
)

add_executable(textures 04_textures/textures.cpp
        ${include_dir}/glad/src/glad.c
        ${include_dir}/shader_m.h
        ${include_dir}/stb_image.h
        )

target_link_libraries(
        textures
        glfw
)

add_executable(transformations 05_Transformations/transformations.cpp
        ${include_dir}/glad/src/glad.c
        ${include_dir}/shader_m.h
        ${include_dir}/stb_image.h
        )

target_link_libraries(
        transformations
        glfw
)