/*
 * Copyright (c) 2020 Bartlomiej Siwek All rights reserved.
 */

#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

namespace Gris
{

inline bool Implication(bool antecedent, bool consequent) noexcept
{
    return !antecedent || consequent;
}

template<class T, class Allocator = std::allocator<T>>
std::vector<T, Allocator> MakeReservedVector(size_t reservedSize)
{
    auto result = std::vector<T, Allocator>{};
    result.reserve(reservedSize);
    return result;
}

template<typename T>
[[nodiscard]] std::vector<T> ReadFile(const std::filesystem::path & path)
{
    auto file = std::ifstream(path, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
        throw Gris::EngineException("Failed to open file!");
    }

    auto fileSize = file.tellg();
    auto buffer = std::vector<char>(static_cast<size_t>(fileSize));

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    auto resultElementCount = static_cast<size_t>(fileSize) / sizeof(T) + (static_cast<size_t>(fileSize) % sizeof(T) == 0 ? 0 : 1);
    auto result = std::vector<T>(resultElementCount);
    std::memcpy(result.data(), buffer.data(), static_cast<size_t>(fileSize));

    return result;
}

}  // namespace Gris
