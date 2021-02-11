#pragma once

#include <gris/graphics/vulkan/utils.h>

#include <gris/graphics/vulkan/vulkan_headers.h>

namespace Gris::Graphics::Vulkan
{

class PhysicalDevice;
class WindowMixin;

[[nodiscard]] PhysicalDevice FindSuitablePhysicalDevice(const WindowMixin & window);

}  // namespace Gris::Graphics::Vulkan
