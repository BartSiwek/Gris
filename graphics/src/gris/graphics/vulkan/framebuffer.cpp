#include <gris/graphics/vulkan/framebuffer.h>

#include <gris/graphics/vulkan/render_pass.h>
#include <gris/graphics/vulkan/texture_view.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Framebuffer::Framebuffer() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Framebuffer::Framebuffer(const ParentObject<Device> & device, const TextureView & colorImageView, const TextureView & depthImageView, const TextureView & swapChainImageView, const RenderPass & renderPass, uint32_t width, uint32_t height)
    : DeviceResource(device)
{
    std::array attachments = { colorImageView.ImageViewHandle(), depthImageView.ImageViewHandle(), swapChainImageView.ImageViewHandle() };

    auto const framebufferInfo = vk::FramebufferCreateInfo{}
                                     .setRenderPass(renderPass.RenderPassHandle())
                                     .setAttachments(attachments)
                                     .setWidth(width)
                                     .setHeight(height)
                                     .setLayers(1);

    auto createFramebufferResult = DeviceHandle().createFramebuffer(framebufferInfo, nullptr, Dispatch());
    if (createFramebufferResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating framebuffer", createFramebufferResult);
    }

    m_framebuffer = std::move(createFramebufferResult.value);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Framebuffer::Framebuffer(Framebuffer && other) noexcept
    : DeviceResource(std::move(other))
    , m_framebuffer(std::exchange(other.m_framebuffer, {}))
{

}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Framebuffer & Gris::Graphics::Vulkan::Framebuffer::operator=(Framebuffer && other) noexcept
{
    if (this != &other)
    {
        ReleaseResources();

        DeviceResource::operator=(std::move(other));
        m_framebuffer = std::exchange(other.m_framebuffer, {});
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Framebuffer::~Framebuffer()
{
    ReleaseResources();
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Framebuffer::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::Framebuffer::IsValid() const
{
    return DeviceResource::IsValid() && static_cast<bool>(m_framebuffer);
}

// -------------------------------------------------------------------------------------------------

const vk::Framebuffer & Gris::Graphics::Vulkan::Framebuffer::FramebufferHandle() const
{
    return m_framebuffer;
}

// -------------------------------------------------------------------------------------------------

vk::Framebuffer & Gris::Graphics::Vulkan::Framebuffer::FramebufferHandle()
{
    return m_framebuffer;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Framebuffer::Reset()
{
    ReleaseResources();
    ResetParent();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Framebuffer::ReleaseResources()
{
    if (m_framebuffer)
    {
        DeviceHandle().destroyFramebuffer(m_framebuffer, nullptr, Dispatch());
        m_framebuffer = nullptr;
    }
}
