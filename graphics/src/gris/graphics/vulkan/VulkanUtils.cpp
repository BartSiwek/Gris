#include <gris/graphics/vulkan/VulkanUtils.h>

#include <gris/graphics/vulkan/VulkanEngineException.h>

#include <fstream>

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::SwapChainSupportDetails Gris::Graphics::Vulkan::QuerySwapChainSupport(const vk::PhysicalDevice & physicalDevice,
                                                                                                            const vk::SurfaceKHR & surface)
{
    using namespace Vulkan;

    auto const surfaceCapabilitiesResult = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    if (surfaceCapabilitiesResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error getting surface capabilities", surfaceCapabilitiesResult);

    auto const surfaceFormatsResult = physicalDevice.getSurfaceFormatsKHR(surface);
    if (surfaceFormatsResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error getting surface formats", surfaceFormatsResult);

    auto const surfacePresentModesResult = physicalDevice.getSurfacePresentModesKHR(surface);
    if (surfacePresentModesResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error getting surface present modes", surfacePresentModesResult);

    return SwapChainSupportDetails{ surfaceCapabilitiesResult.value,
                                    surfaceFormatsResult.value,
                                    surfacePresentModesResult.value };
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] std::vector<char> Gris::Graphics::Vulkan::ReadFile(const std::string & filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw EngineException("Failed to open file!");
    }

    auto fileSize = file.tellg();
    std::vector<char> buffer(static_cast<size_t>(fileSize));

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}
