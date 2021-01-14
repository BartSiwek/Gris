#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class RenderPass;
class Texture;
class Framebuffer;
class PipelineStateObject;
class BufferView;
class ShaderResourceBinding;

class DeferredContext : public DeviceResource
{
public:
    DeferredContext(Device * device);

    // TODO: Do this better
    [[nodiscard]] vk::CommandBuffer & CommandBufferHandle();

    void Begin();
    void BeginRenderPass(const RenderPass & renderPass, const Framebuffer & framebuffer, const vk::Extent2D & extent);
    void BindPipeline(const PipelineStateObject & pso);
    void BindVertexBuffer(const BufferView & bufferView);
    void BindIndexBuffer(const BufferView & bufferView);
    void BindDescriptorSet(const PipelineStateObject & pso, const ShaderResourceBinding & srb);
    void DrawIndexed(uint32_t indexCount);
    void EndRenderPass();
    void End();

private:
    vk::UniqueCommandPool m_commandPool = {};
    std::vector<vk::UniqueCommandBuffer> m_commandBuffers = {};
    uint32_t m_frameIndex = 0;
};

}  // namespace Gris::Graphics::Vulkan
