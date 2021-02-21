#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class Texture;

class TextureView : public DeviceResource
{
public:
    TextureView();

    TextureView(const ParentObject<Device> & device,
                const Texture & image,
                vk::Format format,
                const vk::ImageAspectFlags & aspectFlags,
                uint32_t mipLevels);

    TextureView(const ParentObject<Device> & device,
                const vk::Image & image,
                vk::Format format,
                const vk::ImageAspectFlags & aspectFlags,
                uint32_t mipLevels);

    TextureView(const TextureView &) = delete;
    TextureView & operator=(const TextureView &) = delete;

    TextureView(TextureView && other) noexcept;
    TextureView & operator=(TextureView && other) noexcept;

    virtual ~TextureView();

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] const vk::ImageView & ImageViewHandle() const;
    [[nodiscard]] vk::ImageView & ImageViewHandle();

    void Reset();

private:
    void ReleaseResources();

    vk::ImageView m_imageView = {};
};

}  // namespace Gris::Graphics::Vulkan
