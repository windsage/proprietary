/********************************************************************
 Copyright (c) 2022 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include "TZCom.h"
#include "IClientEnv.h"
#include "object.h"
#include "CRPMBService.h"
#include "IRPMBService.h"

#ifdef TARGET_ENABLE_QSEECOM
#include "QSEEComAPI.h"
#endif

/** adb log */
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RPMB_CLIENT: "
#ifdef LOG_NDDEBUG
#undef LOG_NDDEBUG
#endif
#define LOG_NDDEBUG 0 //Define to enable LOGD
#ifdef LOG_NDEBUG
#undef LOG_NDEBUG
#endif
#define LOG_NDEBUG  0 //Define to enable LOGV

#define ARG_HAS_ITERATION 4
bool legacyInterface = false;
static uint32_t test_iterations  = 1000;

struct option testCommandOptions[] = {
  {"ProvisionKey", no_argument, NULL, 'p'},
  {"EraseKey", no_argument, NULL, 'e'},
  {"Help", no_argument, NULL, 'h'},
  {NULL, 0, NULL, 0},
};

static int32_t qsc_rpmb_check_legacy() {
#ifdef TARGET_ENABLE_QSEECOM
  int ret = 0;
  uint32_t status;

  ret = QSEECom_send_service_cmd(NULL, 0, &status, sizeof(uint32_t),
                                 QSEECOM_RPMB_CHECK_PROV_STATUS_COMMAND);
  if(ret)
    printf("Failed to check RPMB status, ret = %d\n", ret);
  else {
    switch (status) {
      case 0:
        printf("RMPB Key status: RPMB_KEY_PROVISIONED_AND_OK (%x)\n", status);
        break;
      case QSEECOM_RPMB_KEY_NOT_PROVISIONED:
        printf("RMPB Key status: RPMB_KEY_NOT_PROVISIONED (%x)\n", status);
        break;
      case QSEECOM_RPMB_KEY_PROVISIONED_BUT_MAC_MISMATCH:
        printf("RMPB Key status: RPMB_KEY_PROVISIONED_BUT_MAC_MISMATCH (%x)\n", status);
        break;
      default:
        printf("RPMB Key status: Others (%x)\n", status);
        printf("Please try with smci interface");
        break;
    }
  }
  return ret;
#else
  printf("Legacy interface not supported");
  return -1;
#endif //TARGET_ENABLE_QSEECOM
}

static int32_t qsc_rpmb_check_smci() {
  int ret = 0;
  Object clientEnv = Object_NULL;
  Object rpmbObj = Object_NULL;

  do {
    ret = TZCom_getClientEnvObject(&clientEnv);
    if (Object_isERROR(ret)) {
      clientEnv = Object_NULL;
      printf("TZCom_getClientEnvObject failed with err: %d", ret);
      break;
    }
    ret = IClientEnv_open(clientEnv, CRPMBService_UID, &rpmbObj);
    if (Object_isERROR(ret)) {
      rpmbObj = Object_NULL;
      printf("IClientEnv_open failed with err: %d", ret);
      break;
    }
    ret = IRPMBService_rpmbCheckProv(rpmbObj);
    if (!Object_isERROR(ret))
      printf("RMPB Key status: RPMB_KEY_PROVISIONED_AND_OK (%x)\n", ret);
    else {
      switch(ret) {
        case IRPMBService_ERROR_RPMB_NOT_PROVISIONED:
          printf("RMPB Key status: RPMB_KEY_NOT_PROVISIONED (%x)\n", ret);
          break;
        case IRPMBService_ERROR_RPMB_MAC:
          printf("RMPB Key status: RPMB_KEY_PROVISIONED_BUT_MAC_MISMATCH (%x)\n", ret);
          break;
        default:
          printf("RPMB Key status: Others (%x)\n", ret);
          break;
      }
    }
  }while(0);

  Object_ASSIGN_NULL(clientEnv);
  Object_ASSIGN_NULL(rpmbObj);

  return ret;
}

static int32_t qsc_rpmb_check() {
  if(legacyInterface)
    return qsc_rpmb_check_legacy();
  else
    return qsc_rpmb_check_smci();
}

