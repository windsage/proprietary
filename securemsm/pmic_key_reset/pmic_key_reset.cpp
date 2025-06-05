/********************************************************************
---------------------------------------------------------------------
 Copyright (c) 2021 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
----------------------------------------------------------------------
 PMIC Key Reset Configuration App
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "object.h"
#include "IClientEnv.h"
#include "TZCom.h"
#include "CPmPonResetCfg.h"
#include "IPmPonResetCfg.h"

#define RESET_SOURCE_MIN   0
#define RESET_SOURCE_MAX   2
#define RESET_TYPE_MIN     0
#define RESET_TYPE_MAX     2
#define S1_TIMER_MS_MAX    10256
#define S2_TIMER_MS_MAX    2000

static void usage(void) {
  printf("Usage: pmic_key_reset <Reset_Source> <Reset_Type> <S1_Timer_ms> <S2_Timer_ms>\n\n"
         "Configures the reset type along with S1 and S2 timer values for the specified PMIC key\n"
         "Parameters:\n"
         "Reset_Source: 0 = KPDPWR, 1 = RESIN, 2 = RESIN_AND_KPDPWR\n"
         "Reset_Type:   0 = WARM_RESET, 1 = HARD_RESET, 2 = SHUTDOWN\n"
         "S1_Timer_ms:  0 - 10256\n"
         "S2_timer_ms:  0 - 2000\n\n");
}

int main(int argc, char *argv[])
{
  Object clientEnv = Object_NULL;
  Object resetCfg = Object_NULL;
  int ret = Object_OK;
  int32_t ponResetSource;
  int32_t ponResetType;
  uint32_t s1TimerMs;
  uint32_t s2TimerMs;

  if (argc < 5) {
    usage();
    return -1;
  }

  ponResetSource = atoi(argv[1]);
  ponResetType = atoi(argv[2]);
  s1TimerMs = atoi(argv[3]);
  s2TimerMs = atoi(argv[4]);

  if (ponResetSource > RESET_SOURCE_MAX || ponResetSource < RESET_SOURCE_MIN) {
    printf("Invalid Reset_Source = %d, max allowed = %d\n",
      ponResetSource, RESET_SOURCE_MAX);
    return -1;
  }

  if (ponResetType > RESET_TYPE_MAX || ponResetType < RESET_TYPE_MIN) {
    printf("Invalid Reset_Type = %d, max allowed = %d\n",
      ponResetType, RESET_TYPE_MAX);
    return -1;
  }

  if (s1TimerMs > S1_TIMER_MS_MAX) {
    printf("Invalid S1_Timer_ms = %u, max allowed = %u\n",
      s1TimerMs, S1_TIMER_MS_MAX);
    return -1;
  }

  if (s2TimerMs > S2_TIMER_MS_MAX) {
    printf("Invalid S2_Timer_ms = %u, max allowed = %u\n",
      s2TimerMs, S2_TIMER_MS_MAX);
    return -1;
  }

  do {
    ret = TZCom_getClientEnvObject(&clientEnv);
    if (ret) {
      printf("Client environment get failed, ret=%d\n", ret);
      break;
    }

    ret = IClientEnv_open(clientEnv, CPmPonResetCfg_UID, &resetCfg);
    if (ret) {
      printf("Client environment open failed, ret=%d\n", ret);
      break;
    }

    ret = IPmPonResetCfg_ponResetCfg(resetCfg, ponResetSource,
                                     ponResetType, s1TimerMs, s2TimerMs);
    if (ret == IPmPonResetCfg_ERROR_INVALID_PARAM) {
      printf("Request failed: Invalid parameter value\n");
      break;
    } else if (ret == IPmPonResetCfg_ERROR_WARM_RESET_NOT_ALLOWED) {
      printf("Request failed: WARM_RESET is not allowed\n");
      break;
    } else if (ret) {
      printf("Request failed: ret=%d\n", ret);
      break;
    }
  } while (0);

  Object_RELEASE_IF(resetCfg);
  Object_RELEASE_IF(clientEnv);

  return ret;
}
