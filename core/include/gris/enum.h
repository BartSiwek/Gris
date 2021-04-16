/*
 * Copyright (c) 2020 Bartlomiej Siwek All rights reserved.
 */

#pragma once

#include <gris/casts.h>

#include <type_traits>

namespace Gris::Enum
{

template<typename EnumT>
constexpr inline bool EnableBitmaskOperators(EnumT /* enum */)
{
    return false;
}

template<typename EnumT, typename = std::enable_if_t<std::is_enum_v<EnumT> && EnableBitmaskOperators(EnumT())>>
inline constexpr EnumT operator|(EnumT lhs, EnumT rhs)
{
    return static_cast<EnumT>(UnderlyingCast(lhs) | UnderlyingCast(rhs));
}

template<typename EnumT, typename = std::enable_if_t<std::is_enum_v<EnumT> && EnableBitmaskOperators(EnumT())>>
inline constexpr EnumT operator&(EnumT lhs, EnumT rhs)
{
    return static_cast<EnumT>(UnderlyingCast(lhs) & UnderlyingCast(rhs));
}

template<typename EnumT, typename = std::enable_if_t<std::is_enum_v<EnumT> && EnableBitmaskOperators(EnumT())>>
inline constexpr EnumT operator^(EnumT lhs, EnumT rhs)
{
    return static_cast<EnumT>(UnderlyingCast(lhs) ^ UnderlyingCast(rhs));
}

template<typename EnumT, typename = std::enable_if_t<std::is_enum_v<EnumT> && EnableBitmaskOperators(EnumT())>>
inline constexpr EnumT & operator|=(EnumT & lhs, EnumT rhs)
{
    lhs = static_cast<EnumT>(UnderlyingCast(lhs) | UnderlyingCast(rhs));
    return lhs;
}

template<typename EnumT, typename = std::enable_if_t<std::is_enum_v<EnumT> && EnableBitmaskOperators(EnumT())>>
inline constexpr EnumT & operator&=(EnumT & lhs, EnumT rhs)
{
    lhs = static_cast<EnumT>(UnderlyingCast(lhs) & UnderlyingCast(rhs));
    return lhs;
}

template<typename EnumT, typename = std::enable_if_t<std::is_enum_v<EnumT> && EnableBitmaskOperators(EnumT())>>
inline constexpr EnumT & operator^=(EnumT & lhs, EnumT rhs)
{
    lhs = static_cast<EnumT>(UnderlyingCast(lhs) ^ UnderlyingCast(rhs));
    return lhs;
}

template<typename EnumT, typename = std::enable_if_t<std::is_enum_v<EnumT> && EnableBitmaskOperators(EnumT())>>
inline constexpr EnumT operator~(EnumT value)
{
    return static_cast<EnumT>(~UnderlyingCast(value));
}

}  // namespace Gris::Enum

#define GRIS_ENABLE_ENUM_BIT_FLAGS_OPERATIONS(type)    \
    using Gris::Enum::operator|;                       \
    using Gris::Enum::operator|=;                      \
    using Gris::Enum::operator&;                       \
    using Gris::Enum::operator&=;                      \
    using Gris::Enum::operator^;                       \
    using Gris::Enum::operator^=;                      \
    using Gris::Enum::operator~;                       \
                                                       \
    constexpr inline bool EnableBitmaskOperators(type) \
    {                                                  \
        return true;                                   \
    }
