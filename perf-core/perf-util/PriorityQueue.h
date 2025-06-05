/******************************************************************************
  @file    PriorityQueue.h
  @brief   Implementation of priority queue

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __PRIORITY_QUEUE__
#define __PRIORITY_QUEUE__

#include <algorithm>
#include <queue>
#include <vector>

template <class T>
class PriorityQueue: private std::queue<std::pair<T, int>> {
    public:
        // function to insert Objects into the queue according to their priority.
        void insert(T s, int priority) { this->push(std::make_pair(s, priority)); }
        // function to check if queue is empty.
        bool isEmpty() const { return this->empty(); }
        // function to get Object with highest priority.
        T getData() {
            T s = this->front().first;
            this->pop();
            return s;
        }
        // function to get size of queue.
        uint32_t getSize() const { return this->size(); }
        // function to destory the queue.
        void destroy() {
            while (!this->empty())
                this->pop();
        }
};
#endif

