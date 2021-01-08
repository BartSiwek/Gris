#include <gris/graphics/EngineException.h>
#include <gris/graphics/glfw/GlfwInstance.h>
#include <gris/graphics/vulkan/VulkanBuffer.h>
#include <gris/graphics/vulkan/VulkanBufferView.h>
#include <gris/graphics/vulkan/VulkanDeferredContext.h>
#include <gris/graphics/vulkan/VulkanDevice.h>
#include <gris/graphics/vulkan/VulkanFramebuffer.h>
#include <gris/graphics/vulkan/VulkanImmediateContext.h>
#include <gris/graphics/vulkan/VulkanInputLayout.h>
#include <gris/graphics/vulkan/VulkanInstance.h>
#include <gris/graphics/vulkan/VulkanPhysicalDeviceFactory.h>
#include <gris/graphics/vulkan/VulkanPipelineStateObject.h>
#include <gris/graphics/vulkan/VulkanRenderPass.h>
#include <gris/graphics/vulkan/VulkanSampler.h>
#include <gris/graphics/vulkan/VulkanShaderResourceBinding.h>
#include <gris/graphics/vulkan/VulkanSwapChain.h>
#include <gris/graphics/vulkan/VulkanTexture.h>
#include <gris/graphics/vulkan/VulkanTextureView.h>
#include <gris/graphics/vulkan/glfw/GlfwExtensionGetters.h>
#include <gris/graphics/vulkan/glfw/GlfwVulkanWindow.h>

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
#pragma warning
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

const char * MODEL_PATH = "viking_room.obj";
const char * TEXTURE_PATH = "viking_room.png";

const int MAX_FRAMES_IN_FLIGHT = 2;

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static Gris::Graphics::Vulkan::VulkanInputLayout BuildInputLayout()
    {
        Gris::Graphics::Vulkan::VulkanInputLayout layout;

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
        return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
    }
};

}  // namespace std

struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

class HelloTriangleApplication : public Gris::Graphics::WindowObserver
{
public:
    void run()
    {
        InstallExtensionGetters();
        InitWindow();
        InitVulkan();
        MainLoop();
    }

private:
    std::unique_ptr<Gris::Graphics::Vulkan::Glfw::GlfwVulkanWindow> m_window = {};
    std::unique_ptr<Gris::Graphics::Vulkan::VulkanDevice> m_device = {};
    std::unique_ptr<Gris::Graphics::Vulkan::VulkanSwapChain> m_swapChain = {};

    std::vector<Gris::Graphics::Vulkan::VulkanFramebuffer> m_swapChainFramebuffers = {};

    std::unique_ptr<Gris::Graphics::Vulkan::VulkanRenderPass> m_renderPass = {};
    std::unique_ptr<Gris::Graphics::Vulkan::VulkanPipelineStateObject> m_pso = {};
    std::unique_ptr<Gris::Graphics::Vulkan::VulkanShaderResourceBinding> m_shaderResourceBinding = {};

    std::unique_ptr<Gris::Graphics::Vulkan::VulkanTexture> m_colorImage = {};
    std::unique_ptr<Gris::Graphics::Vulkan::VulkanTextureView> m_colorImageView = {};

    std::unique_ptr<Gris::Graphics::Vulkan::VulkanTexture> m_depthImage = {};
    std::unique_ptr<Gris::Graphics::Vulkan::VulkanTextureView> m_depthImageView = {};

    std::unique_ptr<Gris::Graphics::Vulkan::VulkanTexture> m_textureImage = {};
    std::unique_ptr<Gris::Graphics::Vulkan::VulkanTextureView> m_textureImageView = {};
    std::unique_ptr<Gris::Graphics::Vulkan::VulkanSampler> m_textureSampler = {};

    uint32_t m_indexCount = 0;
    std::unique_ptr<Gris::Graphics::Vulkan::VulkanBuffer> m_vertexBuffer = {};
    std::unique_ptr<Gris::Graphics::Vulkan::VulkanBufferView> m_vertexBufferView = {};

    std::unique_ptr<Gris::Graphics::Vulkan::VulkanBuffer> m_indexBuffer = {};
    std::unique_ptr<Gris::Graphics::Vulkan::VulkanBufferView> m_indexBufferView = {};

    std::vector<Gris::Graphics::Vulkan::VulkanBuffer> m_uniformBuffers = {};
    std::vector<Gris::Graphics::Vulkan::VulkanBufferView> m_uniformBufferViews = {};

    std::vector<Gris::Graphics::Vulkan::VulkanDeferredContext> m_commandBuffers = {};

    std::vector<Gris::Graphics::Vulkan::VulkanVirtualFrame> m_virtualFrames = {};

    bool m_framebufferResized = false;

    void WindowResized(uint32_t /* width */, uint32_t /* height */) override
    {
        m_framebufferResized = true;
    }

