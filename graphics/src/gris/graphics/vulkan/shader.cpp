#include <gris/graphics/vulkan/shader.h>

#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Shader::Shader(Device * device, const std::vector<char> & code)
    : DeviceResource(device)
{
    auto const createInfo = vk::ShaderModuleCreateInfo({}, code.size(), reinterpret_cast<const uint32_t *>(code.data()));

    auto createShaderModuleResult = DeviceHandle().createShaderModuleUnique(createInfo);
    if (createShaderModuleResult.result != vk::Result::eSuccess)
        throw VulkanEngineException("Error creating shader module", createShaderModuleResult);

    m_shaderModule = std::move(createShaderModuleResult.value);
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] const vk::ShaderModule & Gris::Graphics::Vulkan::Shader::ModuleHandle() const
{
    return m_shaderModule.get();
}

// -------------------------------------------------------------------------------------------------

// TODO: Do this better
[[nodiscard]] vk::ShaderModule & Gris::Graphics::Vulkan::Shader::ModuleHandle()
{
    return m_shaderModule.get();
}
