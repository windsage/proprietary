/*===================================================================================
  Copyright (c) 2020-2024 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===================================================================================*/

/******************************************************************************
 *                         Header Inclusions
 *****************************************************************************/
#include <cstring>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <fstream>

#include "ITEnv.hpp"
#include "TUILog.h"
#include "TUIUtils.h"
#include "TouchInput.h"
#include "ITUITypes.hpp"

extern Object gTVMEnv;

/******************************************************************************
 *       Constant Definitions And Local Variables
*****************************************************************************/
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TUIVMInput::TouchInputVM"
#define MAX_RETRY_ATTEMPTS 5

using namespace std;

/******************************************************************************
 *                        Private Class Function Definitions
 *****************************************************************************/
#ifdef TEST_ON_LEVM

/* Description : This API searches for the controlfd sysfs path.
 *
 *
 * Return : -EINVAL,
 *          -ENOENT,
 *          ITrustedInput::ERROR_INVALID_DISPLAY_TYPE,
 *          ITrustedInput::SUCCESS
 *
 */

int32_t CTouchInput::searchControlDevice() {
    int32_t ret = -ENOENT;
    std::string path = mQTSFileLocation + mDisplayType;
    DIR *dirp = NULL;
    struct dirent *dirInfo = NULL;
    string devicePath;
    TUI_CHECK_ERR(!path.empty(), -EINVAL);

    dirp = opendir(&path[0]);
    TUI_CHECK_ERR(dirp != NULL, ret);

    devicePath = path + '/';

    while ((dirInfo = readdir(dirp)) != NULL) {
        if ((!strncmp(dirInfo->d_name, ".", 10)) ||
            (!strncmp(dirInfo->d_name, "..", 10)))
            continue;
        TUILOGD("%s::%d - d_name - %s", __func__, __LINE__,
                dirInfo->d_name);
        if ((dirInfo->d_type == DT_REG) &&
                (strcmp(dirInfo->d_name, controlFileName[1].c_str()) == 0)) {
            mTouchTypeFile = devicePath + controlFileName[0];
            mControlFile = devicePath + controlFileName[1];
            mTouchDevicePathFile = devicePath + controlFileName[2];

            ret = getDeviceBusPath();
            if (ret != ITrustedInput::SUCCESS) {
                mTouchTypeFile.clear();
                mControlFile.clear();
                mTouchDevicePathFile.clear();
                break;
            }

            TUILOGD("%s::%d - Trusted touch enable found @ %s", __func__,
                    __LINE__, mControlFile.c_str());
            ret = ITrustedInput::SUCCESS;
            break;
        }
    }
    closedir(dirp);

errorExit:
    return ret;
}

int32_t CTouchInput::getDeviceBusPath() {
    int32_t ret = ITrustedInput::SUCCESS;
    std::ifstream touchFile;

    touchFile.open(mTouchDevicePathFile.c_str(), std::ifstream::in);
    TUI_CHECK_ERR(!touchFile.fail(), -EINVAL);
    getline(touchFile, mDeviceBusPath, '\0');
    TUILOGE("%s::%d Touch device bus path: %s", __func__, __LINE__, mDeviceBusPath.c_str());

errorExit:
    touchFile.close();
    return ret;
}

/* Description : This API gets the controlfd and validates if the touch driver
 * fd can now be accessed by the TVM.
 *
 *
 * Return : ITrustedInput::ERROR_ACCESS_CONTROLFD,
 *          ITrustedInput::ERROR_OPEN_CONTROLFD,
 *          ITrustedInput::ERROR_WRITE_CONTROLFD,
 *          ITrustedInput::ERROR_INVALID_DISPLAY_TYPE,
 *          ITrustedInput::SUCCESS
 *
 */

