/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IIPCHandlerRegistration

  Copyright  (c) 2015, 2021 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/


#ifndef __IZAT_MANAGER_IIPCHANDLERREGISTRATION_H__
#define __IZAT_MANAGER_IIPCHANDLERREGISTRATION_H__

#include <string>
#include <IOsObserver.h>

using loc_core::IOsObserver;

namespace izat_manager {

class IIPCHandlerRegistration {
public:
    // If you get messages for this string, call on pointer
    virtual void reg
    (
        const std :: string & name,
        IOsObserver * iOBface
    ) = 0;

    // dtor
    virtual ~IIPCHandlerRegistration () {}
};

}

#endif // #ifndef __IZAT_MANAGER_IIPCHANDLERREGISTRATION_H__
