/*
* Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
#pragma once
#ifndef RIL_SOCKET_H_INCLUDED
#define RIL_SOCKET_H_INCLUDED

void socket_start(qcril_instance_id_e_type instance_id);
void notifyClientServerIsReady(int fd);
#endif // RIL_SOCKET_H_INCLUDED