int32_t CTouchInput::validateDeviceAssignToVM() {
  int32_t ret = ITrustedInput::SUCCESS;
  ssize_t writtenBytes = 0;
  ssize_t readBytes = 0;
  char c;
  int32_t retry = MAX_RETRY_ATTEMPTS;

  Object_retain(gTVMEnv);
  ITEnv tvmEnvOpener(gTVMEnv);

  /* Search recursively for the controlfile name */
  ret = searchControlDevice();
  TUI_CHECK_COND(ret == ITrustedInput::SUCCESS && !mControlFile.empty() && !mTouchTypeFile.empty(),
                 (ret == ITrustedInput::ERROR_INVALID_DISPLAY_TYPE ? ret : ITrustedInput::ERROR_ACCESS_CONTROLFD));


  TUILOGD("%s::%d - Opening control file and touchdata file: %s", __func__,
          __LINE__, mControlFile.c_str());

  TUI_CHECK_ERR(!tvmEnvOpener.isNull(), ITrustedInput::ERROR_OPEN_CONTROLFD);
  mTouchControl = new ITTouchControlService();
  TUI_CHECK_COND(mTouchControl != nullptr, ITrustedInput::ERROR_OPEN_CONTROLFD);
  tvmEnvOpener.open(CTTouchControlService_UID, *mTouchControl);
  TUI_CHECK_ERR(!mTouchControl->isNull(), ITrustedInput::ERROR_OPEN_CONTROLFD);

  ret = mTouchControl->acquireTouch(mControlFile.c_str(), mControlFile.length() + 1);

errorExit:
  return ret;
}
#endif

/* Description : This API polls on mTouchDataFd and mAbortFd with a timeout as
 * set in mTimeout.
 *
 *
 * Return : -errno,
 *          -ETIMEDOUT,
 *          -ECONNABORTED,
 *          ITrustedInput::SUCCESS
 *
 */

int32_t CTouchInput::waitForEvent() {
  int32_t ret = -1;
  uint64_t w;
  struct pollfd fds[MAX_EVENTS]; /* Used for poll() */
  /* TOUCH FD  */
  int32_t touchDataFd = mTouchDevice->getTouchDeviceFd();
  TUI_CHECK_ERR(touchDataFd >= 0, ITrustedInput::ERROR_OPEN_TOUCHDATAFD);
  TUILOGD("%s::%d - mTouchDataFd - %d, mAbortFd - %d", __func__, __LINE__,
          touchDataFd, mAbortFd);
  fds[TOUCH_EVENT].fd = touchDataFd;
  fds[TOUCH_EVENT].events = POLLIN | POLLPRI | POLLERR;
  /* FD for abort requests */
  fds[ABORT_EVENT].fd = mAbortFd;
  fds[ABORT_EVENT].events = POLLIN;

  ret = poll(fds, MAX_EVENTS, mTimeout);
  TUILOGD("%s::%d - poll return value : %d", __func__, __LINE__, ret);
  /* Error, return error condition */
  TUI_CHECK_ERR(ret >= ITrustedInput::SUCCESS, -errno);
  /* timeout */
  TUI_CHECK_ERR(ret > ITrustedInput::SUCCESS, -ETIMEDOUT);

  /* Check for TouchData event */
  if (fds[0].revents) {
    ret = ITrustedInput::SUCCESS;
    goto errorExit;
  }
  /* Check for external abort */
  else if (fds[1].revents) {
    TUILOGD("%s::%d - Read from abortFd to empty it", __func__, __LINE__);
    read(mAbortFd, &w, sizeof(uint64_t));
    ret = -ECONNABORTED;
    goto errorExit;
  }

errorExit:
  TUILOGD("%s::%d - fd[0].revents:%d fd[1], revents:%d ret : %d, errno = %s",
          __func__, __LINE__, fds[0].revents, fds[1].revents, ret,
          strerror(errno));

  return ret;
}

