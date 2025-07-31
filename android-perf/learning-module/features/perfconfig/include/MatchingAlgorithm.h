/*
 * Copyright (C) 2025 Transsion Holdings
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MATCHINGALGORITHM_H
#define MATCHINGALGORITHM_H

#include "Algorithm.h"

#define LOG_TAG_MATCHING_ALGO "PERF-CONFIG-MATCHALGO"

class MatchingAlgorithm : public Algorithm {
public:
    MatchingAlgorithm(FeatureState &featureState);
    ~MatchingAlgorithm();

    void *getAlgorithmOutputDb();
    int runAlgorithm();
    void runAlgo(const std::atomic_bool &interrupt);

private:
    bool mInitialized;
    uint32_t mRunCount;

    void logAlgorithmRun();
};

#endif /* MATCHINGALGORITHM_H */
