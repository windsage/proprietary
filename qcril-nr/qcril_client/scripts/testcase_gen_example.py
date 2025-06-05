#******************************************************************************
#  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************

# Example Script to demonstrate AFL Testcase Generation APIs

import ril_api
import os.path
import argparse
from time import sleep

parser = argparse.ArgumentParser(description="An example of testcase generation.")
parser.add_argument('output_dir', help="Directory where testcases will be written to.")

def init_callback():
    print("API Session Error")
    return

def dial_callback():
    print("dial cb")
    return

def gen_cb():
    print("generic cb")
    return

def main(output_dir):
    ril_api.initialize(init_callback)
    ril_api.setSaveTestcases(True)
    print("Testcase generation is set to " + str(ril_api.getSaveTestcases()))
    ril_api.setTestcaseOutputDir(output_dir)
    print("Output directory set to " + str(ril_api.getTestcaseOutputDir()))
    ril_api.dial("1234567890", dial_callback)
    ril_api.getVoiceRegStatus(gen_cb)
    print("Sleeping so client doesn't disconnect before testcases are written.")
    sleep(5)

if __name__ == "__main__":
    args = parser.parse_args()
    main(args.output_dir)
