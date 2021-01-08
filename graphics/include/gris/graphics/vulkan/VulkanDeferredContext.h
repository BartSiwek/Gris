#pragma once

#include <gris/graphics/vulkan/VulkanDeviceResource.h>

namespace Gris::Graphics::Vulkan
{

class VulkanRenderPass;
class VulkanTexture;
class VulkanFramebuffer;
class VulkanPipelineStateObject;
class VulkanBufferView;
class VulkanShaderResourceBinding;

class VulkanDeferredContext : public VulkanDeviceResource
{
public:
    VulkanDeferredContext(VulkanDevice * device);

    // TODO: Do this better
    [[nodiscard]] vk::CommandBuffer & CommandBufferHandle();

    void Begin();
    void BeginRenderPass(const VulkanRenderPass & renderPass, const VulkanFramebuffer & framebuffer, const vk::Extent2D & extent);
    void BindPipeline(const VulkanPipelineStateObject & pso);
    void BindVertexBuffer(const VulkanBufferView & bufferView);
    void BindIndexBuffer(const VulkanBufferView & bufferView);
    void BindDescriptorSet(const VulkanPipelineStateObject & pso, const VulkanShaderResourceBinding & srb);
    void DrawIndexed(uint32_t indexCount);
    void EndRenderPass();
    void End();

private:
    vk::UniqueCommandPool m_commandPool = {};
    std::vector<vk::UniqueCommandBuffer> m_commandBuffers = {};
    uint32_t m_frameIndex = 0;
};

}  // namespace Gris::Graphics::Vulkan
