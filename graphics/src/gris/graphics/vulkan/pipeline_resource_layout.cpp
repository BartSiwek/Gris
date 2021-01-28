#include <gris/graphics/vulkan/pipeline_resource_layout.h>

#include <gris/graphics/vulkan/device.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::PipelineResourceLayout::PipelineResourceLayout(Device * device, const Span<const vk::DescriptorSetLayoutBinding> & bindings)
    : DeviceResource(device)
{
    auto const layoutInfo = vk::DescriptorSetLayoutCreateInfo{}.setBindings({ static_cast<uint32_t>(bindings.size()), bindings.data() });

    auto createDescriptorSetLayoutResult = DeviceHandle().createDescriptorSetLayoutUnique(layoutInfo, nullptr, Dispatch());
    if (createDescriptorSetLayoutResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating descriptor set layout", createDescriptorSetLayoutResult);
    }

    m_descriptorSetLayout = std::move(createDescriptorSetLayoutResult.value);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::DescriptorSetLayout & Gris::Graphics::Vulkan::PipelineResourceLayout::DescriptorSetLayoutHandle() const
{
    return m_descriptorSetLayout.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::DescriptorSetLayout & Gris::Graphics::Vulkan::PipelineResourceLayout::DescriptorSetLayoutHandle()
{
    return m_descriptorSetLayout.get();
}
