/*
 * Copyright (c) 2020 Bartlomiej Siwek All rights reserved.
 */

#pragma once

#include <vector>

namespace Gris
{

template<class T, class Allocator = std::allocator<T>>
std::vector<T, Allocator> MakeReservedVector(size_t reservedSize)
{
    auto result = std::vector<T, Allocator>{};
    result.reserve(reservedSize);
    return result;
}

template<class T, class Allocator = std::allocator<T>>
std::vector<T, Allocator> MakeResizedVector(size_t size)
{
    auto result = std::vector<T, Allocator>{};
    result.resize(size);
    return result;
}

}  // namespace Gris
