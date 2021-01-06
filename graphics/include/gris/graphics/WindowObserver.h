#pragma once

#include <cstdint>

class WindowObserver
{
public:
    WindowObserver() = default;

    WindowObserver(const WindowObserver&) = default;
    WindowObserver& operator=(const WindowObserver&) = default;

    WindowObserver(WindowObserver&&) = default;
    WindowObserver& operator=(WindowObserver&&) = default;

    virtual ~WindowObserver() = default;

    virtual void WindowResized(uint32_t width, uint32_t height) = 0;
};
