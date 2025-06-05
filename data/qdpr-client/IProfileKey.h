/*===========================================================================

  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __IPROFILEKEY_H__
#define __IPROFILEKEY_H__

#include "CommonDefs.h"
#include <memory>
#include <string>

namespace qdp {

/**
 *
 * @brief
 * Interface that provides information on a unique profile key
 * @note
 * Client cannot create an instance of this interface. It will
 * always be returned as response to one of the APIs as a shared
 * pointer reference. When any API returns a reference to
 * IProfileKey, internally the reference count on the profile is
 * incremented by one. When the client releases the reference to
 * IProfileKey shared pointer, the reference count on it will be
 * decremented and the corresponding profile may get deleted if
 * it is a non persistent profile and there are no more
 * references to it.
 */
class IProfileKey : public std::enable_shared_from_this<IProfileKey> {
public:
    virtual ~IProfileKey() {}
    /**
     *
     * @brief
     * Returns the profile id of the unique key
     *
     * @return ProfileId
     */
    virtual ProfileId getProfileId() = 0;
    /**
     *
     * @brief
     * Returns the tech type of the unique key
     *
     * @return TechType
     */
    virtual TechType getTechType() = 0;
    /**
     *
     * @brief
     * Client can pass in a listener to be invoked when the profile
     * key is no longer valid. This can happen if the underlying
     * modem profile has been modified or deleted.
     * @note Once the profile key is no longer valid, the client
     *       cannot use it for any future modify or delete APIs
     *
     * @param onExpiration Callback that needs to be invoked. It
     *                     will be called in the qdp thread and not
     *                     the client thread.
     */
    virtual void registerExpirationListener(ExpirationListener onExpiration) = 0;
    /**
     *
     * @brief
     * Logging information
     *
     * @return std::string
     */
    virtual std::string dump() = 0;

protected:
    IProfileKey() {}
};

} //namespace

#endif /*__IPROFILEKEY_H__*/