/* Description :  This API polls for the eventfd and touch event.
 * If there is a touch event, it notifies either
 * a) If timeout, then  it notifies the VM Client App that it couldn't get the
 * touchdata.
 * b) Notifies the VM Client App with the touch data within the timeout period.
 *
 *
 * Return : -errno,
 *          -ETIMEDOUT,
 *          -ECONNABORTED,
 *          -EBUSY,
 *          ITrustedInput::ERROR_READ_FAILED,
 *          ITrustedInput::ERROR_INVALID_FINGER_NUM,
 *          ITrustedInput::SUCCESS
 *
 */

void CTouchInput::getTouchData() {
  int32_t ret = ITrustedInput::SUCCESS;
  int32_t notifyRet = ITrustedInput::SUCCESS;
  vector<int8_t> touchInput;
  size_t touchInputSize = 0;
  if (mTouchInputNotify == nullptr) {
    TUILOGE("%s::%d - Invalid Input Callback (null)", __func__, __LINE__);
    return;
  }
  do {
    ret = waitForEvent();
    TUILOGD("%s::%d - waitForEvent returned ret - %d", __func__, __LINE__, ret);
    if (ret == -ETIMEDOUT) {
      /*If timedout and no touchevent occurred, then call VMClientApp via
       * notifyTimeout API call. */
      notifyRet = mTouchInputNotify->notifyTimeout();
      TUILOGD("%s::%d - notifyTimeout ret - %d", __func__, __LINE__, notifyRet);
      return;
    }

    TUI_CHECK_ERR(ret == ITrustedInput::SUCCESS, ret);

    /*After poll returns success for touchdata fd, read the touch data and
    * process it to get the touch events */
    ret = mTouchDevice->readTouchData(touchInput, touchInputSize);
    if (ret == ERROR_INVALID_FINGER_NUM) {
      continue;
    }
    TUI_CHECK_COND(ret == ITrustedInput::SUCCESS, ret);
    /* Pass the touch data events to the VMClientApp via notifyInput API
     * call. */
    notifyRet = mTouchInputNotify->notifyInput(
        static_cast<void *>(touchInput.data()), touchInputSize);
    TUILOGD("%s::%d - notifyInput ret - %d", __func__, __LINE__, notifyRet);
  } while (ret ==  ERROR_INVALID_FINGER_NUM);
errorExit:
  /* If any error occurs during getTouchData, notify the error to
   * VMClientApp */
  if ((ret) && (ret != -ECONNABORTED)) {
    TUILOGD("Notify app of error in case  it was not an abort triggerd by app");
    TUILOGE("%s::%d - notifyError to app - %d", __func__, __LINE__, ret);
    notifyRet = mTouchInputNotify->notifyError(ret);
    TUILOGD("%s::%d - notifyError ret - %d", __func__, __LINE__, notifyRet);

  }
  return;
}

/* Description : This API creates a mutex lock during init and waits for the
 * getInput API to set the condition variable to poll for the touch event.
 */
void CTouchInput::touchHandler() {
  unique_lock<mutex> lk(mMutex);
  while (mTouchSessionActive) {
    // unique_lock<mutex> lk(mMutex);
    mThreadState = WAITING_ON_APP_REQ;
    /* Wait for the getInput call from VMClientApp to trigger getTouchData
     * call */
    TUILOGD("%s, %d : Going to wait for App request", __func__, __LINE__);
    mCondVar.wait(lk);
    /* Check session state again in-case it changed while we were waiting on app
     * request */
    if (mTouchSessionActive == false) {
      break;
    }
    mThreadState = WAITING_ON_INPUT_EVENT;
    getTouchData();
  }
  TUILOGV("Exiting Touch handler thread");
  mThreadState = INVALID;
}

/* Override retain and release function calls */
void CTouchInput::retain() {
  auto old_value = refcount++;
  assert(old_value > 0);
}

bool CTouchInput::release() {
  auto old_value = refcount--;
  if(refcount < 1) {
    TUILOGE("Refcount %d terminate input session",uint32_t(refcount));
    this->terminate();
    this->deregisterCallback();
  }
  assert(old_value > 0);
  return old_value == 1;
}


