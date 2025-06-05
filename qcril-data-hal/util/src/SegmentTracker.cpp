/**
* Copyright (c) 2019-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include "SegmentTracker.h"
#include "framework/Log.h"
#include "iterator"
#include "UnSolMessages/SegmentTimeoutMessage.h"

using namespace rildata;
using namespace std;

void SegmentTracker::sendErrorResponse(SegmentationStatus_t status, MessageId_t msgId, uint16_t txId) {
  vector<shared_ptr<Message>> respMessage;
  sendResponse(status, msgId, txId, respMessage);
}

SegmentTracker::KeyType_t SegmentTracker::serialize(MessageId_t msgId, uint16_t txId) {
  return to_string(msgId) + ":" + to_string(txId);
}

void SegmentTracker::sendResponse(SegmentationStatus_t status, MessageId_t msgId, uint16_t txId,
                                  vector<shared_ptr<Message>>& segments) {
  if (mHandlers.find(msgId) != mHandlers.end()) {
    auto completeHandler = mHandlers[msgId];
    completeHandler(status, txId, segments);
  } else {
    Log::getInstance().d("[SegmentTracker] Handler not found for msg=" + to_string(msgId));
  }
}

void SegmentTracker::timeoutHandler(MessageId_t msgId, uint16_t txId, void *) {
  Log::getInstance().d("[SegmentTracker] : sending Segment Timeout Message");
  auto msg = std::make_shared<SegmentTimeoutMessage>(txId, msgId, SegmentationStatus_t::TimedOut, mEndPointName);
  msg->broadcast();
}

void SegmentTracker::handleSegmentTimeout(MessageId_t msgId, uint16_t txId) {
  Log::getInstance().d("[SegmentTracker] handleSegmentTimeout");
  SegmentTracker::KeyType_t key = serialize(msgId, txId);

  if (timers.find(key) == timers.end()) {
      Log::getInstance().d("[SegmentTracker] timer expired but message was already processed");
      return;
  }

  if (transactions.find(key) != transactions.end()) {
      PendingTransaction& transaction = transactions[key];
      stringstream ss;
      copy(transaction.remainingSegments.begin(), transaction.remainingSegments.end(), ostream_iterator<int>(ss, ","));
      Log::getInstance().d("[SegmentTracker] timer expired while waiting for segments " + ss.str());
      transactions.erase(key);
  } else {
      Log::getInstance().d("[SegmentTracker] timer expired but no message found");
  }
  timers.erase(key);
  sendErrorResponse(SegmentationStatus_t::TimedOut, msgId, txId);
}

void SegmentTracker::setHandlers(const unordered_map<MessageId_t, transactionCompleteCb_t>& handlers) {
  mHandlers = handlers;
}

void SegmentTracker::startTransactionTimer(MessageId_t msgId, uint16_t txId) {
  SegmentTracker::KeyType_t key = serialize(msgId, txId);
  using placeholders::_1;
  PendingTimer pendingTimer;
  auto timeoutFn = bind(&SegmentTracker::timeoutHandler, this, msgId, txId, _1);
  pendingTimer.timerId = TimeKeeper::getInstance().set_timer(timeoutFn, nullptr, SEGMENT_TIMEOUT);
  pendingTimer.msgId = msgId;
  timers[key] = pendingTimer;
}

void SegmentTracker::addSegment(MessageId_t msgId, uint16_t txId, uint8_t segmentIndex,
                                uint8_t totalSegments, shared_ptr<Message> msg) {
  SegmentTracker::KeyType_t key = serialize(msgId, txId);
  using placeholders::_1;
  if (transactions.find(key) == transactions.end()) {
    transactions.emplace(key, totalSegments);
    PendingTransaction transaction{totalSegments};
    auto timeoutFn = bind(&SegmentTracker::timeoutHandler, this, msgId, txId, _1);
    if (timers.find(key) != timers.end()) {
      TimeKeeper::getInstance().clear_timer(timers[key].timerId);
    }
    TimeKeeper::millisec timeoutValue = totalSegments * SEGMENT_TIMEOUT;
    timers[key].timerId = TimeKeeper::getInstance().set_timer(timeoutFn, nullptr, timeoutValue);
    timers[key].msgId = msgId;
    transactions[key] = transaction;
  }

  PendingTransaction& transaction = transactions[key];
  PendingTimer& timer = timers[key];
  if (transaction.segments.size() <= segmentIndex) {
    Log::getInstance().d("[SegmentTracker] Segment out of bounds for tx=" + key);
    TimeKeeper::timer_id timerId = timer.timerId;
    transactions.erase(key);
    timers.erase(key);
    TimeKeeper::getInstance().clear_timer(timerId);
    sendErrorResponse(SegmentationStatus_t::Error, txId, msgId);
    return;
  }
  transaction.segments[segmentIndex] = msg;
  Log::getInstance().d("[SegmentTracker] [tx=" + key + "] Processing segment " +
                       to_string(segmentIndex) + " total=" + to_string(totalSegments));
  transaction.remainingSegments.erase(segmentIndex);
  if (transaction.remainingSegments.empty()) {
    Log::getInstance().d("[SegmentTracker] Segmentation complete for tx=" + key);
    TimeKeeper::timer_id timerId = timer.timerId;
    vector<shared_ptr<Message>> segments;
    segments.swap(transaction.segments);
    transactions.erase(key);
    timers.erase(key);
    TimeKeeper::getInstance().clear_timer(timerId);
    sendResponse(SegmentationStatus_t::Success, msgId, txId, segments);
  }
}

void SegmentTracker::addFragment(MessageId_t msgId, uint16_t txId,
                                 uint8_t segmentIndex, uint8_t totalSegments,
                                 uint8_t fragmentIndex, uint8_t totalFragments,
                                 std::shared_ptr<Message> msg) {
  SegmentTracker::KeyType_t key = serialize(msgId, txId);
  using placeholders::_1;
  if (transactions.find(key) == transactions.end()) {
    transactions.emplace(key, totalSegments);
    PendingTransaction transaction{totalSegments};
    auto timeoutFn = bind(&SegmentTracker::timeoutHandler, this, msgId, txId, _1);
    if (timers.find(key) != timers.end()) {
      TimeKeeper::getInstance().clear_timer(timers[key].timerId);
    }
    TimeKeeper::millisec timeoutValue = totalSegments * SEGMENT_TIMEOUT;
    timers[key].timerId = TimeKeeper::getInstance().set_timer(timeoutFn, nullptr, timeoutValue);
    timers[key].msgId = msgId;
    transactions[key] = transaction;
  }

  PendingTransaction& transaction = transactions[key];
  PendingTimer& timer = timers[key];
  if (transaction.segments.size() <= segmentIndex) {
    Log::getInstance().d("[SegmentTracker] Segment out of bounds for tx=" + key);
    TimeKeeper::timer_id timerId = timer.timerId;
    transactions.erase(key);
    timers.erase(key);
    TimeKeeper::getInstance().clear_timer(timerId);
    sendErrorResponse(SegmentationStatus_t::Error, txId, msgId);
    return;
  }

  if (transaction.remainingFragments[segmentIndex].size() == 0) {
    transaction.setTotalFragments(segmentIndex, totalFragments);
  }
  transaction.fragments[segmentIndex].push_back(msg);
  Log::getInstance().d("[SegmentTracker] [tx=" + key + "] Processing fragment " +
                      to_string(fragmentIndex) + "/" + to_string(totalFragments) +
                      " of segment " + to_string(segmentIndex) + "/" + to_string(totalSegments) +
                      " for tx " + key);
  transaction.remainingFragments[segmentIndex].erase(fragmentIndex);
  if (transaction.remainingFragments[segmentIndex].empty()) {
    Log::getInstance().d("[SegmentTracker] Fragmentation complete for segment " + to_string(segmentIndex));
    transaction.remainingSegments.erase(segmentIndex);
    if (transaction.remainingSegments.empty()) {
      Log::getInstance().d("[SegmentTracker] Segmentation complete for tx=" + key);
      TimeKeeper::timer_id timerId = timer.timerId;
      vector<shared_ptr<Message>> fragments;
      for(int i = 0 ; i < transaction.fragments.size() ; i++) {
        for(auto m : transaction.fragments[i]) {
          fragments.push_back(m);
        }
      }
      transactions.erase(key);
      timers.erase(key);
      TimeKeeper::getInstance().clear_timer(timerId);
      sendResponse(SegmentationStatus_t::Success, msgId, txId, fragments);
    }
  }
}