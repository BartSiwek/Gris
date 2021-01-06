#pragma once

#include <array>

#ifdef NDEBUG
constexpr static bool ENABLE_VALIDATION_LAYERS = false;
#else
constexpr static bool ENABLE_VALIDATION_LAYERS = true;
#endif

constexpr static std::array VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};
