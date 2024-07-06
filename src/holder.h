#ifndef HOLDER_H
#define HOLDER_H

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>
#include "vk/vertex.h"
#include "config.h"

extern SDL_Window* window;
extern uint8_t active;
extern VkInstance instance;
extern VkSurfaceKHR surface;
extern VkPhysicalDevice physicalDevice;
extern VkSurfaceCapabilitiesKHR surfaceCapabilities;
extern VkSurfaceFormatKHR surfaceFormat;
extern VkPresentModeKHR surfacePresentMode;
extern uint32_t queueFamilyIndex;
extern VkPhysicalDeviceMemoryProperties physicalDeviceMemProps;
extern VkDevice device;
extern VkQueue graphicsQueue;
extern VkExtent2D swapchainExtent;
extern VkSwapchainKHR swapchain;
extern VkRenderPass renderPass;
extern VkPipelineLayout graphicsPipelineLayout;
extern VkPipeline graphicsPipeline;
extern VkCommandPool mainCommandPool;
extern VkCommandPool shortCommandPool;
extern VkCommandBuffer swapchainCommandBuffers[MAX_FRAMES_IN_FLIGHT];
extern VkBuffer vertexBuffer;
extern VkDeviceMemory vertexBufferMemory;
extern VkBuffer indexBuffer;
extern VkDeviceMemory indexBufferMemory;
extern uint8_t depthImageHaveStencil;
extern VkImage depthImage;
extern VkDeviceMemory depthImageMemory;
extern VkImageView depthImageView;
extern VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
extern VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
extern VkFence inFlightFences[MAX_FRAMES_IN_FLIGHT];
extern uint32_t frame;


#define VERT_NUM 3
extern vertex vertBuf[VERT_NUM];
#define IDX_NUM 3
extern uint32_t idxBuf[IDX_NUM];

#endif
