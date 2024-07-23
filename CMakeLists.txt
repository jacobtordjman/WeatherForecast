cmake_minimum_required(VERSION 3.10)
project(WeatherForecast)

set(CMAKE_CXX_STANDARD 11)

# Include directories
include_directories(include)
include_directories(include/imgui)
include_directories(include/imgui/backends)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    include/imgui/imgui.cpp
    include/imgui/imgui_demo.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/backends/imgui_impl_glfw.cpp
    include/imgui/backends/imgui_impl_opengl3.cpp
)

# Create the executable
add_executable(WeatherForecast ${SOURCES})

# Link libraries
target_link_libraries(WeatherForecast
    OpenGL::GL
    glfw
    GLEW::GLEW
    ${GLFW_LIBRARIES}
)
