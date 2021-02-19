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

    RenderPass(RenderPass && other) noexcept;
    RenderPass & operator=(RenderPass && other) noexcept;

    virtual ~RenderPass();

    explicit operator bool() const;

    [[nodiscard]] bool IsValid() const;

    [[nodiscard]] const vk::RenderPass & RenderPassHandle() const;
    [[nodiscard]] vk::RenderPass & RenderPassHandle();

private:
    void CreateRenderPass(vk::Format swapChainFormat, vk::Format depthFormat);

    void Reset();

    vk::RenderPass m_renderPass = {};
};

}  // namespace Gris::Graphics::Vulkan
