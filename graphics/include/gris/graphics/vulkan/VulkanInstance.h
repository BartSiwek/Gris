#pragma once

#define VULKAN_HPP_TYPESAFE_CONVERSION
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

class VulkanAllocator;
class VulkanWindowMixin;

class VulkanInstanceHandleBadge
{
public:
    friend class VulkanWindowMixin;

private:
    VulkanInstanceHandleBadge() {}
};

class VulkanInstance
{
public:
    using ExtensionGetter = std::vector<const char*>(*)();

    static void InstallExtensionGetter(ExtensionGetter getter);

    [[nodiscard]] static vk::Instance InstanceHandle(VulkanInstanceHandleBadge badge);
    [[nodiscard]] static std::vector<vk::PhysicalDevice> EnumeratePhysicalDevices();
    [[nodiscard]] static VulkanAllocator CreateVulkanMemoryAllocator(const vk::PhysicalDevice& physicalDevice, const vk::Device& device);

    VulkanInstance(const VulkanInstance&) = delete;
    VulkanInstance& operator=(const VulkanInstance&) = delete;

    VulkanInstance(VulkanInstance&&) noexcept = delete;
    VulkanInstance& operator=(VulkanInstance&&) noexcept = delete;

    ~VulkanInstance() = default;

private:
    [[nodiscard]] static VulkanInstance& Instance();

    [[nodiscard]] static std::vector<const char*> GetRequiredExtensions();

    static ExtensionGetter s_extensionGetter;

    VulkanInstance();

    void CreateInstance();
    void SetupDebugMessenger();

    [[nodiscard]] bool CheckValidationLayerSupport() const;

    vk::UniqueInstance m_instance = {};
    vk::DispatchLoaderDynamic m_dispatch = {};
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> m_debugMessenger = {};
};