#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Texture;

class TextureView : public DeviceResource
{
public:
    TextureView();

    TextureView(std::shared_ptr<DeviceSharedData> sharedData,
                const Texture & image,
                vk::Format format,
                const vk::ImageAspectFlags & aspectFlags,
                uint32_t mipLevels);

    TextureView(std::shared_ptr<DeviceSharedData> sharedData,
                const vk::Image & image,
                vk::Format format,
                const vk::ImageAspectFlags & aspectFlags,
                uint32_t mipLevels);

    TextureView(const TextureView &) = delete;
    TextureView & operator=(const TextureView &) = delete;

    TextureView(TextureView &&) noexcept = default;
    TextureView & operator=(TextureView &&) noexcept = default;

    ~TextureView() = default;

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] const vk::ImageView & ImageViewHandle() const;
    [[nodiscard]] vk::ImageView & ImageViewHandle();

private:
    vk::UniqueImageView m_imageView = {};
};

}  // namespace Gris::Graphics::Vulkan
