#include <gris/graphics/vulkan/framebuffer.h>

#include <gris/graphics/vulkan/engine_exception.h>
#include <gris/graphics/vulkan/render_pass.h>
#include <gris/graphics/vulkan/texture_view.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanFramebuffer::VulkanFramebuffer(VulkanDevice * device, const VulkanTextureView & colorImageView, const VulkanTextureView & depthImageView, const VulkanTextureView & swapChainImageView, const VulkanRenderPass & renderPass, uint32_t width, uint32_t height)
    : VulkanDeviceResource(device)
{
    std::array attachments = { colorImageView.ImageViewHandle(), depthImageView.ImageViewHandle(), swapChainImageView.ImageViewHandle() };

    auto const framebufferInfo = vk::FramebufferCreateInfo({}, renderPass.RenderPassHandle(), attachments, width, height, 1);

    auto createFramebufferResult = DeviceHandle().createFramebufferUnique(framebufferInfo);
    if (createFramebufferResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating framebuffer", createFramebufferResult);

    m_framebuffer = std::move(createFramebufferResult.value);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
const vk::Framebuffer & Gris::Graphics::Vulkan::VulkanFramebuffer::FramebufferHandle() const
{
    return m_framebuffer.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
vk::Framebuffer & Gris::Graphics::Vulkan::VulkanFramebuffer::FramebufferHandle()
{
    return m_framebuffer.get();
}