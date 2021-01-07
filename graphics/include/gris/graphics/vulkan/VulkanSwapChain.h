#pragma once

#include "VulkanDeviceResource.h"

#include "VulkanFence.h"
#include "VulkanSemaphore.h"
#include "VulkanTextureView.h"

#include <optional>

namespace Gris::Graphics::Vulkan
{

class VulkanWindowMixin;

struct VulkanVirtualFrame
{
    uint32_t VirtualFrameIndex;
    uint32_t SwapChainImageIndex;
};

class VulkanSwapChain : public VulkanDeviceResource
{
public:
    VulkanSwapChain(VulkanDevice * device, const VulkanWindowMixin & window, uint32_t width, uint32_t height, uint32_t virtualFrameCount);

    [[nodiscard]] uint32_t ImageCount() const;

    [[nodiscard]] const VulkanTextureView & ImageView(size_t index) const;
    [[nodiscard]] VulkanTextureView & ImageView(size_t index);

    [[nodiscard]] vk::Format Format() const;

    [[nodiscard]] vk::Extent2D Extent() const;

    [[nodiscard]] std::optional<VulkanVirtualFrame> NextImage();

    [[nodiscard]] bool Present(const VulkanVirtualFrame & virtualFrame);

    [[nodiscard]] const VulkanFence & RenderingFinishedFence(const VulkanVirtualFrame & frame) const
    {
        return m_renderFinishedFences[frame.VirtualFrameIndex];
    }

    [[nodiscard]] VulkanFence & RenderingFinishedFence(const VulkanVirtualFrame & frame)
    {
        return m_renderFinishedFences[frame.VirtualFrameIndex];
    }

    [[nodiscard]] const VulkanSemaphore & ImageAvailableSemaphore(const VulkanVirtualFrame & frame) const
    {
        return m_imageAvailableSemaphores[frame.VirtualFrameIndex];
    }

    [[nodiscard]] VulkanSemaphore & ImageAvailableSemaphore(const VulkanVirtualFrame & frame)
    {
        return m_imageAvailableSemaphores[frame.VirtualFrameIndex];
    }

    [[nodiscard]] const VulkanSemaphore & RenderingFinishedSemaphore(const VulkanVirtualFrame & frame) const
    {
        return m_renderFinishedSemaphores[frame.VirtualFrameIndex];
    }

    [[nodiscard]] VulkanSemaphore & RenderingFinishedSemaphore(const VulkanVirtualFrame & frame)
    {
        return m_renderFinishedSemaphores[frame.VirtualFrameIndex];
    }

private:
    vk::UniqueSwapchainKHR m_swapChain = {};
    std::vector<vk::Image> m_swapChainImages = {};
    vk::Queue m_presentQueue = {};

    vk::Format m_swapChainImageFormat = {};
    vk::Extent2D m_swapChainExtent = {};

    std::vector<VulkanTextureView> m_swapChainImageViews = {};

    std::vector<VulkanFence> m_renderFinishedFences = {};
    std::vector<VulkanSemaphore> m_imageAvailableSemaphores = {};
    std::vector<VulkanSemaphore> m_renderFinishedSemaphores = {};

    uint32_t m_currentVirtualFrame = 0;
    uint32_t m_virtualFrameCount = 1;
    std::vector<uint32_t> m_swapChainImageToVirtualFrame = {};
};


}  // namespace Gris::Graphics::Vulkan
