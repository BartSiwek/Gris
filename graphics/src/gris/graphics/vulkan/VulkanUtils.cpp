#include <gris/graphics/vulkan/VulkanUtils.h>

#include <gris/graphics/vulkan/VulkanEngineException.h>

#include <fstream>

// -------------------------------------------------------------------------------------------------

[[nodiscard]] SwapChainSupportDetails QuerySwapChainSupport(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface)
{
    auto const surfaceCapabilitiesResult = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    if (surfaceCapabilitiesResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error getting surface capabilities", surfaceCapabilitiesResult);

    auto const surfaceFormatsResult = physicalDevice.getSurfaceFormatsKHR(surface);
    if (surfaceFormatsResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error getting surface formats", surfaceFormatsResult);

    auto const surfacePresentModesResult = physicalDevice.getSurfacePresentModesKHR(surface);
    if (surfacePresentModesResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error getting surface present modes", surfacePresentModesResult);

    return SwapChainSupportDetails{ surfaceCapabilitiesResult.value, surfaceFormatsResult.value, surfacePresentModesResult.value };
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] std::vector<char> ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw VulkanEngineException("Failed to open file!");
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}
