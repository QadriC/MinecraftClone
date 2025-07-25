cmake_minimum_required(VERSION 3.10)
project(MinecraftClone)

set(CMAKE_CXX_STANDARD 17)

# Check memory leaks
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer -g -O1")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")


# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
add_executable(MinecraftClone ${SOURCES})

# Include headers
target_include_directories(MinecraftClone PRIVATE include include/stb)

# Search OpenGL
cmake_policy(SET CMP0072 NEW)
find_package(OpenGL REQUIRED)

# Search GLFW using pkg-config if find_package fails
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

target_include_directories(MinecraftClone PRIVATE ${GLFW_INCLUDE_DIRS})
target_link_libraries(MinecraftClone ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})

add_custom_command(TARGET MinecraftClone POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:MinecraftClone>/shaders
)

add_custom_command(TARGET MinecraftClone POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/textures
        $<TARGET_FILE_DIR:MinecraftClone>/textures
)
