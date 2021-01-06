﻿#include <gris/graphics/vulkan/VulkanFramebuffer.h>

#include <gris/graphics/vulkan/VulkanRenderPass.h>
#include <gris/graphics/vulkan/VulkanTextureView.h>
#include <gris/graphics/vulkan/VulkanEngineException.h>

// -------------------------------------------------------------------------------------------------

VulkanFramebuffer::VulkanFramebuffer(VulkanDevice* device, const VulkanTextureView& colorImageView, const VulkanTextureView& depthImageView, const VulkanTextureView& swapChainImageView, const VulkanRenderPass& renderPass, uint32_t width, uint32_t height) : VulkanDeviceResource(device)
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
const vk::Framebuffer& VulkanFramebuffer::FramebufferHandle() const
{
    return m_framebuffer.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
vk::Framebuffer& VulkanFramebuffer::FramebufferHandle()
{
    return m_framebuffer.get();
}