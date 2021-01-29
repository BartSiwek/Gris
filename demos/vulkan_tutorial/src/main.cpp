#include <gris/directory_registry.h>
#include <gris/engine_exception.h>
#include <gris/graphics/glfw/instance.h>
#include <gris/graphics/vulkan/buffer.h>
#include <gris/graphics/vulkan/buffer_view.h>
#include <gris/graphics/vulkan/deferred_context.h>
#include <gris/graphics/vulkan/device.h>
#include <gris/graphics/vulkan/framebuffer.h>
#include <gris/graphics/vulkan/glfw/window.h>
#include <gris/graphics/vulkan/immediate_context.h>
#include <gris/graphics/vulkan/input_layout.h>
#include <gris/graphics/vulkan/instance.h>
#include <gris/graphics/vulkan/physical_device_factory.h>
#include <gris/graphics/vulkan/pipeline_state_object.h>
#include <gris/graphics/vulkan/render_pass.h>
#include <gris/graphics/vulkan/sampler.h>
#include <gris/graphics/vulkan/shader.h>
#include <gris/graphics/vulkan/shader_resource_bindings.h>
#include <gris/graphics/vulkan/shader_resource_bindings_layout.h>
#include <gris/graphics/vulkan/swap_chain.h>
#include <gris/graphics/vulkan/texture.h>
#include <gris/graphics/vulkan/texture_view.h>
#include <gris/log.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4201)
#endif

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <system_error>
#include <unordered_map>
#include <vector>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const char * const MODEL_PATH = "viking_room.obj";
const char * const TEXTURE_PATH = "viking_room.png";
const char * const VERTEX_SHADER_PATH = "vert.spv";
const char * const FRAGMENT_SHADER_PATH = "frag.spv";

const int MAX_FRAMES_IN_FLIGHT = 2;

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static Gris::Graphics::Vulkan::InputLayout BuildInputLayout()
    {
        Gris::Graphics::Vulkan::InputLayout layout;

        layout.AddBinding(0, sizeof(Vertex), vk::VertexInputRate::eVertex);

        layout.AddAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos));
        layout.AddAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color));
        layout.AddAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, texCoord));

        return layout;
    }

    bool operator==(const Vertex & other) const
    {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

namespace std
{

template<>
struct hash<Vertex>
{
    size_t operator()(Vertex const & vertex) const noexcept
    {
        return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1U)) >> 1U) ^ (hash<glm::vec2>()(vertex.texCoord) << 1U);
    }
};

}  // namespace std

constexpr static size_t GlslMatrixAlignment = 16;

struct UniformBufferObject
{
    alignas(GlslMatrixAlignment) glm::mat4 model;
    alignas(GlslMatrixAlignment) glm::mat4 view;
    alignas(GlslMatrixAlignment) glm::mat4 proj;
};

template<typename T>
[[nodiscard]] std::vector<T> ReadFile(const std::filesystem::path & path)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw Gris::EngineException("Failed to open file!");
    }

    auto fileSize = file.tellg();
    std::vector<char> buffer(static_cast<size_t>(fileSize));

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    auto resultElementCount = static_cast<size_t>(fileSize) / sizeof(T) + (static_cast<size_t>(fileSize) % sizeof(T) == 0 ? 0 : 1);
    std::vector<T> result(resultElementCount);
    memcpy(result.data(), buffer.data(), static_cast<size_t>(fileSize));

    return result;
}

class HelloTriangleApplication : public Gris::Graphics::WindowObserver
{
public:
    void run()
    {
        SetLogLevel(Gris::Log::Level::Trace);
        InitWindow();
        InitVulkan();
        MainLoop();
    }

private:
    std::unique_ptr<Gris::Graphics::Vulkan::Glfw::Window> m_window = {};
    std::unique_ptr<Gris::Graphics::Vulkan::Device> m_device = {};
    std::unique_ptr<Gris::Graphics::Vulkan::SwapChain> m_swapChain = {};

    std::vector<Gris::Graphics::Vulkan::Framebuffer> m_swapChainFramebuffers = {};

    std::unique_ptr<Gris::Graphics::Vulkan::RenderPass> m_renderPass = {};
    std::unique_ptr<Gris::Graphics::Vulkan::Shader> m_vertexShader = {};
    std::unique_ptr<Gris::Graphics::Vulkan::Shader> m_fragmentShader = {};
    std::unique_ptr<Gris::Graphics::Vulkan::ShaderResourceBindingsLayout> m_resourceLayout = {};
    std::unique_ptr<Gris::Graphics::Vulkan::PipelineStateObject> m_pso = {};
    std::vector<Gris::Graphics::Vulkan::ShaderResourceBindings> m_shaderResourceBindings = {};

