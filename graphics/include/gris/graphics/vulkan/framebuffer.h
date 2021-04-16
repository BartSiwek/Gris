#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class RenderPass;
class TextureView;

class Framebuffer : public DeviceResource
{
public:
    Framebuffer();

    Framebuffer(const ParentObject<Device> & device, const TextureView & colorImageView, const TextureView & depthImageView, const TextureView & swapChainImageView, const RenderPass & renderPass, uint32_t width, uint32_t height);

    Framebuffer(const Framebuffer &) = delete;
    Framebuffer & operator=(const Framebuffer &) = delete;

    Framebuffer(Framebuffer && other) noexcept;
    Framebuffer & operator=(Framebuffer && other) noexcept;

    ~Framebuffer() override;

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] const vk::Framebuffer & FramebufferHandle() const;
    [[nodiscard]] vk::Framebuffer & FramebufferHandle();

    void Reset();

private:
    void ReleaseResources();

    vk::Framebuffer m_framebuffer = {};
};

}  // namespace Gris::Graphics::Vulkan
