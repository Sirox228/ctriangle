#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>
#include "vk/vertex.h"
#include "config.h"

SDL_Window* window;
uint8_t active = 1;
VkInstance instance;
VkSurfaceKHR surface;
VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
VkSurfaceCapabilitiesKHR surfaceCapabilities;
VkSurfaceFormatKHR surfaceFormat;
VkPresentModeKHR surfacePresentMode;
uint32_t queueFamilyIndex;
VkPhysicalDeviceMemoryProperties physicalDeviceMemProps;
VkDevice device;
VkQueue graphicsQueue;
VkExtent2D swapchainExtent;
VkSwapchainKHR swapchain;
VkRenderPass renderPass;
VkPipelineLayout graphicsPipelineLayout;
VkPipeline graphicsPipeline;
VkCommandPool mainCommandPool;
VkCommandPool shortCommandPool;
VkCommandBuffer swapchainCommandBuffers[MAX_FRAMES_IN_FLIGHT];
VkBuffer vertexBuffer;
VkDeviceMemory vertexBufferMemory;
VkBuffer indexBuffer;
VkDeviceMemory indexBufferMemory;
uint8_t depthImageHaveStencil;
VkImage depthImage;
VkDeviceMemory depthImageMemory;
VkImageView depthImageView;
VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
VkFence inFlightFences[MAX_FRAMES_IN_FLIGHT];
uint32_t frame = 0;


#define VERT_NUM 3
vertex vertBuf[VERT_NUM] = {
    {{0.0f, -0.5f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}},
    {{-0.5f, 0.5f, 0.0f}}
};
#define IDX_NUM 3
uint32_t idxBuf[IDX_NUM] = {
    0, 1, 2
};
