#include <gris/graphics/vulkan/instance.h>

#include "validation_layers.h"

#include <gris/graphics/vulkan/allocator.h>
#include <gris/graphics/vulkan/vma_headers.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

#include <gris/assert.h>
#include <gris/log.h>

#ifdef GRIS_CHRAPHICS_HAS_GLFW
#include "glfw/extension_getters.h"
#endif

// -------------------------------------------------------------------------------------------------

namespace
{

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             const VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
                                             void * /* pUserData */)
{
    std::string messageTypeLabel;
    switch (messageType)
    {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
        messageTypeLabel = "[General]";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
        messageTypeLabel = "[Validation]";
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
        messageTypeLabel = "[Performance]";
        break;
    default:
        messageTypeLabel = "[Unknown]";
        break;
    }

    switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        Gris::Log::Debug("{} {}", messageTypeLabel, pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        Gris::Log::Info("{} {}", messageTypeLabel, pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        Gris::Log::Warning("{} {}", messageTypeLabel, pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    default:
        Gris::Log::Error("{} {}", messageTypeLabel, pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
        GRIS_ALWAYS_ASSERT(false, "VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT is not a valid message severity");
        break;
    }

    return VK_FALSE;
}

}  // namespace

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Instance::Instance(Instance && other) noexcept
    : m_loader(std::exchange(other.m_loader, {}))
    , m_dispatch(std::exchange(other.m_dispatch, {}))
    , m_instance(std::exchange(other.m_instance, {}))
    , m_debugMessenger(std::exchange(other.m_debugMessenger, {}))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Instance & Gris::Graphics::Vulkan::Instance::operator=(Instance && other) noexcept
{
    if (this != &other)
    {
        ReleaseResources();

        m_loader = std::exchange(other.m_loader, {});
        m_dispatch = std::exchange(other.m_dispatch, {});
        m_instance = std::exchange(other.m_instance, {});
        m_debugMessenger = std::exchange(other.m_debugMessenger, {});
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Instance::~Instance()
{
    ReleaseResources();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::Instance & Gris::Graphics::Vulkan::Instance::InstanceHandle()
{
    return GetInstance().m_instance;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::DispatchLoaderDynamic & Gris::Graphics::Vulkan::Instance::Dispatch()
{
    return GetInstance().m_dispatch;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::DispatchLoaderDynamic Gris::Graphics::Vulkan::Instance::CreateDispatch(const vk::Device & device)
{
    auto & instance = GetInstance();

    vk::DispatchLoaderDynamic result;
    auto getInstanceProcAddr = instance.m_loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    auto getDeviceProcAddr = instance.m_loader.getProcAddress<PFN_vkGetDeviceProcAddr>("vkGetDeviceProcAddr");
    result.init(instance.m_instance, getInstanceProcAddr, device, getDeviceProcAddr);
    return result;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] Gris::Graphics::Vulkan::Allocator Gris::Graphics::Vulkan::Instance::CreateAllocator(const vk::PhysicalDevice & physicalDevice, const vk::Device & device, const vk::DispatchLoaderDynamic & dispatch)
{
    // Copy the function pointers from device dispatch to the VMA's internal structure
    auto vulkanFunctions = VmaVulkanFunctions{};
    vulkanFunctions.vkGetPhysicalDeviceProperties = dispatch.vkGetPhysicalDeviceProperties;
    vulkanFunctions.vkGetPhysicalDeviceMemoryProperties = dispatch.vkGetPhysicalDeviceMemoryProperties;
    vulkanFunctions.vkAllocateMemory = dispatch.vkAllocateMemory;
    vulkanFunctions.vkFreeMemory = dispatch.vkFreeMemory;
    vulkanFunctions.vkMapMemory = dispatch.vkMapMemory;
    vulkanFunctions.vkUnmapMemory = dispatch.vkUnmapMemory;
    vulkanFunctions.vkFlushMappedMemoryRanges = dispatch.vkFlushMappedMemoryRanges;
    vulkanFunctions.vkInvalidateMappedMemoryRanges = dispatch.vkInvalidateMappedMemoryRanges;
    vulkanFunctions.vkBindBufferMemory = dispatch.vkBindBufferMemory;
    vulkanFunctions.vkBindImageMemory = dispatch.vkBindImageMemory;
    vulkanFunctions.vkGetBufferMemoryRequirements = dispatch.vkGetBufferMemoryRequirements;
    vulkanFunctions.vkGetImageMemoryRequirements = dispatch.vkGetImageMemoryRequirements;
    vulkanFunctions.vkCreateBuffer = dispatch.vkCreateBuffer;
    vulkanFunctions.vkDestroyBuffer = dispatch.vkDestroyBuffer;
    vulkanFunctions.vkCreateImage = dispatch.vkCreateImage;
    vulkanFunctions.vkDestroyImage = dispatch.vkDestroyImage;
    vulkanFunctions.vkCmdCopyBuffer = dispatch.vkCmdCopyBuffer;
    vulkanFunctions.vkGetBufferMemoryRequirements2KHR = (GRIS_VULKAN_API_VERSION >= VK_MAKE_VERSION(1, 1, 0)) ? dispatch.vkGetBufferMemoryRequirements2 : dispatch.vkGetBufferMemoryRequirements2KHR;
    vulkanFunctions.vkGetImageMemoryRequirements2KHR = (GRIS_VULKAN_API_VERSION >= VK_MAKE_VERSION(1, 1, 0)) ? dispatch.vkGetImageMemoryRequirements2 : dispatch.vkGetImageMemoryRequirements2KHR;
    vulkanFunctions.vkBindBufferMemory2KHR = (GRIS_VULKAN_API_VERSION >= VK_MAKE_VERSION(1, 1, 0)) ? dispatch.vkBindBufferMemory2 : dispatch.vkBindBufferMemory2KHR;
    vulkanFunctions.vkBindImageMemory2KHR = (GRIS_VULKAN_API_VERSION >= VK_MAKE_VERSION(1, 1, 0)) ? dispatch.vkBindImageMemory2 : dispatch.vkBindImageMemory2KHR;
    vulkanFunctions.vkGetPhysicalDeviceMemoryProperties2KHR = (GRIS_VULKAN_API_VERSION >= VK_MAKE_VERSION(1, 1, 0)) ? dispatch.vkGetPhysicalDeviceMemoryProperties2 : dispatch.vkGetPhysicalDeviceMemoryProperties2KHR;

    // Create the VMA
    auto allocatorInfo = VmaAllocatorCreateInfo{};
    allocatorInfo.flags = {};
    allocatorInfo.physicalDevice = static_cast<VkPhysicalDevice>(physicalDevice);
    allocatorInfo.device = static_cast<VkDevice>(device);
    allocatorInfo.preferredLargeHeapBlockSize = 0;
    allocatorInfo.pAllocationCallbacks = nullptr;
    allocatorInfo.pDeviceMemoryCallbacks = nullptr;
    allocatorInfo.frameInUseCount = 0;
    allocatorInfo.pHeapSizeLimit = nullptr;
    allocatorInfo.pVulkanFunctions = &vulkanFunctions;
    allocatorInfo.pRecordSettings = nullptr;
    allocatorInfo.instance = static_cast<VkInstance>(GetInstance().m_instance);
    allocatorInfo.vulkanApiVersion = GRIS_VULKAN_API_VERSION;

    VmaAllocator allocator = {};
    auto const allocatorCreateResult = static_cast<vk::Result>(vmaCreateAllocator(&allocatorInfo, &allocator));
    if (allocatorCreateResult != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating Vulkan Memory Allocator", allocatorCreateResult);
    }

    return Allocator(allocator);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] std::vector<vk::PhysicalDevice> Gris::Graphics::Vulkan::Instance::EnumeratePhysicalDevices()
{
    auto & instance = GetInstance();

    auto const enumeratePhysicalDevicesResult = instance.m_instance.enumeratePhysicalDevices(instance.m_dispatch);
    if (enumeratePhysicalDevicesResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error enumerating physical devices", enumeratePhysicalDevicesResult);
    }

    return enumeratePhysicalDevicesResult.value;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] std::vector<const char *> Gris::Graphics::Vulkan::Instance::GetRequiredExtensions()
{
    auto extensions = std::vector<const char *>{};

#ifdef GRIS_CHRAPHICS_HAS_GLFW
    Glfw::GetInstanceExtensionsFromGLFW(&extensions);
#endif

    if constexpr (ENABLE_VALIDATION_LAYERS)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Instance & Gris::Graphics::Vulkan::Instance::GetInstance()
{
    static Instance s_instance = {};
    return s_instance;
}


// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Instance::Instance()
{
    CreateInstance();
    SetupDebugMessenger();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Instance::CreateInstance()
{
    auto vkGetInstanceProcAddr = m_loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    m_dispatch.init(vkGetInstanceProcAddr);

    if constexpr (ENABLE_VALIDATION_LAYERS)
    {
        auto const validationLayerSupport = CheckValidationLayerSupport();
        if (!validationLayerSupport)
        {
            throw VulkanEngineException("Requested validation layers not found");
        }
    }

    auto const appInfo = vk::ApplicationInfo{}
                             .setPApplicationName("Vulkan tutorial")
                             .setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
                             .setPEngineName("No Engine")
                             .setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
                             .setApiVersion(GRIS_VULKAN_API_VERSION);

    std::vector<const char *> enabledLayers;
    if constexpr (ENABLE_VALIDATION_LAYERS)
    {
        enabledLayers.insert(enabledLayers.begin(), VALIDATION_LAYERS.begin(), VALIDATION_LAYERS.end());
    }

    auto extensions = GetRequiredExtensions();
    const auto createInfo = vk::InstanceCreateInfo{}
                                .setPApplicationInfo(&appInfo)
                                .setPEnabledLayerNames(enabledLayers)
                                .setPEnabledExtensionNames(extensions);

    auto createInstanceResult = vk::createInstance(createInfo, nullptr, m_dispatch);
    if (createInstanceResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating Vulkan instance", createInstanceResult);
    }

    m_instance = std::move(createInstanceResult.value);
    m_dispatch.init(m_instance);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Instance::SetupDebugMessenger()
{
    if constexpr (!ENABLE_VALIDATION_LAYERS)
    {
        return;
    }

    auto const createInfo = vk::DebugUtilsMessengerCreateInfoEXT{}
                                .setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
                                .setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
                                .setPfnUserCallback(&DebugCallback);

    auto createDebugUtilsMessengerResult = m_instance.createDebugUtilsMessengerEXT(createInfo, nullptr, m_dispatch);
    if (createDebugUtilsMessengerResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating debug messenger", createDebugUtilsMessengerResult);
    }

    m_debugMessenger = std::move(createDebugUtilsMessengerResult.value);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::Instance::CheckValidationLayerSupport()
{
    auto const enumerateInstanceLayerPropertiesResult = vk::enumerateInstanceLayerProperties(m_dispatch);
    if (enumerateInstanceLayerPropertiesResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error enumerating instance layer properties", enumerateInstanceLayerPropertiesResult);
    }

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
        {
            return false;
        }
    }

    return true;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Instance::ReleaseResources()
{
    if (m_debugMessenger)
    {
        m_instance.destroyDebugUtilsMessengerEXT(m_debugMessenger, nullptr, m_dispatch);
        m_debugMessenger = nullptr;
    }

    if (m_instance)
    {
        m_instance.destroy(nullptr, m_dispatch);
        m_instance = nullptr;
    }
}

