#pragma once

#include <gris/graphics/vulkan/device_resource.h>

namespace Gris::Graphics::Vulkan
{

class RenderPass : public DeviceResource
{
public:
    RenderPass();

    RenderPass(const ParentObject<Device> & device, vk::Format swapChainFormat, vk::Format depthFormat);

    RenderPass(const RenderPass &) = delete;
    RenderPass & operator=(const RenderPass &) = delete;

    RenderPass(RenderPass &&) noexcept = default;
    RenderPass & operator=(RenderPass &&) noexcept = default;

    ~RenderPass() = default;

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] const vk::RenderPass & RenderPassHandle() const;
    [[nodiscard]] vk::RenderPass & RenderPassHandle();

private:
    void CreateRenderPass(vk::Format swapChainFormat, vk::Format depthFormat);

    vk::UniqueRenderPass m_renderPass = {};
};

}  // namespace Gris::Graphics::Vulkan
