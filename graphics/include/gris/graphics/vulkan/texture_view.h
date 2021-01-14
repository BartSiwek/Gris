#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Texture;

class TextureView : public DeviceResource
{
public:
    TextureView(Device * device,
                      const Texture & image,
                      vk::Format format,
                      const vk::ImageAspectFlags & aspectFlags,
                      uint32_t mipLevels);

    TextureView(Device * device,
                      const vk::Image & image,
                      vk::Format format,
                      const vk::ImageAspectFlags & aspectFlags,
                      uint32_t mipLevels);

    // TODO: Do this better
    [[nodiscard]] const vk::ImageView & ImageViewHandle() const;
    [[nodiscard]] vk::ImageView & ImageViewHandle();

private:
    vk::UniqueImageView m_imageView;
};

}  // namespace Gris::Graphics::Vulkan
