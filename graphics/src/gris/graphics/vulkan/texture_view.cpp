#include <gris/graphics/vulkan/texture_view.h>

#include <gris/graphics/vulkan/device.h>
#include <gris/graphics/vulkan/texture.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::TextureView::TextureView() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::TextureView::TextureView(std::shared_ptr<Device *> device,
                                                 const Texture & image,
                                                 vk::Format format,
                                                 const vk::ImageAspectFlags & aspectFlags,
                                                 uint32_t mipLevels)
    : TextureView(device, image.ImageHandle(), format, aspectFlags, mipLevels)
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::TextureView::TextureView(std::shared_ptr<Device *> device,
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

    auto createImageViewResult = DeviceHandle().createImageViewUnique(viewInfo, nullptr, Dispatch());
    if (createImageViewResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating image view", createImageViewResult);
    }

    m_imageView = std::move(createImageViewResult.value);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::TextureView::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

bool Gris::Graphics::Vulkan::TextureView::IsValid() const
{
    return DeviceResource::IsValid() && static_cast<bool>(m_imageView);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::ImageView & Gris::Graphics::Vulkan::TextureView::ImageViewHandle() const
{
    return m_imageView.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::ImageView & Gris::Graphics::Vulkan::TextureView::ImageViewHandle()
{
    return m_imageView.get();
}
