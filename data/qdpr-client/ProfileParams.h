/*===========================================================================

  Copyright (c) 2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef __PROFILEPARAMS_H__
#define __PROFILEPARAMS_H__

#include "CommonDefs.h"
#include <optional>
#include <string>

namespace qdp {

/**
 * @brief
 * Class that encapsulates all the profile related parameters.
 * All the profile parameters are optional.
 */

class ProfileParams {
  public:
    ProfileParams();
    virtual ~ProfileParams();

  /**
   *
   * @brief
   * Get APN name
   *
   * @return std::string APN name
   */
    std::string getApnName() const;
  /**
   *
   * @brief
   * Get user name
   *
   * @return std::string User name
   */
    std::string getUserName() const;
  /**
   *
   * @brief
   * Get password
   *
   * @return std::string Password
   */
    std::string getPassword() const;
  /**
   *
   * @brief
   * Get Auth type
   *
   * @return AuthType
   */
    AuthType getAuthType() const;
  /**
   *
   * @brief
   * Get Home IP type
   *
   * @return IPType Home IP type
   */
    IPType getHomeIPType() const;
  /**
   *
   * @brief
   * Get Roam IP type
   *
   * @return IPType Roam IP type
   */
    IPType getRoamIPType() const;
  /**
   *
   * @brief
   * Get APN types bitmask
   *
   * @return uint32_t ApnTypes bitmask
   */
    uint32_t getApnTypesMask() const;
  /**
   *
   * @brief
   * Get APN class
   *
   * @return ApnClass
   */
    ApnClass getApnClass() const;
  /**
   *
   * @brief
   * Get if CLAT is supported on the profile.
   *
   * @return bool is CLAT supported
   */
    bool getIsClatEnabled() const;
  /**
   *
   * @brief
   * Get Max PDN connections per block.
   * @note
   * Only valid for 3GPP profiles
   *
   * @return int32_t Max PDN connections per block
   */
    int32_t getMaxPdnsConnPerBlock() const;
  /**
   *
   * @brief
   * Get Max PDN connection timer
   * @note
   * Only valid for 3GPP profiles
   *
   * @return int32_t Max PDN connection timer
   */
    int32_t getMaxPdnConnTimer() const;
  /**
   *
   * @brief
   * Get PDN request wait interval
   * @note
   * Only valid for 3GPP profiles
   *
   * @return int32_t PDN request wait interval
   */
    int32_t getPdnReqWaitInterval() const;
  /**
   *
   * @brief
   * Get if APN is disabled
   * @note
   * Only valid for 3GPP profiles
   *
   * @return bool APN disabled
   */
    bool getIsApnDisabled() const;
  /**
   *
   * @brief
   * Get if emergency calls are supported by the profile
   * @note
   * Only valid for 3GPP profiles
   *
   * @return bool Emergency call supported
   */
    bool getIsEmergencySupported() const;
  /**
   *
   * @brief
   * Get if this is a QDP profile
   * @note
   * Only valid for 3GPP profiles
   *
   * @return bool is QDP profile
   */
    bool getIsQdpProfile() const;
  /**
   *
   * @brief
   * Get if this is an IA profile
   * @note
   * Only valid for 3GPP profiles
   *
   * @return bool is IA profile
   */
    bool getIsIAProfile() const;

