// Copyright (c) 2015 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.

#ifndef __BBUF_H
#define __BBUF_H

#include <stddef.h>
#include <stdint.h>

#if defined (__cplusplus)
extern "C" {
#endif


// `BBuf` implements "one-shot suballocation": functions are provided for
// initialization and sub-allocation, but not for freeing
// previously-allocated ranges.

typedef struct {
   char *ptr;
   size_t len;
} BBuf;


// Construct a BBuf instance.
//
// ptr[0...size-1] descibes a buffer that is to be used to satisfy
// allocations from this instance.  Memory returned by allocation operations
// will be guaranteed to fall entirely within this buffer.  Allocation
// operations do not modify the contents of that buffer.
//
void BBuf_construct(BBuf *me, void *ptr, size_t size);


// Allocate a range of memory from `me`, returning a pointer to the start of
// the block, or NULL if there is not enough memory left.
//
void *BBuf_alloc(BBuf *me, size_t size);


// Allocate a range of memory from `me`.  The total amount of memory
// requested is given by a*b.  On failure, NULL is returned.
//
// Note: For optimal performance and code size, if one of the two values is
// known at compile-time, it should be passed as parameter `a` rather than
// `b`.
//
static inline void *BBuf_allocMul(BBuf *me, size_t a, size_t b)
{
   if (b > SIZE_MAX / a) {
      return NULL;
   }
   return BBuf_alloc(me, a * b);
}


// Type-safe wrapper for BBuf_alloc().  A type name is given in place of a
// size argument.
//
#define BBuf_ALLOC_TYPE(pbb, typ)                \
   ((typ*) BBuf_alloc((pbb), sizeof(typ)))


// Type-safe wrapper for BBuf_allocMul().  A type name is given in place of
// the first size argument.
//
#define BBuf_ALLOC_ARRAY(pbb, typ, count)                \
   ((typ*) BBuf_allocMul((pbb), sizeof(typ), (count)))


#if defined (__cplusplus)
}
#endif

#endif /* __BBUF_H */