static void qsc_usage(void)
{
  printf("*************************************************************\n");
  printf("************       RMPB TEST CLIENT           ************\n");
  printf("*************************************************************\n");
  printf("\n"
         "Runs the user space tests specified by the TEST_TYPE\n"
         "OPTION can be:\n"
         "<Interface>  Interface with which you want to run the test\n\n"
         "\tl : Legacy Interface (QSEECOM)\n"
         "\ts : Smcinvoke Interface \n"
         "<cmd_id>   cmd_id of the test(below)\n"
         "\t -p: PROVISION RPMB KEY\n"
         "\t -e: ERASE RPMB PARTITION\n"
         "<Iterations> : Test iterations (> 0) \n"
         "h : Print this help message and exit\n\n\n"
         "\t-'adb push <BUILD>out/target/product/<TARGET>/obj/EXECUTABLES"
         "/rpmbClient_intermediates/rpmbClient /data'\n"
         "\t- Connect to device: From command shell, do 'adb shell'\n"
         "\t- Once in the shell: do 'cd /data' \n"
         "\t- Change permission for rpmbClient: \n"
         "\t- do  chmod 777 rpmbClient\n"
         "\t- Run rpmbClient:\n"
         "\t- do './rpmbClient <Interface> <CMD_ID> <Iterations>'\n"
         "\t- Example :-"
         "\t\t 1. rpmbClient legacy -p 1 \n"
         "\t\t 2. rpmbClient legacy -e 1 \n"
         "\t\t 3. rpmbClient smci -p 1 \n"
         "\t\t 4. rpmbClient smci -e 1 \n"
         "---------------------------------------------------------\n\n\n");
}

static int32_t qsc_rpmb_provision_key_legacy() {
#ifdef TARGET_ENABLE_QSEECOM
  int ret = 0;
  struct qseecom_rpmb_provision_key send_buf = {0};

  qsc_usage();
  printf("\n\n");
  printf("\t-------------------------------------------------------\n");
  printf("\t\t LEGACY INTERFACE \n");
  printf("\t WARNING!!! You are about to provision the RPMB key.\n");
  printf("\t This is a ONE time operation and CANNOT be reversed.\n");
  printf("\t-------------------------------------------------------\n");
  printf("\t 0 -> Provision Production key\n");
  printf("\t 1 -> Provision Test key\n");
  printf("\t 2 -> Check RPMB key provision status\n");
  printf("\t-------------------------------------------------------\n");
  printf("\t Select an option to proceed: ");

  send_buf.key_type = getchar() - '0';

  switch (send_buf.key_type) {
    case 0:
    case 1:
      ret = QSEECom_send_service_cmd((void*) &send_buf, sizeof(send_buf),
                             NULL, 0, QSEECOM_RPMB_PROVISION_KEY_COMMAND);
      if(!ret)
        printf("RPMB key provisioning completed\n");
      else
        printf("RPMB key provisioning failed (%d)\n", ret);
        break;
    case 2:
      ret = qsc_rpmb_check();
      break;
    default:
      printf("Invalid RPMB provision keytype (%d)\n", send_buf.key_type);
      ret = -1;
      break;
  }
  return ret;
#else
  printf("\t LEGACY INTERFACE NOT ENABLED \n");
  printf("\t Try to run the testcase with smci interface \n");
  return -1;
#endif //TARGET_ENABLE_QSEECOM
}

static int32_t qsc_rpmb_provision_key_smci() {
  int ret = 0;
  Object clientEnv = Object_NULL;
  Object rpmbObj = Object_NULL;

  qsc_usage();
  printf("\n\n");
  printf("\t-------------------------------------------------------\n");
  printf("\t\t SMCINVOKE INTERFACE \n");
  printf("\t WARNING!!! You are about to provision the RPMB key.\n");
  printf("\t This is a ONE time operation and CANNOT be reversed.\n");
  printf("\t-------------------------------------------------------\n");
  printf("\t 0 -> Provision Production key\n");
  printf("\t 1 -> Provision Test key\n");
  printf("\t 2 -> Check RPMB key provision status\n");
  printf("\t-------------------------------------------------------\n");
  printf("\t Select an option to proceed: ");

  uint32_t key_type = getchar() - '0';

  switch (key_type) {
    case 0:
    case 1:
      ret = TZCom_getClientEnvObject(&clientEnv);
      if (Object_isERROR(ret)) {
        clientEnv = Object_NULL;
        printf("TZCom_getClientEnvObject failed with err: %d", ret);
        break;
      }

      ret = IClientEnv_open(clientEnv, CRPMBService_UID, &rpmbObj);
      if (Object_isERROR(ret)) {
        rpmbObj = Object_NULL;
        printf("IClientEnv_open failed with err: %d", ret);
        break;
      }

      ret = IRPMBService_rpmbProvisionKey(rpmbObj, key_type);
      if(!Object_isERROR(ret))
        printf("RPMB key provisioning completed\n");
      else
        printf("RPMB key provisioning failed with err: %d", ret);
      break;

    case 2:
      ret = qsc_rpmb_check();
      break;

    default:
      printf("Invalid RPMB provision keytype (%d)\n", key_type);
      ret = -1;
      break;
  }

  Object_ASSIGN_NULL(clientEnv);
  Object_ASSIGN_NULL(rpmbObj);

  return ret;
}

