#include "holder.h"
#include "shader.h"
#include "vertex.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

void createPipelineLayout(VkPipelineLayout* pPipelineLayout, VkDescriptorSetLayout* pSetLayouts, uint32_t setLayoutCount, VkPushConstantRange* pPushConstantRanges, uint32_t pushConstantRangeCount) {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = setLayoutCount;
    pipelineLayoutInfo.pSetLayouts = pSetLayouts;
    pipelineLayoutInfo.pushConstantRangeCount = pushConstantRangeCount;
    pipelineLayoutInfo.pPushConstantRanges = pPushConstantRanges;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, pPipelineLayout) != VK_SUCCESS) {
        printf("failed to create pipeline layout\n");
        exit(0);
    }
}

void createGraphicsPipeline(VkPipeline* pPipeline, VkPipelineLayout sourceLayout, char* vertPath, char* fragPath) {
    uint32_t vertSize, fragSize;
    char* vertCode = getFileContent(vertPath, &vertSize);
    char* fragCode = getFileContent(fragPath, &fragSize);
    VkShaderModule vert, frag;
    createShaderModule(&vert, vertCode, vertSize);
    createShaderModule(&frag, fragCode, fragSize);
    free(vertCode);
    free(fragCode);

    VkPipelineShaderStageCreateInfo fragStageInfo = {};
    fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStageInfo.module = frag;
    fragStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo vertStageInfo = {};
    vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStageInfo.module = vert;
    vertStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo stages[] = {vertStageInfo, fragStageInfo};

    VkVertexInputBindingDescription vertInpBindDesc = {};
    vertInpBindDesc.binding = 0;
    vertInpBindDesc.stride = sizeof(vertex);
    vertInpBindDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vertInpAttrDesc = {};
    vertInpAttrDesc.location = 0;
    vertInpAttrDesc.binding = 0;
    vertInpAttrDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
    vertInpAttrDesc.offset = offsetof(vertex, pos);

    VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = {};
    vertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateInfo.pVertexBindingDescriptions = &vertInpBindDesc;
    vertexInputStateInfo.vertexAttributeDescriptionCount = 1;
    vertexInputStateInfo.pVertexAttributeDescriptions = &vertInpAttrDesc;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo = {};
    inputAssemblyStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyStateInfo.primitiveRestartEnable = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo = {};
    depthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateInfo.depthTestEnable = VK_FALSE;
    depthStencilStateInfo.depthWriteEnable = VK_FALSE;
    depthStencilStateInfo.depthCompareOp = VK_COMPARE_OP_GREATER;
    depthStencilStateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateInfo.minDepthBounds = 0.0f;
    depthStencilStateInfo.maxDepthBounds = 1.0f;
    depthStencilStateInfo.stencilTestEnable = VK_FALSE;
    depthStencilStateInfo.front = (VkStencilOpState){};
    depthStencilStateInfo.back = (VkStencilOpState){};

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = swapchainExtent.width;
    viewport.height = swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = (VkOffset2D){0, 0};
    scissor.extent = swapchainExtent;

    VkPipelineViewportStateCreateInfo viewportStateInfo = {};
    viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateInfo.viewportCount = 1;
    viewportStateInfo.pViewports = &viewport;
    viewportStateInfo.scissorCount = 1;
    viewportStateInfo.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationStateInfo = {};
    rasterizationStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateInfo.cullMode = VK_CULL_MODE_NONE;
    rasterizationStateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationStateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateInfo.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisampleStateInfo = {};
    multisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleStateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateInfo.minSampleShading = 1.0f;
    multisampleStateInfo.pSampleMask = NULL;
    multisampleStateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleStateInfo.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState blendAttachment = {};
    blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    blendAttachment.blendEnable = VK_TRUE;
    blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo blendStateInfo = {};
    blendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blendStateInfo.logicOpEnable = VK_FALSE;
    blendStateInfo.logicOp = VK_LOGIC_OP_COPY;
    blendStateInfo.attachmentCount = 1;
    blendStateInfo.pAttachments = &blendAttachment;
    blendStateInfo.blendConstants[0] = 0.0f;
    blendStateInfo.blendConstants[1] = 0.0f;
    blendStateInfo.blendConstants[2] = 0.0f;
    blendStateInfo.blendConstants[3] = 0.0f;

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = stages;
    pipelineInfo.pVertexInputState = &vertexInputStateInfo;
    pipelineInfo.pInputAssemblyState = &inputAssemblyStateInfo;
    pipelineInfo.pViewportState = &viewportStateInfo;
    pipelineInfo.pRasterizationState = &rasterizationStateInfo;
    pipelineInfo.pMultisampleState = &multisampleStateInfo;
    pipelineInfo.pDepthStencilState = &depthStencilStateInfo;
    pipelineInfo.pColorBlendState = &blendStateInfo;
    pipelineInfo.pDynamicState = NULL;
    pipelineInfo.layout = sourceLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = NULL;

    if (vkCreateGraphicsPipelines(device, NULL, 1, &pipelineInfo, NULL, pPipeline) != VK_SUCCESS) {
        printf("failed to crrate graphics pipeline\n");
        exit(0);
    }

    vkDestroyShaderModule(device, frag, NULL);
    vkDestroyShaderModule(device, vert, NULL);
}
