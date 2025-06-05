/******************************************************************************
  @file    ResourceQueues.cpp
  @brief   Implementation of resource queue

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-RESOURCEQS"
#include <cstdio>
#include <cstring>

#include "Request.h"
#include "ResourceQueues.h"
#include "MpctlUtils.h"
#include "PerfLog.h"

ResourceQueue::ResourceQueue() {
}

ResourceQueue::~ResourceQueue() {
}

int16_t ResourceQueue::Init() {
    q_node *nodes = NULL;
    TargetConfig &tc = TargetConfig::getTargetConfig();
    uint16_t i = 0;

    memset(resource_qs, 0x00, sizeof(resource_qs));
    /*Pre-creation of right node is needed to handle display off case,
      where we may directly add the node in pend and current remians
      empty*/
    for (i = 0; i < MAX_MINOR_RESOURCES; i++) {
         nodes = (q_node *)calloc(tc.getTotalNumCores(), sizeof(q_node));
         if (NULL == nodes) {
             QLOGL(LOG_TAG, QLOG_WARNING, "Failed to pend new request for optimization");
         } else {
             memset(nodes, 0x00, sizeof(q_node)*tc.getTotalNumCores());
             resource_qs[i].right = nodes;
         }
    }

    return 0;
}

void ResourceQueue::CopyQnode(q_node *node1, q_node *node2) {
    if (node1 == NULL || node2 == NULL) {
        return;
    }
    node1->handle = node2->handle;
    node1->next = node2->next;
    node1->resource = node2->resource;
}

q_node *ResourceQueue::GetNode(Resource &resObj) {
    q_node *tmp = NULL;
    q_node *nodes = NULL;
    TargetConfig &tc = TargetConfig::getTargetConfig();
    uint16_t idx = resObj.qindex;

    if ((idx >= MAX_MINOR_RESOURCES) ||
        (resObj.core < 0) || (resObj.core > tc.getTotalNumCores())) {
        return NULL;
    }

    if (0 == resObj.core) {
        QLOGL(LOG_TAG, QLOG_L3, "Returning from core 0 index");
        tmp = &resource_qs[idx];
        return tmp;
    }

    if (NULL == resource_qs[idx].right) {
        QLOGL(LOG_TAG, QLOG_L3, "Before calloc of qnode for inserting node in ResourceQ");
        nodes = (q_node *)calloc(tc.getTotalNumCores(), sizeof(q_node));
        if (NULL == nodes) {
            QLOGL(LOG_TAG, QLOG_WARNING, "Failed to pend new request for optimization [0x%" PRIX16 ",0x%" PRIX16 "]",
                  resObj.major, resObj.minor);
            tmp = NULL;
        }
        else {
            memset(nodes, 0x00, sizeof(q_node)*tc.getTotalNumCores());
            resource_qs[idx].right = nodes;
            tmp = &resource_qs[idx].right[resObj.core];
        }
    }
    else {
        tmp = &resource_qs[idx].right[resObj.core];
    }
    return tmp;
}

