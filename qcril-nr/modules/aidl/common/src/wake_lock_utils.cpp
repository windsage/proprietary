/******************************************************************************
#  Copyright (c) 2022 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "wake_lock_utils.h"
#include "QtiMutex.h"
#include "WakeLock.h"
#include "framework/TimeKeeper.h"

#define ANDROID_WAKE_LOCK_NAME "radio-interface"
#define WAKE_LOCK_TIMEOUT_MS 200

static qtimutex::QtiSharedMutex s_wakeLockCountMutex;
static int s_wakelock_count = 0;

static WakeLock androidWakeLock(ANDROID_WAKE_LOCK_NAME);

namespace qti {
namespace ril {
namespace utils {

static void wakeLockTimeoutCallback(void* /*param*/);

void grabPartialWakeLock() {
  s_wakeLockCountMutex.lock();

  androidWakeLock.acquire();

  TimeKeeper::getInstance().set_timer(
      wakeLockTimeoutCallback,
      nullptr,
      WAKE_LOCK_TIMEOUT_MS);

  s_wakelock_count++;

  s_wakeLockCountMutex.unlock();
}

void releaseWakeLock() {
  s_wakeLockCountMutex.lock();

  if (s_wakelock_count > 1) {
    s_wakelock_count--;
  } else {
    s_wakelock_count = 0;
    androidWakeLock.release();
  }

  s_wakeLockCountMutex.unlock();
}

static void wakeLockTimeoutCallback(void* /*param*/) {
  s_wakeLockCountMutex.lock();

  s_wakelock_count--;
  if (s_wakelock_count == 0) {
    androidWakeLock.release();
  }

  s_wakeLockCountMutex.unlock();
}

} // namespace utils
} // namespace ril
} // namespace qti
