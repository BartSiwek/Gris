#include <gris/graphics/glfw/window_mixin.h>

#include <gris/graphics/glfw/instance.h>

#include <gris/engine_exception.h>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <utility>

// -------------------------------------------------------------------------------------------------

namespace
{

Gris::Graphics::MouseButton ToButton(int glfwButton)
{
    if (glfwButton == GLFW_MOUSE_BUTTON_LEFT)
        return Gris::Graphics::MouseButton::Left;
    if (glfwButton == GLFW_MOUSE_BUTTON_RIGHT)
        return Gris::Graphics::MouseButton::Right;
    if (glfwButton == GLFW_MOUSE_BUTTON_MIDDLE)
        return Gris::Graphics::MouseButton::Middle;

    throw Gris::EngineException("Unrecognized GLFW mouse button.", std::to_string(glfwButton));
}

Gris::Graphics::MouseButtonAction ToAction(int glfwAction)
{
    if (glfwAction == GLFW_PRESS)
        return Gris::Graphics::MouseButtonAction::Down;
    if (glfwAction == GLFW_RELEASE)
        return Gris::Graphics::MouseButtonAction::Up;

    throw Gris::EngineException("Unrecognized GLFW mouse button action.", std::to_string(glfwAction));
}

}  // namespace

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
    glfwSetMouseButtonCallback(m_window, [](GLFWwindow * window, int button, int action, int /* mods */)
                               {
                                   auto * windowPtr = static_cast<WindowMixin *>(glfwGetWindowUserPointer(window));

                                   double x;
                                   double y;
                                   glfwGetCursorPos(window, &x, &y);

                                   windowPtr->OnMouseButtonEvent(ToButton(button), ToAction(action), static_cast<float>(x), static_cast<float>(y));
                               });
    glfwSetCursorPosCallback(m_window, [](GLFWwindow * window, double x, double y)
                             {
                                 auto * windowPtr = static_cast<WindowMixin *>(glfwGetWindowUserPointer(window));
                                 windowPtr->OnMouseMoveEvent(static_cast<float>(x), static_cast<float>(y));
                             });
    glfwSetScrollCallback(m_window, [](GLFWwindow * window, double /* xoffset */, double yoffset)
                          {
                              auto * windowPtr = static_cast<WindowMixin *>(glfwGetWindowUserPointer(window));

                              double x;
                              double y;
                              glfwGetCursorPos(window, &x, &y);

                              windowPtr->OnMouseWheelEvent(static_cast<float>(yoffset), static_cast<float>(x), static_cast<float>(y));
                          });
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::WindowMixin::WindowMixin(WindowMixin && other) noexcept
    : m_width(std::exchange(other.m_width, 0))
    , m_height(std::exchange(other.m_height, 0))
    , m_window(std::exchange(other.m_window, nullptr))
    , m_observers(std::exchange(other.m_observers, {}))
{
    glfwSetWindowUserPointer(m_window, this);
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

        glfwSetWindowUserPointer(m_window, this);
    }

    return *this;
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::WindowMixin::~WindowMixin()
{
    glfwDestroyWindow(m_window);
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::WindowMixin::operator bool() const
{
    return IsValid();
}

// -------------------------------------------------------------------------------------------------

[[nodiscard]] bool Gris::Graphics::Glfw::WindowMixin::IsValid() const
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

void Gris::Graphics::Glfw::WindowMixin::OnMouseButtonEvent(MouseButton button, MouseButtonAction action, float x, float y)
{
    NotifyMouseButtonEvent(button, action, x, y);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::WindowMixin::OnMouseMoveEvent(float x, float y)
{
    NotifyMouseMoveEvent(x, y);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::WindowMixin::OnMouseWheelEvent(float x, float y, float delta)
{
    NotifyMouseWheelEvent(x, y, delta);
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::WindowMixin::NotifySizeChanged()
{
    for (auto * observer : m_observers)
    {
        observer->WindowResized(m_width, m_height);
    }
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::WindowMixin::NotifyMouseButtonEvent(MouseButton button, MouseButtonAction action, float x, float y)
{
    for (auto * observer : m_observers)
    {
        observer->MouseButtonEvent(button, action, x, y);
    }
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::WindowMixin::NotifyMouseMoveEvent(float x, float y)
{
    for (auto * observer : m_observers)
    {
        observer->MouseMoveEvent(x, y);
    }
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::WindowMixin::NotifyMouseWheelEvent(float x, float y, float delta)
{
    for (auto * observer : m_observers)
    {
        observer->MouseWheelEvent(x, y, delta);
    }
}
