#include <gris/graphics/vulkan/shader.h>

#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Shader::Shader(Device * device, const std::vector<uint32_t> & code, std::string entryPoint)
    : DeviceResource(device)
    , m_entryPoint(std::move(entryPoint))
{
    auto const createInfo = vk::ShaderModuleCreateInfo{}.setCode(code);

    auto createShaderModuleResult = DeviceHandle().createShaderModuleUnique(createInfo, nullptr, Dispatch());
    if (createShaderModuleResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating shader module", createShaderModuleResult);
    }

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

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const std::string & Gris::Graphics::Vulkan::Shader::EntryPoint() const
{
    return m_entryPoint;
}
