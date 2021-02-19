/*
 * Copyright (c) 2020 Bartlomiej Siwek All rights reserved.
 */

#pragma once

#include <memory>

namespace Gris
{

template<typename ParentT>
class ChildObject;

template<typename T>
class ParentObject
{
public:
    friend class ChildObject<T>;

    using PatentPointerType = T * const *;

    explicit ParentObject()
        : m_parent(std::make_unique<T *>())
    {
        ReassignParentToThis();
    }

    ParentObject(const ParentObject &) = delete;
    ParentObject & operator=(const ParentObject &) = delete;

    ParentObject(ParentObject && other) noexcept
        : m_parent(std::exchange(other.m_parent, {}))
    {
        ReassignParentToThis();
    }

    ParentObject & operator=(ParentObject && other) noexcept
    {
        if (this != &other)
        {
            m_parent = std::exchange(other.m_parent, {});
            ReassignParentToThis();
        }

        return *this;
    }

    virtual ~ParentObject()
    {
        if (m_parent)
        {
            *m_parent = nullptr;
        }
    }

private:
    const PatentPointerType ParentPointer() const
    {
        GRIS_ALWAYS_ASSERT(static_cast<bool>(m_parent), "The parent pointer pointer cannot be null");
        return m_parent.get();
    }

    PatentPointerType ParentPointer()
    {
        GRIS_ALWAYS_ASSERT(static_cast<bool>(m_parent), "The parent pointer pointer cannot be null");
        return m_parent.get();
    }

    void ReassignParentToThis()
    {
        GRIS_ALWAYS_ASSERT(static_cast<bool>(m_parent), "The parent pointer pointer cannot be null");
        *m_parent = static_cast<T *>(this);
    }

    std::unique_ptr<T *> m_parent = {};
};

template<typename ParentT>
class ChildObject
{
public:
    using PatentPointerType = typename ParentObject<ParentT>::PatentPointerType;

    ChildObject() = default;

    ChildObject(const ParentObject<ParentT> & parent)
        : m_parent(parent.ParentPointer())
    {
    }

    ChildObject(const ChildObject &) = default;
    ChildObject & operator=(const ChildObject &) = default;

    ChildObject(ChildObject &&) noexcept = default;
    ChildObject & operator=(ChildObject &&) noexcept = default;

    virtual ~ChildObject()
    {
        ResetParent();
    }

    explicit operator bool() const
    {
        return IsValid();
    }

    bool IsValid() const
    {
        return m_parent != nullptr;
    }

protected:
    void ResetParent()
    {
        m_parent = nullptr;
    }

    ParentT & Parent()
    {
        GRIS_ALWAYS_ASSERT(m_parent != nullptr, "The parent pointer pointer should not be null");
        return **m_parent;
    }

    const ParentT & Parent() const
    {
        GRIS_ALWAYS_ASSERT(m_parent != nullptr, "The parent pointer pointer should not be null");
        return **m_parent;
    }

private:
    PatentPointerType m_parent = nullptr;
};

}  // namespace Gris
