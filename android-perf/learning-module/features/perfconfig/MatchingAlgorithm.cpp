/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#include "MatchingAlgorithm.h"

#include "TLog.h"

MatchingAlgorithm::MatchingAlgorithm(FeatureState &featureState)
    : Algorithm(featureState), mInitialized(true), mRunCount(0) {
    TLOGD("%s: Constructor", LOG_TAG_MATCHING_ALGO);
}

MatchingAlgorithm::~MatchingAlgorithm() {
    TLOGD("%s: Destructor", LOG_TAG_MATCHING_ALGO);
}

void *MatchingAlgorithm::getAlgorithmOutputDb() {
    return nullptr;
}

int MatchingAlgorithm::runAlgorithm() {
    if (!mInitialized) {
        TLOGE("%s: Algorithm not initialized", LOG_TAG_MATCHING_ALGO);
        return -1;
    }

    mRunCount++;
    logAlgorithmRun();

    return 0;
}

void MatchingAlgorithm::logAlgorithmRun() {
    TLOGV("%s: Algorithm run #%u", LOG_TAG_MATCHING_ALGO, mRunCount);
}

void MatchingAlgorithm::runAlgo(const std::atomic_bool &interrupt) {
    if (!mInitialized) {
        TLOGE("%s: Algorithm not initialized", LOG_TAG_MATCHING_ALGO);
        return;
    }

    runAlgorithm();

    TLOGV("%s: runAlgo completed", LOG_TAG_MATCHING_ALGO);
}
