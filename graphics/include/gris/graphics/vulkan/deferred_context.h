#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class RenderPass;
class Texture;
class Framebuffer;
class PipelineStateObject;
class BufferView;
class ShaderResourceBindings;

class DeferredContext : public DeviceResource
{
public:
    DeferredContext();

    explicit DeferredContext(Device * device);

    DeferredContext(const DeferredContext &) = delete;
    DeferredContext & operator=(const DeferredContext &) = delete;

    DeferredContext(DeferredContext &&) noexcept = default;
    DeferredContext & operator=(DeferredContext &&) noexcept = default;

    ~DeferredContext() = default;

    explicit operator bool() const;

    bool IsValid() const;

    [[nodiscard]] vk::CommandBuffer & CommandBufferHandle();

    void Begin();
    void BeginRenderPass(const RenderPass & renderPass, const Framebuffer & framebuffer, const vk::Extent2D & extent);
    void BindPipeline(const PipelineStateObject & pso);
    void BindVertexBuffer(const BufferView & bufferView);
    void BindIndexBuffer(const BufferView & bufferView);
    void BindDescriptorSet(const PipelineStateObject & pso, const ShaderResourceBindings & srb);
    void DrawIndexed(uint32_t indexCount);
    void EndRenderPass();
    void End();

private:
    vk::UniqueCommandPool m_commandPool = {};
    vk::UniqueCommandBuffer m_commandBuffer = {};
};

}  // namespace Gris::Graphics::Vulkan
