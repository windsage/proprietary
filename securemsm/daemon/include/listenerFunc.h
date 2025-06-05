/********************************************
Copyright (c) 2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
********************************************/

#ifndef __LISTENERFUNC_H
#define __LISTENERFUNC_H

#include <stdint.h>
#ifdef __cplusplus
extern "C"{
#endif
int init_listeners(void);
int start_listeners_smci(void);
#ifdef __cplusplus
}
#endif
#endif //__LISTENERFUNC_H
