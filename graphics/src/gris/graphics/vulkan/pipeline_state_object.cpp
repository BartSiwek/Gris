#include <gris/graphics/vulkan/pipeline_state_object.h>

#include <gris/graphics/vulkan/device.h>
#include <gris/graphics/vulkan/input_layout.h>
#include <gris/graphics/vulkan/render_pass.h>
#include <gris/graphics/vulkan/shader.h>
#include <gris/graphics/vulkan/shader_resource_bindings_layout.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::PipelineStateObject::PipelineStateObject() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::PipelineStateObject::PipelineStateObject(
    const ParentObject<Device> & device,
    uint32_t swapChainWidth,
    uint32_t swapChainHeight,
    const RenderPass & renderPass,
    const InputLayout & inputLayout,
    const ShaderResourceBindingsLayout & resourceLayout,
    const Shader & vertexShader,
    const Shader & fragmentShader)
    : DeviceResource(device)
{
    auto const shaderStages = std::array{
        vk::PipelineShaderStageCreateInfo{}
            .setStage(vk::ShaderStageFlagBits::eVertex)
            .setModule(vertexShader.ModuleHandle())
            .setPName(vertexShader.EntryPoint().c_str()),
        vk::PipelineShaderStageCreateInfo{}
            .setStage(vk::ShaderStageFlagBits::eFragment)
            .setModule(fragmentShader.ModuleHandle())
            .setPName(fragmentShader.EntryPoint().c_str()),
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
                                  .setDepthTestEnable(static_cast<vk::Bool32>(true))
                                  .setDepthWriteEnable(static_cast<vk::Bool32>(true))
                                  .setDepthCompareOp(vk::CompareOp::eLess)
                                  .setStencilTestEnable(static_cast<vk::Bool32>(false));

    auto const colorBlendAttachments = std::array{
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

    auto const descriptorSetLayouts = std::array{ resourceLayout.DescriptorSetLayoutHandle() };
    auto const pipelineLayoutInfo = vk::PipelineLayoutCreateInfo{}.setSetLayouts(descriptorSetLayouts);

    auto createPipelineLayoutResult = DeviceHandle().createPipelineLayout(pipelineLayoutInfo, nullptr, Dispatch());
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
                                  .setLayout(m_pipelineLayout)
                                  .setRenderPass(renderPass.RenderPassHandle())
                                  .setSubpass(0);

    auto createGraphicsPipelineResult = DeviceHandle().createGraphicsPipeline({}, pipelineInfo, nullptr, Dispatch());
    if (createGraphicsPipelineResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating graphics pipeline", createGraphicsPipelineResult);
    }

    m_graphicsPipeline = std::move(createGraphicsPipelineResult.value);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::PipelineStateObject::PipelineStateObject(PipelineStateObject && other) noexcept
    : DeviceResource(std::move(other))
    , m_pipelineLayout(std::exchange(other.m_pipelineLayout, {}))
    , m_graphicsPipeline(std::exchange(other.m_graphicsPipeline, {}))

{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::PipelineStateObject & Gris::Graphics::Vulkan::PipelineStateObject::operator=(PipelineStateObject && other) noexcept
{
    if (this != &other)
    {
        Reset();

        DeviceResource::operator=(std::move(other));
        m_pipelineLayout = std::exchange(other.m_pipelineLayout, {});
        m_graphicsPipeline = std::exchange(other.m_graphicsPipeline, {});
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::PipelineStateObject::~PipelineStateObject()
{
    Reset();
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::PipelineStateObject::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::PipelineStateObject::IsValid() const
{
    return DeviceResource::IsValid() && static_cast<bool>(m_pipelineLayout);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::PipelineLayout & Gris::Graphics::Vulkan::PipelineStateObject::PipelineLayoutHandle() const
{
    return m_pipelineLayout;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::PipelineLayout & Gris::Graphics::Vulkan::PipelineStateObject::PipelineLayoutHandle()
{
    return m_pipelineLayout;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::Pipeline & Gris::Graphics::Vulkan::PipelineStateObject::GraphicsPipelineHandle() const
{
    return m_graphicsPipeline;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Pipeline & Gris::Graphics::Vulkan::PipelineStateObject::GraphicsPipelineHandle()
{
    return m_graphicsPipeline;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::PipelineStateObject::Reset()
{
    if (m_graphicsPipeline)
    {
        DeviceHandle().destroyPipeline(m_graphicsPipeline, nullptr, Dispatch());
        m_graphicsPipeline = nullptr;
    }

    if (m_pipelineLayout)
    {
        DeviceHandle().destroyPipelineLayout(m_pipelineLayout, nullptr, Dispatch());
        m_pipelineLayout = nullptr;
    }

    ResetParent();
}
