#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

namespace Gris::Graphics::Vulkan
{

class Device;

struct DeviceSharedData
{
    DeviceSharedData(vk::DispatchLoaderDynamic dispatch, Device * device)
        : Dispatch(dispatch)
        , ParentDevice(device)
    {
    }

    vk::DispatchLoaderDynamic Dispatch = {};
    Device * ParentDevice = {};
};

}  // namespace Gris::Graphics::Vulkan
