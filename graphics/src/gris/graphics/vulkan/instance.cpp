#include <gris/graphics/vulkan/instance.h>

#include "validation_layers.h"

#include <gris/graphics/vulkan/allocator.h>
#include <gris/graphics/vulkan/engine_exception.h>
#include <gris/graphics/vulkan/vma_headers.h>

#include <gris/assert.h>

#include <iostream>

// -------------------------------------------------------------------------------------------------

namespace
{

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             const VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
                                             void * /* pUserData */)
{

    switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        std::cerr << "[Verbose] ";
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        std::cerr << "[Info] ";
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        std::cerr << "[Warning] ";
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        std::cerr << "[Error] ";
        break;
    default:
        std::cerr << "[Unknown] ";
        break;
    }

    switch (messageType)
    {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
        std::cerr << "[General] ";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
        std::cerr << "[Validation] ";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
        std::cerr << "[Performance] ";
        break;
    default:
        std::cerr << "[Unknown] ";
        break;
    }

    std::cerr << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

}  // namespace

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanInstance::ExtensionGetter Gris::Graphics::Vulkan::VulkanInstance::s_extensionGetter = nullptr;

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanInstance::InstallExtensionGetter(ExtensionGetter getter)
{
    s_extensionGetter = getter;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Instance Gris::Graphics::Vulkan::VulkanInstance::InstanceHandle(VulkanInstanceHandleBadge /* badge */)
{
    return Instance().m_instance.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] std::vector<vk::PhysicalDevice> Gris::Graphics::Vulkan::VulkanInstance::EnumeratePhysicalDevices()
{
    auto const enumeratePhysicalDevicesResult = Instance().m_instance->enumeratePhysicalDevices();
    if (enumeratePhysicalDevicesResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error enumerating physical devices", enumeratePhysicalDevicesResult);
    return enumeratePhysicalDevicesResult.value;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::VulkanAllocator Gris::Graphics::Vulkan::VulkanInstance::CreateVulkanMemoryAllocator(const vk::PhysicalDevice & physicalDevice, const vk::Device & device)
{
    auto allocatorInfo = VmaAllocatorCreateInfo{};
    allocatorInfo.flags = {};
    allocatorInfo.physicalDevice = static_cast<VkPhysicalDevice>(physicalDevice);
    allocatorInfo.device = static_cast<VkDevice>(device);
    allocatorInfo.preferredLargeHeapBlockSize = 0;
    allocatorInfo.pAllocationCallbacks = nullptr;
    allocatorInfo.pDeviceMemoryCallbacks = nullptr;
    allocatorInfo.frameInUseCount = 0;  // TODO: Easy to set this correctly
    allocatorInfo.pHeapSizeLimit = nullptr;
    allocatorInfo.pVulkanFunctions = nullptr;
    allocatorInfo.pRecordSettings = nullptr;
    allocatorInfo.instance = static_cast<VkInstance>(Instance().m_instance.get());
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_0;

    VmaAllocator allocator;
    auto const allocatorCreateResult = static_cast<vk::Result>(vmaCreateAllocator(&allocatorInfo, &allocator));
    if (allocatorCreateResult != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating Vulkan Memory Allocator", allocatorCreateResult);

    return VulkanAllocator(allocator);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanInstance & Gris::Graphics::Vulkan::VulkanInstance::Instance()
{
    static VulkanInstance s_instance = {};
    return s_instance;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] std::vector<const char *> Gris::Graphics::Vulkan::VulkanInstance::GetRequiredExtensions()
{
    GRIS_ALAWYS_ASSERT(s_extensionGetter != nullptr, "Extension getter cannot be null");
    auto extensions = s_extensionGetter();

    if constexpr (ENABLE_VALIDATION_LAYERS)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::VulkanInstance::VulkanInstance()
{
    CreateInstance();
    SetupDebugMessenger();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanInstance::CreateInstance()
{
    if constexpr (ENABLE_VALIDATION_LAYERS)
    {
        auto const validationLayerSupport = CheckValidationLayerSupport();
        if (!validationLayerSupport)
            throw VulkanEngineException("Requested validation layers not found");
    }

    vk::ApplicationInfo appInfo("Vulkan tutorial",
                                VK_MAKE_VERSION(1, 0, 0),
                                "No Engine",
                                VK_MAKE_VERSION(1, 0, 0),
                                VK_API_VERSION_1_0);

    std::vector<const char *> enabledLayers;
    if constexpr (ENABLE_VALIDATION_LAYERS)
        enabledLayers.insert(enabledLayers.begin(), VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end());

    auto extensions = GetRequiredExtensions();
    const vk::InstanceCreateInfo createInfo(vk::InstanceCreateFlags{}, &appInfo, enabledLayers, extensions);

    auto createInstanceResult = vk::createInstanceUnique(createInfo);
    if (createInstanceResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating Vulkan instance", createInstanceResult);

    m_instance = std::move(createInstanceResult.value);
    m_dispatch = vk::DispatchLoaderDynamic(m_instance.get(), vkGetInstanceProcAddr);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::VulkanInstance::SetupDebugMessenger()
{
    if constexpr (!ENABLE_VALIDATION_LAYERS)
        return;

    auto const createInfo = vk::DebugUtilsMessengerCreateInfoEXT({},
                                                                 vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
                                                                 vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
                                                                 &DebugCallback,
                                                                 nullptr);

    auto createDebugUtilsMessengerResult = m_instance->createDebugUtilsMessengerEXTUnique(createInfo, nullptr, m_dispatch);
    if (createDebugUtilsMessengerResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating debug messenger", createDebugUtilsMessengerResult);

    m_debugMessenger = std::move(createDebugUtilsMessengerResult.value);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::VulkanInstance::CheckValidationLayerSupport() const
{
    auto const enumerateInstanceLayerPropertiesResult = vk::enumerateInstanceLayerProperties();
    if (enumerateInstanceLayerPropertiesResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error enumerating instance layer properties", enumerateInstanceLayerPropertiesResult);

    auto const & availableLayers = enumerateInstanceLayerPropertiesResult.value;

    for (auto const & layerName : VALIDATION_LAYERS)
    {
        auto layerFound = false;

        for (auto const & layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
    }

    return true;
}
