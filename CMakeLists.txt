cmake_minimum_required(VERSION 3.15)
project(vulkan-hello-world)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17 -lvulkan -lglfw -O2")

file(GLOB BASE_SRC "base/vulkan/*.h" "base/vulkan/*.cpp" "base/window/*.h" "base/window/*.cpp" "base/window/glfw/*.h" "base/window/glfw/*.cpp" "*.h" "*.cpp")
#file(GLOB IMGUI_SRC "imgui/*.h" "imgui/*.cpp" "thirdparty/imgui/*.cpp" "thirdparty/imgui/*.h")

add_executable(vulkan-hello-world "${BASE_SRC}" model/Car.cpp model/Car.h model/Human.cpp model/Human.h)