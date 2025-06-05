#pragma once
/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "proxy_base.hpp"
#include "ILoadAlgoTA_invoke.hpp"
#include "ITEnv.hpp"

#define KB 1024
#define MB (KB*KB)

class CLoadalgo_TA : public Loadalgo_TAImplBase {
   public:
      CLoadalgo_TA();
      virtual ~CLoadalgo_TA();
      virtual int32_t run_algo(const ProxyBase &soFile_ref, uint32_t soFileSize_val,
                               const uint8_t* moduleName_ptr, size_t moduleName_len,
                               uint32_t heapSize_val, uint32_t poolSize_val);
      virtual int32_t enable_runtime_farf(const IMemObject &farfMask_ref);
      const static uint32_t WIDTH = 1920;
      const static uint32_t HEIGHT = 1080;
   private:
      IMemObject farf_buffer = Object_NULL;
};

