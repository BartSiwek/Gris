/*
 * Copyright (c) 2020 Bartlomiej Siwek All rights reserved.
 */

#pragma once

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#define GRIS_PRINTF_FORMAT_ATTRIBUTE(FORMAT_POS, ARGS_POS) __attribute__((format(printf, FORMAT_POS, ARGS_POS)))
#else
#define GRIS_PRINTF_FORMAT_ATTRIBUTE(FORMAT_POS, ARGS_POS)
#endif