    std::unique_ptr<Gris::Graphics::Vulkan::Texture> m_colorImage = {};
    std::unique_ptr<Gris::Graphics::Vulkan::TextureView> m_colorImageView = {};

    std::unique_ptr<Gris::Graphics::Vulkan::Texture> m_depthImage = {};
    std::unique_ptr<Gris::Graphics::Vulkan::TextureView> m_depthImageView = {};

    std::unique_ptr<Gris::Graphics::Vulkan::Texture> m_textureImage = {};
    std::unique_ptr<Gris::Graphics::Vulkan::TextureView> m_textureImageView = {};
    std::unique_ptr<Gris::Graphics::Vulkan::Sampler> m_textureSampler = {};

    uint32_t m_indexCount = 0;
    std::unique_ptr<Gris::Graphics::Vulkan::Buffer> m_vertexBuffer = {};
    std::unique_ptr<Gris::Graphics::Vulkan::BufferView> m_vertexBufferView = {};

    std::unique_ptr<Gris::Graphics::Vulkan::Buffer> m_indexBuffer = {};
    std::unique_ptr<Gris::Graphics::Vulkan::BufferView> m_indexBufferView = {};

    std::vector<Gris::Graphics::Vulkan::Buffer> m_uniformBuffers = {};
    std::vector<Gris::Graphics::Vulkan::BufferView> m_uniformBufferViews = {};

    std::vector<Gris::Graphics::Vulkan::DeferredContext> m_commandBuffers = {};

    std::vector<Gris::Graphics::Vulkan::VirtualFrame> m_virtualFrames = {};

    bool m_framebufferResized = false;

    void WindowResized(uint32_t /* width */, uint32_t /* height */) override
    {
        m_framebufferResized = true;
    }

    void InitWindow()
    {
        m_window = std::make_unique<Gris::Graphics::Vulkan::Glfw::Window>(WIDTH, HEIGHT, "Vulkan tutorial");
        m_window->AddObserver(this);
    }

