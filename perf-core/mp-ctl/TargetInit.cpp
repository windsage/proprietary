/******************************************************************************
  @file    TargetInit.cpp
  @brief   Implementation of targets

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-TARGET-INIT"
#include <fcntl.h>
#include <cstdlib>
#include <string.h>
#include <sys/utsname.h>
#include "PerfController.h"
#include "Request.h"
#include "client.h"
#include "Target.h"
#include "OptsData.h"
#include "MpctlUtils.h"
#include "BoostConfigReader.h"
#include "HintExtHandler.h"
#include "PerfLog.h"

//Default frequency map for value mapping
int msmDefaultFreqMap[FREQ_MAP_MAX] = {
    800,    /*LOWEST_FREQ*/
    1100,   /*LEVEL1_FREQ*/
    1300,   /*LEVEL2_FREQ*/
    1500,   /*LEVEL3_FREQ*/
    1650    /*HIGHEST_FREQ*/
};

//Default pre-defined cluster map .
int msmDefaultPreDefinedClusterMap[MAX_PRE_DEFINE_CLUSTER - START_PRE_DEFINE_CLUSTER] = {
    0,  /*LAUNCH_CLUSTER*/
    1,  /*SCROLL_CLUSTER*/
    1,  /*ANIMATION_CLUSTER*/
};

/* Init Sequence
 * 1. Populate the target resources config
 * 2. Init the resources node
 */