  /**
   *
   * @brief
   * Set the APN name
   *
   * @param apnName
   *
   * @return ProfileParams& reference to this
   */
    ProfileParams& setApnName(std::string apnName);
  /**
   *
   * @brief
   * Set the User name
   *
   * @param userName
   *
   * @return ProfileParams& reference to this
   */
    ProfileParams& setUserName(std::string userName);
  /**
   *
   * @brief
   * Set the Password
   *
   * @param password
   *
   * @return ProfileParams& reference to this
   */
    ProfileParams& setPassword(std::string password);
  /**
   *
   * @brief
   * Set the Auth type
   *
   *  @param authType
   *
   * @return ProfileParams& reference to this
   */
    ProfileParams& setAuthType(AuthType authType);
  /**
   *
   * @brief
   * Set the Home IP type
   *
   * @param homeIP
   *
   * @return ProfileParams& reference to this
   */
    ProfileParams& setHomeIPType(IPType homeIP);
  /**
   *
   * @brief
   * Set the Roam IP type
   *
   * @param roamIP
   *
   * @return ProfileParams& reference to this
   */
    ProfileParams& setRoamIPType(IPType roamIP);
  /**
   *
   * @brief
   * Set the ApnTypes bitmask
   *
   * @param apnTypesMask
   *
   * @return ProfileParams& reference to this
   */
    ProfileParams& setApnTypesMask(uint32_t apnTypesMask);
  /**
   *
   * @brief
   * Set the APN class
   *
   * @param apnClass
   *
   * @return ProfileParams& reference to this
   */
    ProfileParams& setApnClass(ApnClass apnClass);
  /**
   *
   * @brief
   * Set if CLAT is enabled
   *
   * @param isClatEnabled
   *
   * @return ProfileParams& reference to this
   */
    ProfileParams& setIsClatEnabled(bool isClatEnabled);
  /**
   *
   * @brief
   * Set the Max PDN connections per block
   * @note
   * Only valid for 3GPP profiles
   *
   * @param maxPdnsConnPerBlock
   *
   * @return ProfileParams& reference to this
   */
    ProfileParams& setMaxPdnsConnPerBlock(int32_t maxPdnsConnPerBlock);
  /**
   *
   * @brief
   * Set the Max PDN connection timer
   * @note
   * Only valid for 3GPP profiles
   *
   * @param maxPdnConnTimer
   *
   * @return ProfileParams& reference to this
   */
    ProfileParams& setMaxPdnConnTimer(int32_t maxPdnConnTimer);
  /**
   *
   * @brief
   * Set the PDN request wait interval
   * @note
   * Only valid for 3GPP profiles
   *
   * @param pdnReqWaitInterval
   *
   * @return ProfileParams& reference to this
   */
    ProfileParams& setPdnReqWaitInterval(int32_t pdnReqWaitInterval);
  /**
   *
   * @brief
   * Set if APN is disabled
   * @note
   * Only valid for 3GPP profiles
   *
   * @param isApnDisabled
   *
   * @return ProfileParams& reference to this
   */
    ProfileParams& setIsApnDisabled(bool isApnDisabled);
  /**
   *
   * @brief
   * Set if emergency calls are supported
   * @note
   * Only valid for 3GPP profiles
   *
   * @param isEmergencySupported
   *
   * @return ProfileParams& reference to this
   */
    ProfileParams& setIsEmergencySupported(bool isEmergencySupported);

  /**
   *
   * @brief
   * Checks if APN name is set
   *
   * @return bool
   */
    bool hasApnName() const;
  /**
   *
   * @brief
   * Checks if User name is set
   *
   * @return bool
   */
    bool hasUserName() const;
  /**
   *
   * @brief
   * Checks if Password is set
   *
   * @return bool
   */
    bool hasPassword() const;
  /**
   *
   * @brief
   * Checks if Auth type is set
   *
   * @return bool
   */
    bool hasAuthType() const;
  /**
   *
   * @brief
   * Checks if Home IP type is set
   *
   * @return bool
   */
    bool hasHomeIPType() const;
  /**
   *
   * @brief
   * Checks if Roam IP is set
   *
   * @return bool
   */
    bool hasRoamIPType() const;
  /**
   *
   * @brief
   * Checks if APN types bitmask if set
   *
   * @return bool
   */
    bool hasApnTypesMask() const;
  /**
   *
   * @brief
   * Checks if APN class is set
   *
   * @return bool
   */
    bool hasApnClass() const;
  /**
   *
   * @brief
   * Checks if is CLAT enabled is set
   *
   * @return bool
   */
    bool hasIsClatEnabled() const;
  /**
   *
   * @brief
   * Checks if Max PDNsconnection per block is set
   *
   * @return bool
   */
    bool hasMaxPdnsConnPerBlock() const;
  /**
   *
   * @brief
   * Checks if Max PDN connection timer is set
   *
   * @return bool
   */
    bool hasMaxPdnConnTimer() const;
  /**
   *
   * @brief
   * Checks if PDN request wait interval is set
   *
   * @return bool
   */
    bool hasPdnReqWaitInterval() const;
  /**
   *
   * @brief
   * Checks if is APN disabled is set
   *
   * @return bool
   */
    bool hasIsApnDisabled() const;
  /**
   *
   * @brief
   * Checks if is emergency supported is set
   *
   * @return bool
   */
    bool hasIsEmergencySupported() const;
  /**
   *
   * @brief
   * Dumps logging information
   *
   * @return std::string
   */
    virtual std::string dump() const;