    void InitVulkan()
    {
        m_device = std::make_unique<Gris::Graphics::Vulkan::Device>(Gris::Graphics::Vulkan::PhysicalDeviceFactory::FindPhysicalDevice(*m_window));

        createTextureImage();
        m_textureImageView = std::make_unique<Gris::Graphics::Vulkan::TextureView>(m_device->CreateTextureView(*m_textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor, m_textureImage->MipLevels()));
        m_textureSampler = std::make_unique<Gris::Graphics::Vulkan::Sampler>(m_device->CreateSampler(0.0F, static_cast<float>(m_textureImage->MipLevels())));

        auto [vertices, indices] = loadModel();
        m_indexCount = static_cast<uint32_t>(indices.size());
        createVertexBuffer(vertices);
        createIndexBuffer(indices);

        m_swapChain = std::make_unique<Gris::Graphics::Vulkan::SwapChain>(m_device->CreateSwapChain(*m_window, m_window->Width(), m_window->Height(), MAX_FRAMES_IN_FLIGHT));
        m_renderPass = std::make_unique<Gris::Graphics::Vulkan::RenderPass>(m_device.get(), m_swapChain->Format(), findDepthFormat());

        auto const vertexShaderPath = Gris::DirectoryRegistry::TryResolvePath(VERTEX_SHADER_PATH);
        if (!vertexShaderPath)
        {
            throw Gris::EngineException("Error resolving vertex shader path", VERTEX_SHADER_PATH);
        }

        m_vertexShader = std::make_unique<Gris::Graphics::Vulkan::Shader>(m_device->CreateShader(ReadFile<uint32_t>(*vertexShaderPath), "main"));

        auto const fragmentShaderPath = Gris::DirectoryRegistry::TryResolvePath(FRAGMENT_SHADER_PATH);
        if (!fragmentShaderPath)
        {
            throw Gris::EngineException("Error resolving fragment shader path", FRAGMENT_SHADER_PATH);
        }

        m_fragmentShader = std::make_unique<Gris::Graphics::Vulkan::Shader>(m_device->CreateShader(ReadFile<uint32_t>(*fragmentShaderPath), "main"));

        auto const resourceLayouts = std::array{
            Gris::Graphics::Backend::ShaderResourceBindingLayout{
                "ubo",
                0,
                Gris::Graphics::Backend::ShaderResourceType::UniformBuffer,
                1,
                Gris::Graphics::Backend::ShaderStageFlags::Vertex,
            },
            Gris::Graphics::Backend::ShaderResourceBindingLayout{
                "texSampler",
                1,
                Gris::Graphics::Backend::ShaderResourceType::CombinedImageSampler,
                1,
                Gris::Graphics::Backend::ShaderStageFlags::Fragment,
            },
        };
        Gris::Graphics::Backend::ShaderResourceBindingsLayout bindingsLayout{ resourceLayouts };
        m_resourceLayout = std::make_unique<Gris::Graphics::Vulkan::ShaderResourceBindingsLayout>(m_device->CreateShaderResourceBindingsLayout(bindingsLayout));

        m_pso = std::make_unique<Gris::Graphics::Vulkan::PipelineStateObject>(m_device->CreatePipelineStateObject(m_swapChain->Extent().width, m_swapChain->Extent().height, *m_renderPass, Vertex::BuildInputLayout(), *m_resourceLayout, *m_vertexShader, *m_fragmentShader));
        createColorResources();
        createDepthResources();
        createFramebuffers();

        // TODO: This has to be removed
        m_device->CreateDescriptorPool(m_swapChain->ImageCount());

        for (size_t i = 0; i < m_swapChain->ImageCount(); i++)
        {
            auto & newBuffer = m_uniformBuffers.emplace_back(m_device->CreateBuffer(sizeof(UniformBufferObject), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
            m_uniformBufferViews.emplace_back(&newBuffer, 0, static_cast<uint32_t>(sizeof(UniformBufferObject)));

            auto & newBindings = m_shaderResourceBindings.emplace_back(m_device->CreateShaderResourceBindings(m_resourceLayout.get()));
            newBindings.SetCombinedSamplerAndImageView("texSampler", *m_textureSampler, *m_textureImageView);
            newBindings.SetUniformBuffer("ubo", m_uniformBufferViews[i]);
            newBindings.PrepareBindings();
        }

        createCommandBuffers(m_indexCount);

        m_virtualFrames.resize(MAX_FRAMES_IN_FLIGHT);
    }

    void MainLoop()
    {
        while (!m_window->ShouldClose())
        {
            Gris::Graphics::Glfw::Instance::PollEvents();
            drawFrame();
        }

        m_device->WaitIdle();
    }

    void recreateSwapChain()
    {
        m_device->WaitIdle();

        m_swapChain.reset();
        m_swapChain = std::make_unique<Gris::Graphics::Vulkan::SwapChain>(m_device->CreateSwapChain(*m_window, m_window->Width(), m_window->Height(), MAX_FRAMES_IN_FLIGHT));
        m_renderPass = std::make_unique<Gris::Graphics::Vulkan::RenderPass>(m_device.get(), m_swapChain->Format(), findDepthFormat());
        m_pso = std::make_unique<Gris::Graphics::Vulkan::PipelineStateObject>(m_device->CreatePipelineStateObject(m_swapChain->Extent().width, m_swapChain->Extent().height, *m_renderPass, Vertex::BuildInputLayout(), *m_resourceLayout, *m_vertexShader, *m_fragmentShader));
        createColorResources();
        createDepthResources();
        createFramebuffers();

        // TODO: This has to be removed
        m_device->CreateDescriptorPool(m_swapChain->ImageCount());

        m_uniformBuffers.clear();
        m_uniformBufferViews.clear();
        for (size_t i = 0; i < m_swapChain->ImageCount(); i++)
        {
            auto & newBuffer = m_uniformBuffers.emplace_back(m_device->CreateBuffer(sizeof(UniformBufferObject), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
            auto const & newView = m_uniformBufferViews.emplace_back(&newBuffer, 0, static_cast<uint32_t>(sizeof(UniformBufferObject)));
            m_shaderResourceBindings[i].SetUniformBuffer("ubo", newView);
            m_shaderResourceBindings[i].PrepareBindings();
        }

        createCommandBuffers(m_indexCount);
    }

    void createFramebuffers()
    {
        m_swapChainFramebuffers.clear();

        auto const swapChainExtent = m_swapChain->Extent();
        for (size_t i = 0; i < m_swapChain->ImageCount(); i++)
        {
            m_swapChainFramebuffers.emplace_back(m_device->CreateFramebuffer(*m_colorImageView, *m_depthImageView, m_swapChain->ImageView(i), *m_renderPass, swapChainExtent.width, swapChainExtent.height));
        }
    }

    void createColorResources()
    {
        auto const swapChainExtent = m_swapChain->Extent();
        auto const swapChainFormat = m_swapChain->Format();
        m_colorImage = std::make_unique<Gris::Graphics::Vulkan::Texture>(m_device->CreateTexture(swapChainExtent.width, swapChainExtent.height, 1, m_device->MsaaSamples(), swapChainFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal));
        m_colorImageView = std::make_unique<Gris::Graphics::Vulkan::TextureView>(m_device->CreateTextureView(*m_colorImage, swapChainFormat, vk::ImageAspectFlagBits::eColor, 1));
    }

    void createDepthResources()
    {
        auto const depthFormat = findDepthFormat();
        auto const swapChainExtent = m_swapChain->Extent();
        m_depthImage = std::make_unique<Gris::Graphics::Vulkan::Texture>(m_device->CreateTexture(swapChainExtent.width, swapChainExtent.height, 1, m_device->MsaaSamples(), depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal));
        m_depthImageView = std::make_unique<Gris::Graphics::Vulkan::TextureView>(m_device->CreateTextureView(*m_depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth, 1));
    }

    [[nodiscard]] vk::Format findDepthFormat() const
    {
        return m_device->FindSupportedFormat(
            { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment);
    }

    void createTextureImage()
    {
        auto const texturePath = Gris::DirectoryRegistry::TryResolvePath(TEXTURE_PATH);
        if (!texturePath)
        {
            throw Gris::EngineException("Failed to resolve texture image path", TEXTURE_PATH);
        }

        auto resolvedPath = texturePath->string();

        int texWidth = 0;
        int texHeight = 0;
        int texChannels = 0;
        auto * pixels = stbi_load(resolvedPath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        vk::DeviceSize imageSize = static_cast<vk::DeviceSize>(texWidth) * static_cast<vk::DeviceSize>(texHeight) * 4;
        auto const mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

        if (pixels == nullptr)
        {
            throw Gris::EngineException("Failed to load texture image");
        }

        auto stagingBuffer = m_device->CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        stagingBuffer.SetData(pixels, imageSize);

        stbi_image_free(pixels);

        m_textureImage = std::make_unique<Gris::Graphics::Vulkan::Texture>(m_device->CreateTexture(static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), mipLevels, vk::SampleCountFlagBits::e1, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal));
        m_device->Context()->TransitionImageLayout(*m_textureImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        m_device->Context()->CopyBufferToImage(stagingBuffer, *m_textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
        // Transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
        m_device->Context()->GenerateMipmaps(*m_textureImage, vk::Format::eR8G8B8A8Srgb, texWidth, texHeight);
    }

    [[nodiscard]] static std::pair<std::vector<Vertex>, std::vector<uint32_t>> loadModel()
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;

        auto modelPath = Gris::DirectoryRegistry::TryResolvePath(MODEL_PATH);
        if (!modelPath)
        {
            throw Gris::EngineException("Error resolving model path - file not found", MODEL_PATH);
        }

        auto resolvedPath = modelPath->string();
        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, resolvedPath.c_str(), nullptr, false))
        {
            throw Gris::EngineException("Error loading model", err);
        }

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        for (auto const & shape : shapes)
        {
            for (auto const & index : shape.mesh.indices)
            {
                Vertex vertex{};

                vertex.pos = {
                    attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 0],
                    attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 1],
                    attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 2]
                };

                vertex.texCoord = {
                    attrib.texcoords[2 * static_cast<size_t>(index.texcoord_index) + 0],
                    1.0F - attrib.texcoords[2 * static_cast<size_t>(index.texcoord_index) + 1]
                };

                vertex.color = { 1.0F, 1.0F, 1.0F };

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }

                indices.push_back(uniqueVertices[vertex]);
            }
        }

        return { vertices, indices };
    }