/******************************************************************************
 *                        Public Class Function Definitions
 *****************************************************************************/

/* Description :  This is the mink interface API that is called by
 * CoreService to initiate the session.
 * The following functionalities are performed:
 * Functionality 1 - Here, we check if the touch driver is assigned to
 * TouchInput VM, if so, we obtain the file descriptor for Touch driver
 * and also eventfd.
 * Functionality 2 - Create a thread that conditionally waits for the getInput
 * call from VMCLientApp.
 * Functionality 3 - Retain the call-back object of VMClientApp to call
 * notifyTimeout or to notify touchdata or notify error in the previously
 * created thread.
 *
 *
 * In : TouchInputCBORef : Touch input Callback object of VMClientAapp.
 *      screenX : screen X value from the Coreservice.
 *      screenY : screen Y value from the Coreservice.
 *
 * Return : ITrustedInput::ERROR_ACCESS_CONTROLFD,
 *          ITrustedInput::ERROR_OPEN_CONTROLFD,
 *          ITrustedInput::ERROR_READ_DEVICENODE,
 *          ITrustedInput::ERROR_GET_DEVICENODE,
 *          ITrustedInput::ERROR_IOCTL_TOUCHDATAFD,
 *          ITrustedInput::ERROR_OPEN_TOUCHDATAFD,
 *          ITrustedInput::ERROR_REGISTER_CBO,
 *          ITrustedInput::ERROR_CREATE_THREAD,
 *          ITrustedInput::ERROR_INVALID_TOUCH_DEVICENODE,
 *          ITrustedInput::ERROR_TOUCH_SESSION_ACTIVE,
 *          ITrustedInput::ERROR_INVALID_DISPLAY_TYPE,
 *          ITrustedInput::SUCCESS
 *
 */

int32_t CTouchInput::init(uint32_t displayType, uint32_t screenX,
                          uint32_t screenY) {
  int32_t ret = ITrustedInput::SUCCESS;
  if (mTouchSessionActive == true) {
    TUILOGE("%s::%d - Already init call has been made and session has started.",
            __func__, __LINE__);
    return ITrustedInput::ERROR_TOUCH_SESSION_ACTIVE;
  }

#ifdef TEST_ON_LEVM
  TUILOGD("%s::%d displayType - %u", __func__, __LINE__, displayType);
  switch (displayType) {
    case ITUICORESERVICE_PRIMARY_DISPLAY:
      mDisplayType = displayTypes[0];
    break;
    case ITUICORESERVICE_SECONDARY_DISPLAY:
      mDisplayType = displayTypes[1];
    break;
  }
  ret = validateDeviceAssignToVM();
  TUI_CHECK_COND(ret == ITrustedInput::SUCCESS, ret);
#endif

  ret = mTouchDevice->openTouchDeviceFd(screenX, screenY, mDeviceBusPath);
  TUI_CHECK_COND(ret == ITrustedInput::SUCCESS, ret);
  /* Create an fd for forced termination of the thread polling on input events
  * during any interrupt from external user applications */
  mAbortFd = eventfd(0, 0);
  TUILOGD("%s::%d - mAbortFd - %d", __func__, __LINE__, mAbortFd);

  mTouchSessionActive = true;

  /* Create a thread to take care of touchinput events in a separate thread.
   */
  mThread = make_shared<thread>([&]() { this->touchHandler(); });
  TUI_CHECK_ERR(mThread != nullptr, -EPERM);
  /* Wait for touch thread to get started and actively wait on app requests */
  while (mThreadState != WAITING_ON_APP_REQ) {
    TUILOGD("%s::%d - wait for touch thread to start ", __func__, __LINE__);
    usleep(100);
  };
errorExit:
  if (ret) {
    mTouchSessionActive = false;
    if (mTouchControl) {
      mTouchControl->releaseTouch();
      delete mTouchControl;
      mTouchControl = nullptr;
    }
    mTouchDevice->closeTouchDeviceFd();
    if (mAbortFd >= 0) {
      close(mAbortFd);
      mAbortFd = -1;
    }
  }
  return ret;
}

