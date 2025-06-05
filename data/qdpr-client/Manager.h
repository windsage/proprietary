/*===========================================================================

  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __MANAGER_H__
#define __MANAGER_H__

#include "IProfileKey.h"
#include "ProfileParams.h"
#include <list>
#include <optional>

namespace qdp {

class EventManager;
class ClientRequestManager;

/**
 * @brief
 * Manager class has APIs for core QDP functionality.
 */

class Manager {
  public:
  /**
   * @brief
   * Used to get the Singleton instance of Manager
   *
   * @return Manager& Reference to Singleton instance
   */
    static Manager& getInstance()
    {
        static Manager mgr;
        return mgr;
    }

  /**
   *
   * @brief
   * Initialize Manager Singleton with specific subscription id
   * @note None of the other Manager APIs can be invoked until
   * init has been called
   *
   * @param subsId Subscription id to be used by Manager for
   *               interaction with the modem
   * @param logToAdb Indicates whether to log to ADB. Defaults to
   *                 QXDM when false.
   */
    void init(int subsId, bool logToAdb = false);

  /**
   * @brief
   * Refresh information in Manager. Manager will clear its cache
   * and re-read information from the modem when this API is
   * invoked
   * @note This API should be called by the client during MCFG
   *       refresh
   *
   */
    void refresh(PDCRefreshStatus status);
  /**
   *
   * @brief
   * Creates a new profile on the modem and in the internal cache
   *
   * @param params Contains profile parameters and also other
   *               information like whether the profile should be
   *               created as persistent, whether it is an IA
   *               profile
   *
   * @return std::shared_ptr<IProfileKey> Returns a shared pointer
   *         to IProfileKey which contains unique key for the
   *         profile.
   * @note If the profile is created as non persistent and client
   *       releases the reference to IProfileKey shared pointer,
   *       the profile will get deleted as there are no more
   *       references to it
   */
    std::shared_ptr<IProfileKey> addProfile(const AddProfileParams& params);
  /**
   *
   * @brief
   * Modifies the profile identified by IProfileKey. Profile is
   * modified on the modem as well as in the internal cache
   *
   * @param params Contains the profile parameters that need to be
   *               modified. Since all parameters in
   *               ModifyProfileParams are optional, client only
   *               needs to add the actual parameters that need to
   *               be modified
   * @note The modify request sent to the modem will only contain
   *       those parameters which are different between modem
   *       profile and ModifyProfileParams. Thus if all values are
   *       the same, the modify request will be a NOOP.
   *
   * @param key Unique key that identifies the profile that needs
   *            to be modified
   *
   * @return bool Return value of true indicates profile was
   *         modified successfully. Whereas false indicates a
   *         failure
   */
    bool modifyProfile(const ModifyProfileParams& params, std::shared_ptr<IProfileKey> key);
  /**
   *
   * @brief
   * Deletes the profile identified by the key. Profile will be
   * deleted on the modem as well as in the internal cache
   *
   * @param key Unique key that identifies profile that needs to
   *            be deleted
   *
   * @return bool Return value of true indicates profile was
   *         deleted successfully. Whereas false indicates a
   *         failure
   */
    bool deleteProfile(std::shared_ptr<IProfileKey> key);
  /**
   *
   * @brief
   * Check all the modem profiles against the passed in criteria
   * and return a list of profiles that match the criteria
   *
   * @param filterParams Contains the profile parameters that need
   *                     to be matched. Can also contain
   *                     additional parameters like profile id and
   *                     tech type, as well as additional criteria
   *                     like subset/superset matching for some
   *                     parameters
   *
   * @return std::list<std::shared_ptr<IProfileKey>> List of
   *         shared pointers to IProfileKey for profiles that
   *         matched the filterParams.
   * @note The reference count on the profile corresponding to
   *         each IProfileKey is incremented when this list is
   *         returned. If the client no longer retains a reference
   *         to the shared pointer, the reference count will get
   *         decremented and the profile may get deleted if is a
   *         non persistent profile and there are no more
   *         references to it
   */
    std::list<std::shared_ptr<IProfileKey>> filter(const FilterProfileParams& filterParams);

   /**
   *
   * @brief
   * Returns all profile parameters for the profile identified by
   * the key
   *
   * @param key Unique key that identifies profile of interest
   *
   * @return std::optional<ProfileParams> Profile parameters of the
   *         profile.
   * @note Return type is optional. Profile parameters will be
   *       returned only if profile can be found. Else nothing will
   *       be returned
   */
    std::optional<ProfileParams> getProfile(std::shared_ptr<IProfileKey> key);
  /**
   *
   * @brief
   * Matches the profile identified by the key against the
   * provided criteria
   *
   * @param params Contains the criteria against which profile
   *               needs to be matched. It contains profile
   *               parameters and additional criteria that can be
   *               used for matching. Clients only need to add the
   *               specific parameters that they are interested in
   *               matching.
   * @param key Unique key that identifies the profile that needs
   *            to be matched
   *
   * @return bool Returns true if the profile was matched
   *         successfully and false otherwise
   */
    bool match(const FilterProfileParams& params, std::shared_ptr<IProfileKey> key);
  /**
   *
   * @brief
   * Dumps logging information
   *
   * @return std::vector<std::string> Vector of strings that
   *         contains critical logs
   */
    std::vector<std::string> dump();
#ifdef QDP_TEST_BUILD
    void deinit();
#endif

  private:
    Manager();
    ~Manager();
    std::unique_ptr<ClientRequestManager> crm;

};

} //namespace

#endif /*__MANAGER_H__*/
