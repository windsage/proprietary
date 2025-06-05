/*
 * Copyright (c) 2016, 2017, 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _CFG80211_NLWRAPPER_H__
#define _CFG80211_NLWRAPPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

#define MAX_COMMAND_STR_LEN 600
#define MAX_COMMAND_RESULT_LEN 100
#define IF_NAME_SIZE 16

#define GET_VAP_IFACE_NAME_CMD "ucitool get_iface %s"

#define MCC_VAP_NAME_PREFIX "wlan"
#define MCC_VAP_NAME_PREFIX_SIZE 4

enum wlan_chip_type {
	WLAN_CHIP_TYPE_MCC,
	WLAN_CHIP_TYPE_WIN,
	WLAN_CHIP_TYPE_UNKNOWN,
};

typedef struct wifi_cfg80211_t {
	/* command socket object */
	struct nl_sock *cmd_sock;
	struct nl_sock *event_sock;
	pthread_mutex_t cmd_sock_lock;
	/* private command socket ids*/
	int pvt_cmd_sock_id;
	int pvt_event_sock_id;
	/* family id for 80211 driver */
	int nl80211_family_id;
	void (*event_callback)(char *ifname, uint32_t subcmd, uint8_t *data,
			size_t len);
	void (*event_callback_afc_vendor_cmd)(char *ifname, uint32_t subcmd, uint8_t *data,
			size_t len, int wiphy_idx);
	pthread_t event_thread_handle;
	volatile int event_thread_running;
	enum wlan_chip_type chip_type;
} wifi_cfg80211_context;

/* cfg80211 nlwrapper context that needs to be paseed when sending command */
struct cfg80211_data {

	void *data; /* data pointer */
	void *nl_vendordata; /* vendor data */
	unsigned int nl_vendordata_len; /* vendor data length */
	unsigned int length; /* data length */
	unsigned int flags; /* flags for data */
	unsigned int parse_data; /* 1 - data parsed by caller 0- data parsed by wrapper */
	/* callback that needs to be called when data recevied from driver */
	void (*callback) (struct cfg80211_data *);
};

#define NL80211_ATTR_32BIT 0x00000001
#define NL80211_ATTR_8BIT  0x00000010
/* nlwrapper specific data that needs to be passed when using std command */
struct nlwrapper_data {
	unsigned int cmd;        /* NL80211 cmd */
	unsigned int attr;       /* NL80211 attr */
	unsigned int value;      /* attr value */
	unsigned int flags;      /* flags */
};

/* QCA reserved enum are used as optional param attributes */
#define MAX_OPTIONAL_PARAMS 13
 /**
 * struct  optional_params: structure to hold optional params
 * @num_params: Total number of params.
 * @attr_num: enum value from qca_nl80211_vendor_subcmds.
 * @attr_value: value of the optional param.
 *
 */
struct optional_params {
	int num_params;
	uint32_t attr_num[MAX_OPTIONAL_PARAMS];
	uint32_t attr_value[MAX_OPTIONAL_PARAMS];
};

#define MLO_INVALID_LINK_ID 0xff
/**
 * wifi_init_nl80211: initiliaze nlsocket
 * @ctx: wifi cfg80211 context
 *
 * return 1/0
 */
int wifi_init_nl80211(wifi_cfg80211_context *ctx);

/**
 * wifi_init_nl80211_largebuf: Generalized procedure to initialize nlsocket
 * @ctx: wifi cfg80211 context
 * @largebuf: whether large socket buffer is needed.
 *
 * return 1/0
 */
int wifi_init_nl80211_largebuf(wifi_cfg80211_context *ctx, uint8_t largebuf);

/**
 * wifi_nl80211_start_event_thread: Start the thread which processes
 *                                  the async netlink events
 * @ctx: wifi cfg80211 context
 *
 * return 1/0
 */
int wifi_nl80211_start_event_thread(wifi_cfg80211_context *ctx);

/**
 * wifi_destroy_nl80211: destroy nl80211 socket
 * @ctx: wifi cfg80211 context
 *
 * return 1/0
 */
void wifi_destroy_nl80211(wifi_cfg80211_context *ctx);

/**
 * wifi_cfg80211_send_getparam_command_linkid: sends getparm command
 * @ctx: pointer to wifi_cfg80211_context
 * @cmdid: command id
 * @param: param (enum) for which value need to be retrived.
 * @ifname: interface name
 * @link :link id of MLD netdev
 * @buffer: buffer data
 * @len: length
 *
 * return NL state.
 */

int wifi_cfg80211_send_getparam_command_linkid(wifi_cfg80211_context *ctx, int cmdid,
        int param, const char *ifname, uint8_t link, char *buffer, int len);

