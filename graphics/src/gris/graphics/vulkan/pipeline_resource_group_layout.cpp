#include <gris/graphics/vulkan/pipeline_resource_group_layout.h>

#include <gris/graphics/vulkan/device.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

#include <gris/casts.h>
#include <gris/utils.h>

// -------------------------------------------------------------------------------------------------

static_assert(std::is_same_v<std::underlying_type_t<Gris::Graphics::Backend::PipelineResourceType>, std::underlying_type_t<vk::DescriptorType>>);

static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::PipelineResourceType::Sampler) == Gris::UnderlyingCast(vk::DescriptorType::eSampler));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::PipelineResourceType::CombinedImageSampler) == Gris::UnderlyingCast(vk::DescriptorType::eCombinedImageSampler));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::PipelineResourceType::SampledImage) == Gris::UnderlyingCast(vk::DescriptorType::eSampledImage));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::PipelineResourceType::StorageImage) == Gris::UnderlyingCast(vk::DescriptorType::eStorageImage));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::PipelineResourceType::UniformTexelBuffer) == Gris::UnderlyingCast(vk::DescriptorType::eUniformTexelBuffer));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::PipelineResourceType::StorageTexelBuffer) == Gris::UnderlyingCast(vk::DescriptorType::eStorageTexelBuffer));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::PipelineResourceType::UniformBuffer) == Gris::UnderlyingCast(vk::DescriptorType::eUniformBuffer));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::PipelineResourceType::StorageBuffer) == Gris::UnderlyingCast(vk::DescriptorType::eStorageBuffer));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::PipelineResourceType::UniformBufferDynamic) == Gris::UnderlyingCast(vk::DescriptorType::eUniformBufferDynamic));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::PipelineResourceType::StorageBufferDynamic) == Gris::UnderlyingCast(vk::DescriptorType::eStorageBufferDynamic));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::PipelineResourceType::InputAttachment) == Gris::UnderlyingCast(vk::DescriptorType::eInputAttachment));

// -------------------------------------------------------------------------------------------------

static_assert(std::is_same_v<std::underlying_type_t<Gris::Graphics::Backend::PipelineStageFlags>, std::underlying_type_t<vk::ShaderStageFlagBits>>);

static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::PipelineStageFlags::Vertex) == Gris::UnderlyingCast(vk::ShaderStageFlagBits::eVertex));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::PipelineStageFlags::Hull) == Gris::UnderlyingCast(vk::ShaderStageFlagBits::eTessellationControl));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::PipelineStageFlags::Domain) == Gris::UnderlyingCast(vk::ShaderStageFlagBits::eTessellationEvaluation));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::PipelineStageFlags::Geometry) == Gris::UnderlyingCast(vk::ShaderStageFlagBits::eGeometry));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::PipelineStageFlags::Fragment) == Gris::UnderlyingCast(vk::ShaderStageFlagBits::eFragment));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::PipelineStageFlags::Compute) == Gris::UnderlyingCast(vk::ShaderStageFlagBits::eCompute));

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::PipelineResourceGroupLayout::PipelineResourceGroupLayout(Device * device, const Gris::Graphics::Backend::PipelineResourceGroupLayoutDesc & desc)
    : DeviceResource(device)
{
    auto bindings = Gris::MakeReservedVector<vk::DescriptorSetLayoutBinding>(desc.ResourceLayouts.size());
    for (const auto & resourceLayout : desc.ResourceLayouts)
    {
        auto binding = vk::DescriptorSetLayoutBinding{}
                           .setBinding(resourceLayout.Binding)
                           .setDescriptorType(static_cast<vk::DescriptorType>(resourceLayout.ResourceType))
                           .setDescriptorCount(resourceLayout.Count)
                           .setStageFlags(static_cast<vk::ShaderStageFlagBits>(resourceLayout.Stages));
        m_nameToBinding[std::string{ resourceLayout.Semantic }] = binding;
        bindings.emplace_back(std::move(binding));
    }

    auto const layoutInfo = vk::DescriptorSetLayoutCreateInfo{}.setBindings(bindings);

    auto createDescriptorSetLayoutResult = DeviceHandle().createDescriptorSetLayoutUnique(layoutInfo, nullptr, Dispatch());
    if (createDescriptorSetLayoutResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating descriptor set layout", createDescriptorSetLayoutResult);
    }

    m_descriptorSetLayout = std::move(createDescriptorSetLayoutResult.value);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::DescriptorSetLayout & Gris::Graphics::Vulkan::PipelineResourceGroupLayout::DescriptorSetLayoutHandle() const
{
    return m_descriptorSetLayout.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::DescriptorSetLayout & Gris::Graphics::Vulkan::PipelineResourceGroupLayout::DescriptorSetLayoutHandle()
{
    return m_descriptorSetLayout.get();
}

// -------------------------------------------------------------------------------------------------

const vk::DescriptorSetLayoutBinding & Gris::Graphics::Vulkan::PipelineResourceGroupLayout::NameToBinding(const std::string_view & name) const
{
    auto it = m_nameToBinding.find(name);
    return it->second;
}
