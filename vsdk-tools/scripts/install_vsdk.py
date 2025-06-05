#!/usr/bin/python
# -*- coding: utf-8 -*-
#Copyright (c) 2021-2022 Qualcomm Technologies, Inc.
#All Rights Reserved.
#Confidential and Proprietary - Qualcomm Technologies, Inc.

import argparse
import logging
import os
import sys
from subprocess import check_call
from subprocess import Popen
from subprocess import check_output
import xml.etree.ElementTree
import glob

'''
Script Versioning:

Version 1.0:
  - Downloads the vsdk from a remote git and installs it. Supports
    downloading it to a custom local path (--vsdk_download_path).
  - Supports picking up vsdk from a local path as well (--local_vsdk_path).
  - Before installing vsdk, vsdk version matching is done with what the vendor
    expects. (--force_install) can be given to skip it for developmental purposes.

'''
__version__ = '1.0'

logger = logging.getLogger(__name__)

VSDK_DOWNLOAD_PATH = "vendor/qcom/vsdk/"
VSDK_HWASAN_DOWNLOAD_PATH = "vendor/qcom/vsdk-hwasan/"
VSDK_TOOLS_PATH = "vendor/qcom/proprietary/vsdk-tools/"
VSDK_METADATA_FILE = "vsdk_metadata.xml"

def unzip_raw_data(ws_root, vsdk_path):
    artifact_dir = vsdk_path
    artifact_pattern = "raw-qssi.zip"
    artifacts = glob.glob(os.path.join(artifact_dir, artifact_pattern))
    for artifact in artifacts:
        logging.info('Unzipping raw data: {}'.format(artifact))
        status = check_call(['unzip', '-qn', artifact, '-d', ws_root])

def RunCommand(command):
  cmd = command.split()
  logging.info("Running: " + str(cmd))
  status = check_call(cmd)

def validate_vsdk_metadata(ws_root, vsdk_path):
  # Compare the vsdk version ID against what the vendor build expects.
  vendor_expected_vsdk_version = xml.etree.ElementTree.parse(ws_root + "/" + VSDK_TOOLS_PATH + VSDK_METADATA_FILE).getroot().find('Version').get('ID')
  if os.path.isfile(vsdk_path + "/" + VSDK_METADATA_FILE):
    if xml.etree.ElementTree.parse(vsdk_path + "/" + VSDK_METADATA_FILE).getroot().find('Version') is None:
      logging.error(VSDK_METADATA_FILE + " doesn't have a VSDK version. This is expected with a developer generated VSDK. If that is the case, please use/add --force_install option while invoking install_vsdk.py")
      sys.exit(1)
    else:
      actual_vsdk_version = xml.etree.ElementTree.parse(vsdk_path + "/" + VSDK_METADATA_FILE).getroot().find('Version').get('ID')
  else:
    logging.error(VSDK_METADATA_FILE + " file not found in the VSDK path (" + vsdk_path + "), please ensure the VSDK is correctly generated/downloaded, aborting...")
    sys.exit(1)

  if vendor_expected_vsdk_version != actual_vsdk_version:
    logging.error("VSDK version provided: " + actual_vsdk_version)
    logging.error("VSDK version expected: " + vendor_expected_vsdk_version)
    logging.error("VSDK incompatible with this vendor workspace sync due to version mismatch, aborting vsdk installation..")
    sys.exit(1)

