#include <gris/graphics/vulkan/swap_chain.h>

#include <gris/graphics/vulkan/device.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>
#include <gris/graphics/vulkan/window_mixin.h>

#include <gris/assert.h>

#include <iostream>

// -------------------------------------------------------------------------------------------------

namespace
{

vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> & availableFormats)
{
    for (auto const & availableFormat : availableFormats)
    {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

// -------------------------------------------------------------------------------------------------

vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> & availablePresentModes)
{
    for (auto const & availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox)
        {
            return availablePresentMode;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

// -------------------------------------------------------------------------------------------------

vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR & capabilities, uint32_t width, uint32_t height)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }

    vk::Extent2D actualExtent = { width, height };
    actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

    return actualExtent;
}

}  // namespace

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::SwapChain::SwapChain() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::SwapChain::SwapChain(
    const ParentObject<Device> & device,
    const WindowMixin & window,
    uint32_t width,
    uint32_t height,
    uint32_t virtualFrameCount)
    : DeviceResource(device)
    , m_virtualFrameCount(virtualFrameCount)
{
    CreateSwapChain(window, width, height, {});
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::SwapChain::SwapChain(
    const ParentObject<Device> & device,
    const WindowMixin & window,
    uint32_t width,
    uint32_t height,
    uint32_t virtualFrameCount,
    SwapChain prevSwapChain)
    : DeviceResource(device)
    , m_virtualFrameCount(virtualFrameCount)
{
    CreateSwapChain(window, width, height, prevSwapChain.m_swapChain);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::SwapChain::SwapChain(SwapChain && other) noexcept
    : DeviceResource(std::move(other))
    , m_swapChain(std::exchange(other.m_swapChain, {}))
    , m_swapChainImages(std::exchange(other.m_swapChainImages, {}))
    , m_presentQueue(std::exchange(other.m_presentQueue, {}))
    , m_swapChainImageFormat(std::exchange(other.m_swapChainImageFormat, {}))
    , m_swapChainExtent(std::exchange(other.m_swapChainExtent, {}))
    , m_swapChainImageViews(std::exchange(other.m_swapChainImageViews, {}))
    , m_renderFinishedFences(std::exchange(other.m_renderFinishedFences, {}))
    , m_imageAvailableSemaphores(std::exchange(other.m_imageAvailableSemaphores, {}))
    , m_renderFinishedSemaphores(std::exchange(other.m_renderFinishedSemaphores, {}))
    , m_currentVirtualFrame(std::exchange(other.m_currentVirtualFrame, 0))
    , m_virtualFrameCount(std::exchange(other.m_virtualFrameCount, 1))
    , m_swapChainImageToVirtualFrame(std::exchange(other.m_swapChainImageToVirtualFrame, {}))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::SwapChain & Gris::Graphics::Vulkan::SwapChain::operator=(SwapChain && other) noexcept
{
    if (this != &other)
    {
        Reset();

        m_swapChain = std::exchange(other.m_swapChain, {});
        m_swapChainImages = std::exchange(other.m_swapChainImages, {});
        m_presentQueue = std::exchange(other.m_presentQueue, {});
        m_swapChainImageFormat = std::exchange(other.m_swapChainImageFormat, {});
        m_swapChainExtent = std::exchange(other.m_swapChainExtent, {});
        m_swapChainImageViews = std::exchange(other.m_swapChainImageViews, {});
        m_renderFinishedFences = std::exchange(other.m_renderFinishedFences, {});
        m_imageAvailableSemaphores = std::exchange(other.m_imageAvailableSemaphores, {});
        m_renderFinishedSemaphores = std::exchange(other.m_renderFinishedSemaphores, {});
        m_currentVirtualFrame = std::exchange(other.m_currentVirtualFrame, 0);
        m_virtualFrameCount = std::exchange(other.m_virtualFrameCount, 1);
        m_swapChainImageToVirtualFrame = std::exchange(other.m_swapChainImageToVirtualFrame, {});
        DeviceResource::operator=(std::move(other));
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::SwapChain::~SwapChain()
{
    Reset();
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::SwapChain::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::SwapChain::IsValid() const
{
    return DeviceResource::IsValid() && static_cast<bool>(m_swapChain);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] uint32_t Gris::Graphics::Vulkan::SwapChain::ImageCount() const
{
    return static_cast<uint32_t>(m_swapChainImages.size());
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const Gris::Graphics::Vulkan::TextureView & Gris::Graphics::Vulkan::SwapChain::ImageView(const size_t index) const
{
    GRIS_ALWAYS_ASSERT(index < m_swapChainImageViews.size(), "Swap chain index must be in range");
    return m_swapChainImageViews[index];
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::TextureView & Gris::Graphics::Vulkan::SwapChain::ImageView(const size_t index)
{
    GRIS_ALWAYS_ASSERT(index < m_swapChainImageViews.size(), "Swap chain index must be in range");
    return m_swapChainImageViews[index];
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Format Gris::Graphics::Vulkan::SwapChain::Format() const
{
    return m_swapChainImageFormat;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Extent2D Gris::Graphics::Vulkan::SwapChain::Extent() const
{
    return m_swapChainExtent;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] std::optional<Gris::Graphics::Vulkan::VirtualFrame> Gris::Graphics::Vulkan::SwapChain::NextImage()
{
    auto const virtualFrameIndex = m_currentVirtualFrame;
    m_currentVirtualFrame = (m_currentVirtualFrame + 1) % m_virtualFrameCount;

    std::array fences = { m_renderFinishedFences[virtualFrameIndex].FenceHandle() };
    auto const waitResult = DeviceHandle().waitForFences(fences, static_cast<vk::Bool32>(true), std::numeric_limits<uint64_t>::max(), Dispatch());
    if (waitResult != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Failed to wait for current frame fence!", waitResult);
    }

    ///

    auto const acquireResult = DeviceHandle().acquireNextImageKHR(m_swapChain, std::numeric_limits<uint64_t>::max(), m_imageAvailableSemaphores[virtualFrameIndex].SemaphoreHandle(), {}, Dispatch());
    if (acquireResult.result == vk::Result::eErrorOutOfDateKHR)
    {
        return {};
    }
    if (acquireResult.result != vk::Result::eSuccess && acquireResult.result != vk::Result::eSuboptimalKHR)
    {
        throw VulkanEngineException("Failed to acquire swap chain image!", acquireResult);
    }

    uint32_t imageIndex = acquireResult.value;

    ///

    auto const previousVirtualFrameIndex = m_swapChainImageToVirtualFrame[imageIndex];
    if (previousVirtualFrameIndex != virtualFrameIndex && previousVirtualFrameIndex != std::numeric_limits<uint32_t>::max())
    {
        std::array additionalFences = { m_renderFinishedFences[previousVirtualFrameIndex].FenceHandle() };
        auto const additionalFenceWaitResult = DeviceHandle().waitForFences(additionalFences, static_cast<vk::Bool32>(true), std::numeric_limits<uint64_t>::max(), Dispatch());
        if (additionalFenceWaitResult != vk::Result::eSuccess)
        {
            throw VulkanEngineException("Failed to wait for image in flight fence!", additionalFenceWaitResult);
        }
    }

    m_swapChainImageToVirtualFrame[imageIndex] = virtualFrameIndex;

    ///

    fences = { m_renderFinishedFences[virtualFrameIndex].FenceHandle() };
    auto const resetResult = DeviceHandle().resetFences(fences, Dispatch());
    if (resetResult != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error resetting current frame fence", resetResult);
    }

    return VirtualFrame{ virtualFrameIndex, imageIndex };
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::SwapChain::Present(const VirtualFrame & virtualFrame)
{
    std::array swapChains = { m_swapChain };
    std::array imageIndices = { virtualFrame.SwapChainImageIndex };
    auto presentInfo = vk::PresentInfoKHR{}
                           .setWaitSemaphores(m_renderFinishedSemaphores[virtualFrame.VirtualFrameIndex].SemaphoreHandle())
                           .setSwapchains(swapChains)
                           .setImageIndices(imageIndices);

    auto const presentResult = m_presentQueue.presentKHR(&presentInfo, Dispatch());
    if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR)
    {
        return false;
    }
    if (presentResult != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Failed to present swap chain image", presentResult);
    }

    return true;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::SwapChain::Reset()
{
    m_swapChainImageToVirtualFrame.clear();
    m_virtualFrameCount = 1;
    m_currentVirtualFrame = 0;

    m_renderFinishedSemaphores.clear();
    m_imageAvailableSemaphores.clear();
    m_renderFinishedFences.clear();

    m_swapChainImageViews.clear();

    m_swapChainExtent = vk::Extent2D{};
    m_swapChainImageFormat = {};

    m_presentQueue = nullptr;
    m_swapChainImages.clear();

    if (m_swapChain)
    {
        DeviceHandle().destroySwapchainKHR(m_swapChain, nullptr, Dispatch());
        m_swapChain = nullptr;
    }

    ResetParent();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::SwapChain::CreateSwapChain(
    const WindowMixin & window,
    uint32_t width,
    uint32_t height,
    vk::SwapchainKHR oldSwapChain)
{
    auto const & indices = ParentDevice().QueueFamilies();
    auto const swapChainSupport = ParentDevice().SwapChainSupport(window);
    auto const surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    auto const presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    auto const extent = ChooseSwapExtent(swapChainSupport.capabilities, width, height);

    auto imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    auto imageSharingMode = vk::SharingMode::eExclusive;
    std::vector<uint32_t> queueFamilyIndices;
    if (indices.graphicsFamily != indices.presentFamily)
    {
        imageSharingMode = vk::SharingMode::eConcurrent;
        queueFamilyIndices.push_back(indices.graphicsFamily.value());
        queueFamilyIndices.push_back(indices.presentFamily.value());
    }

    auto const createInfo = vk::SwapchainCreateInfoKHR{}
                                .setSurface(window.SurfaceHandle())
                                .setMinImageCount(imageCount)
                                .setImageFormat(surfaceFormat.format)
                                .setImageColorSpace(surfaceFormat.colorSpace)
                                .setImageExtent(extent)
                                .setImageArrayLayers(1)
                                .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                                .setImageSharingMode(imageSharingMode)
                                .setQueueFamilyIndices(queueFamilyIndices)
                                .setPreTransform(swapChainSupport.capabilities.currentTransform)
                                .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                                .setPresentMode(presentMode)
                                .setClipped(static_cast<vk::Bool32>(true))
                                .setOldSwapchain(oldSwapChain);

    auto createSwapChainResult = DeviceHandle().createSwapchainKHR(createInfo, nullptr, Dispatch());
    if (createSwapChainResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating swap chain", createSwapChainResult);
    }

    m_swapChain = std::move(createSwapChainResult.value);

    auto swapChainImagesResult = DeviceHandle().getSwapchainImagesKHR(m_swapChain, Dispatch());
    if (swapChainImagesResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error getting swap chain images", swapChainImagesResult);
    }

    m_swapChainImages = std::move(swapChainImagesResult.value);
    m_presentQueue = DeviceHandle().getQueue(ParentDevice().QueueFamilies().presentFamily.value(), 0, Dispatch());

    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;

    m_swapChainImageViews.reserve(m_swapChainImages.size());
    for (auto const & swapChainImage : m_swapChainImages)
    {
        m_swapChainImageViews.emplace_back(ParentDevice().CreateTextureView(swapChainImage, m_swapChainImageFormat, vk::ImageAspectFlagBits::eColor, 1));
    }

    m_renderFinishedFences.reserve(m_swapChainImages.size());
    m_imageAvailableSemaphores.reserve(m_swapChainImages.size());
    m_renderFinishedSemaphores.reserve(m_swapChainImages.size());
    for (uint32_t frameIndex = 0; frameIndex < m_virtualFrameCount; ++frameIndex)
    {
        m_renderFinishedFences.emplace_back(ParentDevice().CreateFence(true));
        m_imageAvailableSemaphores.emplace_back(ParentDevice().CreateSemaphore());
        m_renderFinishedSemaphores.emplace_back(ParentDevice().CreateSemaphore());
    }

    m_swapChainImageToVirtualFrame.resize(m_swapChainImages.size(), std::numeric_limits<uint32_t>::max());
}
