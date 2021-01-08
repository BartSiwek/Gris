#pragma once

#include <gris/assert.h>

/*
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)
#endif
*/

#define VULKAN_HPP_ASSERT(cond) GRIS_FAST_ASSERT(cond, "Vulkan Hpp asser failed")
#define VULKAN_HPP_TYPESAFE_CONVERSION
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

/*
#ifdef _MSC_VER
#pragma warning(pop)
#endif
*/
