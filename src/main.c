#include <SDL2/SDL.h>
#include "holder.h"
#include "config.h"
#include "vk/instance.h"
#include "vk/surface.h"
#include "vk/physicalDevice.h"
#include "vk/device.h"
#include "vk/swapchain.h"
#include "vk/renderPass.h"
#include "vk/pipeline.h"
#include "vk/commandPool.h"
#include "vk/commandBuffer.h"
#include "vk/depthImage.h"
#include "vk/framebuffer.h"
#include "vk/sync.h"
#include "vk/render.h"
#include "vk/vkutil.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    SDL_Init(SDL_INIT_EVENTS);
    window = SDL_CreateWindow("game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == NULL) {
        printf("failed to create sdl window\n");
        exit(0);
    }

    createInstance(&instance);
    createSurface(&surface);
    findPhysicalDevice(&physicalDevice, &surfaceFormat, &surfacePresentMode, &surfaceCapabilities);
    createDevice(&device, physicalDevice);
    vkGetDeviceQueue(device, queueFamilyIndex, 0, &graphicsQueue);
    createSwapchain(&swapchain, surface);

    uint32_t swapchainImageCount;
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, NULL);
    VkImage swapchainImages[swapchainImageCount];
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages);

    VkImageView swapchainImageViews[swapchainImageCount];
    for (uint32_t i = 0; i < swapchainImageCount; i++)
        createImageView(swapchainImages[i], surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, &swapchainImageViews[i]);

    createRenderPass(&renderPass);
    createPipelineLayout(&graphicsPipelineLayout, NULL, 0, NULL, 0);
    createGraphicsPipeline(&graphicsPipeline, graphicsPipelineLayout, "shaders/vert.spv", "shaders/frag.spv");
    createCommandPool(&mainCommandPool, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    createCommandPool(&shortCommandPool, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);

    createSwapchainCommandBuffers(swapchainCommandBuffers, mainCommandPool);

    createDepthImage(&depthImage, &depthImageMemory, &depthImageHaveStencil, &depthImageView);

    VkFramebuffer swapchainFramebuffers[swapchainImageCount];
    for (uint32_t i = 0; i < swapchainImageCount; i++)
        createFramebuffer((VkImageView[]){swapchainImageViews[i], depthImageView}, 2, renderPass, swapchainExtent, &swapchainFramebuffers[i]);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createSemaphore(&imageAvailableSemaphores[i]);
        createSemaphore(&renderFinishedSemaphores[i]);
        createFence(1, &inFlightFences[i]);
    }

    createAndFillBufferFromStaging(sizeof(vertex) * VERT_NUM, vertBuf, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertexBuffer, &vertexBufferMemory);
    createAndFillBufferFromStaging(sizeof(uint32_t) * IDX_NUM, idxBuf, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &indexBuffer, &indexBufferMemory);

    SDL_Event event;
    while (active) {
        while (SDL_PollEvent(&event)) {
    		if (event.type == SDL_QUIT) {
    			active = 0;
    			break;
    		}
        }

        render(swapchainFramebuffers);
        frame = (frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    vkDeviceWaitIdle(device);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyFence(device, inFlightFences[i], NULL);
        vkDestroySemaphore(device, renderFinishedSemaphores[i], NULL);
        vkDestroySemaphore(device, imageAvailableSemaphores[i], NULL);
    }
    for (uint32_t i = 0; i < swapchainImageCount; i++)
        vkDestroyFramebuffer(device, swapchainFramebuffers[i], NULL);
    vkDestroyBuffer(device, indexBuffer, NULL);
    vkFreeMemory(device, indexBufferMemory, NULL);
    vkDestroyBuffer(device, vertexBuffer, NULL);
    vkFreeMemory(device, vertexBufferMemory, NULL);
    vkDestroyImageView(device, depthImageView, NULL);
    vkDestroyImage(device, depthImage, NULL);
    vkFreeMemory(device, depthImageMemory, NULL);
    vkDestroyCommandPool(device, shortCommandPool, NULL);
    vkDestroyCommandPool(device, mainCommandPool, NULL);
    vkDestroyPipeline(device, graphicsPipeline, NULL);
    vkDestroyPipelineLayout(device, graphicsPipelineLayout, NULL);
    vkDestroyRenderPass(device, renderPass, NULL);
    for (uint32_t i = 0; i < swapchainImageCount; i++)
        vkDestroyImageView(device, swapchainImageViews[i], NULL);
    vkDestroySwapchainKHR(device, swapchain, NULL);
    vkDestroyDevice(device, NULL);
    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyInstance(instance, NULL);

    SDL_DestroyWindow(window);
    SDL_Quit();
}
