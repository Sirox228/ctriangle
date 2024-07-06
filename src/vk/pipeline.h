#ifndef PIPELINE_H
#define PIPELINE_H

#include <vulkan/vulkan.h>

void createPipelineLayout(VkPipelineLayout* pPipelineLayout, VkDescriptorSetLayout* pSetLayouts, uint32_t setLayoutCount, VkPushConstantRange* pPushConstantRanges, uint32_t pushConstantRangeCount);
void createGraphicsPipeline(VkPipeline* pPipeline, VkPipelineLayout sourceLayout, char* vertPath, char* fragPath);

#endif