/**
 * wifi_cfg80211_send_getparam_command: sends getparm command
 * @ctx: pointer to wifi_cfg80211_context
 * @cmdid: command id
 * @param: param (enum) for which value need to be retrived.
 * @ifname: interface name
 * @buffer: buffer data
 * @len: length
 *
 * return NL state.
 */
int wifi_cfg80211_send_getparam_command(wifi_cfg80211_context *ctx, int cmdid,
        int param, const char *ifname, char *buffer, int len);

/**
 * wifi_cfg80211_send_setparam_command_linkid: sends setparm command
 * @ctx: pointer to wifi_cfg80211_context
 * @cmdid: command id
 * @param: param (enum) for which value need to be set.
 * @ifname: interface name
 * @link :link id of MLD netdev
 * @buffer: buffer data
 * @len: length attribute
 * @flags: flags attribute
 *
 * return NL state.
 */

int wifi_cfg80211_send_setparam_command_linkid(wifi_cfg80211_context *ctx, int cmdid,
        int param, const char *ifname, uint8_t link, char *buffer,
		int param_attribute_length, unsigned int param_attribute_flags);

/**
 * wifi_cfg80211_send_setparam_command: sends setparm command
 * @ctx: pointer to wifi_cfg80211_context
 * @cmdid: command id
 * @param: param (enum) for which value need to be set.
 * @ifname: interface name
 * @buffer: buffer data
 * @len: length attribute
 * @flags: flags attribute
 *
 * return NL state.
 */

int wifi_cfg80211_send_setparam_command(wifi_cfg80211_context *ctx, int cmdid,
        int param, const char *ifname, char *buffer,
		int param_attribute_length, unsigned int param_attribute_flags);

/**
 * wifi_cfg80211_sendcmd: sends cfg80211 sendcmd
 * @ctx: pointer to wifi_cfg80211_context
 * @cmdid: command id
 * @ifname: interface name
 * @buffer: buffer data
 * @len: length
 *
 * return NL state.
 */
int wifi_cfg80211_sendcmd(wifi_cfg80211_context *ctx, int cmdid, const char *ifname,
		char *buffer, int len);

/**
 * wifi_cfg80211_user_send_geneic_command: sends cfg80211 sendcmd.
 * @ctx: pointer to wifi_cfg80211_context.
 * @vendor_command: vendor command.
 * @cmdid : internal command id.
 * @value : Data to fill in "value" filed of NL message.
 * @ifname: interface name.
 * @link :link id of MLD netdev
 * @buffer: Buffer to fill in "data" filed of NL message.
 * @len    : Length to fill in "length" filed of NL message.
 * return NL state.
 */
#ifdef QCANL_LIB_ADD_API_SUPPORT_SINGLE_WIPHY_MLO
int wifi_cfg80211_user_send_generic_command(wifi_cfg80211_context *ctx,
					    int vendor_command, int cmdid,
					    int value, const char *ifname, uint8_t link,
					    char *buffer, uint32_t len);
#else
int wifi_cfg80211_user_send_generic_command(wifi_cfg80211_context *ctx,
					    int vendor_command, int cmdid,
					    int value, const char *ifname,
					    char *buffer, uint32_t len);
#endif

#ifdef QCANL_LIB_ADD_API_SUPPORT_SINGLE_WIPHY_MLO
int wifi_cfg80211_send_mctbl(wifi_cfg80211_context *ctx, int vendor_command,
		int cmdid, const char *ifname, uint8_t link, char *buffer, int size);
#else
int wifi_cfg80211_send_mctbl(wifi_cfg80211_context *ctx, int vendor_command,
		int cmdid, const char *ifname, char *buffer, int size);
#endif

/**
 * wifi_cfg80211_send_generic_command_optional_params: sends cfg80211 generic
 * commands with optional params
 * @ctx: pointer to wifi_cfg80211_context
 * @vendor_command: vendor command
 * @cmdid: internal command id
 * @ifname: interface name
 * @link :link id of MLD netdev
 * @buffer: data buffer
 * @len: length of buffer
 * @opt_params: pointer to optional params
 *
 * return NL state.
 */
#ifdef QCANL_LIB_ADD_API_SUPPORT_SINGLE_WIPHY_MLO
int wifi_cfg80211_send_generic_command_optional_params(
		wifi_cfg80211_context *ctx, int vendor_command, int cmdid,
		const char *ifname, uint8_t link, char *buffer, int len,
		struct optional_params *opt_params);
#else
int wifi_cfg80211_send_generic_command_optional_params(
		wifi_cfg80211_context *ctx, int vendor_command, int cmdid,
		const char *ifname, char *buffer, int len,
		struct optional_params *opt_params);
#endif

