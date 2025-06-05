/*===========================================================================

  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __UTIL_H__
#define __UTIL_H__

#include "IProfileKey.h"
#include "ProfileParams.h"
#include "Manager.h"
#include <list>
#include <optional>

namespace qdp {
/**
 * @brief
 * Util class contains all the meta APIs that can be used by the
 * client for complex functionality. They are all implemented
 * using Manager APIs and require Manager to have been
 * initalized
 */

class Util {
  public:
    Util();
    ~Util();

  /**
   *
   * @brief
   * Set whether modem profile update for IP type is allowed
   * @note If this is set to true, IP type is not used for
   *       comparison in some of the Util APIs and it gets updated
   *
   * @param profileUpdateAllowed True if update allowed and false
   *                             otherwise. It is false by default
   */
    void setProfileUpdateAllowed(bool profileUpdateAllowed);
  /**
   *
   * @brief
   * Match LTE attach profile id against the passed in profile
   * parameters
   *
   * @param profileId Modem attach profile id against which the
   *                  parameters need to be matched
   * @param inputParams Profile parameters against which the modem
   *                    profile id is matched
   *
   * @return bool True if it matched, false otherwise
   */
    bool matchLteAttachProfileId(ProfileId profileId, const ProfileParams& inputParams);
  /**
   *
   * @brief
   * Searches the modem 3GPP profiles using parameters passed in
   * and identifies a profile that can be used for LTE attach. If
   * a profile does not exist, an IA persistent profile gets
   * created. If a profile exists, it may get updated to match the
   * passed in parameters. In either case key to the identified
   * profile is returned.
   *
   * @param inputParams Profile parameters to be used to identify
   *                    an initial attach profile
   *
   * @return std::shared_ptr<IProfileKey> Shared pointer to
   *         IProfileKey for profile that can be used for LTE
   *         attach. Client needs to maintain a reference to the
   *         profile key if it is interested in it
   */
    std::shared_ptr<IProfileKey> findOrCreateProfileForLteAttach(const ProfileParams& inputParams);
  /**
   *
   * @brief
   * Updates profiles on the modem that match APN class and tech
   * type of the 3GPP and 3GPP2 profiles that are passed in. If a
   * matching profile is not found for the tech type and APN class
   * combination, a persistent profile will be created with the
   * corresponding profile parameters.
   * @note This API should get called for profiles for which
   * modem_cognitive/persistent is true
   *
   * @param input3gppProfileList List of 3GPP profiles for which
   *                             APN class needs to be matched and
   *                             profiles updated or created
   * @param input3gpp2ProfileList List of 3GPP2 profiles for which
   *                              APN class needs to be matched
   *                              and profiles updated or created
   */
    void updateModemProfileByApnClass(const std::list<ProfileParams>& input3gppProfileList, const std::list<ProfileParams>& input3gpp2ProfileList);
  /**
   *
   * @brief
   * This API consolidates the APN type bitmask per APN name of
   * the profiles that are passed in, and updates APN type bitmask
   * of the modem profiles after matching APN name and IP type.
   *
   * @param setDataProfileList List of profiles using which the
   *                           the APN type bitmask of modem
   *                           profiles is updated
   */
    void updateAPNTypeBitmaskOnModemProfile(const std::list<ProfileParams>& setDataProfileList);
  /**
   *
   * @brief
   * This API is used to identify a profile that can be used for
   * setupDataCall using the passed in profile parameters and the
   * tech type. If a profile is found, its IP type could get
   * updated based on whether isProfileUpdateAllowed is set to
   * true. If a profile is not found, a new non persistent will be
   * created with the passed in parameters. In either case a key
   * to the identified profile is returned.
   * @note This API needs to be called per tech type. Thus if the
   *       client needs a profile for 3GPP and 3GPP2, this API
   *       would have to be called twice with tech type set to
   *       3GPP and 3GPP2 respectively.
   *
   * @param techType Tech type for which profile needs to be
   *                 identified
   * @param inputParams Profile parameters using which modem
   *                    profile needs to be identified
   *
   * @return std::shared_ptr<IProfileKey> Shared pointer to
   *         IProfileKey that has been identified for the
   *         setupDataCall. Client needs to maintain a reference
   *         to the profile key it is interested in it
   * @note If a profile is created by this API it will be created
   *       as a non persistent profile. Thus if the client
   *       releases the reference on IProfileKey, it will get
   *       deleted if there are no more references to it
   */
    std::shared_ptr<IProfileKey> findOrCreateProfileForSetupDataCall(TechType techType, const ProfileParams& inputParams);
  /**
   *
   * @brief
   * This API is used to identify a profile that can be used for
   * emergency data call using the passed in profile parameters.
   * It only looks for 3GPP profiles and does a very loose search
   * using APN name or any emergency profile. If a profile is
   * not found, an emergency persistent profile will be created
   * using passed in profile parameters
   *
   * @param inputParams Profile parameters using which emergency
   *                    modem profile needs to be identified
   *
   * @return std::shared_ptr<IProfileKey> Shared pointer to
   *         IProfileKey that has been identified for the
   *         emergency data call. Client needs to maintain a
   *         reference to the profile key it is interested in it
   */
    std::shared_ptr<IProfileKey> findOrCreateProfileForEmergencyDataCall(const ProfileParams& inputParams);

  private:
    Manager &mgr;
    bool isProfileUpdateAllowed;
    void handleCombinedProfileUpdate(uint32_t combinedBitmask, std::string apnName);
    void handleProfileUpdateByTechType(TechType techType,
                                       uint32_t v4Bitmask,
                                       uint32_t v6Bitmask,
                                       uint32_t v4v6Bitmask,
                                       std::string apnName);
    bool updateByIpType(TechType techType, IPType ipType, uint32_t apnBitmask, std::string apnName);
    void updateModemProfileByApnClassAndTechType(const ProfileParams& profile, TechType techType);
};

} //namespace

#endif /*__UTIL_H__*/
