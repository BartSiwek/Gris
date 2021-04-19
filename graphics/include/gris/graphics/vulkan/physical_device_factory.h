#pragma once

namespace Gris::Graphics::Vulkan
{

class PhysicalDevice;
class WindowMixin;

[[nodiscard]] PhysicalDevice FindSuitablePhysicalDevice(const WindowMixin & window);

}  // namespace Gris::Graphics::Vulkan
