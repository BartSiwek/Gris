#pragma once

#include <gris/graphics/vulkan/vulkan_headers.h>

#include <array>

#ifdef NDEBUG
constexpr static bool ENABLE_VALIDATION_LAYERS = false;
#else
constexpr static bool ENABLE_VALIDATION_LAYERS = true;
#endif

constexpr static uint32_t GRIS_VULKAN_API_VERSION = VK_API_VERSION_1_1;

constexpr static std::array VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};
