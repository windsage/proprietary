/*===================================================================================
  Copyright (c) 2020-2023 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===================================================================================*/

/******************************************************************************
 *                         Header Inclusions
 *****************************************************************************/

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <vector>
#include <fstream>

#include <utils/Log.h>
#include "TouchInputVM.h"
#include <unistd.h>

using namespace std;

/******************************************************************************
*       Constant Definitions And Local Variables
*****************************************************************************/
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "HLOSTouchInputVM"

namespace aidl {
namespace vendor {
namespace qti {
namespace hardware {
namespace trustedui {

std::mutex TouchInputVM::sLock;
std::shared_ptr<TouchInputVM> TouchInputVM::sInstance = nullptr;


/* Description :  This API will not perform any functionality",
 * as this is taken care by TUIListener via TrustedInputUtils.
 *
 * Return :   Response::TUI_FAILURE
 *
 *
 */

::ndk::ScopedAStatus TouchInputVM::init(const std::shared_ptr<ITrustedInputCallback>& in_cb,
                             const std::string& in_displayType, TUIOutputID* out_outParam,
                             TUIResponse* _aidl_return)
{
    /*init will not perform any functionality, it will be taken
     * care by TUIListener via TrustedInputUtils */
    ALOGE("%s, %d : will not perform any functionality, will be taken care by TUIListener"
          , __func__, __LINE__);
    *_aidl_return = TUIResponse::TUI_FAILURE;
    return ndk::ScopedAStatus::ok();
}

/* Description :  This API always returns "Response::TUI_FAILURE",
 * as this is taken care by TUIListener via TrustedInputUtils.
 *
 * Return :   Response::TUI_FAILURE
 *
 *
 */

::ndk::ScopedAStatus TouchInputVM::terminate(TUIResponse* _aidl_return)
{
    /*terminate will not perform any functionality, it will be taken
     * care by TUIListener via TrustedInputUtils */
    ALOGE("%s, %d : will not perform any functionality, will be taken care by TUIListener"
          , __func__, __LINE__);
    *_aidl_return = TUIResponse::TUI_FAILURE;
    return ndk::ScopedAStatus::ok();
}

/* Description :  This API always returns "Response::TUI_FAILURE",
 * as this is mainly executed by LEVM TouchInput.
 *
 * Return :   Response::TUI_FAILURE
 *
 *
 */
::ndk::ScopedAStatus TouchInputVM::getInput(int32_t in_timeout, TUIResponse* _aidl_return)
{
    /*getInput will not perform any functionality in Android, it will be taken
     * care by LEVM TouchInput */
    ALOGE("%s, %d : getInput is not implemented in Android", __func__,
          __LINE__);
    *_aidl_return = TUIResponse::TUI_FAILURE;
    return ndk::ScopedAStatus::ok();
}

}  // namespace trustedui
}  // namespace hardware
}  // namespace qti
}  // namespace vendor
}  // namespace aidl
