#include <gris/graphics/vulkan/pipeline_state_object.h>

#include <gris/graphics/vulkan/device.h>
#include <gris/graphics/vulkan/input_layout.h>
#include <gris/graphics/vulkan/render_pass.h>
#include <gris/graphics/vulkan/shader.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::PipelineStateObject::PipelineStateObject(
         Device * device,
         uint32_t swapChainWidth,
         uint32_t swapChainHeight,
         const RenderPass & renderPass,
         const InputLayout & inputLayout,
         const Shader & vertexShader,
         const Shader & fragmentShader)
    : DeviceResource(device)
{
    CreateDescriptorSetLayout();
    CreateGraphicsPipeline(swapChainWidth, swapChainHeight, renderPass, inputLayout, vertexShader, fragmentShader);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::DescriptorSetLayout & Gris::Graphics::Vulkan::PipelineStateObject::DescriptorSetLayoutHandle() const
{
    return m_descriptorSetLayout.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::DescriptorSetLayout & Gris::Graphics::Vulkan::PipelineStateObject::DescriptorSetLayoutHandle()
{
    return m_descriptorSetLayout.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::PipelineLayout & Gris::Graphics::Vulkan::PipelineStateObject::PipelineLayoutHandle() const
{
    return m_pipelineLayout.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::PipelineLayout & Gris::Graphics::Vulkan::PipelineStateObject::PipelineLayoutHandle()
{
    return m_pipelineLayout.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::Pipeline & Gris::Graphics::Vulkan::PipelineStateObject::GraphicsPipelineHandle() const
{
    return m_graphicsPipeline.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::Pipeline & Gris::Graphics::Vulkan::PipelineStateObject::GraphicsPipelineHandle()
{
    return m_graphicsPipeline.get();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::PipelineStateObject::CreateDescriptorSetLayout()
{
    auto const uboLayoutBinding = vk::DescriptorSetLayoutBinding(0,
                                                                 vk::DescriptorType::eUniformBuffer,
                                                                 1,
                                                                 vk::ShaderStageFlagBits::eVertex,
                                                                 nullptr);

    auto const samplerLayoutBinding = vk::DescriptorSetLayoutBinding(1,
                                                                     vk::DescriptorType::eCombinedImageSampler,
                                                                     1,
                                                                     vk::ShaderStageFlagBits::eFragment,
                                                                     nullptr);

    auto const bindings = std::array{ uboLayoutBinding, samplerLayoutBinding };
    auto const layoutInfo = vk::DescriptorSetLayoutCreateInfo{}.setBindings(bindings);

    auto createDescriptorSetLayoutResult = DeviceHandle().createDescriptorSetLayoutUnique(layoutInfo);
    if (createDescriptorSetLayoutResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating descriptor set layout", createDescriptorSetLayoutResult);
    }

    m_descriptorSetLayout = std::move(createDescriptorSetLayoutResult.value);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::PipelineStateObject::CreateGraphicsPipeline(
         uint32_t swapChainWidth,
         uint32_t swapChainHeight,
         const RenderPass & renderPass,
         const InputLayout & inputLayout,
         const Shader & vertexShader,
         const Shader & fragmentShader)
{
    m_graphicsPipeline.reset();
    m_pipelineLayout.reset();

    auto const shaderStages = std::array{
        vk::PipelineShaderStageCreateInfo{}
                 .setStage(vk::ShaderStageFlagBits::eVertex)
                 .setModule(vertexShader.ModuleHandle())
                 .setPName("main"),
        vk::PipelineShaderStageCreateInfo{}
                 .setStage(vk::ShaderStageFlagBits::eFragment)
                 .setModule(fragmentShader.ModuleHandle())
                 .setPName("main"),
    };

    auto const & bindingDescriptors = inputLayout.BindingDescription();
    auto const & attributeDescriptors = inputLayout.AttributeDescriptions();
    auto const vertexInputInfo = vk::PipelineVertexInputStateCreateInfo{}
                                          .setVertexBindingDescriptions(bindingDescriptors)
                                          .setVertexAttributeDescriptions(attributeDescriptors);

    auto const inputAssembly = vk::PipelineInputAssemblyStateCreateInfo{}
                                        .setTopology(vk::PrimitiveTopology::eTriangleList)
                                        .setPrimitiveRestartEnable(static_cast<vk::Bool32>(false));

    auto const viewports = std::array{
        vk::Viewport{}
                 .setX(0.0F)
                 .setY(0.0F)
                 .setWidth(static_cast<float>(swapChainWidth))
                 .setHeight(static_cast<float>(swapChainHeight))
                 .setMinDepth(0.0F)
                 .setMaxDepth(1.0F)
    };
    auto const scissors = std::array{
        vk::Rect2D{}
                 .setOffset({ 0, 0 })
                 .setExtent({ swapChainWidth, swapChainHeight })
    };
    auto const viewportState = vk::PipelineViewportStateCreateInfo{}
                                        .setViewports(viewports)
                                        .setScissors(scissors);

    auto const rasterizer = vk::PipelineRasterizationStateCreateInfo{}
                                     .setDepthClampEnable(static_cast<vk::Bool32>(false))
                                     .setRasterizerDiscardEnable(static_cast<vk::Bool32>(false))
                                     .setPolygonMode(vk::PolygonMode::eFill)
                                     .setCullMode(vk::CullModeFlagBits::eBack)
                                     .setFrontFace(vk::FrontFace::eCounterClockwise)
                                     .setDepthBiasEnable(static_cast<vk::Bool32>(false))
                                     .setDepthBiasConstantFactor(0.0F)
                                     .setDepthBiasClamp(0.0F)
                                     .setDepthBiasSlopeFactor(0.0F)
                                     .setLineWidth(1.0F);

    auto const multisampleInfo = vk::PipelineMultisampleStateCreateInfo{}
                                          .setRasterizationSamples(ParentDevice().MsaaSamples())
                                          .setSampleShadingEnable(static_cast<vk::Bool32>(false));

    auto const depthStencil = vk::PipelineDepthStencilStateCreateInfo{}
                                       .setDepthTestEnable(true)
                                       .setDepthWriteEnable(true)
                                       .setDepthCompareOp(vk::CompareOp::eLess)
                                       .setStencilTestEnable(false);

    auto const colorBlendAttachments = std::array
    {
        vk::PipelineColorBlendAttachmentState{}
                 .setBlendEnable(static_cast<vk::Bool32>(false))
                 .setSrcColorBlendFactor(vk::BlendFactor::eZero)
                 .setDstColorBlendFactor(vk::BlendFactor::eZero)
                 .setColorBlendOp(vk::BlendOp::eAdd)
                 .setSrcAlphaBlendFactor(vk::BlendFactor::eZero)
                 .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
                 .setAlphaBlendOp(vk::BlendOp::eAdd)
                 .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
    };

    auto const colorBlending = vk::PipelineColorBlendStateCreateInfo{}
                                        .setLogicOpEnable(static_cast<vk::Bool32>(false))
                                        .setLogicOp(vk::LogicOp::eCopy)
                                        .setAttachments(colorBlendAttachments)
                                        .setBlendConstants({ 0.0F, 0.0F, 0.0F, 0.0F });

    auto const descriptorSetLayouts = std::array{ m_descriptorSetLayout.get() };
    auto const pipelineLayoutInfo = vk::PipelineLayoutCreateInfo{}.setSetLayouts(descriptorSetLayouts);

    auto createPipelineLayoutResult = DeviceHandle().createPipelineLayoutUnique(pipelineLayoutInfo);
    if (createPipelineLayoutResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating pipeline layout", createPipelineLayoutResult);
    }

    m_pipelineLayout = std::move(createPipelineLayoutResult.value);

    auto const pipelineInfo = vk::GraphicsPipelineCreateInfo{}
                                       .setStages(shaderStages)
                                       .setPVertexInputState(&vertexInputInfo)
                                       .setPInputAssemblyState(&inputAssembly)
                                       .setPTessellationState({})
                                       .setPViewportState(&viewportState)
                                       .setPRasterizationState(&rasterizer)
                                       .setPMultisampleState(&multisampleInfo)
                                       .setPDepthStencilState(&depthStencil)
                                       .setPColorBlendState(&colorBlending)
                                       .setPDynamicState({})
                                       .setLayout(m_pipelineLayout.get())
                                       .setRenderPass(renderPass.RenderPassHandle())
                                       .setSubpass(0);

    auto createGraphicsPipelineResult = DeviceHandle().createGraphicsPipelineUnique({}, pipelineInfo);
    if (createGraphicsPipelineResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating graphics pipeline", createGraphicsPipelineResult);
    }

    m_graphicsPipeline = std::move(createGraphicsPipelineResult.value);
}
