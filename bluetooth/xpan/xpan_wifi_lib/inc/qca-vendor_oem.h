/**
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef QCA_VENDOR_OEM_H
#define QCA_VENDOR_OEM_H

#define SIZEOF_TLV_HDR 4

/**
 *@QCA_NL80211_VENDOR_SUBCMD_AUDIO_TRANSPORT_SWITCH: This vendor subcommand is
 *used to configure and indicate the audio transport switch in both
 *command and event paths. This is used when two or more audio transports
 *(ex: WLAN and bluetooth) are available between peers.
 *
 *If the driver needs to perform operations like scan, connection,
 *roaming, RoC etc. and AP concurrency policy is set to either
 *QCA_WLAN_CONCURRENT_AP_POLICY_GAMING_AUDIO or
 *QCA_WLAN_CONCURRENT_AP_POLICY_LOSSLESS_AUDIO_STREAMING, the driver sends
 *audio transport switch event to userspace. Userspace application upon
 *receiving the event, can try to switch to requested audio transport.
 *The userspace uses this command to send the status of transport
 *switching (either confirm or reject) to the driver using this
 *subcommand. The driver continues with the pending operation either upon
 *receiving the command from userspace or after waiting for timeout since
 *sending the event to userspace. The driver can request userspace to
 *switch to WLAN upon availability of WLAN audio transport once after the
 *concurrent operations are completed.
 *
 *Userspace can also request audio transport switch from non-WLAN to WLAN
 *using this subcommand to the driver. The driver can accept or reject
 *depending on other concurrent operations in progress. The driver returns
 *success if it can allow audio transport when it receives the command or
 *appropriate kernel error code otherwise. Userspace indicates the audio
 *transport switch from WLAN to non-WLAN using this subcommand and the
 *driver can do other concurrent operations without needing to send any
 *event to userspace. This subcommand is used by userspace only when the
 *driver advertises support for
 *QCA_WLAN_VENDOR_FEATURE_ENHANCED_AUDIO_EXPERIENCE_OVER_WLAN.
 *
 *The attributes used with this command are defined in enum
 *qca_wlan_vendor_attr_audio_transport_switch.
 */
#define QCA_NL80211_VENDOR_SUBCMD_AUDIO_TRANSPORT_SWITCH 232

/**
 * enum qca_wlan_audio_transport_switch_type - Represents the possible transport
 * switch types.
 *
 * @QCA_WLAN_AUDIO_TRANSPORT_SWITCH_TYPE_NON_WLAN: Request to route audio data
 * via non-WLAN transport (ex: bluetooth).
 *
 * @QCA_WLAN_AUDIO_TRANSPORT_SWITCH_TYPE_WLAN: Request to route audio data via
 * WLAN transport.
 */
enum qca_wlan_audio_transport_switch_type {
	QCA_WLAN_AUDIO_TRANSPORT_SWITCH_TYPE_NON_WLAN = 0,
	QCA_WLAN_AUDIO_TRANSPORT_SWITCH_TYPE_WLAN = 1,
};

/**
 * enum qca_wlan_audio_transport_switch_status - Represents the status of audio
 * transport switch request.
 *
 * @QCA_WLAN_AUDIO_TRANSPORT_SWITCH_STATUS_REJECTED: Request to switch transport
 * has been rejected. For ex: when transport switch is requested from WLAN
 * to non-WLAN transport, user space modules and peers would evaluate the switch
 * request and may not be ready for switch and hence switch to non-WLAN transport
 * gets rejected.
 *
 * @QCA_WLAN_AUDIO_TRANSPORT_SWITCH_STATUS_COMPLETED: Request to switch transport
 * has been completed. This is sent only in command path. Ex: when host driver
 * had requested for audio transport switch and userspace modules as well as
 * peers are ready for the switch, userspace module switches the transport and
 * sends subcommand with status completed to host driver.
 */
