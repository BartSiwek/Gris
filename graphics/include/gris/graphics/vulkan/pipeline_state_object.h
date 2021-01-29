#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class InputLayout;
class RenderPass;
class ShaderResourceBindingsLayout;
class Shader;

class PipelineStateObject : public DeviceResource
{
public:
    PipelineStateObject(
        Device * device,
        uint32_t swapChainWidth,
        uint32_t swapChainHeight,
        const RenderPass & renderPass,
        const InputLayout & inputLayout,
        const ShaderResourceBindingsLayout & resourceLayout,
        const Shader & vertexShader,
        const Shader & fragmentShader);

    // TODO: Do this better
    [[nodiscard]] const vk::PipelineLayout & PipelineLayoutHandle() const;
    [[nodiscard]] vk::PipelineLayout & PipelineLayoutHandle();

    // TODO: Do this better
    [[nodiscard]] const vk::Pipeline & GraphicsPipelineHandle() const;
    [[nodiscard]] vk::Pipeline & GraphicsPipelineHandle();

private:
    vk::UniquePipelineLayout m_pipelineLayout;
    vk::UniquePipeline m_graphicsPipeline;
};

}  // namespace Gris::Graphics::Vulkan
