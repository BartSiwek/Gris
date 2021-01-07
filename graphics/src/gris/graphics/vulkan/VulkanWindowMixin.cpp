#include <gris/graphics/vulkan/VulkanWindowMixin.h>

#include <gris/graphics/vulkan/VulkanInstance.h>

// -------------------------------------------------------------------------------------------------

vk::Instance Gris::Graphics::Vulkan::VulkanWindowMixin::VulkanInstanceHandle()
{
    return VulkanInstance::InstanceHandle({});
}