static int32_t qsc_rpmb_provision_key() {
  if(legacyInterface)
    return qsc_rpmb_provision_key_legacy();
  else
    return qsc_rpmb_provision_key_smci();
}

static int32_t qsc_rpmb_erase_legacy() {
#ifdef TARGET_ENABLE_QSEECOM
  int ret = 0;
  char input;

  printf("\t---------------------------------------------------------------\n");
  printf("\t\t LEGACY INTERFACE \n");
  printf("\t WARNING!!! You are about to erase the entire RPMB partition.\n");
  printf("\t----------------------------------------------------------------\n");
  printf("\t Do you want to proceed (y/n)? ");

  input = getchar();
  if (input != 'y')
    return 0;

  ret = QSEECom_send_service_cmd(NULL, 0, NULL , 0, QSEECOM_RPMB_ERASE_COMMAND);
  if (!ret)
    printf("RPMB partition erase completed\n");
  else
    printf("RPMB partition erase failed (%d)\n", ret);

  return ret;
#else
  printf("\t LEGACY INTERFACE NOT ENABLED \n");
  printf("\t Try to run the testcase with smci interface \n");
  return -1;
#endif //TARGET_ENABLE_QSEECOM
}

static int32_t qsc_rpmb_erase_smci() {
  int ret = 0;
  Object clientEnv = Object_NULL;
  Object rpmbObj = Object_NULL;
  char input;

  printf("\t-------------------------------------------------------\n");
  printf("\t\t SMCINVOKE INTERFACE \n");
  printf("\t WARNING!!! You are about to erase the entire RPMB partition.\n");
  printf("\t----------------------------------------------------------------\n");
  printf("\t Do you want to proceed (y/n)? ");

  input = getchar();
  if (input != 'y')
    return 0;

  do {
    ret = TZCom_getClientEnvObject(&clientEnv);
    if (Object_isERROR(ret)) {
        clientEnv = Object_NULL;
        printf("TZCom_getClientEnvObject failed with err: %d", ret);
        break;
    }
    ret = IClientEnv_open(clientEnv, CRPMBService_UID, &rpmbObj);
    if (Object_isERROR(ret)) {
      rpmbObj = Object_NULL;
      printf("IClientEnv_open failed with err: %d", ret);
      break;
    }
    ret = IRPMBService_rpmbErase(rpmbObj);
    if(!Object_isERROR(ret))
      printf("RPMB erase completed\n");
    else {
      printf("RPMB erase failed with err: %d", ret);
      break;
    }
  }while(0);

  Object_ASSIGN_NULL(clientEnv);
  Object_ASSIGN_NULL(rpmbObj);
  return ret;
}

static int32_t qsc_rpmb_erase() {
  if (legacyInterface)
    return qsc_rpmb_erase_legacy();
  else
    return qsc_rpmb_erase_smci();
}

static unsigned int parseInterface(int argc, char *const argv[])
{
  char *command;
  int optind = 1;

  if (argc < ARG_HAS_ITERATION) {
    qsc_usage();
    return -1;
  }

  if (argv == NULL) {
    printf("No arguments to process, exiting! \n");
    return -1;
  }

  command = argv[1];
  if (strcmp(command, "legacy") == 0) {
    printf("Using QSEECOM interface \n");
    legacyInterface = true;
  }
  else if (strcmp(command, "smci") == 0) {
    printf("Using Smcinvoke interface \n");
    legacyInterface = false;
  }
  else
    qsc_usage();
  return 0;
}

int runTestCommand(int argc, char *argv[])
{
  uint32_t optind = 2;
  uint32_t command = 0;
  uint32_t i=0;
  unsigned int ret = 0;

  command = getopt_long(argc, argv, "peh", testCommandOptions, NULL);

  /* Iterations */
  test_iterations = atoi(argv[3]);
  for(i=0; i< test_iterations; i++) {
    switch (command) {
      case 'p':
        ret = qsc_rpmb_provision_key();
        break;
      case 'e':
        ret = qsc_rpmb_erase();
        break;
      case 'h':
      default:
        qsc_usage();
        break;
    }
  }
  return ret;
}

int main(int argc, char *argv[])
{
  uint32_t ret = 0;
  ret = parseInterface(argc, argv);
  if (!ret)
    ret = runTestCommand(argc, argv);
    return ret;
}
