#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class VulkanRenderPass : public VulkanDeviceResource
{
public:
    VulkanRenderPass(VulkanDevice * device, vk::Format swapChainFormat, vk::Format depthFormat);

    // TODO: Do this better
    [[nodiscard]] const vk::RenderPass & RenderPassHandle() const;
    [[nodiscard]] vk::RenderPass & RenderPassHandle();

private:
    void CreateRenderPass(vk::Format swapChainFormat, vk::Format depthFormat);

    vk::UniqueRenderPass m_renderPass;
};

}  // namespace Gris::Graphics::Vulkan
