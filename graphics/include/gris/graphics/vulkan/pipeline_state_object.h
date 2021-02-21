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

    PipelineStateObject(PipelineStateObject && other) noexcept;
    PipelineStateObject & operator=(PipelineStateObject && other) noexcept;

    virtual ~PipelineStateObject();

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] const vk::PipelineLayout & PipelineLayoutHandle() const;
    [[nodiscard]] vk::PipelineLayout & PipelineLayoutHandle();

    [[nodiscard]] const vk::Pipeline & GraphicsPipelineHandle() const;
    [[nodiscard]] vk::Pipeline & GraphicsPipelineHandle();

    void Reset();

private:
    void ReleaseResources();

    vk::PipelineLayout m_pipelineLayout = {};
    vk::Pipeline m_graphicsPipeline = {};
};

}  // namespace Gris::Graphics::Vulkan
