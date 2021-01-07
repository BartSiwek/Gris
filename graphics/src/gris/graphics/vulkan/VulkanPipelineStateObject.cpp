﻿#include <gris/graphics/vulkan/VulkanPipelineStateObject.h>

#include <gris/graphics/vulkan/VulkanRenderPass.h>
#include <gris/graphics/vulkan/VulkanShader.h>
#include <gris/graphics/vulkan/VulkanInputLayout.h>
#include <gris/graphics/vulkan/VulkanDevice.h>
#include <gris/graphics/vulkan/VulkanEngineException.h>

// -------------------------------------------------------------------------------------------------

namespace
{

const std::string VERTEX_SHADER_PATH = "vert.spv";
const std::string FRAGMENT_SHADER_PATH = "frag.spv";

}  // namespace <anonymous>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanPipelineStateObject::VulkanPipelineStateObject(VulkanDevice* device, uint32_t swapChainWidth, uint32_t swapChainHeight,
                                                                             const VulkanRenderPass& renderPass, const VulkanInputLayout& inputLayout)
    : VulkanDeviceResource(device)
{
    CreateDescriptorSetLayout();
    CreateGraphicsPipeline(swapChainWidth, swapChainHeight, renderPass, inputLayout);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::DescriptorSetLayout& Gris::Graphics::Vulkan::VulkanPipelineStateObject::DescriptorSetLayoutHandle() const
{
    return m_descriptorSetLayout.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::DescriptorSetLayout& Gris::Graphics::Vulkan::VulkanPipelineStateObject::DescriptorSetLayoutHandle()
{
    return m_descriptorSetLayout.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::PipelineLayout& Gris::Graphics::Vulkan::VulkanPipelineStateObject::PipelineLayoutHandle() const
{
    return m_pipelineLayout.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::PipelineLayout& Gris::Graphics::Vulkan::VulkanPipelineStateObject::PipelineLayoutHandle()
{
    return m_pipelineLayout.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::Pipeline& Gris::Graphics::Vulkan::VulkanPipelineStateObject::GraphicsPipelineHandle() const
{
    return m_graphicsPipeline.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::Pipeline& Gris::Graphics::Vulkan::VulkanPipelineStateObject::GraphicsPipelineHandle()
{
    return m_graphicsPipeline.get();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanPipelineStateObject::CreateDescriptorSetLayout()
{
    auto const uboLayoutBinding = vk::DescriptorSetLayoutBinding(
        0,
        vk::DescriptorType::eUniformBuffer,
        1,
        vk::ShaderStageFlagBits::eVertex,
        nullptr
    );

    auto const samplerLayoutBinding = vk::DescriptorSetLayoutBinding(
        1,
        vk::DescriptorType::eCombinedImageSampler,
        1,
        vk::ShaderStageFlagBits::eFragment,
        nullptr
    );

    auto const bindings = std::array{ uboLayoutBinding, samplerLayoutBinding };
    auto const layoutInfo = vk::DescriptorSetLayoutCreateInfo({}, bindings);

    auto createDescriptorSetLayoutResult = DeviceHandle().createDescriptorSetLayoutUnique(layoutInfo);
    if (createDescriptorSetLayoutResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating descriptor set layout", createDescriptorSetLayoutResult);

    m_descriptorSetLayout = std::move(createDescriptorSetLayoutResult.value);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanPipelineStateObject::CreateGraphicsPipeline(uint32_t swapChainWidth, uint32_t swapChainHeight, const VulkanRenderPass& renderPass, const VulkanInputLayout & inputLayout)
{
    m_graphicsPipeline.reset();
    m_pipelineLayout.reset();

    auto vertexShader = ParentDevice().CreateShader(ReadFile(VERTEX_SHADER_PATH));
    auto fragmentShader = ParentDevice().CreateShader(ReadFile(FRAGMENT_SHADER_PATH));

    auto const shaderStages = std::array{
        vk::PipelineShaderStageCreateInfo(
            {},
            vk::ShaderStageFlagBits::eVertex,
            vertexShader.ModuleHandle(),
            "main"
        ),
        vk::PipelineShaderStageCreateInfo(
            {},
            vk::ShaderStageFlagBits::eFragment,
            fragmentShader.ModuleHandle(),
            "main"
        )
    };

    auto const& bindingDescriptors = inputLayout.BindingDescription();
    auto const& attributeDescriptors = inputLayout.AttributeDescriptions();
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo({}, bindingDescriptors, attributeDescriptors);

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly({}, vk::PrimitiveTopology::eTriangleList, false);

    auto const viewports = std::array{
        vk::Viewport(
            0.0f,
            0.0f,
            static_cast<float>(swapChainWidth),
            static_cast<float>(swapChainHeight),
            0.0f,
            1.0f
        )
    };
    auto const scissors = std::array{
        vk::Rect2D(
            { 0, 0 },
            { swapChainWidth, swapChainHeight }
        )
    };
    auto const viewportState = vk::PipelineViewportStateCreateInfo({}, viewports, scissors);

    auto const rasterizer = vk::PipelineRasterizationStateCreateInfo(
        {},
        false,
        false,
        vk::PolygonMode::eFill,
        vk::CullModeFlagBits::eBack,
        vk::FrontFace::eCounterClockwise,
        false,
        0.0f,
        0.0f,
        0.0f,
        1.0f
    );

    auto const multisampleInfo = vk::PipelineMultisampleStateCreateInfo({}, ParentDevice().MsaaSamples(), false);

    auto const depthStencil = vk::PipelineDepthStencilStateCreateInfo(
        {},
        true,
        true,
        vk::CompareOp::eLess,
        false,
        false
    );

    auto const colorBlendAttachments = std::array{
        vk::PipelineColorBlendAttachmentState(
            false,
            vk::BlendFactor::eZero,
            vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            vk::BlendFactor::eZero,
            vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
        )
    };

    auto const colorBlending = vk::PipelineColorBlendStateCreateInfo(
        {},
        false,
        vk::LogicOp::eCopy,
        colorBlendAttachments,
        { 0.0f, 0.0f, 0.0f, 0.0f }
    );

    auto const descriptorSetLayouts = std::array{ m_descriptorSetLayout.get() };
    auto const pipelineLayoutInfo = vk::PipelineLayoutCreateInfo({}, descriptorSetLayouts);

    auto createPipelineLayoutResult = DeviceHandle().createPipelineLayoutUnique(pipelineLayoutInfo);
    if (createPipelineLayoutResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating pipeline layout", createPipelineLayoutResult);

    m_pipelineLayout = std::move(createPipelineLayoutResult.value);

    auto const pipelineInfo = vk::GraphicsPipelineCreateInfo(
        {},
        shaderStages,
        &vertexInputInfo,
        &inputAssembly,
        {},
        &viewportState,
        &rasterizer,
        &multisampleInfo,
        &depthStencil,
        &colorBlending,
        {},
        m_pipelineLayout.get(),
        renderPass.RenderPassHandle(),
        0,
        {},
        0
    );

    auto createGraphicsPipelineResult = DeviceHandle().createGraphicsPipelineUnique({}, pipelineInfo);
    if (createGraphicsPipelineResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating graphics pipeline", createGraphicsPipelineResult);

    m_graphicsPipeline = std::move(createGraphicsPipelineResult.value);
}