bool ResourceQueue::AddAndApply(Request *req) {
    q_node *current = NULL;
    q_node *pended = NULL, *recent = NULL, *iter = NULL;
    uint32_t level = 0;
    int32_t rc = 0;
    uint32_t i = 0;
    bool ret = false;
    OptsHandler &oh = OptsHandler::getInstance();
    Target &target = Target::getCurTarget();

    if (NULL == req) {
        return false;
    }

    QLOGL(LOG_TAG, QLOG_L3, "no. of locks %" PRIu32, req->GetNumLocks());

    for (i = 0; i < req->GetNumLocks(); i++) {
        int8_t needAction = FAILED, action = FAILED;
        ResourceInfo *res = req->GetResource(i);
        if (NULL == res) {
            continue;
        }
        Resource &resObj = res->GetResourceObject();
        uint16_t qindex = resObj.qindex;
        res->Dump();

        /* If resource requested is not supported, ignore it */
        if (!target.isResourceSupported(resObj.major, resObj.minor)) {
            QLOGE(LOG_TAG, "Resource with major=%" PRIu16 ", minor=%" PRIu16 " not supported", resObj.major, resObj.minor);
            continue;
        }

        level = resObj.value;
        if (level == MAX_LVL) {
            QLOGE(LOG_TAG, "Resource value %" PRIx32 " is not supported", level);
            continue;
        }

        current = GetNode(resObj);
        if (NULL == current) {
            continue;
        }

        /* Display off/Doze lock is active, pend all the coming requests*/
        if ((display_off == true || display_doze == true) && (req->GetPriority() != ALWAYS_ALLOW)) {
            QLOGL(LOG_TAG, QLOG_L3, "display off/display doze true pend it qindex:%" PRIu16, qindex);
            needAction = PEND_REQUEST;
        }
        //Check if at this resource index queue is empty
        else if ((resObj.core != 0 && (resource_qs[resObj.qindex].right[resObj.core].handle)) ||
                (resObj.core == 0 && (resource_qs[resObj.qindex].handle))) {
            QLOGL(LOG_TAG, QLOG_L3, "Calling compareopt with level %" PRIu32 ", currnt->level %" PRIu32, level, current->resource.level);
            needAction = oh.CompareOpt(resObj.qindex, level, current->resource.level);
        } else {
            QLOGL(LOG_TAG, QLOG_L3, "First request in Queue");
            needAction = ADD_NEW_REQUEST;
        }

        if (needAction == FAILED) {
            QLOGE(LOG_TAG, "Failed to apply correct action 0x%" PRIx16, resObj.qindex);
            continue;
        }
        QLOGL(LOG_TAG, QLOG_L3, "Need action returned as %" PRId8, needAction);

        if (needAction == ADD_NEW_REQUEST) {
            //current->level == 0
            //resource is not being used by perflock
            QLOGL(LOG_TAG, QLOG_L3, "Resource not being used");
            rc = oh.ApplyOpt(resObj);
            if (rc < 0) {
                QLOGE(LOG_TAG, "Failed to apply optimization [%" PRIu16 ", %" PRIu16 "]", resObj.major, resObj.minor);
                continue;
            }
            current->handle = req;
            current->resource = resObj;
        } else if (needAction == ADD_AND_UPDATE_REQUEST || needAction == ADD_IN_ORDER) {
            // (level > current->level) {
            /* new request is higher lvl than current */
            QLOGL(LOG_TAG, QLOG_L3, "New request is higher level than current so pend");

            rc = oh.ApplyOpt(resObj);
            if (rc < 0) {
                QLOGE(LOG_TAG, "Failed to apply optimization [%" PRIu16 ", %" PRIu16 "]", resObj.major, resObj.minor);
                continue;
            }

            pended = (struct q_node *)calloc(1, sizeof(struct q_node));
            if (pended == NULL) {
                QLOGL(LOG_TAG, QLOG_WARNING, "Failed to pend existing request");
                continue;
            }
            CopyQnode(pended, current);

            current->handle = req;
            current->resource = resObj;
            current->next = pended;
        } else {
            /* new is equal or lower lvl than current */
            QLOGL(LOG_TAG, QLOG_L3, "New request is equal or lower lvl than current");
            recent = (struct q_node *)calloc(1, sizeof(struct q_node));
            if (recent == NULL) {
                QLOGL(LOG_TAG, QLOG_WARNING, "Failed to pend new request for optimization [%" PRIu16 ", %" PRIu16 "]",
                      resObj.major, resObj.minor);
                continue;
            }
            recent->handle = req;
            recent->resource = resObj;

            iter = GetNode(resObj);

            if (NULL == iter) {
                //something terribly wrong, we should not be here
                free(recent);
                recent = NULL;
                continue;
            }

            //First request for Queue, adding at head itself
            if (iter->handle == NULL) {
               iter->handle = recent->handle;
               iter->resource = recent->resource;
               free(recent);
               recent = NULL;
               continue;
            }

            if (NULL != iter->next) {
                action = oh.CompareOpt(resObj.qindex, level, iter->next->resource.level);
            }

            while ((NULL != iter->next) && (action == PEND_REQUEST || action == PEND_ADD_IN_ORDER)) {
                iter = iter->next;
                if (NULL == iter->next) {
                    break;
                }
                action = oh.CompareOpt(resObj.qindex, level, iter->next->resource.level);
            }

            if ((action == FAILED && needAction == PEND_ADD_IN_ORDER)
                   || (needAction != EQUAL_ADD_IN_ORDER && (action == ADD_IN_ORDER || action == PEND_ADD_IN_ORDER))) {
                rc = oh.ApplyOpt(resObj);
                if (rc < 0) {
                    free(recent);
                    recent = NULL;
                    QLOGE(LOG_TAG, "Failed to apply optimization [%" PRIu16 ", %" PRIu16 "]", resObj.major, resObj.minor);
                    continue;
                }
            }
            recent->next = iter->next;
            iter->next = recent;
        }
        ret = true;
    }
    return ret;
}