  #ifndef QMI_RIL_UTF
  protected:
  #endif
    std::optional<bool> isQdpProfile;
    std::optional<bool> isIAProfile;

  private:
    std::optional<TechType> tt;
    std::optional<std::string> apnName;
    std::optional<std::string> userName;
    std::optional<std::string> password;
    std::optional<AuthType> authType;
    std::optional<IPType> homeIP;
    std::optional<IPType> roamIP;
    std::optional<uint32_t> apnTypesMask;
    std::optional<ApnClass> apnClass;
    std::optional<bool> isClatEnabled;
    std::optional<bool> isEmergencySupported;
    std::optional<bool> isApnDisabled;
    std::optional<int32_t> maxPdnsConnPerBlock;
    std::optional<int32_t> maxPdnConnTimer;
    std::optional<int32_t> pdnReqWaitInterval;
};

/**
 *
 * @brief
 * Derived class that inherits all the profile parameters and
 * also encapsulates additional parameters needed for add
 * profile
 */
class AddProfileParams : public ProfileParams {
  public:
  /**
   *
   * @brief
   * AddProfileParams constructor
   *
   * @param isPersistent
   * Whether the profile should be created as persistent or non
   * persistent
   * @param techType
   * Whether the profile should be created with tech type 3GPP or
   * 3GPP2
   * @param isIAProfile
   * Whether the profile should be created as an IA profile
   * @note
   * This information is relevant when trying to identify a
   * profile for initial attach
   */
    AddProfileParams(bool isPersistent, TechType techType, bool isIAProfile=false);
  /**
   *
   * @brief
   * AddProfileParams constructor with additional ProfileParams
   *
   * @param base
   * All ProfileParams are copied from base to the
   * AddProfileParams object being constructed.
   * @param isPersistent
   * Whether the profile should be created as persistent or non
   * persistent
   * @param techType
   * Whether the profile should be created with tech type 3GPP or
   * 3GPP2
   * @param isIAProfile
   * Whether the profile should be created as an IA profile
   * @note
   * This information is relevant when trying to identify a
   * profile for initial attach
   */
    AddProfileParams(const ProfileParams& base, bool isPersistent, TechType techType, bool isIAProfile=false);
    ~AddProfileParams() {};
  /**
   *
   * @brief
   * Get whether add profile should be persistent
   *
   * @return bool
   */
    bool getIsPersistent() const;
  /**
   *
   * @brief
   * Get tech type of add profile
   *
   * @return TechType
   */
    TechType getTechType() const;
  /**
   *
   * @brief
   * Dumps logging information
   *
   * @return std::string
   */
    std::string dump() const override;
  private:
    bool isPersistent;
    TechType techType;
};

/**
 * @brief
 * Derived class that inherits all the profile parameters and
 * also encapsulates other parameters needed for filter and
 * match operations. If a parameter has been explicitly set, it
 * will be used for the filter/match. Else it will not be used
 * for comparison.
 */

