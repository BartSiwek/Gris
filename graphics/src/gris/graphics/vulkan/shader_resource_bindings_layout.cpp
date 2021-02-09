#include <gris/graphics/vulkan/shader_resource_bindings_layout.h>

#include <gris/graphics/vulkan/device.h>
#include <gris/graphics/vulkan/vulkan_engine_exception.h>

#include <gris/casts.h>
#include <gris/utils.h>

// -------------------------------------------------------------------------------------------------

static_assert(std::is_same_v<std::underlying_type_t<Gris::Graphics::Backend::ShaderResourceType>, std::underlying_type_t<vk::DescriptorType>>);

static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::ShaderResourceType::Sampler) == Gris::UnderlyingCast(vk::DescriptorType::eSampler));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::ShaderResourceType::CombinedImageSampler) == Gris::UnderlyingCast(vk::DescriptorType::eCombinedImageSampler));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::ShaderResourceType::SampledImage) == Gris::UnderlyingCast(vk::DescriptorType::eSampledImage));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::ShaderResourceType::StorageImage) == Gris::UnderlyingCast(vk::DescriptorType::eStorageImage));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::ShaderResourceType::UniformTexelBuffer) == Gris::UnderlyingCast(vk::DescriptorType::eUniformTexelBuffer));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::ShaderResourceType::StorageTexelBuffer) == Gris::UnderlyingCast(vk::DescriptorType::eStorageTexelBuffer));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::ShaderResourceType::UniformBuffer) == Gris::UnderlyingCast(vk::DescriptorType::eUniformBuffer));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::ShaderResourceType::StorageBuffer) == Gris::UnderlyingCast(vk::DescriptorType::eStorageBuffer));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::ShaderResourceType::UniformBufferDynamic) == Gris::UnderlyingCast(vk::DescriptorType::eUniformBufferDynamic));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::ShaderResourceType::StorageBufferDynamic) == Gris::UnderlyingCast(vk::DescriptorType::eStorageBufferDynamic));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::ShaderResourceType::InputAttachment) == Gris::UnderlyingCast(vk::DescriptorType::eInputAttachment));

// -------------------------------------------------------------------------------------------------

static_assert(std::is_same_v<std::underlying_type_t<Gris::Graphics::Backend::ShaderStageFlags>, std::underlying_type_t<vk::ShaderStageFlagBits>>);

static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::ShaderStageFlags::Vertex) == Gris::UnderlyingCast(vk::ShaderStageFlagBits::eVertex));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::ShaderStageFlags::Hull) == Gris::UnderlyingCast(vk::ShaderStageFlagBits::eTessellationControl));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::ShaderStageFlags::Domain) == Gris::UnderlyingCast(vk::ShaderStageFlagBits::eTessellationEvaluation));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::ShaderStageFlags::Geometry) == Gris::UnderlyingCast(vk::ShaderStageFlagBits::eGeometry));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::ShaderStageFlags::Fragment) == Gris::UnderlyingCast(vk::ShaderStageFlagBits::eFragment));
static_assert(Gris::UnderlyingCast(Gris::Graphics::Backend::ShaderStageFlags::Compute) == Gris::UnderlyingCast(vk::ShaderStageFlagBits::eCompute));

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindingsLayout::ShaderResourceBindingsLayout() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::ShaderResourceBindingsLayout::ShaderResourceBindingsLayout(std::shared_ptr<DeviceSharedData> sharedData, const Gris::Graphics::Backend::ShaderResourceBindingsLayout & bindingsLayout)
    : DeviceResource(std::move(sharedData))
{
    auto bindings = Gris::MakeReservedVector<vk::DescriptorSetLayoutBinding>(bindingsLayout.Layouts.size());
    for (const auto & resourceLayout : bindingsLayout.Layouts)
    {
        auto binding = vk::DescriptorSetLayoutBinding{}
                           .setBinding(resourceLayout.Binding)
                           .setDescriptorType(static_cast<vk::DescriptorType>(resourceLayout.Type))
                           .setDescriptorCount(resourceLayout.Count)
                           .setStageFlags(static_cast<vk::ShaderStageFlagBits>(resourceLayout.Stages));
        m_nameToBinding.emplace_back(NameAndBinding{ std::string(resourceLayout.Semantic), binding });
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

Gris::Graphics::Vulkan::ShaderResourceBindingsLayout::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::ShaderResourceBindingsLayout::IsValid() const
{
    return DeviceResource::IsValid() && static_cast<bool>(m_descriptorSetLayout);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::DescriptorSetLayout & Gris::Graphics::Vulkan::ShaderResourceBindingsLayout::DescriptorSetLayoutHandle() const
{
    return m_descriptorSetLayout.get();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::DescriptorSetLayout & Gris::Graphics::Vulkan::ShaderResourceBindingsLayout::DescriptorSetLayoutHandle()
{
    return m_descriptorSetLayout.get();
}

// -------------------------------------------------------------------------------------------------

const vk::DescriptorSetLayoutBinding & Gris::Graphics::Vulkan::ShaderResourceBindingsLayout::NameToBinding(const std::string_view & name) const
{
    auto it = std::find_if(std::begin(m_nameToBinding), std::end(m_nameToBinding), [&name](const auto & entry)
                           { return entry.Name == name; });
    GRIS_ALWAYS_ASSERT(it != std::end(m_nameToBinding), "Request binding was not found in the layout");
    return it->Binding;
}