/**
 * wifi_cfg80211_send_setparam_command_optional_params_linkid: sends set param
 * commands with optional params
 * @ctx: pointer to wifi_cfg80211_context
 * @cmdid: internal command id
 * @ifname: interface name
 * @link :link id of MLD netdev
 * @buffer: data buffer
 * @param_attribute_length: length of buffer
 * @param_attribute_flags: flags attributes
 * @opt_params: pointer to optional params
 *
 * return NL state.
 */

int wifi_cfg80211_send_setparam_command_optional_params_linkid(
		wifi_cfg80211_context *ctx, int cmdid,
		int param, const char *ifname, uint8_t link, char *buffer,
		int param_attribute_length, unsigned int param_attribute_flags,
		struct optional_params *opt_params);

/**
 * wifi_cfg80211_send_setparam_command_optional_params: sends set param
 * commands with optional params
 * @ctx: pointer to wifi_cfg80211_context
 * @cmdid: internal command id
 * @ifname: interface name
 * @buffer: data buffer
 * @param_attribute_length: length of buffer
 * @param_attribute_flags: flags attributes
 * @opt_params: pointer to optional params
 *
 * return NL state.
 */

int wifi_cfg80211_send_setparam_command_optional_params(
		wifi_cfg80211_context *ctx, int cmdid,
		int param, const char *ifname, char *buffer,
		int param_attribute_length, unsigned int param_attribute_flags,
		struct optional_params *opt_params);

/**
 * wifi_cfg80211_send_geneic_command: sends cfg80211 sendcmd
 * @ctx: pointer to wifi_cfg80211_context
 * @vendor_command: vendor command
 * @cmdid: internal command id
 * @ifname: interface name
 * @link :link id of MLD netdev
 *
 * return NL state.
 */
#ifdef QCANL_LIB_SUPPORT_SINGLE_WIPHY_MLO
int wifi_cfg80211_send_generic_command(wifi_cfg80211_context *ctx, int vendor_command, int cmdid, const char *ifname, uint8_t link, char *buffer, int len);
#else
int wifi_cfg80211_send_generic_command(wifi_cfg80211_context *ctx, int vendor_command, int cmdid, const char *ifname, char *buffer, int len);
#endif
/**
 * wifi_cfg80211_prepare_command: prepare cfg80211 command and return nl_msg to called
 * caller need to populate data.
 * @ctx: pointer to wifi_cfg80211_context
 * @cmdid: command id
 * @ifname: interface name
 *
 * return nl_msg pointer
 */

struct nl_msg *wifi_cfg80211_prepare_command(wifi_cfg80211_context *ctx, int cmdid, const char *ifname);

/**
 * send_nlmsg: send nlmsg to kernel.
 * caller need to populate data.
 * @ctx: pointer to wifi_cfg80211_context
 * @nlmsg: pointer to nl message.
 * @date: pointer to data
 *
 * return nl_msg pointer
 */

int send_nlmsg(wifi_cfg80211_context *ctx, struct nl_msg *nlmsg, void *data);

/**
 * recv_nlmsg_nb: receive non blocking message on event socket
 * @ctx: pointer to wifi_cfg80211_context
 *
 * return 0 on success or a negative error code
 */

int recv_nlmsg_nb(wifi_cfg80211_context *ctx);


struct nlattr *start_vendor_data(struct nl_msg *nlmsg);
void end_vendor_data(struct nl_msg *nlmsg, struct nlattr *attr);

 /**
 * wifi_cfg80211_send_nl80211_standard_command: send cfg80211 send standard cmd
 * @ctx: pointer to wifi_cfg80211_context
 * @wifi_name: wifi interface name
 * @vap_name: vap interface name
 * @buffer: nlwrapper data buffer
 *
 * return NL state.
 */

int wifi_cfg80211_send_nl80211_standard_command(wifi_cfg80211_context *ctx,
	uint8_t *wifi_name,uint8_t *vap_name, char *buffer);

 /**
 * execute_system_cmd: execute system command and return the output of the command
 *
 * @cmd: system command
 * @result: output of the command
 * @result_len: output length of the command
 *
 * return 0 on success; otherwise -1
 */
int execute_system_cmd(const char *cmd, char *result, int result_len);

 /**
 * get_vap_name_from_radio_name: find the vap name of the given radio name
 *
 * @radio_name: radio name
 * @vap_name: vap name
 *
 * return 0 on success; otherwise -1
 */
int get_vap_name_from_radio_name(const char *radio_name, char *vap_name);

 /**
 * get_wlan_chip_type: get wlan chip type
 *
 * return wlan chip type
 */
enum wlan_chip_type get_wlan_chip_type();

#ifdef __cplusplus
}
#endif
#endif
