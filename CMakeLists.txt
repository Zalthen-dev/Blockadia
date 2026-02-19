cmake_minimum_required(VERSION 3.16)
project(blockadia LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

set(LUAU_BUILD_CLI OFF CACHE BOOL "" FORCE)
set(LUAU_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LUAU_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(LUAU_BUILD_WEB OFF CACHE BOOL "" FORCE)

find_package(raylib REQUIRED)
add_subdirectory(vendor/luau)

file(GLOB_RECURSE PROJECT_SRC src/*.cpp)
file(GLOB RAYLIB_SRC vendor/raylib/src/*.c)

add_executable(${PROJECT_NAME} ${PROJECT_SRC} ${RAYLIB_SRC})
target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/vendor/raylib/src
)

target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_DESKTOP)

if(UNIX)
	add_compile_definitions(_GLFW_X11)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE raylib Luau.Compiler Luau.VM)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Blockadia")