void Target::InitializeTarget() {
    uint32_t ret = 0;
    char tmp_s[NODE_MAX];
    int32_t rc = 0;
    uint16_t qindx = 0;
    char clk_scaling_s[NODE_MAX];
    uint32_t res = 0;
    const char* target_name = mTargetConfig.getTargetName().c_str();
    ResourceInfo tmpr;

    QLOGV(LOG_TAG, "Inside InitializeTarget");
    mSetAllResourceSupported();
    mInitAllResourceValueMap();
    mResetResourceSupported(MISC_MAJOR_OPCODE, UNSUPPORTED_OPCODE);
    mInitGpuAvailableFreq();
    mInitGpuBusAvailableFreq();

    // Identifying storage device type. (UFS/EMMC)
    FREAD_STR(STORAGE_UFS_CLK_SCALING_DISABLE, clk_scaling_s, NODE_MAX, rc);
    if (rc > 0) {
        strlcpy(mStorageNode, STORAGE_UFS_CLK_SCALING_DISABLE,
                strlen(STORAGE_UFS_CLK_SCALING_DISABLE)+1);
    } else {
        strlcpy(mStorageNode, STORAGE_EMMC_CLK_SCALING_DISABLE,
                strlen(STORAGE_EMMC_CLK_SCALING_DISABLE)+1);
    }


    rc = 0;
    //populate boost conigs, target configs  & params mapping tables
    mPerfDataStore = PerfDataStore::getPerfDataStore();
    mPerfDataStore->Init();

    /*intialize hint extnsion handler and register actions
     * Note: Number of hint registrations allowed is limited.
     * Modify the MAX_HANDLERS limit in HintExtension as needed
    */
    HintExtHandler &hintExt = HintExtHandler::getInstance();
    hintExt.Reset();
    hintExt.Register(VENDOR_HINT_FIRST_LAUNCH_BOOST, NULL, LaunchBoostAction::LaunchBoostPostAction,
             LaunchBoostAction::LaunchBoostHintExcluder);
    hintExt.Register(VENDOR_HINT_FPS_UPDATE, FpsUpdateAction::FpsUpdatePreAction,
                     FpsUpdateAction::FpsUpdatePostAction);
    hintExt.Register(VENDOR_HINT_BOOST_RENDERTHREAD, NULL, TaskBoostAction::TaskBoostPostAction);
    hintExt.Register(VENDOR_HINT_PICARD_GPU_DISPLAY_MODE,
                                DisplayEarlyWakeupAction::DisplayEarlyWakeupPreAction, NULL);
    hintExt.Register(VENDOR_HINT_PICARD_GPU_ONLY_MODE,
                                DisplayEarlyWakeupAction::DisplayEarlyWakeupPreAction, NULL);
    hintExt.Register(VENDOR_HINT_PICARD_DISPLAY_ONLY_MODE,
                                DisplayEarlyWakeupAction::DisplayEarlyWakeupPreAction, NULL);

    hintExt.Register(VENDOR_HINT_SCROLL_BOOST, NULL, CPUFreqAction::CPUFreqPostAction,
                     ScrollBoostAction::ScrollBoostHintExcluder);

    hintExt.Register(VENDOR_HINT_LARGER_COMP_CYCLE, LargeComp::LargeCompPreAction, NULL);
    hintExt.Register(VENDOR_HINT_HDR_CONTENT, LargeComp::LargeCompPreAction, NULL);
    hintExt.Register(VENDOR_HINT_PASS_PID, StorePID::StorePIDPreAction, NULL);
    hintExt.Register(VENDOR_HINT_DRAG_BOOST, NULL, CPUFreqAction::CPUFreqPostAction,
                     ScrollBoostAction::ScrollBoostHintExcluder);

    hintExt.Register(VENDOR_HINT_DISPLAY_ON, NULL, DisplayAction::DisplayPostAction);
    hintExt.Register(VENDOR_HINT_DISPLAY_OFF,NULL, DisplayAction::DisplayPostAction);
    hintExt.Register(VENDOR_HINT_PERF_INIT_CHECK ,NULL, NULL, PerfInitAction::PerfInitCheckHintExcluder);
    hintExt.Register(VENDOR_HINT_TEST ,NULL, NULL, PerfCtxHelper::PerfCtxHelperExcluder);
    // add for perflock unlock screen by chao.xu5 at Jul 15th, 2025 start.
    hintExt.Register(VENDOR_HINT_TRAN_UNLOCK_SCREEN, TranUnlockScreenAction::UnlockScreenPreAction, TranUnlockScreenAction::UnlockScreenPostAction, TranUnlockScreenAction::UnlockScreenHintExcluder);
    // add for perflock unlock screen by chao.xu5 at Jul 15th, 2025 end.

    /* All the target related information parsed from XML file are initialized in the TargetInit()
    function which remains same for all the targets. For any target specific initializations provided
    an ExtendedTargetInit() function, which works on SocId. */

    TargetInit();
    ExtendedTargetInit();

    /* Init for per target resource file.
     * Moved the call after TargetInit as it need target name
     */
    mPerfDataStore->TargetResourcesInit();

    /* Moved Init KPM Nodes after Target Resource as the values read from config files
     * are needed in KPM nodes init method
     */
    InitializeKPMnodes();

    //cluster map from xml
    ret = mPerfDataStore->GetClusterMap(&mPredefineClusterMap, target_name);

    //default cluster map if not available
    if ((NULL == mPredefineClusterMap) || !ret) {
        mPredefineClusterMap = msmDefaultPreDefinedClusterMap;
    }

    tmpr.SetMajor(CPUFREQ_MAJOR_OPCODE);
    tmpr.SetMinor(CPUFREQ_MIN_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();

    mValueMap[qindx].mapSize = mPerfDataStore->GetFreqMap(res, &mValueMap[qindx].map, target_name);

    //default it to a map if no mappings exists
    if ((NULL == mValueMap[qindx].map) || !mValueMap[qindx].mapSize) {
        mValueMap[qindx].mapSize = FREQ_MAP_MAX;
        mValueMap[qindx].map = msmDefaultFreqMap;
    }

    //Define for max_freq. Freq mapped in Mhz.
    tmpr.SetMinor(CPUFREQ_MAX_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();

    mValueMap[qindx].mapSize = mPerfDataStore->GetFreqMap(res,
                                           &mValueMap[qindx].map, target_name);

    //default it to a map if no mappings exists
    if ((NULL == mValueMap[qindx].map) || !mValueMap[qindx].mapSize) {
        mValueMap[qindx].mapSize = FREQ_MAP_MAX;
        mValueMap[qindx].map = msmDefaultFreqMap;
    }
}

void invoke_wa_libs() {
#define KERNEL_WA_NODE "/sys/module/app_setting/parameters/lib_name"
   int32_t rc;
   const char *wl_libs[] = {
      "libvoH264Dec_v7_OSMP.so",
      "libvossl_OSMP.so",
      "libTango.so"
   };
   uint16_t i;

   uint16_t len = sizeof (wl_libs) / sizeof (*wl_libs);

   for(i = 0; i < len; i++) {
      FWRITE_STR(KERNEL_WA_NODE, wl_libs[i], strlen(wl_libs[i]), rc);
      QLOGL(LOG_TAG, QLOG_L2, "Writing to node (%s)  (%s) rc:%" PRId32 "\n", KERNEL_WA_NODE, wl_libs[i], rc);
   }

}

void invoke_tinyu_wa(uint16_t socid) {
  static struct {
       char node[NODE_MAX];
       char const *val;
  }tinyu_wa_detl [] = {
      {"/proc/sys/kernel/sched_lib_mask_check", "0x80"},
      {"/proc/sys/kernel/sched_lib_mask_force", "0xf0"},
      {"/proc/sys/kernel/sched_lib_name", "libunity.so, libfb.so"},
  };

  int32_t rc;
  uint16_t len = sizeof (tinyu_wa_detl) / sizeof (tinyu_wa_detl[0]);
  uint16_t st_idx = 0;

  /*for taro kalama parrot diwali cape bengal khaje anorak pineapple crow sun cliffs pitti volcano ravelin neo61*/
  if (socid == 367 || socid == 377 || socid == 405 || socid == 457 ||
      socid == 417 || socid == 420 || socid == 444 || socid == 445 || socid == 469 || socid == 470 ||
      socid == 482 || socid == 552 || socid == 506 || socid == 547 ||
      socid == 564 || socid == 530 || socid == 531 || socid == 532 ||
      socid == 533 || socid == 534 || socid == 540 || socid == 591 ||
      socid == 519 || socid == 536 || socid == 600 || socid == 601 || socid == 537 ||
      socid == 518 || socid == 561 || socid == 585 || socid == 586 || socid == 549 ||
      socid == 557 || socid == 577 || socid == 603 || socid == 604 || socid == 583 || socid == 682 ||
      socid == 613 || socid == 619 || socid == 608 || socid == 618 || socid == 631 || socid == 633 ||
      socid == 634 || socid == 638 || socid == 614 || socid == 632 || socid == 642 || socid == 643 ||
      socid == 644 || socid == 623 || socid == 649 || socid == 663 || socid == 629 || socid == 652 ||
      socid == 636 || socid == 640 || socid == 641 || socid == 657 || socid == 658 || socid == 712 ||
      socid == 568 || socid == 602 || socid == 653 || socid == 654 || socid == 687 || socid == 696 || socid == 554 || socid == 579 ||
      socid == 700) {
      strlcpy(tinyu_wa_detl[1].node, "/proc/sys/walt/sched_lib_mask_force", NODE_MAX);
      strlcpy(tinyu_wa_detl[2].node, "/proc/sys/walt/sched_lib_name", NODE_MAX);
      st_idx = 1;
  }

  /*for lito,atoll,lagoon,holi,orchid, pitti, blair we have only two gold cores, so mask is changed accrodingly*/
  if (socid == 400 || socid == 440 || socid == 407 || socid == 434 ||  socid == 454 || socid == 459 || socid == 476 || socid == 507 || socid == 578 ||
      socid == 568 || socid == 602 || socid == 623 || socid == 647 || socid == 653 || socid == 654) {
      tinyu_wa_detl[1].val = "0xc0";
  }

  /* for kalama,cliffs 3 silver cores, 4 golden cores, 1 prime core */
  if (socid == 519 || socid == 536 || socid == 600 || socid == 601 || socid == 614 || socid == 632 ||
      socid == 642 || socid == 643 || socid == 700) {
      tinyu_wa_detl[1].val = "0xf8";
  }

  /* for pineapple, 2 silver cores, 5 gold cores, 1 prime core */
  if (socid == 557 || socid == 577 || socid == 682 || socid == 696) {
      tinyu_wa_detl[1].val = "0xfc";
  }

  /* for anorak, niobe 2 gold core and 4 prime cores */
  if (socid == 549 || socid == 649 || socid == 629 || socid == 652) {
      tinyu_wa_detl[1].val = "0x3c";
  }

  /* for sun, 6 gold cores and 2 prime cores */
  if (socid == 618) {
      tinyu_wa_detl[1].val = "0xff";
  }

  /* for lemans/gen4_au, 4 gold and 4 silver cores */
  if (socid == 532 || socid == 533 || socid == 534 || socid == 619) {
      tinyu_wa_detl[1].val = "0xf0";
  }

  /* for volcano 1 prime, 3 gold and 4 Silver*/
  if (socid == 636 || socid == 640 || socid == 641 || socid == 657 || socid == 658 || socid == 712) {
      tinyu_wa_detl[1].val = "0xf0";
  }

  /* for neo61 4 gold cores*/
  if (socid == 554 || socid == 579){
      tinyu_wa_detl[1].val = "0x0f";
  }

  for(uint16_t i = st_idx; i < len; i++) {
      FWRITE_STR(tinyu_wa_detl[i].node, tinyu_wa_detl[i].val, strlen(tinyu_wa_detl[i].val), rc);
      QLOGE(LOG_TAG, "Writing to node (%s)  (%s) rc:%" PRId32 "\n", tinyu_wa_detl[i].node, tinyu_wa_detl[i].val, rc);
  }
}

void Target::TargetInit() {

    readPmQosWfiValue();
    mCalculateCoreIdx();
    if (mTargetConfig.getType() == 0) {
        //cluster 0 is big
        mLogicalPerfMapPerfCluster();
    } else {
        //cluster 0 is little
        mLogicalPerfMapPowerCluster();
    }
}

//KPM node initialization
void Target::InitializeKPMnodes() {
    char tmp_s[NODE_MAX];
    int32_t rc = 0;

    PerfDataStore *store = PerfDataStore::getPerfDataStore();
    char kpmNumClustersNode[NODE_MAX];
    char kpmManagedCpusNode[NODE_MAX];
    char kpmCpuMaxFreqNode[NODE_MAX];
    memset(kpmNumClustersNode, 0, sizeof(kpmNumClustersNode));
    memset(kpmManagedCpusNode, 0, sizeof(kpmManagedCpusNode));
    memset(kpmCpuMaxFreqNode, 0, sizeof(kpmCpuMaxFreqNode));

    store->GetSysNode(KPM_NUM_CLUSTERS, kpmNumClustersNode);
    store->GetSysNode(KPM_MANAGED_CPUS, kpmManagedCpusNode);
    store->GetSysNode(KPM_CPU_MAX_FREQ_NODE, kpmCpuMaxFreqNode);

    FREAD_STR(kpmManagedCpusNode, tmp_s, NODE_MAX, rc);
    if (rc < 0) {
        QLOGE(LOG_TAG, "Error reading KPM nodes. Does KPM exist\n");
    } else {
        snprintf(tmp_s, NODE_MAX , "%" PRIu8, mTargetConfig.getNumCluster());
        FWRITE_STR(kpmNumClustersNode, tmp_s, strlen(tmp_s), rc);

        if(mTargetConfig.getCoresInCluster(0) == -1)
            return;

        //Initializing KPM nodes for each cluster, with their cpu ranges and max possible frequencies.
        for (int8_t i=0, prevcores = 0; i < mTargetConfig.getNumCluster(); i++) {
            snprintf(tmp_s, NODE_MAX , "%" PRId8 "-%" PRId8, prevcores, prevcores + mTargetConfig.getCoresInCluster(i)-1);
            FWRITE_STR(kpmManagedCpusNode, tmp_s, strlen(tmp_s), rc);

            rc = update_freq_node(prevcores, prevcores + mTargetConfig.getCoresInCluster(i) - 1 , mTargetConfig.getCpuMaxFreqResetVal(i) , tmp_s, NODE_MAX);
            if (rc >= 0) {
                FWRITE_STR(kpmCpuMaxFreqNode, tmp_s, strlen(tmp_s), rc);
            }
            prevcores += mTargetConfig.getCoresInCluster(i);
        }
    }
}

void Target::ExtendedTargetInit() {
    char tmp_s[NODE_MAX];

    switch (mTargetConfig.getSocID()) {
    case 246: /*8996*/
    case 291: /*8096*/
        invoke_wa_libs();
        break;

    case 305: /*8996Pro*/
    case 312: /*8096Pro*/
        invoke_wa_libs();
        break;

    case 339: /*msmnile*/
    case 361: /*msmnile without modem and SDX55*/
    case 356: /*kona*/
    case 394: /*trinket*/
    case 417: /*bengal*/
    case 420: /*bengal*/
    case 444: /*bengal*/
    case 445: /*bengal*/
    case 469: /*bengal*/
    case 470: /*bengal*/
    case 467: /*trinket*/
    case 468: /*trinket*/
    case 518: /*khaje*/
    case 561: /*khaje*/
    case 585: /*khaje*/
    case 586: /*khaje*/
    case 400: /*lito*/
    case 440: /*lito*/
    case 407: /*atoll*/
    case 415: /*lahaina*/
    case 439: /*lahaina*/
    case 456: /*lahaina-APQ*/
    case 501: /*lahaina - SM8345 - APQ */
    case 502: /*lahaina - SM8325p */
    case 434: /*lagoon*/
    case 459: /*lagoon*/
    case 454: /*holi*/
    case 507: /*holi*/
    case 578: /*holi*/
    case 647: /*blair*/
    case 450: /*shima*/
    case 476: /*orchid*/
    case 475: /*yupik*/
    case 497: /*yupik*/
    case 498: /*yupik*/
    case 499: /*yupik*/
    case 515: /*yupik*/
    case 457: /*taro*/
    case 482: /*taro - APQ*/
    case 552: /*Alakai*/
    case 519: /*kalama*/
    case 536: /*kalama*/
    case 600: /*kalama*/
    case 601: /*kalama*/
    case 525: /*neo*/
    case 532: /*gen4_au*/
    case 533: /*gen4_au*/
    case 534: /*gen4_au*/
    case 619: /*gen4_au*/
    case 556: /*sdxpinn*/
    case 530: /*cape*/
    case 531: /*cape*/
    case 537: /*parrot*/
    case 583: /*parrot*/
    case 613: /*parrot*/
    case 631: /*parrot */
    case 633: /*parrot */
    case 634: /*parrot */
    case 638: /*parrot */
    case 663: /*parrot */
    case 540: /*cape*/
    case 591: /*cape*/
    case 554: /*neo-la, neo61*/
    case 579: /*neo-la, neo61*/
    case 506: /*diwali*/
    case 547: /*diwali*/
    case 564: /*diwali*/
    case 549: /*anorak*/
    case 649: /*anorak*/
    case 557: /*pineapple*/
    case 577: /*pineapple*/
    case 682: /*pineapple*/
    case 696: /*pineapple*/
    case 568: /*ravelin*/
    case 602: /*ravelin*/
    case 653: /*ravelin*/
    case 654: /*ravelin*/
    case 608: /*crow*/
    case 644: /*crow*/
    case 687: /*crow*/
    case 618: /*sun*/
    case 614: /*cliffs*/
    case 632: /*cliffs*/
    case 642: /*cliffs*/
    case 643: /*cliffs*/
    case 700: /*cliffs*/
    case 623: /*pitti*/
    case 629: /*niobe*/
    case 652: /*niobe*/
    case 636: /*volcano*/
    case 640: /*volcano*/
    case 641: /*volcano*/
    case 657: /*volcano*/
    case 658: /*volcano*/
    case 712: /*volcano*/
       invoke_tinyu_wa(mTargetConfig.getSocID());
       mUpDownComb = true;
       mGrpUpDownComb = true;
       break;
    }

    Dump();
}
