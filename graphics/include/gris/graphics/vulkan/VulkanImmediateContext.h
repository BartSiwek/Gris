#pragma once

#include <gris/graphics/vulkan/VulkanDeviceResource.h>

namespace Gris::Graphics::Vulkan
{

class VulkanBuffer;
class VulkanTexture;
class VulkanDeferredContext;
class VulkanSemaphore;
class VulkanFence;

class VulkanImmediateContext : public VulkanDeviceResource
{
public:
    explicit VulkanImmediateContext(VulkanDevice * device);

    void GenerateMipmaps(const VulkanTexture & texture, const vk::Format & imageFormat, int32_t texWidth, int32_t texHeight);
    void CopyBufferToImage(const VulkanBuffer & buffer, const VulkanTexture & texture, uint32_t width, uint32_t height);
    void TransitionImageLayout(const VulkanTexture & texture, const vk::ImageLayout & oldLayout, const vk::ImageLayout & newLayout);
    void CopyBuffer(const VulkanBuffer & srcBuffer, const VulkanBuffer & dstBuffer, vk::DeviceSize size);
    void Submit(VulkanDeferredContext * context, const std::vector<std::reference_wrapper<VulkanSemaphore>> & waitSemaphores, const std::vector<std::reference_wrapper<VulkanSemaphore>> & signalSemaphores, VulkanFence & fence);

private:
    [[nodiscard]] vk::UniqueCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(vk::CommandBuffer & commandBuffer) const;

    vk::Queue m_graphicsQueue = {};
    vk::UniqueCommandPool m_commandPool = {};
};

}  // namespace Gris::Graphics::Vulkan