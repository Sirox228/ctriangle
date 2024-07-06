#ifndef DEPTH_IMAGE_H
#define DEPTH_IMAGE_H

#include <vulkan/vulkan.h>

void createDepthImage(VkImage* pDepthImage, VkDeviceMemory* pDepthImageMemory, uint8_t* pHaveStencil, VkImageView* pDepthImageView);

#endif
