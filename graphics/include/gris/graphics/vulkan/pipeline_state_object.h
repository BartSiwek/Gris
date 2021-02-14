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
    PipelineStateObject();

    PipelineStateObject(
        const ParentObject<Device> & device,
        uint32_t swapChainWidth,
        uint32_t swapChainHeight,
        const RenderPass & renderPass,
        const InputLayout & inputLayout,
        const ShaderResourceBindingsLayout & resourceLayout,
        const Shader & vertexShader,
        const Shader & fragmentShader);

    PipelineStateObject(const PipelineStateObject &) = delete;
    PipelineStateObject & operator=(const PipelineStateObject &) = delete;

    PipelineStateObject(PipelineStateObject &&) noexcept = default;
    PipelineStateObject & operator=(PipelineStateObject &&) noexcept = default;

    ~PipelineStateObject() = default;

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] const vk::PipelineLayout & PipelineLayoutHandle() const;
    [[nodiscard]] vk::PipelineLayout & PipelineLayoutHandle();

    [[nodiscard]] const vk::Pipeline & GraphicsPipelineHandle() const;
    [[nodiscard]] vk::Pipeline & GraphicsPipelineHandle();

private:
    vk::UniquePipelineLayout m_pipelineLayout = {};
    vk::UniquePipeline m_graphicsPipeline = {};
};

}  // namespace Gris::Graphics::Vulkan
