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
    [[nodiscard]] static Instance & Get();

    ~Instance() = default;

    Instance(const Instance &) = delete;
    Instance & operator=(const Instance &) = delete;

    Instance(Instance &&) noexcept = delete;
    Instance & operator=(Instance &&) noexcept = delete;

    [[nodiscard]] const vk::Instance & InstanceHandle(InstanceHandleBadge badge) const;
    [[nodiscard]] vk::Instance & InstanceHandle(InstanceHandleBadge badge);

    [[nodiscard]] const vk::DispatchLoaderDynamic & Dispatch() const;
    [[nodiscard]] vk::DispatchLoaderDynamic & Dispatch();

    [[nodiscard]] vk::DispatchLoaderDynamic CreateDispatch(const vk::Device & device);
    [[nodiscard]] Allocator CreateAllocator(const vk::PhysicalDevice & physicalDevice, const vk::Device & device, const vk::DispatchLoaderDynamic & dispatch);

    [[nodiscard]] std::vector<vk::PhysicalDevice> EnumeratePhysicalDevices();

private:
    [[nodiscard]] static std::vector<const char *> GetRequiredExtensions();

    Instance();

    void CreateInstance();
    void SetupDebugMessenger();

    [[nodiscard]] bool CheckValidationLayerSupport();

    vk::UniqueInstance m_instance = {};
    vk::DynamicLoader m_loader = {};
    vk::DispatchLoaderDynamic m_dispatch;
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> m_debugMessenger = {};
};

}  // namespace Gris::Graphics::Vulkan