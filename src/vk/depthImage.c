#include "holder.h"
#include "vkutil.h"

void createDepthImage(VkImage* pDepthImage, VkDeviceMemory* pDepthImageMemory, uint8_t* pHaveStencil, VkImageView* pDepthImageView) {
    VkFormat depthImageFormat = findFormat((VkFormat[]){VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    *pHaveStencil = depthImageFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || depthImageFormat == VK_FORMAT_D24_UNORM_S8_UINT;

    createImage(swapchainExtent.width, swapchainExtent.height, depthImageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_LAYOUT_UNDEFINED, pDepthImage, pDepthImageMemory);
    createImageView(*pDepthImage, depthImageFormat, VK_IMAGE_ASPECT_DEPTH_BIT, pDepthImageView);
}
