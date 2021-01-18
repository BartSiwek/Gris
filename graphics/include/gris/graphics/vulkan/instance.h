#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

namespace Gris::Graphics::Vulkan
{

class Allocator;
class WindowMixin;

class InstanceHandleBadge
{
public:
    friend class WindowMixin;

private:
    InstanceHandleBadge()
    {
    }
};

class Instance
{
public:
    using ExtensionGetter = std::vector<const char *> (*)();

    static void InstallExtensionGetter(ExtensionGetter getter);

    [[nodiscard]] static vk::Instance InstanceHandle(InstanceHandleBadge badge);
    [[nodiscard]] static std::vector<vk::PhysicalDevice> EnumeratePhysicalDevices();
    [[nodiscard]] static Allocator CreateAllocator(const vk::PhysicalDevice & physicalDevice, const vk::Device & device);

    ~Instance() = default;

    Instance(const Instance &) = delete;
    Instance & operator=(const Instance &) = delete;

    Instance(Instance &&) noexcept = delete;
    Instance & operator=(Instance &&) noexcept = delete;

private:
    [[nodiscard]] static Instance & GetInstance();

    [[nodiscard]] static std::vector<const char *> GetRequiredExtensions();
    [[nodiscard]] static bool CheckValidationLayerSupport();

    static ExtensionGetter s_extensionGetter;

    Instance();

    void CreateInstance();
    void SetupDebugMessenger();

    vk::UniqueInstance m_instance = {};
    vk::DispatchLoaderDynamic m_dispatch = {};
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> m_debugMessenger = {};
};

}  // namespace Gris::Graphics::Vulkan