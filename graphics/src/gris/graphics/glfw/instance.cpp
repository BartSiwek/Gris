#include <gris/graphics/glfw/instance.h>

#include <GLFW/glfw3.h>

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::Instance::Init()
{
    EnsureInstanceCreated();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::Instance::PollEvents()
{
    EnsureInstanceCreated();
    glfwPollEvents();
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::Instance::~Instance()
{
    glfwTerminate();
}

// -------------------------------------------------------------------------------------------------

void Gris::Graphics::Glfw::Instance::EnsureInstanceCreated()
{
    static Instance s_instance = {};
}

// -------------------------------------------------------------------------------------------------

Gris::Graphics::Glfw::Instance::Instance()
{
    glfwInit();
}
