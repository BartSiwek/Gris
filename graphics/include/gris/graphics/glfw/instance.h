#pragma once

namespace Gris::Graphics::Glfw
{

class Instance
{
public:
    static void Init();

    static void PollEvents();

    ~Instance();

    Instance(const Instance &) = delete;
    Instance & operator=(const Instance &) = delete;

    Instance(Instance &&) = delete;
    Instance & operator=(Instance &&) = delete;

private:
    static void EnsureInstanceCreated();

    Instance();
};

}  // namespace Gris::Graphics::Glfw
