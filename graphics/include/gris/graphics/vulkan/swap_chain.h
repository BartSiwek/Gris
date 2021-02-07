#pragma once

#include <gris/graphics/vulkan/device_resource.h>

#include <gris/graphics/vulkan/fence.h>
#include <gris/graphics/vulkan/semaphore.h>
#include <gris/graphics/vulkan/texture_view.h>

#include <optional>

namespace Gris::Graphics::Vulkan
{

class WindowMixin;

struct VirtualFrame
{
    uint32_t VirtualFrameIndex;
    uint32_t SwapChainImageIndex;
};

class SwapChain : public DeviceResource
{
public:
    SwapChain();

    SwapChain(Device * device, const WindowMixin & window, uint32_t width, uint32_t height, uint32_t virtualFrameCount);

    SwapChain(const SwapChain &) = delete;
    SwapChain & operator=(const SwapChain &) = delete;

    SwapChain(SwapChain &&) noexcept = default;
    SwapChain & operator=(SwapChain &&) noexcept = default;

    ~SwapChain() = default;

    explicit operator bool() const;

    bool IsValid() const;

    [[nodiscard]] uint32_t ImageCount() const;

    [[nodiscard]] const TextureView & ImageView(size_t index) const;
    [[nodiscard]] TextureView & ImageView(size_t index);

    [[nodiscard]] vk::Format Format() const;

    [[nodiscard]] vk::Extent2D Extent() const;

    [[nodiscard]] std::optional<VirtualFrame> NextImage();

    [[nodiscard]] bool Present(const VirtualFrame & virtualFrame);

    [[nodiscard]] const Fence & RenderingFinishedFence(const VirtualFrame & frame) const
    {
        return m_renderFinishedFences[frame.VirtualFrameIndex];
    }

    [[nodiscard]] Fence & RenderingFinishedFence(const VirtualFrame & frame)
    {
        return m_renderFinishedFences[frame.VirtualFrameIndex];
    }

    [[nodiscard]] const Semaphore & ImageAvailableSemaphore(const VirtualFrame & frame) const
    {
        return m_imageAvailableSemaphores[frame.VirtualFrameIndex];
    }

    [[nodiscard]] Semaphore & ImageAvailableSemaphore(const VirtualFrame & frame)
    {
        return m_imageAvailableSemaphores[frame.VirtualFrameIndex];
    }

    [[nodiscard]] const Semaphore & RenderingFinishedSemaphore(const VirtualFrame & frame) const
    {
        return m_renderFinishedSemaphores[frame.VirtualFrameIndex];
    }

    [[nodiscard]] Semaphore & RenderingFinishedSemaphore(const VirtualFrame & frame)
    {
        return m_renderFinishedSemaphores[frame.VirtualFrameIndex];
    }

private:
    vk::UniqueSwapchainKHR m_swapChain = {};
    std::vector<vk::Image> m_swapChainImages = {};
    vk::Queue m_presentQueue = {};

    vk::Format m_swapChainImageFormat = {};
    vk::Extent2D m_swapChainExtent = {};

    std::vector<TextureView> m_swapChainImageViews = {};

    std::vector<Fence> m_renderFinishedFences = {};
    std::vector<Semaphore> m_imageAvailableSemaphores = {};
    std::vector<Semaphore> m_renderFinishedSemaphores = {};

    uint32_t m_currentVirtualFrame = 0;
    uint32_t m_virtualFrameCount = 1;
    std::vector<uint32_t> m_swapChainImageToVirtualFrame = {};
};


}  // namespace Gris::Graphics::Vulkan