class FilterProfileParams : public ProfileParams {
  public:
  /**
   *
   * @brief
   * Default constructor
   */
    FilterProfileParams();
  /**
   *
   * @brief
   * Constructor with additional ProfileParams parameter
   *
   * @param base
   * All ProfileParams are copied from base to the
   * FilterProfileParams object being constructed
   */
    FilterProfileParams(const ProfileParams& base);
  /**
   *
   * @brief
   * Adds additional criteria for filter matching
   *
   * @param filterMatchCriteria
   * Bitmask of FilterMatchType that needs to be added
   *
   * @return FilterProfileParams& returns a reference to this
   */
    FilterProfileParams& addFilterMatchCriteria(uint32_t filterMatchCriteria);
  /**
   *
   * @brief
   * Removes criteria for filter matching
   *
   * @param filterMatchCriteria
   * Bitmask of FilterMatchType that needs to be removed
   *
   * @return FilterProfileParams& returns a reference to this
   */
    FilterProfileParams& removeFilterMatchCriteria(uint32_t filterMatchCriteria);
  /**
   *
   * @brief
   * Set the profile id to be used for filter match
   * @note This can be used to look for a modem profile with a
   *       specific profile id
   *
   * @param profileId
   * Profile Id to be used for filter match
   *
   * @return FilterProfileParams& returns a reference to this
   */
    FilterProfileParams& setFilterByProfileId(ProfileId profileId);
  /**
   *
   * @brief
   * Get the profile id that can be used for filter match
   *
   * @return ProfileId
   */
    ProfileId getFilterByProfileId() const;
  /**
   *
   * @brief
   * Check if profile id has been set on this object
   *
   * @return bool
   */
    bool hasFilterByProfileId() const;
  /**
   *
   * @brief
   * Set the tech type to be used for filter match
   *
   * @param techType
   * Tech type to be used for filter match
   *
   * @return FilterProfileParams& returns a reference to this
   */
    FilterProfileParams& setFilterByTechType(TechType techType);
  /**
   *
   * @brief
   * Get the tech type to be used for filter match
   *
   * @return TechType
   */
    TechType getFilterByTechType() const;
  /**
   *
   * @brief
   * Check if tech type has been set on this object
   *
   * @return bool
   */
    bool hasFilterByTechType() const;
    ~FilterProfileParams() {};
  /**
   *
   * @brief
   * Check if filter by is QDP profile has been set on this object
   *
   * @return bool
   */
    bool hasFilterByIsQdpProfile() const;
  /**
   *
   * @brief
   * Set filter by is QDP profile to true or false
   * @note
   * If it is set to true, the filter or match operation will only
   * look for QDP profiles. If it is explicitly set to false, it
   * will only look for modem profiles. If unset it will not
   * consider QDP vs modem profile as a criteria
   *
   * @param filterByIsQdpProfile
   *
   * @return FilterProfileParams& returns a reference to this
   */
    FilterProfileParams& setFilterByIsQdpProfile(bool filterByIsQdpProfile);
  /**
   *
   * @brief
   * Check if filter by is IA profile is set on this object
   *
   * @return bool
   */
    bool hasFilterByIsIAProfile() const;
  /**
   *
   * @brief
   * Set filter by is IA profile to true or false
   * @note
   * If it is set to true, the filter or match operation will only
   * look for QDP IA profiles. It is is explicitly set to false,
   * it will only look for profiles which are not QDP IA (this
   * include only QDP or modem profiles). If unset, is QDP IA will
   * not be used as a factor in comparison
   *
   * @param filterByIsIAProfile
   *
   * @return FilterProfileParams& returns a reference to this
   */
    FilterProfileParams& setFilterByIsIAProfile(bool filterByIsIAProfile);
  /**
   *
   * @brief
   * Get the filter match criteria that has been set
   *
   * @return uint32_t Bitmask of FilterMatchType
   */
    uint32_t getFilterMatchCriteria() const;
  /**
   *
   * @brief
   * Dumps logging information
   *
   * @return std::string
   */
    std::string dump() const override;

  private:
    uint32_t filterMatchCriteria;
    std::optional<ProfileId> filterProfileId;
    std::optional<TechType> filterTechType;
};

/**
 * @brief
 * Derived class that inherits all the profile parameters and
 * also encapsulates other parameters needed to modify a
 * profile.
 */

class ModifyProfileParams : public ProfileParams {
  public:
  /**
   *
   * @brief
   * Default constructor
   */
    ModifyProfileParams( ) {};
  /**
   *
   * @brief
   * Constructor with additional ProfileParams parameter
   *
   * @param base
   * All ProfileParams are copied from base to the
   * ModifyProfileParams object being constructed
   */
    ModifyProfileParams(const ProfileParams& base): ProfileParams(base) {};
    ~ModifyProfileParams() {};
};

} //namespace

#endif /*__PROFILEPARAMS_H__*/
