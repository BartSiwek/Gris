/*
 * Copyright (c) 2020 Bartlomiej Siwek All rights reserved.
 */

#pragma once

#include <type_traits>

namespace Gris
{

namespace Detail
{

template<typename T, template<typename> class CrtpT>
struct Crpt
{
    T & Underlying()
    {
        return static_cast<T &>(*this);
    }

    T const & Underlying() const
    {
        return static_cast<T const &>(*this);
    }
};

}  // namespace Detail

template<typename T, typename TagT, template<typename> class... MixinTs>
class StrongType : public MixinTs<StrongType<T, TagT, MixinTs...>>...
{
public:
    explicit StrongType(const T & value)
        : m_value(value)
    {
    }

    explicit StrongType(T && value) noexcept(std::is_nothrow_move_constructible<T>::value)
        : m_value(std::move(value))
    {
    }

    T & Get() noexcept
    {
        return m_value;
    }

    const T & Get() const noexcept
    {
        return m_value;
    }

    friend void swap(StrongType & a, StrongType & b) noexcept
    {
        using std::swap;
        swap(static_cast<T &>(a), static_cast<T &>(b));
    }

private:
    T m_value;
};

namespace StrongTypeMixins
{

template<typename T>
struct Comparable : Detail::Crpt<T, Comparable>
{

    friend bool operator==(const T & lhs, const T & rhs)
    {
        return lhs.Underlying().Get() == rhs.Underlying().Get();
    }
};

template<typename T>
struct Hashable
{
    static constexpr bool IsHashable = true;
};

}  // namespace StrongTypeMixins

}  // namespace Gris

namespace std
{

template<typename T, typename TagT, template<typename> class... MixinTs>
struct hash<Gris::StrongType<T, TagT, MixinTs...>>
{
    using CompleteStrongType = Gris::StrongType<T, TagT, MixinTs...>;
    using IsHashable = std::enable_if_t<CompleteStrongType::IsHashable, void>;

    size_t operator()(Gris::StrongType<T, TagT, MixinTs...> const & value) const
    {
        return std::hash<T>()(value.Get());
    }
};
}  // namespace std