def install_vsdk(ws_root, force_install, vsdk_path):
  snapshots_enabled_list = xml.etree.ElementTree.parse(vsdk_path + "/" + VSDK_METADATA_FILE).getroot().find('Snapshots').get('Enabled').split(',')
  snapshots_enabled_list = [i.strip() for i in snapshots_enabled_list]

  vndk_snapshot_enabled = "vndk" in snapshots_enabled_list
  vendor_snapshot_enabled = "vendor" in snapshots_enabled_list
  recovery_snapshot_enabled = "recovery" in snapshots_enabled_list
  ramdisk_snapshot_enabled = "ramdisk" in snapshots_enabled_list
  host_snapshot_enabled = "host" in snapshots_enabled_list
  otatools_snapshot_enabled = "otatools" in snapshots_enabled_list
  rawdata_snapshot_enabled = "rawdata" in snapshots_enabled_list
  java_snapshot_enabled = "java" in snapshots_enabled_list

  logging.info("Snapshots enabled: vndk:" + str(vndk_snapshot_enabled) +
          " vendor:" + str(vendor_snapshot_enabled) +
          " recovery:" + str(recovery_snapshot_enabled) +
          " ramdisk:" + str(ramdisk_snapshot_enabled) +
          " host:" + str(host_snapshot_enabled) +
          " otatools:" + str(otatools_snapshot_enabled) +
          " rawdata:" + str(rawdata_snapshot_enabled))

  os.environ["ANDROID_BUILD_TOP"] = ws_root

  if rawdata_snapshot_enabled:
    root = xml.etree.ElementTree.parse(vsdk_path + "/rawdata_frozen_prjs_manifest.xml").getroot()
    frozen_prjs = []
    for prj in root.findall('project'):
      if prj.get("path") is not None:
        src_path = prj.get("path")
      else:
        src_path = prj.get("name")
      frozen_prjs.append(src_path)

    if not os.path.exists(ws_root + "/.repo/local_manifests/"):
      os.makedirs(ws_root + "/.repo/local_manifests/")
    RunCommand("cp " + vsdk_path + "/rawdata_frozen_prjs_manifest.xml " + ws_root + "/.repo/local_manifests/")
    RunCommand("repo sync --no-tags -qc -j8 --optimized-fetch --no-repo-verify --no-clone-bundle " + ' '.join(frozen_prjs))
    unzip_raw_data(ws_root, vsdk_path)

  # Create directories for various snapshots and install them.
  vsdk_snapshots_config_dir = ws_root + "/vendor/qcom/vsdk_snapshots_config"
  RunCommand("rm -rf " + vsdk_snapshots_config_dir)
  RunCommand("mkdir -p " + vsdk_snapshots_config_dir)
  vsdk_snapshots_config_file = open(vsdk_snapshots_config_dir + "/config.mk", "w")

  api_level = "34"
  if vndk_snapshot_enabled:
    RunCommand("mkdir -p prebuilts/vndk/v" + api_level)
    RunCommand("python3 " + ws_root  + "/development/vndk/snapshot/update.py --use-current-branch --local " + vsdk_path + " " + api_level)
    vsdk_snapshots_config_file.write("BOARD_VNDK_VERSION := " + api_level + "\n")
  else:
    vsdk_snapshots_config_file.write("BOARD_VNDK_VERSION := current\n")

  if vendor_snapshot_enabled:
    RunCommand("mkdir -p vendor/qcom/vendor_snapshot")
    RunCommand("python3 " + ws_root + "/development/vendor_snapshot/update.py --image vendor --local " + vsdk_path + " --install-dir vendor/qcom/vendor_snapshot/v" + api_level + " --overwrite " + api_level)

  if recovery_snapshot_enabled:
    RunCommand("mkdir -p vendor/qcom/recovery_snapshot")
    RunCommand("python3 " + ws_root + "/development/vendor_snapshot/update.py --image recovery --local " + vsdk_path + " --install-dir vendor/qcom/recovery_snapshot/v" + api_level + " --overwrite " + api_level)
    vsdk_snapshots_config_file.write("RECOVERY_SNAPSHOT_VERSION := " + api_level + "\n")
  else:
    vsdk_snapshots_config_file.write("RECOVERY_SNAPSHOT_VERSION := current\n")

  if ramdisk_snapshot_enabled:
    RunCommand("mkdir -p vendor/qcom/ramdisk_snapshot")
    RunCommand("python3 " + ws_root + "/development/vendor_snapshot/update.py --image ramdisk --local " + vsdk_path + " --install-dir vendor/qcom/ramdisk_snapshot/v" + api_level + " --overwrite " + api_level)
    vsdk_snapshots_config_file.write("RAMDISK_SNAPSHOT_VERSION := " + api_level + "\n")
  else:
    vsdk_snapshots_config_file.write("RAMDISK_SNAPSHOT_VERSION := current\n")

  if host_snapshot_enabled:
    RunCommand("mkdir -p vendor/qcom/host_snapshot/v" + api_level)
    RunCommand("python3 " + ws_root + "/development/vendor_snapshot/update.py --image host --local " + vsdk_path + " --install-dir vendor/qcom/host_snapshot/v" + api_level + " --overwrite " + api_level)

  if otatools_snapshot_enabled:
    RunCommand("mkdir -p vendor/qcom/otatools_snapshot/")
    RunCommand("cp " + vsdk_path + "/otatools.zip vendor/qcom/otatools_snapshot/")

  vsdk_snapshots_config_file.close()

  if java_snapshot_enabled:
    open(vsdk_snapshots_config_dir + "/java_snapshot_enabled", "w").close()

  if os.path.exists(vsdk_path + "/qssi_manifest.xml"):
    RunCommand("mkdir -p vendor/qcom/vsdk_staging_area/")
    RunCommand("cp " + vsdk_path + "/qssi_manifest.xml vendor/qcom/vsdk_staging_area/")

def delete_existing_vsdk(ws_root):
  os.environ["ANDROID_BUILD_TOP"] = ws_root

  # delete directories for various snapshots
  api_level = "34"
  RunCommand("rm -rf " + ws_root + "/prebuilts/vndk/v" + api_level)
  RunCommand("rm -rf " + ws_root + "/vendor/qcom/vsdk_snapshots_config")
  RunCommand("rm -rf " + ws_root + "/vendor/qcom/vendor_snapshot")
  RunCommand("rm -rf " + ws_root + "/vendor/qcom/recovery_snapshot")
  RunCommand("rm -rf " + ws_root + "/vendor/qcom/ramdisk_snapshot")
  RunCommand("rm -rf " + ws_root + "/vendor/qcom/host_snapshot")
  RunCommand("rm -rf " + ws_root + "/vendor/qcom/otatools_snapshot/")
  RunCommand("rm -rf " + ws_root + "/vendor/qcom/vsdk_staging_area/")
  RunCommand("rm -rf " + VSDK_DOWNLOAD_PATH)
  RunCommand("rm -rf " + VSDK_HWASAN_DOWNLOAD_PATH)

