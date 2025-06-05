/********************************************************************
Copyright (c) 2019 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/

#include "CIO.h"
#include "IIO_invoke.h"
#include "memscpy.h"

static inline int atomic_add(int* pn, int n) {
    return __sync_add_and_fetch(pn, n);  // GCC builtin
}

struct CIO {
    int refs;
    const uint8_t* cred_buffer;
    size_t cred_buffer_len;
};

static int32_t
CIO_release(CIO *me)
{
    if (atomic_add(&me->refs, -1) == 0) {
        delete[] me->cred_buffer;
        delete(me);
    }

    return Object_OK;
}

static int32_t
CIO_retain(CIO *me)
{
    atomic_add(&me->refs, 1);
    return Object_OK;
}

static int32_t
CIO_getLength(CIO *me, uint64_t *len_ptr)
{
    *len_ptr = (uint64_t) me->cred_buffer_len;
    return Object_OK;
}

static int32_t
CIO_readAtOffset(CIO *me, uint64_t offset_val, void *data_ptr, size_t data_len, size_t *data_lenout)
{
    if ((size_t)offset_val >= me->cred_buffer_len) {
        return IIO_ERROR_OFFSET_OUT_OF_BOUNDS;
    }

    size_t needed_len = me->cred_buffer_len - (size_t)offset_val;
    *data_lenout = memscpy(data_ptr, data_len, (const char*)me->cred_buffer + offset_val, needed_len);
    return Object_OK;
}

static int32_t
CIO_writeAtOffset(CIO *me, uint64_t offset_val, const void *data_ptr, size_t data_len)
{
    return Object_ERROR; //Cannot write to credential buffers
}


static IIO_DEFINE_INVOKE(IIO_invoke, CIO_, CIO*)

int32_t CIO_open(const void* cred_buffer,
                  size_t cred_buffer_len,
                  Object* objOut) {
    CIO* me = new CIO();
    if (!me) {
        return Object_ERROR;
    }
    me->refs = 1;
    //copy the credentials for storage
    uint8_t* creds = new uint8_t[cred_buffer_len];
    me->cred_buffer_len = cred_buffer_len;
    memscpy(creds, me->cred_buffer_len,
            cred_buffer, cred_buffer_len);
    me->cred_buffer =  creds;
    *objOut = (Object) {IIO_invoke, me};
    return Object_OK;
}