bool ResourceQueue::RemoveAndApply(Request *req) {
    q_node *current = NULL, *pending = NULL, *del = NULL;
    int32_t rc = 0;
    uint32_t reset_opt_data = 0;
    int32_t i = 0;
    OptsHandler &oh = OptsHandler::getInstance();
    int8_t needAction = FAILED;
    bool ret = false;

    if (NULL == req) {
        return false;
    }

    i = (int32_t)req->GetNumLocks() - 1;

    for ( ; i > -1; i--) {
        ResourceInfo *res = req->GetResource(i);

        if (NULL == res) {
            continue;
        }

        Resource &resObj = res->GetResourceObject();
        uint16_t qindex = resObj.qindex;
        res->Dump();

        current = GetNode(resObj);
        if (NULL == current) {
            //potentail applyopt failure while acquiring lock, so ignore and proceed
            //when cores present
            continue;
        }

        //is the node contains any request? if no, just ignore and continue
        if (NULL == current->handle) {
            reset_opt_data = GetResetOptData(qindex, resObj.value);
            resObj.value = reset_opt_data;
            rc = oh.ResetOpt(resObj);
            if (rc < 0) {
                QLOGE(LOG_TAG, "Failed to reset optimization here [%" PRIu16 ", %" PRIu16 "]", resObj.major, resObj.minor);
            }
            continue;
        }

        if (req->IsResAlwaysResetOnRelease(resObj.major, resObj.minor)) {
            q_node *prev = NULL;
            QLOGL(LOG_TAG, QLOG_L3, "Always Reset On Remove");
            reset_opt_data = GetResetOptData(qindex, resObj.value);
            resObj.value = reset_opt_data;
            rc = oh.ResetOpt(resObj);
            if (rc < 0) {
                QLOGE(LOG_TAG, "Failed to reset optimization [%" PRIu16 ", %" PRIu16 "]", resObj.major, resObj.minor);
            }
            while (current != NULL) {
                pending = current->next;
                if (current->handle == req) {
                    //first node in queue
                    if (prev == NULL) {
                        //only node in queue
                        if(pending == NULL) {
                            current->handle = NULL;
                            current->resource.level = 0;
                            current->resource.qindex = UNSUPPORTED_Q_INDEX;
                        } else {
                            CopyQnode(current, pending);
                            del = pending;
                        }
                    } else {
                        prev->next = current->next;
                        del = current;
                    }

                    if (del) {
                        free(del);
                        del = NULL;
                    }
                    break;
                } else {
                    prev = current;
                    current = current->next;
               }
            }
            continue;
        }

        pending = current->next;

        if (current->handle == req) {
            if (pending != NULL) {
                needAction = oh.CompareOpt(qindex, current->resource.level, pending->resource.level);
                if (needAction == FAILED) {
                    QLOGE(LOG_TAG, "Failed to find correct action for 0x%" PRIx16, qindex);
                }
                if (display_off == false && needAction == ADD_AND_UPDATE_REQUEST) {
                    QLOGL(LOG_TAG, QLOG_L3, "pending level < current->level, so apply next pending optimization");
                    //Just assign the level as it will have correct value information.
                    resObj.level = pending->resource.level;
                    rc = oh.ApplyOpt(resObj);
                    if (rc < 0) {
                        QLOGE(LOG_TAG, "Failed to apply next pending optimization [%" PRIu16 ", %" PRIu16 "]",
                              resObj.major, resObj.minor);
                    }
                } else if (needAction == ADD_IN_ORDER || needAction == PEND_ADD_IN_ORDER
                            || (display_off == true && needAction == ADD_AND_UPDATE_REQUEST)) {
                    QLOGL(LOG_TAG, QLOG_L2, "Rest optimization");
                    reset_opt_data = GetResetOptData(qindex, resObj.value);
                    resObj.value = reset_opt_data;
                    rc = oh.ResetOpt(resObj);
                    if (rc < 0) {
                        QLOGE(LOG_TAG, "Failed to reset optimization [%" PRIu16 ", %" PRIu16 "]", resObj.major, resObj.minor);
                    }
                }
                del = pending;
                //TODO replace by a copy function for q_node.
                CopyQnode(current, pending);
                /*Free q_node */
                if (del != NULL) {
                    free(del);
                    del = NULL;
                }
            }
            else {
                QLOGL(LOG_TAG, QLOG_L2, "Reset optimization");
                reset_opt_data = GetResetOptData(qindex, resObj.value);
                resObj.value = reset_opt_data;
                rc = oh.ResetOpt(resObj);
                if (rc < 0) {
                    QLOGE(LOG_TAG, "Failed to reset optimization [%" PRIu16 ", %" PRIu16 "]", resObj.major, resObj.minor);
                }
                current->handle = NULL;
                current->resource.level = 0;
                current->resource.qindex = UNSUPPORTED_Q_INDEX;
            }
        } else if (pending != NULL) {
            QLOGL(LOG_TAG, QLOG_L2, "Removing pending requested optimization [%" PRIu16 ", %" PRIu16 "]",
                  resObj.major, resObj.minor);
            RemovePendingRequest(resObj, req);
        } else {
            if (qindex == UNSUPPORTED_Q_INDEX) {
                QLOGE(LOG_TAG, "Release warning, resource optimization (%" PRIu16 ", %" PRIu16 ") not supported",
                      resObj.major, resObj.minor);
            }
        }
        ret = true;
    }
    return ret;
}

