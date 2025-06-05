/*
 * Copyright (c) 2020, 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti;

import java.util.Arrays;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

// soc_id to cpu mapping
public class LibsocHelperMsmCpu {
    private static int socId;
    private static final Map<Integer, List<String>> cpuLookup =
        new HashMap<Integer, List<String>>();
    private static List<String> socInfo = new ArrayList<String>();

    public LibsocHelperMsmCpu() {
        // soc_id to msmCpu and cpuVariant mapping
        cpuLookup.put(636, Arrays.asList("MSM_CPU_VOLCANO",  "MSM_CPU_VOLCANO"));
        cpuLookup.put(640, Arrays.asList("MSM_CPU_VOLCANO",  "MSM_CPU_VOLCANO6"));
        cpuLookup.put(641, Arrays.asList("MSM_CPU_VOLCANO",  "APQ_CPU_VOLCANO"));
        cpuLookup.put(657, Arrays.asList("MSM_CPU_VOLCANO",  "MSM_CPU_VOLCANO6I"));
        cpuLookup.put(658, Arrays.asList("MSM_CPU_VOLCANO",  "APQ_CPU_VOLCANO6I"));
        cpuLookup.put(623, Arrays.asList("MSM_CPU_PITTI",  "MSM_CPU_PITTI"));
        cpuLookup.put(629, Arrays.asList("MSM_CPU_NIOBE",  "MSM_CPU_NIOBE"));
        cpuLookup.put(652, Arrays.asList("MSM_CPU_NIOBE",  "MSM_CPU_NIOBEP"));
        cpuLookup.put(519, Arrays.asList("MSM_CPU_KALAMA",    "MSM_CPU_KALAMA"));
        cpuLookup.put(600, Arrays.asList("MSM_CPU_KALAMA",    "MSM_CPU_KALAMA_GAME"));
        cpuLookup.put(601, Arrays.asList("MSM_CPU_KALAMA",    "APQ_CPU_KALAMA_GAME"));
        cpuLookup.put(507, Arrays.asList("MSM_CPU_BLAIR", "MSM_CPU_BLAIR"));
        cpuLookup.put(565, Arrays.asList("MSM_CPU_BLAIR", "APQ_CPU_BLAIR"));
        cpuLookup.put(578, Arrays.asList("MSM_CPU_BLAIR", "MSM_CPU_BLAIR_LITE"));
        cpuLookup.put(454, Arrays.asList("MSM_CPU_HOLI", "MSM_CPU_HOLI"));
        cpuLookup.put(472, Arrays.asList("MSM_CPU_HOLI", "MSM_CPU_HOLI_H"));
        cpuLookup.put(557, Arrays.asList("MSM_CPU_PINEAPPLE", "MSM_CPU_PINEAPPLE"));
        cpuLookup.put(577, Arrays.asList("MSM_CPU_PINEAPPLE", "APQ_CPU_PINEAPPLE"));
        cpuLookup.put(682, Arrays.asList("MSM_CPU_PINEAPPLE", "APQ_CPU_PINEAPPLE_GAME"));
        cpuLookup.put(696, Arrays.asList("MSM_CPU_PINEAPPLE", "APQ_CPU_TABLET"));
        cpuLookup.put(618, Arrays.asList("MSM_CPU_SUN",       "MSM_CPU_SUN"));
        cpuLookup.put(632, Arrays.asList("MSM_CPU_CLIFFS", "MSM_CPU_CLIFFS7"));
        cpuLookup.put(614, Arrays.asList("MSM_CPU_CLIFFS", "MSM_CPU_CLIFFS"));
        cpuLookup.put(642, Arrays.asList("MSM_CPU_CLIFFS", "APQ_CPU_CLIFFS"));
        cpuLookup.put(643, Arrays.asList("MSM_CPU_CLIFFS", "APQ_CPU_CLIFFS7"));
        cpuLookup.put(608, Arrays.asList("MSM_CPU_CROW",  "MSM_CPU_CROW"));
        cpuLookup.put(321, Arrays.asList("MSM_CPU_SDM845",  "MSM_CPU_SDM845"));
        cpuLookup.put(336, Arrays.asList("MSM_CPU_SDM670",  "MSM_CPU_SDM670"));
        cpuLookup.put(339, Arrays.asList("MSM_CPU_SM8150",  "MSM_CPU_SM8150"));
        cpuLookup.put(356, Arrays.asList("MSM_CPU_KONA",    "MSM_CPU_KONA"));
        cpuLookup.put(360, Arrays.asList("MSM_CPU_SDM710",  "MSM_CPU_SDM710"));
        cpuLookup.put(400, Arrays.asList("MSM_CPU_LITO",    "MSM_CPU_LITO"));
        cpuLookup.put(415, Arrays.asList("MSM_CPU_LAHAINA", "MSM_CPU_LAHAINA"));
        cpuLookup.put(439, Arrays.asList("MSM_CPU_LAHAINA", "APQ_CPU_LAHAINA"));
        cpuLookup.put(450, Arrays.asList("MSM_CPU_SHIMA",   "MSM_CPU_SHIMA"));
        cpuLookup.put(457, Arrays.asList("MSM_CPU_TARO",    "MSM_CPU_TARO"));
        cpuLookup.put(482, Arrays.asList("MSM_CPU_TARO",    "APQ_CPU_TARO"));
        cpuLookup.put(552, Arrays.asList("MSM_CPU_TARO",    "MSM_CPU_TARO_LTE"));
        cpuLookup.put(501, Arrays.asList("MSM_CPU_SM8325",  "MSM_CPU_SM8325"));
        cpuLookup.put(502, Arrays.asList("MSM_CPU_SM8325",  "APQ_CPU_SM8325P"));
        cpuLookup.put(475, Arrays.asList("MSM_CPU_YUPIK",   "MSM_CPU_YUPIK"));
        cpuLookup.put(499, Arrays.asList("MSM_CPU_YUPIK",   "APQ_CPU_YUPIK"));
        cpuLookup.put(497, Arrays.asList("MSM_CPU_YUPIK",   "MSM_CPU_YUPIK_IOT"));
        cpuLookup.put(498, Arrays.asList("MSM_CPU_YUPIK",   "APQ_CPU_YUPIKP_IOT"));
        cpuLookup.put(515, Arrays.asList("MSM_CPU_YUPIK",   "MSM_CPU_YUPIK_LTE"));
        cpuLookup.put(506, Arrays.asList("MSM_CPU_DIWALI",   "MSM_CPU_DIWALI"));
        cpuLookup.put(547, Arrays.asList("MSM_CPU_DIWALI",   "APQ_CPU_DIWALI"));
        cpuLookup.put(564, Arrays.asList("MSM_CPU_DIWALI",   "MSM_CPU_DIWALI_LTE"));
        cpuLookup.put(530, Arrays.asList("MSM_CPU_CAPE",   "MSM_CPU_CAPE"));
        cpuLookup.put(531, Arrays.asList("MSM_CPU_CAPE",   "APQ_CPU_CAPE"));
        cpuLookup.put(540, Arrays.asList("MSM_CPU_CAPE",   "MSM_CPU_CAPE_LTE"));
        cpuLookup.put(537, Arrays.asList("MSM_CPU_PARROT",   "MSM_CPU_PARROT"));
        cpuLookup.put(583, Arrays.asList("MSM_CPU_PARROT",   "APQ_CPU_PARROT"));
        cpuLookup.put(613, Arrays.asList("MSM_CPU_PARROT",   "MSM_CPU_PARROT7"));
        cpuLookup.put(591, Arrays.asList("MSM_CPU_UKEE",   "MSM_CPU_UKEE"));
        cpuLookup.put(568, Arrays.asList("MSM_CPU_RAVELIN",   "MSM_CPU_RAVELIN"));
        cpuLookup.put(602, Arrays.asList("MSM_CPU_RAVELIN",   "APQ_CPU_RAVELIN"));
        cpuLookup.put(581, Arrays.asList("MSM_CPU_RAVELIN",   "MSM_CPU_RAVELIN_IOT"));
        cpuLookup.put(582, Arrays.asList("MSM_CPU_RAVELIN",   "APQ_CPU_RAVELIN_IOT"));
        cpuLookup.put(-1,  Arrays.asList("MSM_CPU_UNKNOWN", "MSM_CPU_UNKNOWN"));
    }

    // Populates msmCpu and cpuVariant as a List<String>
    public synchronized List<String> getMsmCpu(int socId) {
        socInfo = cpuLookup.get(socId);
        return socInfo;
    }
}
