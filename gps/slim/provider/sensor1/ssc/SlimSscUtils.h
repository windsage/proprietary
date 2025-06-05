/*
 * Copyright (c) 2020-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef SLIM_SSC_UTILS_INCLUDED
#define SLIM_SSC_UTILS_INCLUDED

//#pragma once
#include <vector>

//#include "ssc_connection.h"
#include "SlimSscConnection.h"

/*uint32 4 bytes represents different flags*/
#define DISABLE_SENSORS_FLAG             0x00000001
/*by default all are enabled , choice to disable like below */
/*      disable_<sensorsname>_sensors*/
#define DISABLE_SENSORS_STRING           "disable_sensors"
#define DISABLE_WAKEUP_SENSORS_FLAG      0x00000002
#define DISABLE_WAKEUP_SENSORS_STRING    "disable_wakeup_sensors"
#define DISABLE_PROXIMITY_SENSORS_FLAG   0x00000004
#define DISABLE_PROXIMITY_SENSORS_STRING "disable_proximity_sensors"
#define SNS_DIAG_CIRC_BUFF_MODE_FLAG      0x00000008
#define SNS_DAIG_CIRC_BUFF_MODE_STRING    "diag_circular_buffering_mode"

/**
 * @brief Struct to represent sensor's unique ID (128-bit)
 */
struct sensor_uid
{
    sensor_uid() : low(0), high(0) {}
    sensor_uid(uint64_t low, uint64_t high): low(low), high(high) {}
    bool operator==(const sensor_uid& rhs) const
    {
        return (low == rhs.low && high == rhs.high);
    }
    uint64_t low, high;
};

/**
 * @brief type alias for an suid event function
 *
 * param datatype: datatype of of the sensor associated with the
 * event
 * param suids: vector of suids available for the given datatype
 */
using suid_event_function =
    std::function<void(const std::string& datatype,
                       const std::vector<sensor_uid>& suids)>;

/**
 * @brief Utility class for discovering available sensors using
 *        dataytpe
 *
 */
class suid_lookup
{
public:
    /**
     * @brief creates a new connection to ssc for suid lookup
     *
     * @param cb callback function for suids
     */
    suid_lookup(suid_event_function cb);

    /**
     *  @brief look up the suid for a given datatype, registered
     *         callback will be called when suid is available for
     *         this datatype
     *
     *  @param datatype data type for which suid is requested
     *  @param default_only option to ask for publishing only default
     *         suid for the given data type. default value is false
     */
    void request_suid(std::string datatype, bool default_only = false);

private:
    suid_event_function _cb;
    void handle_ssc_event(const uint8_t *data, size_t size);
    ssc_connection _ssc_conn;
    ssc_event_cb get_ssc_event_cb()
    {
        return [this](const uint8_t *data, size_t size)
        {
          handle_ssc_event(data, size);
        };
    }
};

#endif /* SLIM_SSC_UTILS_INCLUDED */
