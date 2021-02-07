#pragma once

#include <gris/assert.h>

#define VULKAN_HPP_ASSERT(cond) (void)(cond)
#define VULKAN_HPP_TYPESAFE_CONVERSION
#define VULKAN_HPP_NO_EXCEPTIONS

#define VULKAN_HPP_ENABLE_DYNAMIC_LOADER_TOOL 1
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_DEFAULT_DISPATCHER nullptr
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.hpp>