/* Remove request from pending queue */
void ResourceQueue::RemovePendingRequest(Resource &resObj, Request *req)
{
    struct q_node *del = NULL;
    struct q_node *iter = NULL;

    iter = GetNode(resObj);

    if (NULL == iter) {
        return;
    }

    while ((iter->next != NULL) && (iter->next->handle != req)) {
        iter = iter->next;
    }
    del = iter->next;
    if (del != NULL) {
        OptsHandler &oh = OptsHandler::getInstance();
        struct q_node *pending = del->next;
        int8_t needAction = FAILED;
        int32_t rc = 0;
        uint32_t reset_opt_data = 0;
        uint16_t qindex = resObj.qindex;
        needAction = oh.CompareOpt(qindex, iter->resource.level, del->resource.level);
        if (pending != NULL && (needAction == ADD_IN_ORDER || needAction == PEND_ADD_IN_ORDER)) {
            needAction = oh.CompareOpt(qindex, del->resource.level, pending->resource.level);
        }
        if (needAction == ADD_IN_ORDER || needAction == PEND_ADD_IN_ORDER) {
            QLOGL(LOG_TAG, QLOG_L2, "Rest optimization");
            reset_opt_data = GetResetOptData(qindex, resObj.value);
            resObj.value = reset_opt_data;
            rc = oh.ResetOpt(resObj);
            if (rc < 0) {
                QLOGE(LOG_TAG, "Failed to reset optimization [%" PRIu16 ", %" PRIu16 "]", resObj.major, resObj.minor);
            }
        }
        iter->next = del->next;
    }
    /* Free q_node */
    if (del) {
        free(del);
        del = NULL;
    }
}

