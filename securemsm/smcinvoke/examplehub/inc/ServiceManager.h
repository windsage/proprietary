/********************************************************************
Copyright (c) 2020-2021, 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*********************************************************************/

#ifndef __SERVICEMANAGER_H
#define __SERVICEMANAGER_H

#include <stdint.h>
#include <stddef.h>
#include "object.h"


typedef struct ServiceManager ServiceManager;

/* Returns IOpener error codes
 */
int ServiceManager_getService(ServiceManager *me, uint32_t uid, Object credentials, Object *objOut);

/* IN: uid provided by the module being registered
 * IN: IModule implementing the uid
 * Return: 0 on Success otherwise -1
 */
int ServiceManager_registerModule(ServiceManager *me, uint32_t uid, Object modObj);

/* IN: uid the service
 * Return: if service with uid is found, it would be removed
 *         else nothing happens. No success/failure is returned
 */
void ServiceManager_deregister(ServiceManager *me, uint32_t uid);

/* IN: max app objects that app manager would store
 * Return: Pointer to ServiceManager or NULL if memory allocation fails
 */
ServiceManager *ServiceManager_new(size_t maxApps);

/* IN: pointer received using AppManager_new
 * Return: Nothing. It will release all memory allocated by AppManager_new
 */
void ServiceManager_delete(ServiceManager *me);

#endif //__SERVICEMANAGER_H