def download_vsdk(ws_root, vsdk_repo, vsdk_path, vsdk_hwasan):
  RunCommand("rm -rf " + vsdk_path)
  vsdk_version = xml.etree.ElementTree.parse(ws_root + "/" + VSDK_TOOLS_PATH + VSDK_METADATA_FILE).getroot().find('Version').get('ID')
  if vsdk_repo is not None:
      RunCommand("git clone --depth 1 " + vsdk_repo + " -b " + vsdk_version + " " + vsdk_path)
  else:
      if vsdk_hwasan is False:
          RunCommand("curl -k -O https://artifactory-edge.qualcomm.com/artifactory/qssi-vsdk-virtual/VSDK.14.0/" + vsdk_version + ".zip")
          RunCommand("mkdir " + vsdk_path)
          RunCommand("mv" + " " + vsdk_version + ".zip" + " " + vsdk_path)
          RunCommand("unzip" + " " + vsdk_path + "/" + vsdk_version + ".zip" + " -d " + vsdk_path)
          RunCommand("rm -rf" + " " + vsdk_path + "/" + vsdk_version + ".zip")
      else:
          RunCommand("curl -k -O https://artifactory-edge.qualcomm.com/artifactory/qssi-vsdk-virtual/VSDK.14.0/" + vsdk_version + "_hwasan.zip")
          RunCommand("mkdir " + vsdk_path)
          RunCommand("mv" + " " + vsdk_version + "_hwasan.zip" + " " + vsdk_path)
          RunCommand("unzip" + " " + vsdk_path + "/" + vsdk_version + "_hwasan.zip" + " -d " + vsdk_path)
          RunCommand("rm -rf" + " " + vsdk_path + "/" + vsdk_version + "_hwasan.zip")

def main():
  logging_format = '%(asctime)s - %(filename)s - %(levelname)-8s: %(message)s'
  logging.basicConfig(level=logging.INFO, format=logging_format, datefmt='%Y-%m-%d %H:%M:%S')

  parser = argparse.ArgumentParser()
  parser.add_argument("--local_vsdk_path", dest='local_vsdk_path',
                    help="Local path to pick the vsdk from, instead of remote fetching.")
  parser.add_argument("--vsdk_repo", dest='vsdk_repo',
                    help="Expose this option to indicate the remote git repository to download vsdk.")
  parser.add_argument("--vsdk_download_path", dest='vsdk_download_path',
                    help="Downloads the vsdk from remote git to this local path. This downloaded package can further be re-used through --local_vsdk_path option.")
  parser.add_argument("--force_install", dest='force_install',
                      help="Force install the vsdk, without enforcing the vsdk version matching checks.", action='store_true')
  parser.add_argument('--version', action='version', version=__version__)
  parser.add_argument("--vsdk_hwasan", dest='vsdk_hwasan',
                    help="To download and install hwasan vsdk from repository.", action='store_true')
  args = parser.parse_args()

  logging.info("Starting the VSDK installation process..")

  # Get the workspace root
  ws_root = os.path.abspath(os.getcwd())

  if args.local_vsdk_path is not None and args.vsdk_download_path is not None:
    logging.error("Use only either of --local_vsdk_path or --vsdk_download_path, see help (-h) menu for description.")
    sys.exit(1)

  # Setup the vsdk path and download vsdk as needed.
  delete_existing_vsdk(ws_root)
  if args.local_vsdk_path is not None:
    vsdk_path = os.path.abspath(args.local_vsdk_path)
  else:
    if args.vsdk_download_path:
      vsdk_path = os.path.abspath(args.vsdk_download_path)
    else:
      if args.vsdk_hwasan is False:
        vsdk_path = os.path.abspath(VSDK_DOWNLOAD_PATH)
      else:
        vsdk_path = os.path.abspath(VSDK_HWASAN_DOWNLOAD_PATH)
    # Download vsdk from remote git.
    logging.info("VSDK download path: " + vsdk_path)
    download_vsdk(ws_root, args.vsdk_repo, vsdk_path, args.vsdk_hwasan)

  # Ensure the vsdk provided in the vsdk_path is compatible with the vendor workspace sync,
  # before proceeding with the installation.
  if not args.force_install:
    validate_vsdk_metadata(ws_root, vsdk_path)

  install_vsdk(ws_root, args.force_install, vsdk_path)
  logging.info("VSDK installation completed successfully !")

if __name__ == '__main__':
  main()
