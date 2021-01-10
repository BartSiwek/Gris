#pragma once

#include <gris/assert.h>

#define VULKAN_HPP_ASSERT(cond) GRIS_FAST_ASSERT(cond, "Vulkan Hpp assert failed")
#define VULKAN_HPP_TYPESAFE_CONVERSION
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>
