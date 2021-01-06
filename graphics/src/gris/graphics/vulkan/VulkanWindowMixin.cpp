#include <gris/graphics/vulkan/VulkanWindowMixin.h>

#include <gris/graphics/vulkan/VulkanInstance.h>

// -------------------------------------------------------------------------------------------------

vk::Instance VulkanWindowMixin::VulkanInstanceHandle()
{
    return VulkanInstance::InstanceHandle({});
}
