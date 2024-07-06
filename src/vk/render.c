#include "holder.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

void recordCommands(VkCommandBuffer* pCmdBuf, uint32_t index, VkFramebuffer* pFramebuffer) {
    VkCommandBuffer cmdBuf = *pCmdBuf;

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(cmdBuf, &beginInfo) != VK_SUCCESS) {
        printf("failed to begin command buffer\n");
        exit(0);
    }

    VkClearValue clearColorValue;
    clearColorValue.color = (VkClearColorValue){0.0f, 0.0f, 0.0f, 1.0f};
    VkClearValue clearDepthValue;
    clearDepthValue.depthStencil = (VkClearDepthStencilValue){0.0f, 0.0f};
    VkClearValue clearValues[2] = {clearColorValue, clearDepthValue};

    VkRenderPassBeginInfo rpBeginInfo = {};
    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass = renderPass;
    rpBeginInfo.framebuffer = *pFramebuffer;
    rpBeginInfo.renderArea.offset = (VkOffset2D){0, 0};
    rpBeginInfo.renderArea.extent = swapchainExtent;
    rpBeginInfo.clearValueCount = 2;
    rpBeginInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    VkDeviceSize vertexBufferOffsets[1] = {0};
    vkCmdBindVertexBuffers(cmdBuf, 0, 1, &vertexBuffer, vertexBufferOffsets);
    vkCmdBindIndexBuffer(cmdBuf, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(cmdBuf, IDX_NUM, 1, 0, 0, 0);

    vkCmdEndRenderPass(cmdBuf);

    if (vkEndCommandBuffer(cmdBuf) != VK_SUCCESS) {
        printf("failed to end command buffer\n");
        exit(0);
    }
}

void render(VkFramebuffer* pSwapchainFramebuffers) {
    vkWaitForFences(device, 1, &inFlightFences[frame], VK_TRUE, UINT64_MAX);

    uint32_t index;
    vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphores[frame], NULL, &index);

    vkResetFences(device, 1, &inFlightFences[frame]);

    vkResetCommandBuffer(swapchainCommandBuffers[frame], 0);

    recordCommands(&swapchainCommandBuffers[frame], index, &pSwapchainFramebuffers[index]);

    VkSemaphore waitSems[1] = {imageAvailableSemaphores[frame]};
    VkSemaphore signalSems[1] = {renderFinishedSemaphores[frame]};
    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSems;
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &swapchainCommandBuffers[frame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSems;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[frame]) != VK_SUCCESS) {
        printf("failed to submit rendering\n");
        exit(0);
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSems;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &index;

    vkQueuePresentKHR(graphicsQueue, &presentInfo);
}