/* This is needed because when we are given the opt code
 * we only know what optimization value to apply,
 * but not what value to reset to.
 */
uint32_t ResourceQueue::GetResetOptData(uint16_t idx, uint32_t level)
{
    uint32_t opt_data = MAX_LVL; // = reset_values[idx];

    /* For SCHED_GROUP_OPCODE & SCHED_FREQ_AGGR_GROUP_OPCODE,*/
    /* level remains reset data*/
    if ((idx == (MAX_DISPLAY_MINOR_OPCODE + MAX_PC_MINOR_OPCODE
                       + MAX_CPUFREQ_MINOR_OPCODE + SCHED_GROUP_OPCODE)) ||
        (idx == (MAX_DISPLAY_MINOR_OPCODE + MAX_PC_MINOR_OPCODE
                       + MAX_CPUFREQ_MINOR_OPCODE + SCHED_FREQ_AGGR_GROUP_OPCODE)) ||
        (idx == (SCHED_START_INDEX + SCHED_TASK_BOOST)) ||
        (idx == (SCHED_START_INDEX + SCHED_ENABLE_TASK_BOOST_RENDERTHREAD)) ||
        (idx == (SCHED_START_INDEX + SCHED_DISABLE_TASK_BOOST_RENDERTHREAD)) ||
        (idx == (SCHED_START_INDEX + SCHED_LOW_LATENCY)) ||
        (idx == (SCHED_START_INDEX + SCHED_WAKE_UP_IDLE)) ||
        (idx == (SCHED_START_INDEX + SCHED_BOOST_OPCODE)) ||
        (idx == (SCHED_EXT_START_INDEX + SCHED_TASK_LOAD_BOOST)) ||
        (idx == (MISC_START_INDEX + SET_SCHEDULER)) ||
        (idx == (MISC_START_INDEX + SCHED_THREAD_PIPELINE)))
        return level;

    return opt_data;
}


void ResourceQueue::PendCurrentRequestsOp(uint16_t idx, q_node *current) {
    struct q_node *pended = NULL;
    uint32_t reset_opt_data = 0;
    int32_t rc = 0;
    OptsHandler &oh = OptsHandler::getInstance();
    Resource tmpResource;
    if ((NULL != current) && (NULL != current->handle) &&
                (current->handle->GetPriority() != ALWAYS_ALLOW)) {
        tmpResource = current->resource;
        reset_opt_data = GetResetOptData(idx, tmpResource.level);

        tmpResource.value = reset_opt_data;
        //todo: better reset optdata
        rc = oh.ResetOpt(tmpResource);
        if (rc == NOT_SUPPORTED) {
            QLOGL(LOG_TAG, QLOG_WARNING, "Pend all current requests, resource=0x%" PRIX16 " not supported", idx);
        }
        if (rc == FAILED) {
            QLOGL(LOG_TAG, QLOG_WARNING, "Pend all current requests, failed to apply optimization for resource=0x%" PRIX16, idx);
        }

        tmpResource.level = 0;
        //TODO why not set handle and opcode?
    }
    return;
}

/* Iterate through all the resources, pend the current lock
 * and reset their levels but retain the current locked
 * request struct.
 */
