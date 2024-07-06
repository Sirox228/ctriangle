#ifndef RENDER_H
#define RENDER_H

#include <vulkan/vulkan.h>

void recordCommands(VkCommandBuffer* pCmdBuf, uint32_t index, VkFramebuffer* pFramebuffer);
void render(VkFramebuffer* pSwapchainFramebuffers);

#endif
