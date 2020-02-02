cmake_minimum_required(VERSION 3.15)
project(Vulkan_Try)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-std=c++17 -lvulkan -lglfw")

add_executable(Vulkan_Try main.cpp base/window/glfw/GLFWWindowManager.h Application.cpp Application.h base/vulkan/VulkanHandler.cpp base/vulkan/VulkanHandler.h base/window/glfw/GLFWWindowManager.cpp base/vulkan/VulkanHelper.h base/window/WindowManager.cpp base/window/WindowManager.h base/vulkan/VulkanDevice.cpp base/vulkan/VulkanDevice.h base/vulkan/VulkanSwapChain.cpp base/vulkan/VulkanSwapChain.h base/vulkan/VulkanShader.h base/vulkan/VulkanDefs.h)