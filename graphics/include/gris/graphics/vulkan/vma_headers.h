#pragma once

#include <gris/assert.h>

#ifndef VULKAN_HPP_ASSERT
#define VULKAN_HPP_ASSERT(cond) GRIS_FAST_ASSERT(cond, "Vulkan Hpp assert failed")
#endif
#define VULKAN_HPP_TYPESAFE_CONVERSION
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vk_mem_alloc.hpp>