enum qca_wlan_audio_transport_switch_status {
	QCA_WLAN_AUDIO_TRANSPORT_SWITCH_STATUS_REJECTED = 0,
	QCA_WLAN_AUDIO_TRANSPORT_SWITCH_STATUS_COMPLETED = 1,
};

/**
 * enum qca_wlan_vendor_attr_audio_transport_switch - Attributes used by
 * %QCA_NL80211_VENDOR_SUBCMD_AUDIO_TRANSPORT_SWITCH vendor command.
 *
 * @QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_TYPE: u8 attribute. Indicates
 * the transport switch type from one of the values in enum
 * qca_wlan_audio_transport_switch_type. This is mandatory param in both
 * command and event path. This attribute is included in both requests and
 * responses.
 *
 * @QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_STATUS: u8 attribute. Indicates
 * the transport switch status from one of the values in enum
 * qca_wlan_audio_transport_switch_status. This is optional param and used in
 * both command and event path. This attribute must not be included in requests.
 */
enum qca_wlan_vendor_attr_audio_transport_switch {
	QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_INVALID = 0,
	QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_TYPE = 1,
	QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_STATUS = 2,

	/* keep last */
	QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_AFTER_LAST,
	QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_MAX =
	QCA_WLAN_VENDOR_ATTR_AUDIO_TRANSPORT_SWITCH_AFTER_LAST - 1,
};

/**
 * @QCA_NL80211_VENDOR_SUBCMD_HIGH_AP_AVAILABILITY: This vendor subcommand is
 *	used to configure AP to allow quick discovery by peer STA and attempt
 *	connection when AP is in MCC mode with other concurrent interfaces.
 *	AP starts frequent beaconing while waiting on same channel for
 *	configured duration.
 *
 *	The attributes used with this subcommand are defined in
 *	enum qca_wlan_vendor_attr_high_ap_availability.
 */
#define QCA_NL80211_VENDOR_SUBCMD_HIGH_AP_AVAILABILITY 234

/**
 * enum qca_high_ap_availability_operation: Represents the possible values for
 * %QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_OPERATION attribute.
 *
 * @QCA_HIGH_AP_AVAILABILITY_OPERATION_REQUEST: Userspace requests AP for high
 * availability mode. Host driver provides cookie id
 * %QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_COOKIE in response.
 *
 * @QCA_HIGH_AP_AVAILABILITY_OPERATION_CANCEL: Userspace triggers cancel
 * ongoing %QCA_NL80211_VENDOR_SUBCMD_HIGH_AP_AVAILABILITY operation. A valid
 * cookie id returned from %QCA_HIGH_AP_AVAILABILITY_OPERATION_REQUEST is used
 * with %QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_COOKIE attribute. On
 * cancel completion, host driver exits high availability operation and notifies
 * userspace with %QCA_HIGH_AP_AVAILABILITY_OPERATION_COMPLETED.
 *
 * @QCA_HIGH_AP_AVAILABILITY_OPERATION_STARTED: Host driver notifies userspace
 * when AP starts high availability operation. A valid cookie id returned from
 * %QCA_HIGH_AP_AVAILABILITY_OPERATION_REQUEST is used with
 * %QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_COOKIE attribute.
 *
 * @QCA_HIGH_AP_AVAILABILITY_OPERATION_COMPLETED: Host driver notifies
 * userspace when AP exits/completed high availability operation. A valid cookie
 * id returned from %QCA_HIGH_AP_AVAILABILITY_OPERATION_REQUEST is used with
 * %QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_COOKIE attribute.
 *
 * @QCA_HIGH_AP_AVAILABILITY_OPERATION_CANCELLED: Host driver notifies
 * userspace when host driver cancel high availability operation early (without
 * userspace request for cancel). A valid cookie id returned from
 * %QCA_HIGH_AP_AVAILABILITY_OPERATION_REQUEST is used with
 * %QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_COOKIE attribute.
 */
