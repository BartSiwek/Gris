#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class VulkanRenderPass;
class VulkanTextureView;

class VulkanFramebuffer : public VulkanDeviceResource
{
public:
    VulkanFramebuffer(VulkanDevice * device, const VulkanTextureView & colorImageView, const VulkanTextureView & depthImageView, const VulkanTextureView & swapChainImageView, const VulkanRenderPass & renderPass, uint32_t width, uint32_t height);

    // TODO: Do this better
    const vk::Framebuffer & FramebufferHandle() const;
    vk::Framebuffer & FramebufferHandle();

private:
    vk::UniqueFramebuffer m_framebuffer;
};

}  // namespace Gris::Graphics::Vulkan