/* Description :  This is the mink interface API called by the VMClientApp.
 * This API notifies the thread that was created in init() call, which then
 * polls for the touch data using touchdata file descriptor and also polls
 * for eventfd for any external user-application interrupts.
 * When a touch event occurs, it calls "TouchDevice" class APIs to read the
 * touchevent and processes corresponding finger number data.Eventually it
 * either passes the touch data buffer or it notifies the App that it has timed
 * out.
 * Basically, touch data is asynchronously shared with VMClientApp using
 * call-back object.
 *
 *
 * In : timeout : timeout period to notify touchdata to the VMClientApp.
 *
 * Return :  ITrustedInput::SUCCESS,
 *           ITrustedInput::ERROR_POLL_IS_STILL_ACTIVE
 *
 */

int32_t CTouchInput::getInput(int32_t timeout) {
  int32_t ret = ITrustedInput::SUCCESS;
  if (mTouchSessionActive == false) {
    TUILOGE("%s::%d - Touch Session not yet initiated by init().", __func__,
            __LINE__);
    return ITrustedInput::ERROR_TOUCH_SESSION_INACTIVE;
  }
  {
    lock_guard<mutex> lk(mMutex);
    TUILOGD("%s++, %d Check the thread state with lock held", __func__, __LINE__);
    if (mThreadState != WAITING_ON_APP_REQ) {
      TUILOGE("%s, %d : Previous Poll is still active", __func__, __LINE__);
      return ITrustedInput::ERROR_POLL_IS_STILL_ACTIVE;
    }
  }
  TUILOGD("%s, %d : Notify touchHandler to getInput", __func__, __LINE__);
  mTimeout = timeout;
  mCondVar.notify_one();
  TUILOGD("%s--, %d : ret : %d", __func__, __LINE__, ret);
  return ret;
}

/* Description :  This API is used to release touch if mink hub dies
*
* Return :  ITrustedInput::ERROR_ACCESS_CONTROLFD
*           ITrustedInput::ERROR_OPEN_CONTROLFD
*           ITrustedInput::ERROR_WRITE_CONTROLFD
*           ITrustedInput::SUCCESS,
*
*/
int32_t CTouchInput::releaseTouchHubDied() {
  int32_t ret = ITrustedInput::SUCCESS;
  ssize_t writtenBytes = 0;
  int32_t controlFd = -1;

  TUI_CHECK_COND(!mControlFile.empty(), ITrustedInput::ERROR_ACCESS_CONTROLFD);

  TUILOGD("%s::%d - Opening control file and touchdata file: %s", __func__,
          __LINE__, mControlFile.c_str());
  controlFd = open(mControlFile.c_str(), O_RDWR);
  if (controlFd < 0) {
    TUILOGE("%s::%d - ControlFD open failed", __func__, __LINE__,
            strerror(-errno));
    TUI_CHECK_COND(controlFd >= 0, ITrustedInput::ERROR_OPEN_CONTROLFD);
  }

  /*Write into controlfd, which notifies the HLOS that the touch ownership
   *is released */
  writtenBytes = pwrite(controlFd, "0", 1, 0);
  TUI_CHECK_COND(writtenBytes > 0, ITrustedInput::ERROR_WRITE_CONTROLFD);

  if (ret == ITrustedInput::SUCCESS) {
    TUILOGE("%s::%d - Touch device is released from TVM now", __func__, __LINE__);
  }

errorExit:
  if (controlFd >= 0) {
    close(controlFd);
  }
  return ret;
}

/* Description :  This is the mink interface API called by the CoreService to
 * terminate the session.Here, the following functionalities are performed.
 * Functionality 1 - Control of the touch driver access is returned back to
 * Android.
 * Functionality 2 - VMClientApp call-back object is released.
 * Functionality 3 - Thread is joined.
 * Functionality 4 - Close the touchfd and eventfd.
 *
 *
 * Return :  ITrustedInput::ERROR_WRITE_ABORTFD,
 *           ITrustedInput::ERROR_WRITE_CONTROLFD,
 *           ITrustedInput::SUCCESS
 *
 */

