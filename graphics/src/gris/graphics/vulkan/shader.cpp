#include <gris/graphics/vulkan/shader.h>

#include <gris/graphics/vulkan/vulkan_engine_exception.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Shader::Shader() = default;

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Shader::Shader(const ParentObject<Device> & device, const std::vector<uint32_t> & code, std::string entryPoint)
    : DeviceResource(device)
    , m_entryPoint(std::move(entryPoint))
{
    GRIS_ALWAYS_ASSERT(!m_entryPoint.empty(), "Shader entry point is empty");

    auto const createInfo = vk::ShaderModuleCreateInfo{}.setCode(code);

    auto createShaderModuleResult = DeviceHandle().createShaderModule(createInfo, nullptr, Dispatch());
    if (createShaderModuleResult.result != vk::Result::eSuccess)
    {
        throw VulkanEngineException("Error creating shader module", createShaderModuleResult);
    }

    m_shaderModule = std::move(createShaderModuleResult.value);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Shader::Shader(Shader && other) noexcept
    : DeviceResource(std::move(other))
    , m_shaderModule(std::exchange(other.m_shaderModule, {}))
    , m_entryPoint(std::exchange(other.m_entryPoint, {}))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Shader & Gris::Graphics::Vulkan::Shader::operator=(Shader && other) noexcept
{
    if (this != &other)
    {
        ReleaseResources();

        DeviceResource::operator=(std::move(other));
        m_shaderModule = std::exchange(other.m_shaderModule, {});
        m_entryPoint = std::exchange(other.m_entryPoint, {});
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Shader::~Shader()
{
    ReleaseResources();
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Vulkan::Shader::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Vulkan::Shader::IsValid() const
{
    return DeviceResource::IsValid() && static_cast<bool>(m_shaderModule);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const vk::ShaderModule & Gris::Graphics::Vulkan::Shader::ModuleHandle() const
{
    return m_shaderModule;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] vk::ShaderModule & Gris::Graphics::Vulkan::Shader::ModuleHandle()
{
    return m_shaderModule;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const std::string & Gris::Graphics::Vulkan::Shader::EntryPoint() const
{
    return m_entryPoint;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Shader::Reset()
{
    m_entryPoint.clear();
    ReleaseResources();
    ResetParent();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Vulkan::Shader::ReleaseResources()
{
    if (m_shaderModule)
    {
        DeviceHandle().destroyShaderModule(m_shaderModule, nullptr, Dispatch());
        m_shaderModule = nullptr;
    }
}
