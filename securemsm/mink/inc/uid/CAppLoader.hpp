#pragma once
/***********************************************************
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
************************************************************/

/**
* @addtogroup CAppLoader
* @{
  Class CAppLoader implements \link IAppLoader \endlink interface.

  The class ID `AppLoader` is not included in the default privilege set. It needs to be added to the
  privileges section in the metadata to allow the TA to use the CAppLoader class.
*/

// This class provides an interface to load Secure Applications in QSEE

#include <cstdint>
#include "object.h"
#include "proxy_base.hpp"

static const uint32_t CAppLoader_UID = UINT32_C(3);


