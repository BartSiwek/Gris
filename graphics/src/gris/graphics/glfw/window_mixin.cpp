#include <gris/graphics/glfw/window_mixin.h>

#include <gris/graphics/glfw/instance.h>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <utility>

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::WindowMixin::WindowMixin(const uint32_t width, const uint32_t height, const std::string & title)
    : m_width(width)
    , m_height(height)
{
    Instance::Init();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_window = glfwCreateWindow(static_cast<int>(m_width), static_cast<int>(m_height), title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow * window, const int newWidth, const int newHeight)
                                   {
                                       auto * windowPtr = static_cast<WindowMixin *>(glfwGetWindowUserPointer(window));
                                       windowPtr->OnSizeChanged(static_cast<uint32_t>(newWidth), static_cast<uint32_t>(newHeight));
                                   });
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::WindowMixin::~WindowMixin()
{
    glfwDestroyWindow(m_window);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::WindowMixin::WindowMixin(WindowMixin && other) noexcept
    : m_width(std::exchange(other.m_width, 0))
    , m_height(std::exchange(other.m_height, 0))
    , m_window(std::exchange(other.m_window, nullptr))
    , m_observers(std::exchange(other.m_observers, {}))
{
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::WindowMixin & Gris::Graphics::Glfw::WindowMixin::operator=(WindowMixin && other) noexcept
{
    if (this != &other)
    {
        m_width = std::exchange(other.m_width, 0);
        m_height = std::exchange(other.m_height, 0);
        m_window = std::exchange(other.m_window, nullptr);
        m_observers = std::exchange(other.m_observers, {});
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::WindowMixin::operator bool() const
{
    return m_window != nullptr;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] uint32_t Gris::Graphics::Glfw::WindowMixin::Width() const
{
    return m_width;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] uint32_t Gris::Graphics::Glfw::WindowMixin::Height() const
{
    return m_height;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Glfw::WindowMixin::ShouldClose() const
{
    return static_cast<bool>(glfwWindowShouldClose(m_window));
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::WindowMixin::AddObserver(WindowObserver * observer)
{
    m_observers.emplace_back(observer);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::WindowMixin::RemoveObserver(WindowObserver * observer)
{
    auto const observerIt = std::find(std::begin(m_observers), std::end(m_observers), observer);
    if (observerIt != std::end(m_observers))
    {
        m_observers.erase(observerIt);
    }
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] const GLFWwindow * Gris::Graphics::Glfw::WindowMixin::WindowHandle() const
{
    return m_window;
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] GLFWwindow * Gris::Graphics::Glfw::WindowMixin::WindowHandle()
{
    return m_window;
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::WindowMixin::OnSizeChanged(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;

    NotifySizeChanged();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::WindowMixin::NotifySizeChanged()
{
    for (auto * observer : m_observers)
    {
        observer->WindowResized(m_width, m_height);
    }
}
