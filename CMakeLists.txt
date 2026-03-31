# 1. Set the minimum version and project name
cmake_minimum_required(VERSION 3.10)
project(Breakout)

# 2. Force the compiler to use modern C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Fix the yellow Arch Linux GLVND warning
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# 3. Find all your .cpp files in the src/ folder
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 4. Create the executable and compile ALL .cpp files + glad.c
add_executable(main ${SOURCES} src/glad.c)

# 5. Tell the compiler where to find your custom header files
# Whenever any file asks for an #include, immediately search the include/ folder first.
# This tells CMake to treat 'src' as the starting point for #includes.
target_include_directories(main PRIVATE include src)

# 6. Link the libraries (Keeping X11, dl, and pthread for Arch Linux!)
target_link_libraries(main glfw ${OPENGL_LIBRARIES} dl pthread X11)