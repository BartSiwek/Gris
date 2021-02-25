#include <gris/graphics/image.h>
#include <gris/graphics/loaders/stb_image_loader.h>
#include <gris/graphics/loaders/tinlyobjloader_mesh_loader.h>
#include <gris/graphics/mesh.h>
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
#include <gris/graphics/vulkan/shader_resource_bindings_pool_collection.h>
#include <gris/graphics/vulkan/swap_chain.h>
#include <gris/graphics/vulkan/texture.h>
#include <gris/graphics/vulkan/texture_view.h>

#include <cstdint>
#include <vector>

class HelloTriangleApplication : public Gris::Graphics::WindowObserver
{
public:
    void Run();

private:
    [[nodiscard]] vk::Format FindDepthFormat() const;

    void WindowResized(uint32_t /* width */, uint32_t /* height */) override;

    void InitWindow();
    void CreateVulkanObjects();
    void LoadScene();
    void MainLoop();

    void WaitForDeviceToBeIdle();

    void CreateDevice();
    void CreateSwapChain();
    void CreateRenderPass();
    void CreateMesh();
    void CreateMeshTexture();
    void CreatePipelineStateObject();
    void CreateFramebuffers();
    void CreateShaderResourceBindingsPools();
    void CreateUniformBuffersAndBindings();
    void CreateCommandBuffers();

    void UpdateUniformBuffer(uint32_t currentImage);
    void DrawFrame();

    Gris::Graphics::Vulkan::Glfw::Window m_window = {};
    Gris::Graphics::Vulkan::Device m_device = {};
    Gris::Graphics::Vulkan::SwapChain m_swapChain = {};

    std::vector<Gris::Graphics::Vulkan::Framebuffer> m_swapChainFramebuffers = {};

    Gris::Graphics::Vulkan::RenderPass m_renderPass = {};
    Gris::Graphics::Vulkan::Shader m_vertexShader = {};
    Gris::Graphics::Vulkan::Shader m_fragmentShader = {};
    Gris::Graphics::Vulkan::ShaderResourceBindingsLayout m_resourceLayout = {};
    Gris::Graphics::Vulkan::PipelineStateObject m_pso = {};

    Gris::Graphics::Backend::ShaderResourceBindingsPoolCategory m_shaderResourceBindingsPoolCategory = Gris::Graphics::Backend::ShaderResourceBindingsPoolCategory{ 0 };
    Gris::Graphics::Vulkan::ShaderResourceBindingsPoolCollection m_shaderResourceBindingsPools;
    std::vector<Gris::Graphics::Vulkan::ShaderResourceBindings> m_shaderResourceBindings = {};

    Gris::Graphics::Vulkan::Texture m_colorImage = {};
    Gris::Graphics::Vulkan::TextureView m_colorImageView = {};

    Gris::Graphics::Vulkan::Texture m_depthImage = {};
    Gris::Graphics::Vulkan::TextureView m_depthImageView = {};

    Gris::Graphics::Mesh m_mesh;
    Gris::Graphics::Vulkan::Buffer m_vertexBuffer = {};
    Gris::Graphics::Vulkan::BufferView m_vertexBufferView = {};
    Gris::Graphics::Vulkan::Buffer m_indexBuffer = {};
    Gris::Graphics::Vulkan::BufferView m_indexBufferView = {};

    Gris::Graphics::Vulkan::Texture m_meshTextureImage = {};
    Gris::Graphics::Vulkan::TextureView m_meshTextureImageView = {};
    Gris::Graphics::Vulkan::Sampler m_meshTextureSampler = {};

    std::vector<Gris::Graphics::Vulkan::Buffer> m_uniformBuffers = {};
    std::vector<Gris::Graphics::Vulkan::BufferView> m_uniformBufferViews = {};

    std::vector<Gris::Graphics::Vulkan::DeferredContext> m_commandBuffers = {};

    std::vector<Gris::Graphics::Vulkan::VirtualFrame> m_virtualFrames = {};

    bool m_framebufferResized = false;
};