    void createVertexBuffer(const std::vector<Vertex> & vertices)
    {
        auto const bufferSize = sizeof(vertices[0]) * vertices.size();

        auto stagingBuffer = m_device->CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        stagingBuffer.SetData(vertices.data(), static_cast<size_t>(bufferSize));

        m_vertexBuffer = std::make_unique<Gris::Graphics::Vulkan::Buffer>(m_device->CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal));
        m_device->Context()->CopyBuffer(stagingBuffer, *m_vertexBuffer, bufferSize);

        m_vertexBufferView = std::make_unique<Gris::Graphics::Vulkan::BufferView>(m_vertexBuffer.get(), 0, static_cast<uint32_t>(bufferSize));
    }

    void createIndexBuffer(const std::vector<uint32_t> & indices)
    {
        auto const bufferSize = sizeof(indices[0]) * indices.size();

        auto stagingBuffer = m_device->CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        stagingBuffer.SetData(indices.data(), static_cast<size_t>(bufferSize));

        m_indexBuffer = std::make_unique<Gris::Graphics::Vulkan::Buffer>(m_device->CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal));
        m_device->Context()->CopyBuffer(stagingBuffer, *m_indexBuffer, bufferSize);

        m_indexBufferView = std::make_unique<Gris::Graphics::Vulkan::BufferView>(m_indexBuffer.get(), 0, static_cast<uint32_t>(bufferSize));
    }

    void createCommandBuffers(uint32_t indexCount)
    {
        auto const commandBufferCount = static_cast<uint32_t>(m_swapChainFramebuffers.size());
        m_commandBuffers.clear();

        for (uint32_t i = 0; i < commandBufferCount; i++)
        {
            m_commandBuffers.emplace_back(m_device->CreateDeferredContext());
            auto & commandBuffer = m_commandBuffers.back();

            commandBuffer.Begin();
            commandBuffer.BeginRenderPass(*m_renderPass, m_swapChainFramebuffers[i], m_swapChain->Extent());
            commandBuffer.BindPipeline(*m_pso);
            commandBuffer.BindVertexBuffer(*m_vertexBufferView);
            commandBuffer.BindIndexBuffer(*m_indexBufferView);
            commandBuffer.BindDescriptorSet(*m_pso, m_shaderResourceBindings[i]);
            commandBuffer.DrawIndexed(indexCount);
            commandBuffer.EndRenderPass();
            commandBuffer.End();
        }
    }

    void updateUniformBuffer(uint32_t currentImage)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto const currentTime = std::chrono::high_resolution_clock::now();
        auto const time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        auto const swapChainExtent = m_swapChain->Extent();

        UniformBufferObject ubo = {};

        constexpr glm::vec3 EyeLocation = glm::vec3(2.0F, 2.0F, 2.0F);
        constexpr glm::vec3 Origin = glm::vec3(0.0F, 0.0F, 0.0F);
        constexpr glm::vec3 Up = glm::vec3(0.0F, 0.0F, 1.0F);
        constexpr glm::vec3 RotationAxis = glm::vec3(0.0F, 0.0F, 1.0F);
        constexpr float NearPlane = 0.1F;
        constexpr float FarPlane = 10.0F;

        ubo.model = glm::rotate(glm::mat4(1.0F), time * glm::half_pi<float>(), RotationAxis);
        ubo.view = glm::lookAt(EyeLocation, Origin, Up);
        ubo.proj = glm::perspective(glm::quarter_pi<float>(), static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height), NearPlane, FarPlane);
        ubo.proj[1][1] *= -1;

        m_uniformBuffers[currentImage].SetData(&ubo, sizeof(ubo));
    }

    void drawFrame()
    {
        auto const nextImageResult = m_swapChain->NextImage();
        if (!nextImageResult)
        {
            recreateSwapChain();
            return;
        }

        updateUniformBuffer(nextImageResult->SwapChainImageIndex);

        auto const waitSemaphores = std::vector{ std::ref(m_swapChain->ImageAvailableSemaphore(*nextImageResult)) };
        auto const signalSemaphores = std::vector{ std::ref(m_swapChain->RenderingFinishedSemaphore(*nextImageResult)) };
        m_device->Context()->Submit(&m_commandBuffers[nextImageResult->SwapChainImageIndex], waitSemaphores, signalSemaphores, m_swapChain->RenderingFinishedFence(*nextImageResult));

        auto const presentResult = m_swapChain->Present(*nextImageResult);
        if (!presentResult || m_framebufferResized)
        {
            m_framebufferResized = false;
            recreateSwapChain();
        }
    }
};

int main()
{
    HelloTriangleApplication app;

    try
    {
        app.run();
    }
    catch (const std::exception & e)
    {
        Gris::Log::Critical(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