enum qca_high_ap_availability_operation {
	QCA_HIGH_AP_AVAILABILITY_OPERATION_REQUEST = 0,
	QCA_HIGH_AP_AVAILABILITY_OPERATION_CANCEL = 1,
	QCA_HIGH_AP_AVAILABILITY_OPERATION_STARTED = 2,
	QCA_HIGH_AP_AVAILABILITY_OPERATION_COMPLETED = 3,
	QCA_HIGH_AP_AVAILABILITY_OPERATION_CANCELLED = 4,
};

/**
 * enum qca_wlan_vendor_attr_high_ap_availability - Definition of attributes
 * used by %QCA_NL80211_VENDOR_SUBCMD_HIGH_AP_AVAILABILITY to configure AP to
 * allow quick discovery by peer STA and attempt connection.
 *
 * @QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_OPERATION: u8 attribute, used
 * to request/notify possible operation type defined in enum
 * qca_high_ap_availability_operation.
 *
 * @QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_COOKIE: u16 attribute returned
 * with %QCA_HIGH_AP_AVAILABILITY_OPERATION_REQUEST as unique id. This is used
 * with all subsequent %QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_OPERATION.
 *
 * @QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_DURATION: u32 attribute for max
 * duration (in ms) to allow peer STA attempt discovery and connection.
 */
enum qca_wlan_vendor_attr_high_ap_availability {
	QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_INVALID = 0,
	QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_OPERATION = 1,
	QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_COOKIE = 2,
	QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_DURATION = 3,

	/* keep last */
	QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_AFTER_LAST,
	QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_MAX =
	QCA_WLAN_VENDOR_ATTR_HIGH_AP_AVAILABILITY_AFTER_LAST - 1,
};

struct resp_info {
	u32 subcmd;
	char *reply_buf;
	int reply_buf_len;
	u32 freq;
	u32 ncho_mode_value;
	u32 cmd_type_get_ncho_mode;
	uint8_t country[4];
	bool cb_success;
};

struct oem_data_tlv {
	u16 type;
	u16 length;
	u8 *value;
};



enum oem_data_tlv_type {
	OEM_DATA_TLV_TYPE_INVALID = 0,
	OEM_DATA_TLV_TYPE_HEADER = 1,
	OEM_DATA_TLV_TYPE_IPS = 2,
	OEM_DATA_TLV_TYPE_PHASE = 3,
	OEM_DATA_TLV_TYPE_NPK_TIME = 4,
	OEM_DATA_TLV_TYPE_CHK_STATS = 5,
	OEM_DATA_TLV_TYPE_ENABLE = 6,
	OEM_DATA_TLV_TYPE_BW = 7,

	OEM_DATA_TLV_POWER_STATE = 19,
	OEM_DATA_TLV_SLEEP_TIME = 20,
	OEM_DATA_TLV_TIME_SINCE_ENABLE = 21,
	OEM_DATA_TLV_SCAN_CONNECTED_CHANNEL_DUR = 23,
	OEM_DATA_TLV_SCAN_SINGLE_OFF_CHANNEL_DUR = 24,
	OEM_DATA_TLV_SCAN_TOTAL_OFF_CHANNEL_DUR = 25,
	OEM_DATA_TLV_STATUS = 26,
	OEM_DATA_TLV_GET_CU = 27,
	OEM_DATA_TLV_GET_CU_ABSOLUTE = 28,

	OEM_DATA_TLV_UWB_COEX_ENABLE = 29,
	OEM_DATA_TLV_UWB_START_FREQ = 30,
	OEM_DATA_TLV_UWB_END_FREQ = 31,
	OEM_DATA_TLV_UWB_PREPARE_TIME = 32,
#ifdef CONFIG_SARV2
	OEM_DATA_TLV_SUB6_BAND_INFO = 33,
#endif

	/* timeout in seconds (u32) */
	OEM_DATA_TLV_TYPE_TIMEOUT = 34,

