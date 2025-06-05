/*
 * Copyright (c) 2017, 2023-2024  Qualcomm Technologies, Inc.
// All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __HEXUTILS_H__
#define __HEXUTILS_H__

#include <stddef.h>
#include <stdint.h>

#define BIT_SET(x, y) ((x) |= (1UL << (y)))
#define BIT_CLR(x, y) ((x) &= (~(1UL << (y))))
#define BIT_TST(x, y) ((x) & (1UL << (y)))

#define isHEX(c) (((c) >= '0' && (c) <= '9') || (((c) | 32) >= 'a' && ((c) | 32) <= 'f'))
#define HEX2NUM(c) (((c) >= '0' && (c) <= '9') ? ((c) - '0') : (10 + ((c) | 32) - 'a'))
#define NUM2HEX(c) (((c) < 10) ? '0' + (c) : (((c) < 16) ? 'A' + (c) - 10 : -1))

/**
 * Populates str with the hexadecimal representation of the data in buf. This
 * does not enforce null termination of the str buffer.
 *
 * The size of the output buffer needs to be atleast twice the size of the
 * binary data.
 *
 * @return the size of the generated string or 0 on failure.
 */
size_t bin_to_hex(const uint8_t* buf, size_t buf_sz, char* str, size_t str_sz);

/**
 * Populates the binary buffer with data from the hexadecimal string.
 *
 * @return the size of the generated data or 0 on failure.
 */
size_t hex_to_bin(const char* str, size_t str_sz, uint8_t* buf, size_t buf_sz);

#endif  // __HEXUTILS_H__
