/*
 * Copyright (c) 2020 Bartlomiej Siwek All rights reserved.
 */

#pragma once

#include <nonstd/span.hpp>

namespace Gris
{

using SpanExtent = nonstd::span_lite::extent_t;

constexpr const SpanExtent DynamicExtent = nonstd::dynamic_extent;

template<class T, SpanExtent Extent = DynamicExtent>
using Span = nonstd::span<T, Extent>;

}
