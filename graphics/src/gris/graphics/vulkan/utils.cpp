#include <gris/graphics/vulkan/utils.h>

#include <gris/graphics/vulkan/instance.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::SwapChainSupportDetails Gris::Graphics::Vulkan::QuerySwapChainSupport(const vk::PhysicalDevice & physicalDevice,
                                                                                                            const vk::SurfaceKHR & surface)
{
    using namespace Vulkan;

    auto const surfaceCapabilitiesResult = physicalDevice.getSurfaceCapabilitiesKHR(surface, Instance::Dispatch());
    if (surfaceCapabilitiesResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error getting surface capabilities", surfaceCapabilitiesResult);
    }

    auto const surfaceFormatsResult = physicalDevice.getSurfaceFormatsKHR(surface, Instance::Dispatch());
    if (surfaceFormatsResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error getting surface formats", surfaceFormatsResult);
    }

    auto const surfacePresentModesResult = physicalDevice.getSurfacePresentModesKHR(surface, Instance::Dispatch());
    if (surfacePresentModesResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error getting surface present modes", surfacePresentModesResult);
    }

    return SwapChainSupportDetails{ surfaceCapabilitiesResult.value,
                                    surfaceFormatsResult.value,
                                    surfacePresentModesResult.value };
}
