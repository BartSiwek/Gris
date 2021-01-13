#pragma once

#include <gris/graphics/vulkan/VulkanAllocator.h>
#include <gris/graphics/vulkan/VulkanImmediateContext.h>
#include <gris/graphics/vulkan/VulkanPhysicalDevice.h>

namespace Gris::Graphics::Vulkan
{

class VulkanSwapChain;
class VulkanDeviceResource;
class VulkanBuffer;
class VulkanTexture;
class VulkanDeferredContext;
class VulkanShader;
class VulkanInputLayout;
class VulkanRenderPass;
class VulkanPipelineStateObject;
class VulkanTextureView;
class VulkanSampler;
class VulkanFramebuffer;
class VulkanFence;
class VulkanSemaphore;

class VulkanDevice
{
public:
    friend class VulkanDeviceResource;

    explicit VulkanDevice(VulkanPhysicalDevice physicalDevice);

    [[nodiscard]] const VulkanImmediateContext * Context() const;
    [[nodiscard]] VulkanImmediateContext * Context();

    [[nodiscard]] const vk::SampleCountFlagBits & MsaaSamples() const;

    [[nodiscard]] const DeviceQueueFamilyIndices & QueueFamilies() const;

    [[nodiscard]] SwapChainSupportDetails SwapChainSupport(const VulkanWindowMixin & window) const;

    void WaitIdle();

    [[nodiscard]] vk::Format FindSupportedFormat(const std::vector<vk::Format> & candidates, const vk::ImageTiling & tiling, const vk::FormatFeatureFlags & features) const;

    [[nodiscard]] vk::FormatProperties GetFormatProperties(vk::Format format) const;

    // TODO: Do this better
    [[nodiscard]] const vk::Device & DeviceHandle() const;
    [[nodiscard]] vk::Device & DeviceHandle();

    // TODO: Make device independent of frame count
    void CreateDescriptorPool(uint32_t imageCount);

    [[nodiscard]] VulkanSwapChain CreateSwapChain(const VulkanWindowMixin & window, uint32_t width, uint32_t height, uint32_t virtualFrameCount);

    [[nodiscard]] VulkanDeferredContext CreateDeferredContext();
    [[nodiscard]] VulkanShader CreateShader(const std::vector<char> & code);
    [[nodiscard]] VulkanBuffer CreateBuffer(vk::DeviceSize size, const vk::BufferUsageFlags & usage, const vk::MemoryPropertyFlags & properties);
    [[nodiscard]] VulkanTexture CreateTexture(uint32_t width, uint32_t height, uint32_t mipLevels, vk::SampleCountFlagBits numSamples, vk::Format format, vk::ImageTiling tiling, const vk::ImageUsageFlags & usage, const vk::MemoryPropertyFlags & properties);
    [[nodiscard]] VulkanTextureView CreateTextureView(const VulkanTexture & image, vk::Format format, const vk::ImageAspectFlags & aspectFlags, uint32_t mipLevels);
    [[nodiscard]] VulkanSampler CreateSampler(float minLod, float maxLod);
    [[nodiscard]] VulkanPipelineStateObject CreatePipelineStateObject(uint32_t swapChainWidth, uint32_t swapChainHeight, const VulkanRenderPass & renderPass, const VulkanInputLayout & inputLayout, const VulkanShader & vertexShader, const VulkanShader & fragmentShader);
    [[nodiscard]] VulkanFramebuffer CreateFramebuffer(const VulkanTextureView & colorImageView, const VulkanTextureView & depthImageView, const VulkanTextureView & swapChainImageView, const VulkanRenderPass & renderPass, uint32_t width, uint32_t height);
    [[nodiscard]] VulkanFence CreateFence(bool signaled);
    [[nodiscard]] VulkanSemaphore CreateSemaphore();

private:
    [[nodiscard]] const VulkanAllocator & Allocator() const;
    [[nodiscard]] VulkanAllocator & Allocator();

    [[nodiscard]] const vk::DescriptorPool & DescriptorPoolHandle() const;
    [[nodiscard]] vk::DescriptorPool & DescriptorPoolHandle();

    VulkanPhysicalDevice m_physicalDevice;

    vk::UniqueDevice m_device = {};
    VulkanAllocator m_allocator = {};
    vk::UniqueDescriptorPool m_descriptorPool = {};
    std::unique_ptr<VulkanImmediateContext> m_context = {};
};

}  // namespace Gris::Graphics::Vulkan