    static void InstallExtensionGetters()
    {
        Gris::Graphics::Vulkan::VulkanInstance::InstallExtensionGetter(Gris::Graphics::Vulkan::Glfw::GetVulkanInstanceExtensionsFromGLFW);
    }

    void InitWindow()
    {
        m_window = std::make_unique<Gris::Graphics::Vulkan::Glfw::GlfwVulkanWindow>(WIDTH, HEIGHT, "Vulkan tutorial");
        m_window->AddObserver(this);
    }

    void InitVulkan()
    {
        m_device = std::make_unique<Gris::Graphics::Vulkan::VulkanDevice>(Gris::Graphics::Vulkan::VulkanPhysicalDeviceFactory::FindPhysicalDevice(*m_window));

        createTextureImage();
        m_textureImageView = std::make_unique<Gris::Graphics::Vulkan::VulkanTextureView>(m_device->CreateTextureView(*m_textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor, m_textureImage->MipLevels()));
        m_textureSampler = std::make_unique<Gris::Graphics::Vulkan::VulkanSampler>(m_device->CreateSampler(0.0f, static_cast<float>(m_textureImage->MipLevels())));

        auto [vertices, indices] = loadModel();
        m_indexCount = static_cast<uint32_t>(indices.size());
        createVertexBuffer(vertices);
        createIndexBuffer(indices);

        m_swapChain = std::make_unique<Gris::Graphics::Vulkan::VulkanSwapChain>(m_device->CreateSwapChain(*m_window, m_window->Width(), m_window->Height(), MAX_FRAMES_IN_FLIGHT));
        m_renderPass = std::make_unique<Gris::Graphics::Vulkan::VulkanRenderPass>(m_device.get(), m_swapChain->Format(), findDepthFormat());
        m_pso = std::make_unique<Gris::Graphics::Vulkan::VulkanPipelineStateObject>(m_device->CreatePipelineStateObject(m_swapChain->Extent().width, m_swapChain->Extent().height, *m_renderPass, Vertex::BuildInputLayout()));
        createColorResources();
        createDepthResources();
        createFramebuffers();

        // TODO: This has to be removed
        m_device->CreateDescriptorPool(m_swapChain->ImageCount());

        for (size_t i = 0; i < m_swapChain->ImageCount(); i++)
            m_uniformBuffers.emplace_back(m_device->CreateBuffer(sizeof(UniformBufferObject), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

        for (size_t i = 0; i < m_swapChain->ImageCount(); i++)
            m_uniformBufferViews.emplace_back(&m_uniformBuffers[i], 0, static_cast<uint32_t>(sizeof(UniformBufferObject)));

        m_shaderResourceBinding = std::make_unique<Gris::Graphics::Vulkan::VulkanShaderResourceBinding>(m_pso.get(), m_swapChain->ImageCount());
        for (uint32_t i = 0; i < m_swapChain->ImageCount(); i++)
        {
            m_shaderResourceBinding->SetImageView(i, "texSampler", *m_textureImageView);
            m_shaderResourceBinding->SetSampler(i, "texSampler", *m_textureSampler);
            m_shaderResourceBinding->SetUniformBuffer(i, "ubo", m_uniformBufferViews[i]);
        }
        m_shaderResourceBinding->CreateDescriptorSets();

        createCommandBuffers(m_indexCount);

        m_virtualFrames.resize(MAX_FRAMES_IN_FLIGHT);
    }

    void MainLoop()
    {
        while (!m_window->ShouldClose())
        {
            Gris::Graphics::Glfw::GlfwInstance::PollEvents();
            drawFrame();
        }

        m_device->WaitIdle();
    }

    void recreateSwapChain()
    {
        m_device->WaitIdle();

        m_swapChain.reset();
        m_swapChain = std::make_unique<Gris::Graphics::Vulkan::VulkanSwapChain>(m_device->CreateSwapChain(*m_window, m_window->Width(), m_window->Height(), MAX_FRAMES_IN_FLIGHT));
        m_renderPass = std::make_unique<Gris::Graphics::Vulkan::VulkanRenderPass>(m_device.get(), m_swapChain->Format(), findDepthFormat());
        m_pso = std::make_unique<Gris::Graphics::Vulkan::VulkanPipelineStateObject>(m_device->CreatePipelineStateObject(m_swapChain->Extent().width, m_swapChain->Extent().height, *m_renderPass, Vertex::BuildInputLayout()));
        createColorResources();
        createDepthResources();
        createFramebuffers();

        // TODO: This has to be removed
        m_device->CreateDescriptorPool(m_swapChain->ImageCount());

        m_uniformBuffers.clear();
        m_uniformBufferViews.clear();
        for (size_t i = 0; i < m_swapChain->ImageCount(); i++)
            m_uniformBuffers.emplace_back(m_device->CreateBuffer(sizeof(UniformBufferObject), vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

        for (size_t i = 0; i < m_swapChain->ImageCount(); i++)
            m_uniformBufferViews.emplace_back(&m_uniformBuffers[i], 0, static_cast<uint32_t>(sizeof(UniformBufferObject)));

        m_shaderResourceBinding = std::make_unique<Gris::Graphics::Vulkan::VulkanShaderResourceBinding>(m_pso.get(), m_swapChain->ImageCount());
        for (uint32_t i = 0; i < m_swapChain->ImageCount(); i++)
        {
            m_shaderResourceBinding->SetImageView(i, "texSampler", *m_textureImageView);
            m_shaderResourceBinding->SetSampler(i, "texSampler", *m_textureSampler);
            m_shaderResourceBinding->SetUniformBuffer(i, "ubo", m_uniformBufferViews[i]);
        }
        m_shaderResourceBinding->CreateDescriptorSets();

        createCommandBuffers(m_indexCount);
    }

    void createFramebuffers()
    {
        m_swapChainFramebuffers.clear();

        auto const swapChainExtent = m_swapChain->Extent();
        for (size_t i = 0; i < m_swapChain->ImageCount(); i++)
            m_swapChainFramebuffers.emplace_back(m_device->CreateFramebuffer(*m_colorImageView, *m_depthImageView, m_swapChain->ImageView(i), *m_renderPass, swapChainExtent.width, swapChainExtent.height));
    }

    void createColorResources()
    {
        auto const swapChainExtent = m_swapChain->Extent();
        auto const swapChainFormat = m_swapChain->Format();
        m_colorImage = std::make_unique<Gris::Graphics::Vulkan::VulkanTexture>(m_device->CreateTexture(swapChainExtent.width, swapChainExtent.height, 1, m_device->MsaaSamples(), swapChainFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal));
        m_colorImageView = std::make_unique<Gris::Graphics::Vulkan::VulkanTextureView>(m_device->CreateTextureView(*m_colorImage, swapChainFormat, vk::ImageAspectFlagBits::eColor, 1));
    }

    void createDepthResources()
    {
        auto const depthFormat = findDepthFormat();
        auto const swapChainExtent = m_swapChain->Extent();
        m_depthImage = std::make_unique<Gris::Graphics::Vulkan::VulkanTexture>(m_device->CreateTexture(swapChainExtent.width, swapChainExtent.height, 1, m_device->MsaaSamples(), depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal));
        m_depthImageView = std::make_unique<Gris::Graphics::Vulkan::VulkanTextureView>(m_device->CreateTextureView(*m_depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth, 1));
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
        int texWidth, texHeight, texChannels;
        auto * pixels = stbi_load(TEXTURE_PATH, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        vk::DeviceSize imageSize = texWidth * texHeight * 4;
        auto const mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

        if (!pixels)
        {
            throw Gris::Graphics::EngineException("Failed to load texture image!");
        }

        auto stagingBuffer = m_device->CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        stagingBuffer.SetData(pixels, static_cast<size_t>(imageSize));

        stbi_image_free(pixels);

        m_textureImage = std::make_unique<Gris::Graphics::Vulkan::VulkanTexture>(m_device->CreateTexture(texWidth, texHeight, mipLevels, vk::SampleCountFlagBits::e1, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal));
        m_device->Context()->TransitionImageLayout(*m_textureImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        m_device->Context()->CopyBufferToImage(stagingBuffer, *m_textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
        // Transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
        m_device->Context()->GenerateMipmaps(*m_textureImage, vk::Format::eR8G8B8A8Srgb, texWidth, texHeight);
    }

    std::pair<std::vector<Vertex>, std::vector<uint32_t>> loadModel() const
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, MODEL_PATH, nullptr, false))
        {
            throw Gris::Graphics::EngineException("Error loading model", err);
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
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.texCoord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.color = { 1.0f, 1.0f, 1.0f };

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

        m_vertexBuffer = std::make_unique<Gris::Graphics::Vulkan::VulkanBuffer>(m_device->CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal));
        m_device->Context()->CopyBuffer(stagingBuffer, *m_vertexBuffer, bufferSize);

        m_vertexBufferView = std::make_unique<Gris::Graphics::Vulkan::VulkanBufferView>(m_vertexBuffer.get(), 0, static_cast<uint32_t>(bufferSize));
    }

    void createIndexBuffer(const std::vector<uint32_t> & indices)
    {
        auto const bufferSize = sizeof(indices[0]) * indices.size();

        auto stagingBuffer = m_device->CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        stagingBuffer.SetData(indices.data(), static_cast<size_t>(bufferSize));

        m_indexBuffer = std::make_unique<Gris::Graphics::Vulkan::VulkanBuffer>(m_device->CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal));
        m_device->Context()->CopyBuffer(stagingBuffer, *m_indexBuffer, bufferSize);

        m_indexBufferView = std::make_unique<Gris::Graphics::Vulkan::VulkanBufferView>(m_indexBuffer.get(), 0, static_cast<uint32_t>(bufferSize));
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
            commandBuffer.BindDescriptorSet(*m_pso, *m_shaderResourceBinding);
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
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height), 0.1f, 10.0f);
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
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
