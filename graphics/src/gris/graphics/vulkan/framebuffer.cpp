#include <gris/graphics/vulkan/framebuffer.h>

#include <gris/graphics/vulkan/render_pass.h>
#include <gris/graphics/vulkan/texture_view.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Framebuffer::Framebuffer(Device * device, const TextureView & colorImageView, const TextureView & depthImageView, const TextureView & swapChainImageView, const RenderPass & renderPass, uint32_t width, uint32_t height)
    : DeviceResource(device)
{
    std::array attachments = { colorImageView.ImageViewHandle(), depthImageView.ImageViewHandle(), swapChainImageView.ImageViewHandle() };

    auto const framebufferInfo = vk::FramebufferCreateInfo{}
                                     .setRenderPass(renderPass.RenderPassHandle())
                                     .setAttachments(attachments)
                                     .setWidth(width)
                                     .setHeight(height)
                                     .setLayers(1);

    auto createFramebufferResult = DeviceHandle().createFramebufferUnique(framebufferInfo);
    if (createFramebufferResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating framebuffer", createFramebufferResult);
    }

    m_framebuffer = std::move(createFramebufferResult.value);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
const vk::Framebuffer & Gris::Graphics::Vulkan::Framebuffer::FramebufferHandle() const
{
    return m_framebuffer.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
vk::Framebuffer & Gris::Graphics::Vulkan::Framebuffer::FramebufferHandle()
{
    return m_framebuffer.get();
}