int32_t CTouchInput::terminate() {
  int32_t ret = ITrustedInput::SUCCESS;
  uint64_t c = 1;
  ssize_t writtenBytes = 0;
  ssize_t readBytes = 0;
  int32_t retry = MAX_RETRY_ATTEMPTS;
  if (mTouchSessionActive == false) {
    TUILOGE("%s::%d - No ongoing touch session" , __func__,
            __LINE__);
    return ITrustedInput::ERROR_TOUCH_SESSION_INACTIVE;
  }
  /* Stop the touchEvent thread from waiting for getInput*/
  mTouchSessionActive = false;
  mCondVar.notify_one();
  /* Write to abortFd to forcefully terminate the poll on eventfd. */
  writtenBytes = write(mAbortFd, &c, sizeof(c));
  TUILOGD("%s::%d - write(fd=%d) writtenBytes %zd", __func__, __LINE__,
          mAbortFd, writtenBytes);
  TUI_CHECK_COND(writtenBytes > 0, ITrustedInput::ERROR_WRITE_ABORTFD);
  /* Wait for the touch handler thread to read abortFd and to join */
  if (mThread && mThread->joinable()) {
    TUILOGD("%s::%d - Stopping wait-thread", __func__, __LINE__);
    mThread->join();
    mThread = nullptr;
  }

  TUI_CHECK_COND(mTouchControl != nullptr, ITrustedInput::ERROR_ACCESS_CONTROLFD);
  ret = mTouchControl->releaseTouch();
  if (ret != ITrustedInput::SUCCESS) {
    ret = releaseTouchHubDied();
  }
errorExit:
  mTouchDevice->closeTouchDeviceFd();
  TUILOGD("%s::%d - close abortFd: %d", __func__, __LINE__, mAbortFd);
  close(mAbortFd);
  mAbortFd = -1;
  if (mTouchControl) {
    delete mTouchControl;
    mTouchControl = nullptr;
  }
  mDisplayType.clear();
  mTouchTypeFile.clear();
  mControlFile.clear();

  TUILOGE("%s::%d - Touch Session terminated ret: %d", __func__, __LINE__, ret);
  return ret;
}

/* Description :  This is the mink interface API that is called by
 * CoreService to register the callback object.
 *
 * Following functionalities are performed:
 * Functionality - Retain the call-back object of VMClientApp to call
 * notifyTimeout or to notify touchdata or notify error in the previously
 * created thread.
 *
 * In : TouchInputCBORef : Touch input Callback object of VMClientAapp.
 *
 * Return : ITrustedInput::ERROR_REGISTER_CBO,
 *          ITrustedInput::SUCCESS
 *
 */
int32_t CTouchInput::registerCallback(const TrustedInputCallBack &TouchInputCBORef) {
  int32_t ret = ITrustedInput::SUCCESS;
  mTouchInputNotify = new TrustedInputCallBack(TouchInputCBORef);
  TUI_CHECK_COND((mTouchInputNotify != nullptr && !mTouchInputNotify->isNull()),
                 ITrustedInput::ERROR_REGISTER_CBO);
errorExit:
  return ret;
}

/* Description :  This is the mink interface API that is called by
 * CoreService to release the call-back object of VMClientApp to
 * de-register input callback.
 *
 * Return : ITrustedInput::SUCCESS
 *
 */
int32_t CTouchInput::deregisterCallback() {
  /* Release the VMClientApp call-back object */
  TUILOGD("%s::%d - Release Input CBO reference", __func__, __LINE__);
  if (mTouchInputNotify) {
    delete mTouchInputNotify;
    mTouchInputNotify = nullptr;
  }
  return ITrustedInput::SUCCESS;
}
