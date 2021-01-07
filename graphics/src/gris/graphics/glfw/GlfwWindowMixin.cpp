#include <gris/graphics/glfw/GlfwWindowMixin.h>

#include <gris/graphics/glfw/GlfwInstance.h>

#include <GLFW/glfw3.h>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::GlfwWindowMixin::GlfwWindowMixin(const uint32_t width, const uint32_t height, const std::string& title) : m_width(width), m_height(height)
{
    GlfwInstance::Init();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_window = glfwCreateWindow(m_width, m_height, title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, const int width, const int height)
    {
        auto* windowPtr = static_cast<GlfwWindowMixin*>(glfwGetWindowUserPointer(window));
        windowPtr->OnSizeChanged(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    });
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::GlfwWindowMixin::~GlfwWindowMixin()
{
    glfwDestroyWindow(m_window);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::GlfwWindowMixin::GlfwWindowMixin(GlfwWindowMixin&& other) noexcept
    : m_width(std::exchange(other.m_width, 0))
    , m_height(std::exchange(other.m_height, 0))
    , m_window(std::exchange(other.m_window, nullptr))
    , m_observers(std::exchange(other.m_observers, {}))
{}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::GlfwWindowMixin& Gris::Graphics::Glfw::GlfwWindowMixin::operator=(GlfwWindowMixin&& other) noexcept
{
    if (this != &other) {
        m_width = std::exchange(other.m_width, 0);
        m_height = std::exchange(other.m_height, 0);
        m_window = std::exchange(other.m_window, nullptr);
        m_observers = std::exchange(other.m_observers, {});
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::GlfwWindowMixin::operator bool() const
{
    return m_window != nullptr;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] uint32_t Gris::Graphics::Glfw::GlfwWindowMixin::Width() const
{
    return m_width;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] uint32_t Gris::Graphics::Glfw::GlfwWindowMixin::Height() const
{
    return m_height;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Glfw::GlfwWindowMixin::ShouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::GlfwWindowMixin::AddObserver(WindowObserver* observer)
{
    m_observers.emplace_back(observer);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::GlfwWindowMixin::RemoveObserver(WindowObserver* observer)
{
    auto const observerIt = std::find(std::begin(m_observers), std::end(m_observers), observer);
    if (observerIt != std::end(m_observers))
        m_observers.erase(observerIt);
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const GLFWwindow* const Gris::Graphics::Glfw::GlfwWindowMixin::WindowHandle() const
{
    return m_window;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] GLFWwindow* Gris::Graphics::Glfw::GlfwWindowMixin::WindowHandle()
{
    return m_window;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::GlfwWindowMixin::OnSizeChanged(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;

    NotifySizeChanged();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::GlfwWindowMixin::NotifySizeChanged()
{
    for (auto * observer : m_observers)
        observer->WindowResized(m_width, m_height);
}
