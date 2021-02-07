#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

namespace Gris::Graphics::Vulkan
{

class Allocator;
class WindowMixin;

class Instance
{
public:
    ~Instance() = default;

    Instance(const Instance &) = delete;
    Instance & operator=(const Instance &) = delete;

    Instance(Instance &&) noexcept = default;
    Instance & operator=(Instance &&) noexcept = default;

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

    vk::UniqueInstance m_instance = {};
    vk::DynamicLoader m_loader = {};
    vk::DispatchLoaderDynamic m_dispatch = {};
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> m_debugMessenger = {};
};

}  // namespace Gris::Graphics::Vulkan