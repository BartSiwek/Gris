/*
 * Copyright (c) 2020 Bartlomiej Siwek All rights reserved.
 */

#pragma once

#include <type_traits>

namespace Gris
{

template<typename EnumT>
constexpr std::underlying_type_t<EnumT> UnderlyingCast(EnumT value)
{
    return static_cast<std::underlying_type_t<EnumT>>(value);
}

}  // namespace Gris
