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
add_library(imgui STATIC
        ${include_dir}/imgui/imgui.cpp
        ${include_dir}/imgui/imgui_impl_glfw.cpp
        ${include_dir}/imgui/imgui_impl_opengl3.cpp
        ${include_dir}/imgui/imgui_draw.cpp
        ${include_dir}/imgui/imgui_demo.cpp
        ${include_dir}/imgui/imgui_tables.cpp
        ${include_dir}/imgui/imgui_widgets.cpp)

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

add_executable(01_hello_window 01_hello_window/window.cpp
        ${include_dir}/glad/src/glad.c
        )

target_link_libraries(
        01_hello_window
        glfw
)

add_executable(02_hello_triangle 02_hello_triangle/hello_triangle.cpp
        ${include_dir}/glad/src/glad.c
        )

target_link_libraries(
        02_hello_triangle
        glfw
)

add_executable(03_shaders 03_shaders/shaders.cpp
        ${include_dir}/glad/src/glad.c
        ${include_dir}/shader_m.h
        )

target_link_libraries(
        03_shaders
        glfw
)

add_executable(04_textures 04_textures/textures.cpp
        ${include_dir}/glad/src/glad.c
        ${include_dir}/shader_m.h
        ${include_dir}/stb_image.h
        )

target_link_libraries(
        04_textures
        glfw
)

add_executable(05_transformations 05_Transformations/transformations.cpp
        ${include_dir}/glad/src/glad.c
        ${include_dir}/shader_m.h
        ${include_dir}/stb_image.h
        )

target_link_libraries(
        05_transformations
        glfw
        imgui
)

add_executable(06_coordinate_systems 06_coordinate_systems/coordinate_systems.cpp
        ${include_dir}/glad/src/glad.c
        ${include_dir}/shader_m.h
        ${include_dir}/stb_image.h
        )

target_link_libraries(
        06_coordinate_systems
        glfw
        imgui
)

add_executable(07_camera 07_camera/camera.cpp
        ${include_dir}/glad/src/glad.c
        ${include_dir}/shader_m.h
        ${include_dir}/stb_image.h
        ${include_dir}/camera.h
        )

target_link_libraries(
        07_camera
        glfw
        imgui
)

add_executable(08_colors 08_colors/colors.cpp
        ${include_dir}/glad/src/glad.c
        ${include_dir}/shader_m.h
        ${include_dir}/stb_image.h
        ${include_dir}/camera.h
        )

target_link_libraries(
        08_colors
        glfw
        imgui
)

add_executable(09_basic_lighting 09_basic_lighting/basic_lighting.cpp
        ${include_dir}/glad/src/glad.c
        ${include_dir}/shader_m.h
        ${include_dir}/stb_image.h
        ${include_dir}/camera.h
        )

target_link_libraries(
        09_basic_lighting
        glfw
        imgui
)

add_executable(10_materials 10_materials/materials.cpp
        ${include_dir}/glad/src/glad.c
        ${include_dir}/shader_m.h
        ${include_dir}/stb_image.h
        ${include_dir}/camera.h
        )

target_link_libraries(
        10_materials
        glfw
        imgui
)


add_executable(11_lighting_maps 11_lighting_maps/lighting_maps.cpp
        ${include_dir}/glad/src/glad.c
        ${include_dir}/shader_m.h
        ${include_dir}/stb_image.h
        ${include_dir}/camera.h
        )

target_link_libraries(
        11_lighting_maps
        glfw
        imgui
)

add_executable(12_light_casters 12_light_casters/light_casters.cpp
        ${include_dir}/glad/src/glad.c
        ${include_dir}/shader_m.h
        ${include_dir}/stb_image.h
        ${include_dir}/camera.h
        )

target_link_libraries(
        12_light_casters
        glfw
        imgui
)

add_executable(13_model 13_model/model_loading.cpp
        ${include_dir}/glad/src/glad.c
        ${include_dir}/shader_m.h
        ${include_dir}/stb_image.h
        ${include_dir}/camera.h
        ${include_dir}/mesh.h
        ${include_dir}/model.h
        )

target_link_libraries(
        13_model
        glfw
        imgui
        assimp::assimp
)