	/* wakeup type (u8)
	 *  0 : ALL Broadcast packets(default)
	 *  1 : ALL Multicast packets / ALL Broadcast packets
	 *  2 : Specific Unicast packets / ALL Multicast packets / ALL Broadcast packets
	 *  3 : ALL Unicast packets / ALL Multicast packets / ALL Broadcast packets
	 */
	OEM_DATA_TLV_DELAYED_WAKEUP_TYPE = 35,

	/* IP list to be used when OEM_DATA_TLV_DELAYED_WAKEUP_TYPE=2 */
	OEM_DATA_TLV_DELAYED_WAKEUP_IP_LIST = 36,

	/* bssid list to be used when OEM_DATA_TLV_DELAYED_WAKEUP_TYPE=2 */
	OEM_DATA_TLV_DELAYED_WAKEUP_BSSID_LIST = 37,

	/*interval to be configured for enabling xpan stats */
	OEM_DATA_TLV_TYPE_XPAN_STATS_INTERVAL = 38,

	/* Parameter options required to set up a new PM schedule */
	OEM_DATA_TLV_SCHED_PM_OPTIONS = 61,

	/* Current status of PM schedule */
	OEM_DATA_TLV_SCHED_PM_STATUS_DATA = 62,

	/* Event and reason code received for PM teardown event from firmware */
	OEM_DATA_TLV_SCHED_PM_FW_TEARDOWN = 63,

	OEM_DATA_TLV_TYPE_XPAN_USECASE = 72,
	OEM_DATA_TLV_TYPE_XPAN_VBC_SI = 73,
	OEM_DATA_TLV_TYPE_XPAN_RIGHT_EARBUD_OFFSET = 74,
	OEM_DATA_TLV_TYPE_XPAN_LEFT_EARBUD_MAC = 75,
	OEM_DATA_TLV_TYPE_XPAN_RIGHT_EARBUD_MAC = 76,
	OEM_DATA_TLV_TYPE_XPAN_EB_PAIR_INDEX = 109,

	OEM_DATA_TLV_TYPE_LAST,
	OEM_DATA_TLV_TYPE_MAX =
	OEM_DATA_TLV_TYPE_LAST - 1
};

enum oem_data_cmd {
	OEM_CMD_INVALID = 0,

	OEM_CMD_SET_AP_RPS_PARAMS = 1,
	OEM_CMD_SET_AP_RPS = 2,
	OEM_CMD_SET_AP_SUSPEND = 3,
	OEM_CMD_SET_AP_BW = 4,
	OEM_CMD_GET_AP_RPS = 10,
	OEM_CMD_SET_SCAN_PARAM = 12,
	OEM_CMD_GET_CU = 13,
	OEM_CMD_SET_UWBCX_ENABLE = 14,
	OEM_CMD_GET_UWBCX_ENABLE = 15,
	OEM_CMD_SET_UWBCX_PREPARE_TIME = 16,
	OEM_CMD_GET_UWBCX_PREPARE_TIME = 17,
#ifdef CONFIG_SARV2
	OEM_CMD_GET_SUB6_BAND = 19,
#endif

	OEM_CMD_SET_DELAYED_WAKEUP = 20,
	OEM_CMD_SET_DELAYED_WAKEUP_TYPE = 21,
	OEM_CMD_ENABLE_STATS = 22,
	OEM_CMD_SCHED_PM_SETUP = 31,
	OEM_CMD_SCHED_PM_TEARDOWN = 32,
	OEM_CMD_GET_SCHED_PM_STATUS = 33,
	OEM_CMD_SET_USECASE_PARAMS = 42,
	OEM_CMD_SET_XPAN_EB_PAIR_AUDIO_LOCATION = 61,

	OEM_CMD_LAST,
	OEM_CMD_MAX =
	OEM_CMD_LAST - 1
};

struct oem_data_header {
	u16 cmd_id;
	u16 request_idx;
};

#endif /* QCA_VENDOR_OEM_H */
