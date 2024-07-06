#include "holder.h"
#include "commandBuffer.h"
#include <stdio.h>
#include <stdlib.h>

VkFormat findFormat(VkFormat* formats, uint32_t formatCount, VkImageTiling tiling, VkFormatFeatureFlags features) {
    for (int i = 0; i < formatCount; i++) {
        VkFormatProperties props;

        vkGetPhysicalDeviceFormatProperties(physicalDevice, formats[i], &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            return formats[i];
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            return formats[i];
    }
    return formats[0];
}

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    for (uint32_t i = 0; i < physicalDeviceMemProps.memoryTypeCount; i++) {
        if (typeFilter & (1 << i) && (physicalDeviceMemProps.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    printf("failed to find required memory type. type %d properties %d\n", typeFilter, properties);
    exit(0);
}

void createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect, VkImageView* pView) {
    VkImageViewCreateInfo imageViewInfo = {};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.image = image;
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = format;
    imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.subresourceRange.aspectMask = aspect;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &imageViewInfo, NULL, pView) != VK_SUCCESS) {
        printf("failed to create image view\n");
        exit(0);
    }
}

void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* pBuffer, VkDeviceMemory* pBufferMemory) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, NULL, pBuffer) != VK_SUCCESS) {
        printf("failed to create buffer\n");
        exit(0);
    }

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(device, *pBuffer, &memReq);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, NULL, pBufferMemory) != VK_SUCCESS) {
        printf("failed to allocate buffer memory\n");
        exit(0);
    }

    vkBindBufferMemory(device, *pBuffer, *pBufferMemory, 0);
}

void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size) {
    VkCommandBuffer cmdBuf = getSingleTimeCmdBuf();

    VkBufferCopy copyInfo = {};
    copyInfo.srcOffset = 0;
    copyInfo.dstOffset = 0;
    copyInfo.size = size;

    vkCmdCopyBuffer(cmdBuf, src, dst, 1, &copyInfo);

    submitSingleTimeCmdBuf(&cmdBuf);
}

void createAndFillBufferFromStaging(VkDeviceSize size, void* data, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* pBuffer, VkDeviceMemory* pBufferMemory) {
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMem;

    createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMem);

    void* stagingBufferMemMapped;
    vkMapMemory(device, stagingBufferMem, 0, size, 0, &stagingBufferMemMapped);
    memcpy(stagingBufferMemMapped, data, size);
    vkUnmapMemory(device,  stagingBufferMem);

    createBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, properties, pBuffer, pBufferMemory);

    copyBuffer(stagingBuffer, *pBuffer, size);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMem, NULL);
}

void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageLayout layout, VkImage* pImage, VkDeviceMemory* pImageMemory) {
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = format;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = tiling;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = layout;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;

    if (vkCreateImage(device, &imageInfo, NULL, pImage) != VK_SUCCESS) {
        printf("failed to create image\n");
        exit(0);
    }

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(device, *pImage, &memReq);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, NULL, pImageMemory) != VK_SUCCESS) {
        printf("failed to allocate image memory\n");
        exit(0);
    }

    vkBindImageMemory(device, *pImage, *pImageMemory, 0);
}
