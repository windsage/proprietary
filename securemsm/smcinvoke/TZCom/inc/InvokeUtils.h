/********************************************************************
Copyright (c) 2016-2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/
#ifndef _INVOKEUTILS_H_
#define _INVOKEUTILS_H_

#include <string.h>
#include <cstdlib>
#include "object.h"

#define ZALLOC_REC(ty)		((ty *) calloc(1, sizeof(ty)))

#define ZALLOC_ARRAY(ty, k)	((ty *) calloc((k), sizeof(ty)))

#define FREE_PTR(var)		((void) (free(var), (var) = 0))

#define UNUSED(x)			(void)(x)

#define FOR_ARGS(ndxvar, counts, section)                       \
  for (size_t ndxvar = ObjectCounts_index##section(counts);     \
       ndxvar < (ObjectCounts_index##section(counts)            \
                 + ObjectCounts_num##section(counts));          \
       ++ndxvar)

#define CHECK(condition, tag)					\
	if (!(condition)) {					\
		ALOGE("Failure: %s:%d\n", __FILE__, __LINE__);	\
		goto tag;					\
	}

static inline int atomic_add(int *pn, int n)
{
	return __sync_add_and_fetch(pn, n);  // GCC builtin
}

static inline size_t memscpy(void* dst, size_t dst_size,
					const void* src, size_t src_size)
{
	size_t copy_size = (dst_size <= src_size)? dst_size : src_size;

	memcpy(dst, src, copy_size);

	return copy_size;
}

#endif
