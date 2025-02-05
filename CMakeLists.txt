cmake_minimum_required(VERSION 3.20)
project(ctriangle LANGUAGES C VERSION 1.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(VALIDATION "enabled validation layer" ON)
option(VK_NO_PROTOTYPES "enable VK_NO_PROTOTYPES" OFF)

if(VALIDATION)
    add_compile_definitions(VALIDATION)
endif()

if(VK_NO_PROTOTYPES)
    add_compile_definitions(VK_NO_PROTOTYPES)
endif()

add_compile_definitions(CGLM_FORCE_DEPTH_ZERO_TO_ONE)

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/main.c
    ${CMAKE_CURRENT_LIST_DIR}/src/holder.c
    ${CMAKE_CURRENT_LIST_DIR}/src/util.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vk/instance.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vk/surface.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vk/vkutil.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vk/physicalDevice.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vk/device.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vk/swapchain.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vk/renderPass.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vk/shader.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vk/pipeline.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vk/commandPool.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vk/commandBuffer.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vk/depthImage.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vk/framebuffer.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vk/sync.c
    ${CMAKE_CURRENT_LIST_DIR}/src/vk/render.c
)

find_package(SDL2 REQUIRED)
find_package(cglm REQUIRED)

add_executable(ctriangle ${SOURCES})
target_link_libraries(ctriangle ${SDL2_LIBRARIES} cglm::cglm)
if(NOT VK_NO_PROTOTYPES)
    find_library(vklib NAMES vulkan vulkan-1)
    target_link_libraries(ctriangle ${vklib})
endif()
target_include_directories(ctriangle PRIVATE src)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
