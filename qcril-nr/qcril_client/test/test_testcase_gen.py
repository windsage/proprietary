#******************************************************************************
#  Copyright (c) 2022-2023 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************

import unittest
import ril_api
from .utils import init_cb

import subprocess
import tempfile
import os.path
from time import sleep


# Python Testcase for AFL testcase generation feature
class TestcaseGenerationTestcase(unittest.TestCase):

    def setUp(self):
        ril_api.reinitialize(init_cb)

    def test_set_save_testcases(self):
        ril_api.setSaveTestcases(True)
        self.assertEqual(ril_api.getSaveTestcases(), True)
        ril_api.setSaveTestcases(False)
        self.assertEqual(ril_api.getSaveTestcases(), False)

    def test_set_output_dir_creates_dir(self):
        # Create temp directory where test data will be saved.
        with tempfile.TemporaryDirectory() as tmp_dir:
            output_path = os.path.join(tmp_dir, "testcases")
            ril_api.setTestcaseOutputDir(output_path)
            self.assertTrue(os.path.exists(output_path))

    def test_request_manager_writes_to_testcase_file(self):
        ril_api.setSaveTestcases(True)
        # Create temp directory where test data will be saved.
        with tempfile.TemporaryDirectory() as tmp_dir:
            output_path = os.path.join(tmp_dir, "testcases")
            ril_api.setTestcaseOutputDir(output_path)
            dial_cb = lambda : print("dial cb")
            ril_api.dial("1234567890", dial_cb)
            sleep(1)
            testcase_files = os.listdir(output_path)
            # Test that the output dir is  non-empty
            self.assertTrue(len(testcase_files) > 0)
            for f in testcase_files:
                if "afl_testcase_dial" in f:
                    dial_testcase  = output_path.join(f)
                    # Test that the dial testcase file is non-empty
                    self.assertTrue(os.path.getsize(dial_testcase) > 0)

if __name__ == '__main__':
    unittest.main()
