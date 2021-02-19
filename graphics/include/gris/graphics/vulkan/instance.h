#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

namespace Gris::Graphics::Vulkan
{

class Allocator;
class WindowMixin;

class Instance
{
public:
    Instance(const Instance &) = delete;
    Instance & operator=(const Instance &) = delete;

    Instance(Instance && other) noexcept;
    Instance & operator=(Instance && other) noexcept;

    ~Instance();

    [[nodiscard]] static vk::Instance & InstanceHandle();
    [[nodiscard]] static vk::DispatchLoaderDynamic & Dispatch();

    [[nodiscard]] static vk::DispatchLoaderDynamic CreateDispatch(const vk::Device & device);
    [[nodiscard]] static Allocator CreateAllocator(const vk::PhysicalDevice & physicalDevice, const vk::Device & device, const vk::DispatchLoaderDynamic & dispatch);

    [[nodiscard]] static std::vector<vk::PhysicalDevice> EnumeratePhysicalDevices();

private:
    [[nodiscard]] static std::vector<const char *> GetRequiredExtensions();

    [[nodiscard]] static Instance & GetInstance();

    Instance();

    void CreateInstance();
    void SetupDebugMessenger();

    [[nodiscard]] bool CheckValidationLayerSupport();

    void Reset();

    vk::DynamicLoader m_loader = {};
    vk::DispatchLoaderDynamic m_dispatch = {};
    vk::Instance m_instance = {};
    vk::DebugUtilsMessengerEXT m_debugMessenger = {};
};

}  // namespace Gris::Graphics::Vulkan