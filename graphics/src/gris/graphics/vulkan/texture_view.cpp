#include <gris/graphics/vulkan/texture_view.h>

#include <gris/graphics/vulkan/device.h>
#include <gris/graphics/vulkan/texture.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::TextureView::TextureView() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::TextureView::TextureView(const ParentObject<Device> & device,
                                                 const Texture & image,
                                                 vk::Format format,
                                                 const vk::ImageAspectFlags & aspectFlags,
                                                 uint32_t mipLevels)
    : TextureView(device, image.ImageHandle(), format, aspectFlags, mipLevels)
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::TextureView::TextureView(const ParentObject<Device> & device,
                                                 const vk::Image & image,
                                                 vk::Format format,
                                                 const vk::ImageAspectFlags & aspectFlags,
                                                 uint32_t mipLevels)
    : DeviceResource(device)
{
    auto const viewInfo = vk::ImageViewCreateInfo{}
                              .setImage(image)
                              .setViewType(vk::ImageViewType::e2D)
                              .setFormat(format)
                              .setComponents({})
                              .setSubresourceRange(vk::ImageSubresourceRange{}
                                                       .setAspectMask(aspectFlags)
                                                       .setBaseMipLevel(0)
                                                       .setLevelCount(mipLevels)
                                                       .setBaseArrayLayer(0)
                                                       .setLayerCount(1));

    auto createImageViewResult = DeviceHandle().createImageView(viewInfo, nullptr, Dispatch());
    if (createImageViewResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating image view", createImageViewResult);
    }

    m_imageView = std::move(createImageViewResult.value);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::TextureView::TextureView(TextureView && other) noexcept
    : DeviceResource(std::move(other))
    , m_imageView(std::exchange(other.m_imageView, {}))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::TextureView & Gris::Graphics::Vulkan::TextureView::operator=(TextureView && other) noexcept
{
    if (this != &other)
    {
        ReleaseResources();

        m_imageView = std::exchange(other.m_imageView, {});
        DeviceResource::operator=(std::move(other));
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::TextureView::~TextureView()
{
    ReleaseResources();
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::TextureView::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::TextureView::IsValid() const
{
    return DeviceResource::IsValid() && static_cast<bool>(m_imageView);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::ImageView & Gris::Graphics::Vulkan::TextureView::ImageViewHandle() const
{
    return m_imageView;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::ImageView & Gris::Graphics::Vulkan::TextureView::ImageViewHandle()
{
    return m_imageView;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::TextureView::Reset()
{
    ReleaseResources();
    ResetParent();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::TextureView::ReleaseResources()
{
    if (m_imageView)
    {
        DeviceHandle().destroyImageView(m_imageView, nullptr, Dispatch());
        m_imageView = nullptr;
    }
}
