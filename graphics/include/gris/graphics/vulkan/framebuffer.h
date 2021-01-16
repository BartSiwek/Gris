﻿#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class RenderPass;
class TextureView;

class Framebuffer : public DeviceResource
{
public:
    Framebuffer(Device * device, const TextureView & colorImageView, const TextureView & depthImageView, const TextureView & swapChainImageView, const RenderPass & renderPass, uint32_t width, uint32_t height);

    // TODO: Do this better
    const vk::Framebuffer & FramebufferHandle() const;
    vk::Framebuffer & FramebufferHandle();

private:
    vk::UniqueFramebuffer m_framebuffer;
};

}  // namespace Gris::Graphics::Vulkan