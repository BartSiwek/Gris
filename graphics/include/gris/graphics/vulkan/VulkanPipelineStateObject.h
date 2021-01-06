﻿#pragma once

#include "VulkanDeviceResource.h"

class VulkanInputLayout;
class VulkanRenderPass;

class VulkanPipelineStateObject : public VulkanDeviceResource
{
public:
    VulkanPipelineStateObject(VulkanDevice* device, uint32_t swapChainWidth, uint32_t swapChainHeight, const VulkanRenderPass& renderPass, const VulkanInputLayout& inputLayout);

    // TODO: Do this better
    [[nodiscard]] const vk::DescriptorSetLayout& DescriptorSetLayoutHandle() const;
    [[nodiscard]] vk::DescriptorSetLayout& DescriptorSetLayoutHandle();

    // TODO: Do this better
    [[nodiscard]] const vk::PipelineLayout& PipelineLayoutHandle() const;
    [[nodiscard]] vk::PipelineLayout& PipelineLayoutHandle();

    // TODO: Do this better
    [[nodiscard]] const vk::Pipeline& GraphicsPipelineHandle() const;
    [[nodiscard]] vk::Pipeline& GraphicsPipelineHandle();

private:
    void CreateDescriptorSetLayout();

    void CreateGraphicsPipeline(uint32_t swapChainWidth, uint32_t swapChainHeight, const VulkanRenderPass& renderPass, const VulkanInputLayout& inputLayout);

    vk::UniqueDescriptorSetLayout m_descriptorSetLayout;
    vk::UniquePipelineLayout m_pipelineLayout;
    vk::UniquePipeline m_graphicsPipeline;
};
