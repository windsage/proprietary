/***********************************************************
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
************************************************************/

#include <iostream>
#include "smcinvoke_skeleton.h"
#include "object.h"
using namespace std;

#define ARG_HAS_ITERATION 3

static void usage(void)
{
  cout<<"*************************************************************\n";
  cout<<"************     SMCINVOKE_SKELETON_CPP CLIENT    ***************\n";
  cout<<"*************************************************************\n";
  cout<<"\n"
         "Runs the user space tests specified by the TEST_TYPE\n"
         "OPTION can be:\n"
         "h : Print this help message and exit\n\n\n"
         "\t- adb push smcinvoke_skeleton_ta64.mbn to /data on device from apps crm\n"
         "\t- Connect to device: From command shell, do 'adb shell'\n"
         "\t- Run smcinvoke_skeleton_cpp:\n"
         "\t- do './smcinvoke_skeleton_cpp <appPath> <Iterations>'\n"
         "\t- Exmaple :-\n"
         "\t- smcinvoke_skeleton_cpp /data/smcinvoke_skeleton_cpp_ta64.mbn 1\n"
         "---------------------------------------------------------\n\n\n";
}


int main(int argc, char *argv[])
{
  int32_t exampleRet = Object_ERROR;
  char *appPath = NULL;
  int32_t optind = 1;
  int32_t test_iterations = 0;

  usage();
  if (argc < ARG_HAS_ITERATION) {
    cout<<"Arguments passed are less than expected\n";
    return -1;
  }

  if (argv == NULL) {
    cout<<"No arguments to process, exiting! \n";
    return -1;
  }

  /* read the appPath from cmd line arguments */
  appPath = argv[1];
  cout<<"appPath "<< appPath<<"\n";

  /* Iterations */
  test_iterations = atoi(argv[2]);
  if (test_iterations < 1) {
    cout<<"Iteration passed is less than 1!!\n";
    return -1;
  }
  for (int32_t i=0; i< test_iterations; i++) {
    cout<<"Running for iteration "<<i<<"\n";
    exampleRet = run_smcinvoke_ta_example(appPath);
  }

  if (exampleRet) {
    cout<<"Errors were encountered during execution: "<< exampleRet<<"\n";
  } else {
    cout<<"CA executed successfully.\n";
  }
  return exampleRet;
}
