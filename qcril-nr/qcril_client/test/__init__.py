#******************************************************************************
#  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************

from .utils import init_cb
import ril_api
import subprocess
import unittest

def startTestRun(self):
        # Setup RIL socket before running tests in case the user hasn't already done that.
        try:
            subprocess.run(['adb', 'root']).check_returncode()
            subprocess.run(['adb', 'forward', 'tcp:50000',
                            'localfilesystem:/dev/socket/qcrild/rild0']).check_returncode()
        except subprocess.CalledProcessError:
            print("ADB failed.")
            exit(-1)
        #initialize API Session
        ril_api.initialize(init_cb)

setattr(unittest.TestResult, 'startTestRun', startTestRun)
