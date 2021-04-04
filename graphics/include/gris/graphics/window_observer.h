#pragma once

#include <cstdint>

namespace Gris::Graphics
{

enum class MouseButton
{
    Left,
    Middle,
    Right
};

enum class MouseButtonAction
{
    Up,
    Down
};

class WindowObserver
{
public:
    WindowObserver() = default;

    WindowObserver(const WindowObserver &) = default;
    WindowObserver & operator=(const WindowObserver &) = default;

    WindowObserver(WindowObserver &&) = default;
    WindowObserver & operator=(WindowObserver &&) = default;

    virtual ~WindowObserver() = default;

    virtual void WindowResized(uint32_t width, uint32_t height) = 0;

    virtual void MouseButtonEvent(MouseButton button, MouseButtonAction action, float x, float y) = 0;
    virtual void MouseMoveEvent(float x, float y) = 0;
    virtual void MouseWheelEvent(float x, float y, float delta) = 0;
};

}  // namespace Gris::Graphics