void ResourceQueue::PendCurrentRequests()
{
    uint16_t i = 0;
    int8_t j = 0;
    q_node *right = NULL, *current = NULL;

    for (i = 0; i < MAX_MINOR_RESOURCES; i++) {
        current = &resource_qs[i];
        //apply op on root node
        PendCurrentRequestsOp(i, current);

        //traverse nodes based on core numbers
        right = resource_qs[i].right;
        if (NULL != right) {
            for (j = 1; j < TargetConfig::getTargetConfig().getTotalNumCores(); j++) {
                current = &right[j];
                PendCurrentRequestsOp(i, current);
            }
        }
    } /*for (i = 0; i < MAX_MINOR_RESOURCES; i++)*/
    return;
}

/* After all the requests for display off case
 * have been honored, set the flag to indicate
 * display is off
 */
void ResourceQueue::LockCurrentState(Request *req)
{
    ResourceInfo *hint_resource = req->GetResource(0);
    if(hint_resource != NULL) {
        if(hint_resource->GetQIndex() == DISPLAY_OFF_INDEX) {
            QLOGL(LOG_TAG, QLOG_L2, "setting display_off to true");
            display_off = true;
        } else if(hint_resource->GetQIndex() == DISPLAY_DOZE_INDEX) {
            QLOGL(LOG_TAG, QLOG_L2, "setting display_doze to true");
            display_doze = true;
        }
    }
    return;
}


void ResourceQueue::UnlockCurrentStateOp(uint16_t, q_node *current, q_node *, Request *) {
    q_node *del = NULL;
    OptsHandler &oh = OptsHandler::getInstance();

    if (!current) {
        return;
    }

    if (NULL != current->handle && current->handle->GetPriority() != ALWAYS_ALLOW) {
        oh.ApplyOpt(current->resource);
    }
    return;
}

/* After we have released the lock on the resources
 * requested by the display off scenario, we can go
 * ahead and release all the other resources that were
 * occupied by the display off case.
 */
void ResourceQueue::UnlockCurrentState(Request *req)
{
    uint16_t i = 0;
    int8_t j = 0;
    q_node *current = NULL, *pending = NULL;
    q_node *right = NULL;

    ResourceInfo *hint_resource = req->GetResource(0);
    if(hint_resource != NULL) {
        if(hint_resource->GetQIndex() == DISPLAY_OFF_INDEX) {
            QLOGL(LOG_TAG, QLOG_L2, "UnlockCurrentState setting display_off to false");
            display_off = false;
        } else if(hint_resource->GetQIndex() == DISPLAY_DOZE_INDEX) {
            QLOGL(LOG_TAG, QLOG_L2, "UnlockCurrentState setting display_doze to false");
            display_doze = false;
        }
    }

    for (i = 0; i < MAX_MINOR_RESOURCES; i++) {
        current = &resource_qs[i];
        pending = resource_qs[i].next;
        UnlockCurrentStateOp(i, current, pending, req);

        //traverse nodes based on core numbers
        right = resource_qs[i].right;
        if (NULL != right) {
            for (j = 1; j < TargetConfig::getTargetConfig().getTotalNumCores(); j++) {
                current = &right[j];
                pending = right[j].next;
                UnlockCurrentStateOp(i, current, pending, req);
            }
        }
    } /*for (i = 0; i < MAX_MINOR_RESOURCES; i++)*/
    return;
}

void ResourceQueue::Reset()
{
    uint16_t i = 0;
    int8_t j = 0;
    q_node *current = NULL, *next = NULL, *right = NULL;

    for (i = 0; i < MAX_MINOR_RESOURCES; i++) {
        right = resource_qs[i].right;

        //traverse nodes based on core numbers
        if (NULL != right) {
            for (j = 0; j < TargetConfig::getTargetConfig().getTotalNumCores(); j++) {
                current = right[j].next;
                while (NULL != current) {
                    next = current->next;
                    free(current);
                    current = next;
                }
            }
            free(right);
            right = NULL;
        }

        //traverse pended nodes
        current = resource_qs[i].next;
        while (NULL != current) {
            next = current->next;
            free(current);
            current = next;
        }
    }
}

