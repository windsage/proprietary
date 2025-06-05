/* Copyright (c) 2020 Wi-Fi Alliance                                                */

/* Permission to use, copy, modify, and/or distribute this software for any         */
/* purpose with or without fee is hereby granted, provided that the above           */
/* copyright notice and this permission notice appear in all copies.                */

/* THE SOFTWARE IS PROVIDED 'AS IS' AND THE AUTHOR DISCLAIMS ALL                    */
/* WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                    */
/* WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL                     */
/* THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR                       */
/* CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING                        */
/* FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF                       */
/* CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT                       */
/* OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS                          */
/* SOFTWARE. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include "indigo_api.h"
#include "vendor_specific.h"
#include "utils.h"
#include "wpa_ctrl.h"
#include "indigo_api_callback.h"
#include "hs2_profile.h"
#ifdef _OPENWRT_QTI_
#include "vendor_specific_openwrt.h"
#endif

struct tlv_to_config_name maps[] = {
    /* hapds */
    { TLV_SSID, "ssid", 0 },
    { TLV_CHANNEL, "channel", 0 },
    { TLV_WEP_KEY0, "wep_key0", 0 },
    { TLV_HW_MODE, "hw_mode", 0 },
    { TLV_INTERFACE_NAME, "interface_name", 0 },
    { TLV_AUTH_ALGORITHM, "auth_algs", 0 },
    { TLV_WEP_DEFAULT_KEY, "wep_default_key", 0 },
    { TLV_IEEE80211_D, "ieee80211d", 0 },
    { TLV_IEEE80211_N, "ieee80211n", 0 },
    { TLV_IEEE80211_AC, "ieee80211ac", 0 },
    { TLV_COUNTRY_CODE, "country_code", 0 },
    { TLV_WMM_ENABLED, "wmm_enabled", 0 },
    { TLV_WPA, "wpa", 0 },
    { TLV_WPA_KEY_MGMT, "wpa_key_mgmt", 0 },
    { TLV_RSN_PAIRWISE, "rsn_pairwise", 0 },
    { TLV_WPA_PASSPHRASE, "wpa_passphrase", 0 },
    { TLV_WPA_PAIRWISE, "wpa_pairwise", 0 },
    { TLV_HT_CAPB, "ht_capab", 0 },
    { TLV_IEEE80211_W, "ieee80211w", 0 },
    { TLV_IEEE80211_H, "ieee80211h", 0 },
    { TLV_VHT_OPER_CHWIDTH, "vht_oper_chwidth", 0 },
    { TLV_VHT_OPER_CENTR_FREQ, "vht_oper_centr_freq_seg0_idx", 0 },
    { TLV_VHT_CAPB, "vht_capab", 0 },
    { TLV_IEEE8021_X, "ieee8021x", 0 },
    { TLV_EAP_SERVER, "eap_server", 0 },
    { TLV_AUTH_SERVER_ADDR, "auth_server_addr", 0 },
    { TLV_AUTH_SERVER_PORT, "auth_server_port", 0 },
    { TLV_AUTH_SERVER_SHARED_SECRET, "auth_server_shared_secret", 0 },
    { TLV_IE_OVERRIDE, "own_ie_override", 0 }, // HostAPD Python Interface
    { TLV_SAE_ANTI_CLOGGING_THRESHOLD, "sae_anti_clogging_threshold", 0 }, // HostAPD Python Interface
    { TLV_DISABLE_PMKSA_CACHING, "disable_pmksa_caching", 0 },  // HostAPD Python Interface
    { TLV_SAE_GROUPS, "sae_groups", 0 },
    { TLV_IEEE80211_AX, "ieee80211ax", 0 },
    { TLV_HE_OPER_CHWIDTH, "he_oper_chwidth", 0 },
    { TLV_HE_OPER_CENTR_FREQ, "he_oper_centr_freq_seg0_idx", 0 },
    { TLV_MBO, "mbo", 0 },
    { TLV_MBO_CELL_DATA_CONN_PREF, "mbo_cell_data_conn_pref", 0 },
    { TLV_BSS_TRANSITION, "bss_transition", 0 },
    { TLV_INTERWORKING, "interworking", 0 },
    { TLV_RRM_NEIGHBOR_REPORT, "rrm_neighbor_report", 0 },
    { TLV_RRM_BEACON_REPORT, "rrm_beacon_report", 0 },
    { TLV_COUNTRY3, "country3", 0 },
    { TLV_MBO_CELL_CAPA, "mbo_cell_capa", 0 },
    { TLV_MBO_ASSOC_DISALLOW, "mbo_assoc_disallow", 0 },
    { TLV_GAS_COMEBACK_DELAY, "gas_comeback_delay", 0 },
    { TLV_SAE_PWE, "sae_pwe", 0 },
    { TLV_OWE_GROUPS, "owe_groups", 0 },
    { TLV_HE_MU_EDCA, "he_mu_edca_qos_info_param_count", 0 },
    { TLV_TRANSITION_DISABLE, "transition_disable", 0 },
    { TLV_CONTROL_INTERFACE, "ctrl_interface", 0 },
    { TLV_RSNXE_OVERRIDE_EAPOL, "rsnxe_override_eapol", 0 },
    { TLV_SAE_CONFIRM_IMMEDIATE, "sae_confirm_immediate", 0 },
    { TLV_OWE_TRANSITION_BSS_IDENTIFIER, "owe_transition_ifname", 0 },
    { TLV_OP_CLASS, "op_class", 0 },
    { TLV_HE_UNSOL_PR_RESP_CADENCE, "unsol_bcast_probe_resp_interval", 0 },
    { TLV_HE_FILS_DISCOVERY_TX, "fils_discovery_max_interval", 0 },
    { TLV_SKIP_6G_BSS_SECURITY_CHECK, "skip_6g_bss_security_check", 0 },
    { TLV_HS20, "hs20", 0 },
    { TLV_ACCESS_NETWORK_TYPE, "access_network_type", 0 },
    { TLV_INTERNET, "internet", 0 },
    { TLV_VENUE_GROUP, "venue_group", 0 },
    { TLV_VENUE_TYPE, "venue_type", 0 },
    { TLV_HESSID, "hessid", 0 },
    { TLV_ANQP_3GPP_CELL_NETWORK_INFO, "anqp_3gpp_cell_net", 0 },
    { TLV_OSU_SSID, "osu_ssid", 0 },
    { TLV_PROXY_ARP, "proxy_arp", 0 },
    { TLV_OSU_SERVER_URI, "osu_server_uri", 0 },
    { TLV_OSU_METHOD, "osu_method_list", 0 },
    { TLV_DOMAIN_LIST, "domain_name", 0 },
    { TLV_IGNORE_BROADCAST_SSID, "ignore_broadcast_ssid", 0 },
    { TLV_MANAGE_P2P, "manage_p2p", 0 },
    { TLV_WPS_INDEPENDENT, "wps_independent", 0 },
    { TLV_LOCAL_PWR_CONST, "local_pwr_constraint", 0 },
    { TLV_SPECTRUM_MGMT_REQ, "spectrum_mgmt_required", 0 },

    /* wpas, seperate? */
    { TLV_STA_SSID, "ssid", 1 },
    { TLV_KEY_MGMT, "key_mgmt", 0 },
    { TLV_STA_WEP_KEY0, "wep_key0", 0 },
    { TLV_WEP_TX_KEYIDX, "wep_tx_keyidx", 0 },
    { TLV_GROUP, "group", 0 },
    { TLV_PSK, "psk", 1 },
    { TLV_PROTO, "proto", 0 },
    { TLV_STA_IEEE80211_W, "ieee80211w", 0 },
    { TLV_PAIRWISE, "pairwise", 0 },
    { TLV_EAP, "eap", 0 },
    { TLV_PHASE1, "phase1", 1 },
    { TLV_PHASE2, "phase2", 1 },
    { TLV_IDENTITY, "identity", 1 },
    { TLV_PASSWORD, "password", 1 },
    { TLV_CA_CERT, "ca_cert", 1 },
    { TLV_SERVER_CERT, "ca_cert", 1 },
    { TLV_PRIVATE_KEY, "private_key", 1 },
    { TLV_CLIENT_CERT, "client_cert", 1 },
    { TLV_DOMAIN_MATCH, "domain_match", 1 },
    { TLV_DOMAIN_SUFFIX_MATCH, "domain_suffix_match", 1 },
    { TLV_PAC_FILE, "pac_file", 1 },
    { TLV_STA_OWE_GROUP, "owe_group", 0 },
    { TLV_HE_MU_EDCA, "he_mu_edca_qos_info_param_count", 0 },
    { TLV_TRANSITION_DISABLE, "transition_disable", 0 },
    { TLV_CONTROL_INTERFACE, "ctrl_interface", 0 },
    { TLV_RSNXE_OVERRIDE_EAPOL, "rsnxe_override_eapol", 0 },
    { TLV_SAE_CONFIRM_IMMEDIATE, "sae_confirm_immediate", 0 },
    { TLV_OWE_TRANSITION_BSS_IDENTIFIER, "owe_transition_ifname", 0 },
    { TLV_OP_CLASS, "op_class", 0 },
    { TLV_HE_UNSOL_PR_RESP_CADENCE, "unsol_bcast_probe_resp_interval", 0 },
    { TLV_HE_FILS_DISCOVERY_TX, "fils_discovery_max_interval", 0 },
    { TLV_SKIP_6G_BSS_SECURITY_CHECK, "skip_6g_bss_security_check", 0 },
    { TLV_HS20, "hs20", 0 },
    { TLV_ACCESS_NETWORK_TYPE, "access_network_type", 0 },
    { TLV_INTERNET, "internet", 0 },
    { TLV_VENUE_GROUP, "venue_group", 0 },
    { TLV_VENUE_TYPE, "venue_type", 0 },
    { TLV_HESSID, "hessid", 0 },
    { TLV_ANQP_3GPP_CELL_NETWORK_INFO, "anqp_3gpp_cell_net", 0 },
    { TLV_OSU_SSID, "osu_ssid", 0 },
    { TLV_PROXY_ARP, "proxy_arp", 0 },
    { TLV_OSU_SERVER_URI, "osu_server_uri", 0 },
    { TLV_OSU_METHOD, "osu_method_list", 0 },
    { TLV_DOMAIN_LIST, "domain_name", 0 },
    { TLV_IGNORE_BROADCAST_SSID, "ignore_broadcast_ssid", 0 },
    { TLV_MANAGE_P2P, "manage_p2p", 0 },
    { TLV_WPS_INDEPENDENT, "wps_independent", 0 },
    { TLV_LOCAL_PWR_CONST, "local_pwr_constraint", 0 },
    { TLV_SPECTRUM_MGMT_REQ, "spectrum_mgmt_required", 0 },

    /* wpas, seperate? */
    { TLV_STA_SSID, "ssid", 1 },
    { TLV_KEY_MGMT, "key_mgmt", 0 },
    { TLV_STA_WEP_KEY0, "wep_key0", 0 },
    { TLV_WEP_TX_KEYIDX, "wep_tx_keyidx", 0 },
    { TLV_GROUP, "group", 0 },
    { TLV_PSK, "psk", 1 },
    { TLV_PROTO, "proto", 0 },
    { TLV_STA_IEEE80211_W, "ieee80211w", 0 },
    { TLV_PAIRWISE, "pairwise", 0 },
    { TLV_EAP, "eap", 0 },
    { TLV_PHASE1, "phase1", 1 },
    { TLV_PHASE2, "phase2", 1 },
    { TLV_IDENTITY, "identity", 1 },
    { TLV_PASSWORD, "password", 1 },
    { TLV_CA_CERT, "ca_cert", 1 },
    { TLV_SERVER_CERT, "ca_cert", 1 },
    { TLV_PRIVATE_KEY, "private_key", 1 },
    { TLV_CLIENT_CERT, "client_cert", 1 },
    { TLV_DOMAIN_MATCH, "domain_match", 1 },
    { TLV_DOMAIN_SUFFIX_MATCH, "domain_suffix_match", 1 },
    { TLV_PAC_FILE, "pac_file", 1 },
    { TLV_STA_OWE_GROUP, "owe_group", 0 },
    { TLV_BSSID, "bssid", 0 },
    { TLV_REALM, "realm", 1 },
    { TLV_IMSI, "imsi", 1 },
    { TLV_MILENAGE, "milenage", 1 },
    { TLV_BSSID_FILTER_LIST, "bssid_filter", 0 },
    { TLV_USERNAME, "username", 1 },
    { TLV_HOME_FQDN, "domain", 1 },
    { TLV_PREFER, "priority", 0 },

    /* hapd + wpas */
    { TLV_EAP_FRAG_SIZE, "fragment_size", 0 },
};

struct tlv_to_config_name semicolon_list[] = {
    { TLV_ROAMING_CONSORTIUM, "roaming_consortium", 0 },
};

struct anqp_tlv_to_config_name anqp_maps[] = {
    { "NeighborReportReq", "272" },
    { "QueryListWithCellPref", "mbo:2" },
    { "ANQPCapaList", "257" },
    { "VenueNameInfo", "258" },
    { "NetworkAuthTypeInfo", "260" },
    { "RoamingConsortium", "261" },
    { "IPAddrTypeInfo", "262" },
    { "NAIRealm", "263" },
    { "3GPPCellNetwork", "264" },
    { "DomainName", "268" },
    { "VenueUrl", "277" },
    { "AdviceOfCharge", "278" },
    { "HSCapaList", "hs20:2" },
    { "OperFriendlyName", "hs20:3" },
    { "WANMetrics", "hs20:4" },
    { "ConnCapa", "hs20:5" },
    { "NAIHomeRealm", "hs20:6" },
    { "OperatingClass", "hs20:7" },
    { "OSUProvidersList", "hs20:8" },
    { "IconReq", "hs20:10" },
    { "IconBinaryFile", "hs20:11" },
    { "OperatorIcon", "hs20:12" },
    { "OSUProvidersNaiList", "hs20:13" },
};

char* find_tlv_config_name(int tlv_id) {
    int i;
    for (i = 0; i < sizeof(maps)/sizeof(struct tlv_to_config_name); i++) {
        if (tlv_id == maps[i].tlv_id) {
            return maps[i].config_name;
        }
    }
    return NULL;
}

struct tlv_to_config_name* find_tlv_config(int tlv_id) {
    int i;
    for (i = 0; i < sizeof(maps)/sizeof(struct tlv_to_config_name); i++) {
        if (tlv_id == maps[i].tlv_id) {
            return &maps[i];
        }
    }
    return NULL;
}

struct tlv_to_config_name wpas_global_maps[] = {
    { TLV_STA_SAE_GROUPS, "sae_groups", 0 },
    { TLV_MBO_CELL_CAPA, "mbo_cell_capa", 0 },
    { TLV_SAE_PWE, "sae_pwe", 0 },
    { TLV_CONTROL_INTERFACE, "ctrl_interface", 0 },
    { TLV_RAND_MAC_ADDR, "mac_addr", 0 },
    { TLV_PREASSOC_RAND_MAC_ADDR, "preassoc_mac_addr", 0 },
    { TLV_RAND_ADDR_LIFETIME, "rand_addr_lifetime", 0 },
    { TLV_HS20, "hs20", 0 },
    { TLV_INTERWORKING, "interworking", 0 },
    { TLV_HESSID, "hessid", 0 },
    { TLV_ACCESS_NETWORK_TYPE, "access_network_type", 0 },
    { TLV_FREQ_LIST, "freq_list", 0 },
    { TLV_UPDATE_CONFIG, "update_config", 0 },
    { TLV_P2P_DISABLED, "p2p_disabled", 0 },
};

struct tlv_to_config_name* find_wpas_global_config_name(int tlv_id) {
    int i;
    for (i = 0; i < sizeof(wpas_global_maps)/sizeof(struct tlv_to_config_name); i++) {
        if (tlv_id == wpas_global_maps[i].tlv_id) {
            return &wpas_global_maps[i];
        }
    }
    return NULL;
}

struct tlv_to_config_name* find_generic_tlv_config(int tlv_id, struct tlv_to_config_name* arr, int arr_size) {
    int i;
    for (i = 0; i < arr_size; i++) {
        if (tlv_id == (arr + i)->tlv_id) {
            return (arr + i);
        }
    }
    return NULL;
}

static char pac_file_path[S_BUFFER_LEN] = {0};
struct interface_info* band_transmitter[16];
struct interface_info* band_first_wlan[16];
extern struct sockaddr_in *tool_addr;
int sta_configured = 0;
int sta_started = 0;
#ifdef _OPENWRT_QTI_
extern char *indigo_radio_ifname[];
extern char *indigo_radio_ifname_ap[];
extern int num_radio;
int run_system(const char *cmd);
extern struct indigo_dut dut;
int generate_wireless_config(char *output, int output_size,
            struct packet_wrapper *wrapper,
            struct interface_info* wlanp, struct indigo_dut *dut);
#define BUZZ_SIZE 1024
#endif

void register_apis(void) {
    /* Basic */
    register_api(API_GET_IP_ADDR, NULL, get_ip_addr_handler);
    register_api(API_GET_MAC_ADDR, NULL, get_mac_addr_handler);
    register_api(API_GET_CONTROL_APP_VERSION, NULL, get_control_app_handler);
    register_api(API_START_LOOP_BACK_SERVER, NULL, start_loopback_server);
    register_api(API_STOP_LOOP_BACK_SERVER, NULL, stop_loop_back_server_handler);
    register_api(API_CREATE_NEW_INTERFACE_BRIDGE_NETWORK, NULL, create_bridge_network_handler);
    register_api(API_ASSIGN_STATIC_IP, NULL, assign_static_ip_handler);
    register_api(API_DEVICE_RESET, NULL, reset_device_handler);
    register_api(API_START_DHCP, NULL, start_dhcp_handler);
    register_api(API_STOP_DHCP, NULL, stop_dhcp_handler);
    register_api(API_GET_WSC_PIN, NULL, get_wsc_pin_handler);
    register_api(API_GET_WSC_CRED, NULL, get_wsc_cred_handler);
    /* AP */
    register_api(API_AP_START_UP, NULL, start_ap_handler);
    register_api(API_AP_STOP, NULL, stop_ap_handler);
    register_api(API_AP_CONFIGURE, NULL, configure_ap_handler);
    register_api(API_AP_TRIGGER_CHANSWITCH, NULL, trigger_ap_channel_switch);
    register_api(API_AP_SEND_DISCONNECT, NULL, send_ap_disconnect_handler);
    register_api(API_AP_SET_PARAM , NULL, set_ap_parameter_handler);
    register_api(API_AP_SEND_BTM_REQ, NULL, send_ap_btm_handler);
    register_api(API_AP_START_WPS, NULL, start_wps_ap_handler);
    register_api(API_AP_CONFIGURE_WSC, NULL, configure_ap_wsc_handler);
    /* STA */
    register_api(API_STA_ASSOCIATE, NULL, associate_sta_handler);
    register_api(API_STA_CONFIGURE, NULL, configure_sta_handler);
    register_api(API_STA_DISCONNECT, NULL, stop_sta_handler);
    register_api(API_STA_SEND_DISCONNECT, NULL, send_sta_disconnect_handler);
    register_api(API_STA_REASSOCIATE, NULL, send_sta_reconnect_handler);
    register_api(API_STA_SET_PARAM, NULL, set_sta_parameter_handler);
    register_api(API_STA_SEND_BTM_QUERY, NULL, send_sta_btm_query_handler);
    register_api(API_STA_SEND_ANQP_QUERY, NULL, send_sta_anqp_query_handler);
    register_api(API_STA_SCAN, NULL, sta_scan_handler);
    register_api(API_STA_START_WPS, NULL, start_wps_sta_handler);
    register_api(API_STA_HS2_ASSOCIATE, NULL, set_sta_hs2_associate_handler);
    register_api(API_STA_ADD_CREDENTIAL, NULL, sta_add_credential_handler);
    register_api(API_STA_INSTALL_PPSMO, NULL, set_sta_install_ppsmo_handler);
    /* TODO: Add the handlers */
    register_api(API_STA_SET_CHANNEL_WIDTH, NULL, NULL);
    register_api(API_STA_POWER_SAVE, NULL, NULL);
    register_api(API_P2P_START_UP, NULL, start_up_p2p_handler);
    register_api(API_P2P_FIND, NULL, p2p_find_handler);
    register_api(API_P2P_LISTEN, NULL, p2p_listen_handler);
    register_api(API_P2P_ADD_GROUP, NULL, add_p2p_group_handler);
    register_api(API_P2P_START_WPS, NULL, p2p_start_wps_handler);
    register_api(API_P2P_CONNECT, NULL, p2p_connect_handler);
    register_api(API_P2P_GET_INTENT_VALUE, NULL, get_p2p_intent_value_handler);
    register_api(API_P2P_INVITE, NULL, p2p_invite_handler);
    register_api(API_P2P_STOP_GROUP, NULL, stop_p2p_group_handler);
    register_api(API_P2P_SET_SERV_DISC, NULL, set_p2p_serv_disc_handler);
    register_api(API_P2P_SET_EXT_LISTEN, NULL, set_p2p_ext_listen_handler);
    register_api(API_STA_ENABLE_WSC, NULL, enable_wsc_sta_handler);
    /* AFC */
    register_api(API_GET_CONTROL_APP_VERSION, NULL, get_control_app_handler);
    register_api(API_AFCD_CONFIGURE, NULL, afcd_configure_handler);
    register_api(API_AFCD_OPERATION, NULL, afcd_operation_handler);
    register_api(API_AFCD_GET_INFO, NULL, afcd_get_info_handler);
}

int get_control_app_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    char ipAddress[INET_ADDRSTRLEN];
    char buffer[S_BUFFER_LEN];
#ifdef _VERSION_
    snprintf(buffer, sizeof(buffer), "%s", _VERSION_);
#else
    snprintf(buffer, sizeof(buffer), "%s", TLV_VALUE_APP_VERSION);
#endif
    if (tool_addr) {
        inet_ntop(AF_INET, &(tool_addr->sin_addr), ipAddress, INET_ADDRSTRLEN);
        indigo_logger(LOG_LEVEL_DEBUG, "Tool Control IP address on DUT network path: %s", ipAddress);
    }

    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, TLV_VALUE_STATUS_OK);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(TLV_VALUE_OK), TLV_VALUE_OK);
    fill_wrapper_tlv_bytes(resp, TLV_CONTROL_APP_VERSION, strlen(buffer), buffer);
    return 0;
}

int reset_device_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_RESET_NOT_OK;
    char buffer[TLV_VALUE_SIZE];
    char role[TLV_VALUE_SIZE], log_level[TLV_VALUE_SIZE], band[TLV_VALUE_SIZE];
    struct tlv_hdr *tlv = NULL;

    /* TLV: ROLE */
    tlv = find_wrapper_tlv_by_id(req, TLV_ROLE);
    memset(role, 0, sizeof(role));
    if (tlv) {
        memcpy(role, tlv->value, tlv->len);
    } else {
        goto done;
    }
    /* TLV: DEBUG_LEVEL */
    tlv = find_wrapper_tlv_by_id(req, TLV_DEBUG_LEVEL);
    memset(log_level, 0, sizeof(log_level));
    if (tlv) {
        memcpy(log_level, tlv->value, tlv->len);
    }
    /* TLV: TLV_BAND */
    memset(band, 0, sizeof(band));
    tlv = find_wrapper_tlv_by_id(req, TLV_BAND);
    if (tlv) {
        memcpy(band, tlv->value, tlv->len);
    }

    if (atoi(role) == DUT_TYPE_STAUT) {
        /* stop the wpa_supplicant and release IP address */
        memset(buffer, 0, sizeof(buffer));
#ifdef _OPENWRT_QTI_
        run_system("wifi config > /etc/config/wireless");
        sleep(1);
        memset(&dut,0,sizeof(dut));
#else
        snprintf(buffer, sizeof(buffer), "killall %s 1>/dev/null 2>/dev/null", get_wpas_exec_file());
        indigo_logger(LOG_LEVEL_DEBUG, "SYSTEM_CMD: %s", buffer);
        system(buffer);
        sleep(1);
        reset_interface_ip(get_wireless_interface());
        if (strlen(log_level)) {
            set_wpas_debug_level(get_debug_level(atoi(log_level)));
        }
#endif
        sta_configured = 0;
        sta_started = 0;
    } else if (atoi(role) == DUT_TYPE_APUT) {
        /* stop the hostapd and release IP address */
        memset(buffer, 0, sizeof(buffer));
#ifdef _OPENWRT_QTI_
        run_system("wifi config > /etc/config/wireless");
        sleep(1);
        memset(&dut,0,sizeof(dut));
#else
        snprintf(buffer, sizeof(buffer), "killall %s 1>/dev/null 2>/dev/null", get_hapd_exec_file());
        system(buffer);
        sleep(1);
        reset_interface_ip(get_wireless_interface());
        if (strlen(log_level)) {
            set_hostapd_debug_level(get_debug_level(atoi(log_level)));
        }
        reset_bridge(get_wlans_bridge());
#endif
        /* reset interfaces info */
        clear_interfaces_resource();
    } else if (atoi(role) == DUT_TYPE_P2PUT) {
        /* If TP is P2P client, GO can't stop before client removes group monitor if */
        // sprintf(buffer, "killall %s 1>/dev/null 2>/dev/null", get_wpas_exec_file());
        // reset_interface_ip(get_wireless_interface());
        if (strlen(log_level)) {
            set_wpas_debug_level(get_debug_level(atoi(log_level)));
        }
    }

    if (strcmp(band, TLV_BAND_24GHZ) == 0) {
        set_default_wireless_interface_info(BAND_24GHZ);
    } else if (strcmp(band, TLV_BAND_5GHZ) == 0) {
        set_default_wireless_interface_info(BAND_5GHZ);
    } else if (strcmp(band, TLV_BAND_6GHZ) == 0) {
        set_default_wireless_interface_info(BAND_6GHZ);
    }

    memset(band_transmitter, 0, sizeof(band_transmitter));
    memset(band_first_wlan, 0, sizeof(band_first_wlan));

    vendor_device_reset();

    sleep(1);

    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_RESET_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

// RESP: {<ResponseTLV.STATUS: 40961>: '0', <ResponseTLV.MESSAGE: 40960>: 'AP stop completed : Hostapd service is inactive.'}
int stop_ap_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int len = 0, reset = 0;
    char buffer[S_BUFFER_LEN], reset_type[16];
    char *parameter[] = {"pidof", get_hapd_exec_file(), NULL};
    char *message = NULL;
    struct tlv_hdr *tlv = NULL;

    /* TLV: RESET_TYPE */
    tlv = find_wrapper_tlv_by_id(req, TLV_RESET_TYPE);
    memset(reset_type, 0, sizeof(reset_type));
    if (tlv) {
        memcpy(reset_type, tlv->value, MIN(tlv->len, sizeof(reset_type) - 1));
        reset = atoi(reset_type);
        indigo_logger(LOG_LEVEL_DEBUG, "Reset Type: %d", reset);
    }

    if (reset == RESET_TYPE_INIT) {
        open_tc_app_log();
        system("rm -rf /var/log/hostapd.log >/dev/null 2>/dev/null");
    }

    memset(buffer, 0, sizeof(buffer));
#ifdef _OPENWRT_QTI_
    run_system("wifi down");
    run_system("wifi config > /etc/config/wireless");
#else
    snprintf(buffer, sizeof(buffer), "killall %s 1>/dev/null 2>/dev/null", get_hapd_exec_file());
    system(buffer);
    sleep(2);
#endif

#ifdef _OPENWRT_
#else
    len = system("rfkill unblock wlan");
    if (len) {
        indigo_logger(LOG_LEVEL_DEBUG, "Failed to run rfkill unblock wlan");
    }
    sleep(1);
#endif

    memset(buffer, 0, sizeof(buffer));
    len = pipe_command(buffer, sizeof(buffer), "/bin/pidof", parameter);
    if (len) {
#ifdef _OPENWRT_QTI_
        message = TLV_VALUE_HOSTAPD_STOP_OK;
#else
        message = TLV_VALUE_HOSTAPD_STOP_NOT_OK;
#endif
    } else {
        message = TLV_VALUE_HOSTAPD_STOP_OK;
    }

    /* reset interfaces info */
    clear_interfaces_resource();
    memset(band_transmitter, 0, sizeof(band_transmitter));
    memset(band_first_wlan, 0, sizeof(band_first_wlan));

    /* Test case teardown case */
    if (reset == RESET_TYPE_TEARDOWN) {
        close_tc_app_log();
    }

    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
#ifdef _OPENWRT_QTI_
    fill_wrapper_tlv_byte(resp, TLV_STATUS, TLV_VALUE_STATUS_OK);
#else
    fill_wrapper_tlv_byte(resp, TLV_STATUS, len == 0 ? TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
#endif
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

#ifdef _RESERVED_
/* The function is reserved for the defeault hostapd config */
#define HOSTAPD_DEFAULT_CONFIG_SSID                 "QuickTrack"
#define HOSTAPD_DEFAULT_CONFIG_CHANNEL              "36"
#define HOSTAPD_DEFAULT_CONFIG_HW_MODE              "a"
#define HOSTAPD_DEFAULT_CONFIG_WPA_PASSPHRASE       "12345678"
#define HOSTAPD_DEFAULT_CONFIG_IEEE80211N           "1"
#define HOSTAPD_DEFAULT_CONFIG_WPA                  "2"
#define HOSTAPD_DEFAULT_CONFIG_WPA_KEY_MGMT         "WPA-PSK"
#define HOSTAPD_DEFAULT_CONFIG_RSN_PAIRWISE         "CCMP"

static void append_hostapd_default_config(struct packet_wrapper *wrapper) {
    if (find_wrapper_tlv_by_id(wrapper, TLV_SSID) == NULL) {
        add_wrapper_tlv(wrapper, TLV_SSID, strlen(HOSTAPD_DEFAULT_CONFIG_SSID), HOSTAPD_DEFAULT_CONFIG_SSID);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_CHANNEL) == NULL) {
        add_wrapper_tlv(wrapper, TLV_CHANNEL, strlen(HOSTAPD_DEFAULT_CONFIG_CHANNEL), HOSTAPD_DEFAULT_CONFIG_CHANNEL);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_HW_MODE) == NULL) {
        add_wrapper_tlv(wrapper, TLV_HW_MODE, strlen(HOSTAPD_DEFAULT_CONFIG_HW_MODE), HOSTAPD_DEFAULT_CONFIG_HW_MODE);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_WPA_PASSPHRASE) == NULL) {
        add_wrapper_tlv(wrapper, TLV_WPA_PASSPHRASE, strlen(HOSTAPD_DEFAULT_CONFIG_WPA_PASSPHRASE), HOSTAPD_DEFAULT_CONFIG_WPA_PASSPHRASE);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_IEEE80211_N) == NULL) {
        add_wrapper_tlv(wrapper, TLV_IEEE80211_N, strlen(HOSTAPD_DEFAULT_CONFIG_IEEE80211N), HOSTAPD_DEFAULT_CONFIG_IEEE80211N);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_WPA) == NULL) {
        add_wrapper_tlv(wrapper, TLV_WPA, strlen(HOSTAPD_DEFAULT_CONFIG_WPA), HOSTAPD_DEFAULT_CONFIG_WPA);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_WPA_KEY_MGMT) == NULL) {
        add_wrapper_tlv(wrapper, TLV_WPA_KEY_MGMT, strlen(HOSTAPD_DEFAULT_CONFIG_WPA_KEY_MGMT), HOSTAPD_DEFAULT_CONFIG_WPA_KEY_MGMT);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_RSN_PAIRWISE) == NULL) {
        add_wrapper_tlv(wrapper, TLV_RSN_PAIRWISE, strlen(HOSTAPD_DEFAULT_CONFIG_RSN_PAIRWISE), HOSTAPD_DEFAULT_CONFIG_RSN_PAIRWISE);
    }
}
#endif /* _RESERVED_ */

#ifndef _OPENWRT_QTI_
static int generate_hostapd_config(char *output, int output_size, struct packet_wrapper *wrapper, struct interface_info* wlanp) {
    int has_sae = 0, has_wpa = 0, has_pmf = 0, has_owe = 0, has_sae_trans = 0, has_sae_groups = 0, has_owe_trans = 0, has_multi_ap = 0;
    int has_ssid = 0;
    int channel = 0, chwidth = 1, enable_ax = 0, chwidthset = 0, enable_muedca = 0, vht_chwidthset = 0;
    int i, enable_ac = 0, enable_11h = 0, enable_hs20 = 0;
    int enable_wps = 0, use_mbss = 0;
    char buffer[S_BUFFER_LEN], cfg_item[2*BUFFER_LEN];
    char band[64], value[16];
    char country[16];
    struct tlv_to_config_name* cfg = NULL;
    struct tlv_hdr *tlv = NULL;
    int is_6g_only = 0, unsol_pr_resp_interval = 0;
    int is_sae_only_passphase = 0;
    struct tlv_to_profile *profile = NULL;
    int semicolon_list_size = sizeof(semicolon_list) / sizeof(struct tlv_to_config_name);
    int hs20_icons_attached = 0;
    int is_multiple_bssid = 0;
    char *save_ptr;

#if HOSTAPD_SUPPORT_MBSSID
    if (wlanp->mbssid_enable && !wlanp->transmitter) {
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
        snprintf(output, output_size, "bss=%s\nctrl_interface=%s\n", wlanp->ifname, get_hapd_ctrl_path_no_iface());
#else
        snprintf(output, output_size, "bss=%s\nctrl_interface=%s\n", wlanp->ifname, get_hapd_ctrl_path());
#endif
        is_multiple_bssid = 1;
    } else {
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
        snprintf(output, output_size, "ctrl_interface=%s\ninterface=%s\n", get_hapd_ctrl_path_no_iface(), wlanp->ifname);
#else
        snprintf(output, output_size, "ctrl_interface=%s\nctrl_interface_group=0\ninterface=%s\n", get_hapd_ctrl_path(), wlanp->ifname);
#endif
    }
#else /*HOSTAPD_SUPPORT_MBSSID*/
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
    snprintf(output, output_size, "ctrl_interface=%s\ninterface=%s\n", get_hapd_ctrl_path_no_iface(), wlanp->ifname);
#else
    snprintf(output, output_size, "ctrl_interface=%s\nctrl_interface_group=0\ninterface=%s\n", get_hapd_ctrl_path(), wlanp->ifname);
#endif
#endif /*HOSTAPD_SUPPORT_MBSSID*/

#ifdef _RESERVED_
    /* The function is reserved for the defeault hostapd config */
    append_hostapd_default_config(wrapper);
#endif

    memset(country, 0, sizeof(country));
    memset(band, 0, sizeof(band));

    /* QCA WTS image doesn't apply 11ax, mu_edca, country, 11d, 11h in hostapd */
    for (i = 0; i < wrapper->tlv_num; i++) {
        tlv = wrapper->tlv[i];
        memset(buffer, 0, sizeof(buffer));
        memset(cfg_item, 0, sizeof(cfg_item));

        if (tlv->id == TLV_CHANNEL) {
            memset(value, 0, sizeof(value));
            memcpy(value, tlv->value, MIN(tlv->len, sizeof(value) - 1));
            channel = atoi(value);
            if (is_multiple_bssid) {
               /* channel will be configured on the first wlan */
               continue;
            }
        }

        if (tlv->id == TLV_HE_6G_ONLY) {
            is_6g_only = 1;
            continue;
        }

        if (tlv->id == TLV_BSS_IDENTIFIER) {
            use_mbss = 1;
            if (is_band_enabled(BAND_6GHZ) && !wlanp->mbssid_enable) {
                strlcat(output, "rnr=1\n", output_size);
            }
            /* Multiple wlans configure must carry TLV_BSS_IDENTIFIER */
            has_multi_ap = 1;
            continue;
        }


        /* This is used when hostapd will use multiple lines to
         * configure multiple items in the same configuration parameter
         * (use semicolon to separate multiple configurations) */
        cfg = find_generic_tlv_config(tlv->id, semicolon_list, semicolon_list_size);
        if (cfg) {
            char *token = NULL, *delimit = ";";

            memcpy(buffer, tlv->value, tlv->len);
            token = strtok_r(buffer, delimit, &save_ptr);

            while(token != NULL) {
                snprintf(cfg_item, sizeof(cfg_item), "%s=%s\n", cfg->config_name, token);
                strlcat(output, cfg_item, output_size);
                token = strtok_r(NULL, delimit, &save_ptr);
            }
            continue;
        }

        if (tlv->id == TLV_HESSID && strstr(tlv->value, "self")) {
            char mac_addr[64];

            memset(mac_addr, 0, sizeof(mac_addr));
            get_mac_address(mac_addr, sizeof(mac_addr), get_wireless_interface());
            snprintf(cfg_item, sizeof(cfg_item), "hessid=%s\n", mac_addr);
            strlcat(output, cfg_item, output_size);
            continue;
        }

        /* profile config */
        profile = find_tlv_hs2_profile(tlv->id);
        if (profile) {
            char *hs2_config = 0;
            memcpy(buffer, tlv->value, tlv->len);

            if (((tlv->id == TLV_OSU_PROVIDERS_LIST) || (tlv->id == TLV_OPERATOR_ICON_METADATA)) && (!hs20_icons_attached)) {
                attach_hs20_icons(output);
                hs20_icons_attached = 1;
            }

            if (atoi(buffer) > profile->size) {
               indigo_logger(LOG_LEVEL_ERROR, "profile index out of bound!: %d, array_size:%d", atoi(buffer), profile->size);
            } else {
                hs2_config = (char *)profile->profile[atoi(buffer)];
            }

            if (hs2_config)
                snprintf(cfg_item, sizeof(cfg_item), "%s", hs2_config);
            strlcat(output, cfg_item, output_size);
            continue;
        }

        /* ssid */
        if (tlv->id == TLV_SSID)
            has_ssid = 1;

        /* wps settings */
        if (tlv->id == TLV_WPS_ENABLE) {
            int j;

            /* To get AP wps vendor info */
            wps_setting *s = get_vendor_wps_settings(WPS_AP);
            if (!s) {
                indigo_logger(LOG_LEVEL_WARNING, "Failed to get APUT WPS settings");
                continue;
            }
            enable_wps = 1;
            memcpy(buffer, tlv->value, tlv->len);
            if (atoi(buffer) == WPS_ENABLE_OOB) {
                /* WPS OOB: Out-of-Box */
                for (j = 0; j < AP_SETTING_NUM; j++) {
                    memset(cfg_item, 0, sizeof(cfg_item));
                    // filter out empty wkey
                    if (strlen(s[j].wkey) == 0)
                        continue;
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
                    // filter out ssid from wps settings if TLV_SSID present e.g. QT10555
                    if (has_ssid && strcmp("ssid", s[j].wkey) == 0)
                        continue;
#endif
                    snprintf(cfg_item, sizeof(cfg_item), "%s=%s\n", s[j].wkey, s[j].value);
                    strlcat(output, cfg_item, output_size);
                }
                indigo_logger(LOG_LEVEL_INFO, "APUT Configure WPS: OOB.");
            } else if (atoi(buffer) == WPS_ENABLE_NORMAL){
                /* WPS Normal: Configure manually. */
                for (j = 0; j < AP_SETTING_NUM; j++) {
                    memset(cfg_item, 0, sizeof(cfg_item));
                    /* set wps state */
                    if (atoi(s[j].attr) == atoi(WPS_OOB_ONLY)) {
                        if (!(memcmp(s[j].wkey, WPS_OOB_STATE, strlen(WPS_OOB_STATE)))) {
                            /* set wps state to Configured compulsorily */
                            snprintf(cfg_item, sizeof(cfg_item), "%s=%s\n", s[j].wkey, WPS_OOB_CONFIGURED);
                            strlcat(output, cfg_item, output_size);
                        }
                    }
                    /* set wps common settings */
                    if (atoi(s[j].attr) == atoi(WPS_COMMON)) {
                        // filter out empty wkey
                        if (strlen(s[j].wkey) == 0)
                            continue;
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
                        // filter out ssid from wps settings if TLV_SSID present e.g. QT10555
                        if (has_ssid && strcmp("ssid", s[j].wkey) == 0)
                            continue;
#endif
                        snprintf(cfg_item, sizeof(cfg_item), "%s=%s\n", s[j].wkey, s[j].value);
                        strlcat(output, cfg_item, output_size);
                    }
                }
                indigo_logger(LOG_LEVEL_INFO, "APUT Configure WPS: Manually Configured.");
            } else {
                indigo_logger(LOG_LEVEL_ERROR, "Unknown WPS TLV value: %d (TLV ID 0x%04x)", atoi(buffer), tlv->id);
            }
            continue;
        }

        /* wps er support. upnp */
        if (tlv->id == TLV_WPS_ER_SUPPORT) {
            memset(cfg_item, 0, sizeof(cfg_item));
            snprintf(cfg_item, sizeof(cfg_item), "upnp_iface=%s\n", wlanp->ifname);
            strlcat(output, cfg_item, output_size);
            memset(cfg_item, 0, sizeof(cfg_item));
            snprintf(cfg_item, sizeof(cfg_item), "friendly_name=WPS Access Point\n");
            strlcat(output, cfg_item, output_size);
            memset(cfg_item, 0, sizeof(cfg_item));
            snprintf(cfg_item, sizeof(cfg_item), "model_description=Wireless Access Point\n");
            strlcat(output, cfg_item, output_size);
            continue;
        }
        cfg = find_tlv_config(tlv->id);
        if (!cfg) {
            indigo_logger(LOG_LEVEL_ERROR, "Unknown AP configuration name: TLV ID 0x%04x", tlv->id);
            continue;
        }

        if (tlv->id == TLV_WPA_PASSPHRASE) {
             if (tlv->len < 8 || tlv->len > 63) {
                 is_sae_only_passphase = 1;
             }
        }

        if (tlv->id == TLV_WPA_KEY_MGMT && strstr(tlv->value, "SAE") && strstr(tlv->value, "WPA-PSK")) {
            has_sae_trans = 1;
        }

        if (tlv->id == TLV_WPA_KEY_MGMT && strstr(tlv->value, "OWE")) {
            has_owe = 1;
        }

        if (tlv->id == TLV_WPA_KEY_MGMT && strstr(tlv->value, "SAE")) {
            has_sae = 1;
        }

        if (tlv->id == TLV_WPA && strstr(tlv->value, "2")) {
            has_wpa = 1;
        }

        if (tlv->id == TLV_IEEE80211_W) {
            has_pmf = 1;
        }

        if (tlv->id == TLV_HW_MODE) {
            if (tlv->len >= sizeof(band)) {
                indigo_logger(LOG_LEVEL_ERROR, "Invalid tlv len");
                return -1;
            }
            memcpy(band, tlv->value, tlv->len);
        }

        if (tlv->id == TLV_CHANNEL) {
            if (tlv->len >= sizeof(value)) {
                indigo_logger(LOG_LEVEL_ERROR, "Invalid tlv len");
                return -1;
            }
            memset(value, 0, sizeof(value));
            memcpy(value, tlv->value, tlv->len);
            channel = atoi(value);
        }

        if (tlv->id == TLV_HE_OPER_CHWIDTH) {
            if (tlv->len >= sizeof(value)) {
                indigo_logger(LOG_LEVEL_ERROR, "Invalid tlv len");
                return -1;
            }
            memset(value, 0, sizeof(value));
            memcpy(value, tlv->value, tlv->len);
            chwidth = atoi(value);
            chwidthset = 1;
#ifdef _WTS_OPENWRT_
            continue;
#endif
        }

        if (tlv->id == TLV_VHT_OPER_CHWIDTH) {
            if (tlv->len >= sizeof(value)) {
                indigo_logger(LOG_LEVEL_ERROR, "Invalid tlv len");
                return -1;
            }
            memset(value, 0, sizeof(value));
            memcpy(value, tlv->value, tlv->len);
            chwidth = atoi(value);
            vht_chwidthset = 1;
        }

        if (tlv->id == TLV_IEEE80211_AC && strstr(tlv->value, "1")) {
            enable_ac = 1;
        }

        if (tlv->id == TLV_IEEE80211_AX && strstr(tlv->value, "1")) {
            enable_ax = 1;
#ifdef _WTS_OPENWRT_
            continue;
#endif
        }

        if (tlv->id == TLV_HE_MU_EDCA) {
#ifdef _WTS_OPENWRT_
            continue;
#endif
            enable_muedca = 1;
        }

        if (tlv->id == TLV_SAE_GROUPS) {
            has_sae_groups = 1;
        }

        if (tlv->id == TLV_COUNTRY_CODE) {
            if (tlv->len >= sizeof(country)) {
                indigo_logger(LOG_LEVEL_ERROR, "Invalid tlv len");
                return -1;
            }
            memcpy(country, tlv->value, tlv->len);
#ifdef HOSTAPD_DEFAULT_COUNTRY_VLP
            memset(buffer, 0, sizeof(buffer));
            snprintf(buffer, sizeof(buffer), "COUNTRY %s", country);
            cld_drivercmd(wlanp->ifname, buffer);
#endif
#ifdef _WTS_OPENWRT_
            continue;
#endif
        }

        if (tlv->id == TLV_IEEE80211_H) {
#ifdef _WTS_OPENWRT_
            continue;
#endif
            enable_11h = 1;
        }

#ifdef _WTS_OPENWRT_
        if (tlv->id == TLV_IEEE80211_D || tlv->id == TLV_HE_OPER_CENTR_FREQ)
            continue;

#endif

#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
        /* TLV interface name is not supported by hostapd.conf, and
         * - It is used to auto assign iface (i.e. wlanp) in configure_ap_handler,
         * - Hostapd.conf interface is specified by wlanp->ifname.
         */
        if (tlv->id == TLV_INTERFACE_NAME)
            continue;
#endif
        if (tlv->id == TLV_HE_UNSOL_PR_RESP_CADENCE) {
            if (tlv->len >= sizeof(value)) {
                indigo_logger(LOG_LEVEL_ERROR, "Invalid tlv len");
                return -1;
            }
            memset(value, 0, sizeof(value));
            memcpy(value, tlv->value, tlv->len);
            unsol_pr_resp_interval = atoi(value);
        }

        memset(buffer, 0, sizeof(buffer));
        memset(cfg_item, 0, sizeof(cfg_item));

        if (tlv->id == TLV_HS20 && strstr(tlv->value, "1")) {
            enable_hs20 = 1;
        }

        if (tlv->id == TLV_OWE_TRANSITION_BSS_IDENTIFIER) {
            struct bss_identifier_info bss_info;
            struct interface_info *wlan;
            int bss_identifier;
            char bss_identifier_str[8];
            if (tlv->len >= sizeof(bss_identifier_str)) {
                indigo_logger(LOG_LEVEL_ERROR, "Invalid tlv len");
                return -1;
            }
            memset(&bss_info, 0, sizeof(bss_info));
            memset(bss_identifier_str, 0, sizeof(bss_identifier_str));
            memcpy(bss_identifier_str, tlv->value, tlv->len);
            bss_identifier = atoi(bss_identifier_str);
            parse_bss_identifier(bss_identifier, &bss_info);
            wlan = get_wireless_interface_info(bss_info.band, bss_info.identifier);
            if (NULL == wlan) {
                wlan = assign_wireless_interface_info(&bss_info);
            }
            indigo_logger(LOG_LEVEL_DEBUG, "TLV_OWE_TRANSITION_BSS_IDENTIFIER: TLV_BSS_IDENTIFIER 0x%x identifier %d mapping ifname %s\n",
                    bss_identifier,
                    bss_info.identifier,
                    wlan ? wlan->ifname : "n/a"
                    );
            if (wlan) {
                memcpy(buffer, wlan->ifname, strlen(wlan->ifname));
                snprintf(cfg_item, sizeof(cfg_item), "%s=%s\n", cfg->config_name, buffer);
                strlcat(output, cfg_item, output_size);
                if (has_owe) {
                    memset(cfg_item, 0, sizeof(cfg_item));
                    snprintf(cfg_item, sizeof(cfg_item), "ignore_broadcast_ssid=1\n");
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
                    strlcat(output, "owe_ptk_workaround=1\n", output_size);
#endif
                }
                has_owe_trans = 1;
            }
        } else {
            memcpy(buffer, tlv->value, tlv->len);
            snprintf(cfg_item, sizeof(cfg_item), "%s=%s\n", cfg->config_name, buffer);
            strlcat(output, cfg_item, output_size);
        }
    }

    /* add rf band according to TLV_BSS_IDENTIFIER/TLV_HW_MODE/TLV_WPS_ENABLE */
    if (enable_wps) {
        if (use_mbss) {
            /* The wps test for mbss should always be dual concurrent. */
            strlcat(output, "wps_rf_bands=ag\n", output_size);
        } else {
            if (!strncmp(band, "a", 1)) {
                strlcat(output, "wps_rf_bands=a\n", output_size);
            } else if (!strncmp(band, "g", 1)) {
                strlcat(output, "wps_rf_bands=g\n", output_size);
            }
        }
    }
    if (has_sae_trans && is_sae_only_passphase) {
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
        // 10191 PSK+SAE password length > 63
        memset(output, 0, output_size);
        strlcat(output, "#Empty hostapd.conf due to 10191 SAE+PSK uses long passphrase\n", output_size);
        return strlen(output);
#endif
    }

    if (has_pmf == 0) {
        if (has_sae_trans) {
            strlcat(output, "ieee80211w=1\n", output_size);
        } else if (has_sae && has_wpa) {
            strlcat(output, "ieee80211w=2\n", output_size);
        } else if (has_owe) {
            strlcat(output, "ieee80211w=2\n", output_size);
        } else if (has_wpa) {
            strlcat(output, "ieee80211w=1\n", output_size);
        }
    }

    if (has_sae == 1) {
        strlcat(output, "sae_require_mfp=1\n", output_size);
    }

#if HOSTAPD_SUPPORT_MBSSID
    if (wlanp->mbssid_enable && wlanp->transmitter) {
        strlcat(output, "multiple_bssid=1\n", output_size);
    }
#endif

    // Note: if any new DUT configuration is added for sae_groups,
    // then the following unconditional sae_groups addition should be
    // changed to become conditional on there being no other sae_groups
    // configuration
    // e.g.:
    // if RequestTLV.SAE_GROUPS not in tlv_values:
    //     field_name = tlv_hostapd_config_mapper.get(RequestTLV.SAE_GROUPS)
    //     hostapd_config += "\n" + field_name + "=15 16 17 18 19 20 21"
    // Append the default SAE groups for SAE and no SAE groups TLV
    if (has_sae && has_sae_groups == 0) {
        strlcat(output, "sae_groups=15 16 17 18 19 20 21\n", output_size);
    }

    // Channel width configuration
    // Default: 20MHz in 2.4G(No configuration required) 80MHz(40MHz for 11N only) in 5G
    if (enable_ac == 0 && enable_ax == 0)
        chwidth = 0;


    if (is_6g_only) {
        if (chwidthset == 0) {
            snprintf(buffer, sizeof(buffer), "he_oper_chwidth=%d\n", chwidth);
            strlcat(output, buffer, output_size);
        }
        if (chwidth == 1)
            strlcat(output, "op_class=133\n", output_size);
        else if (chwidth == 2)
            strlcat(output, "op_class=134\n", output_size);
        snprintf(buffer, sizeof(buffer), "he_oper_centr_freq_seg0_idx=%d\n", get_6g_center_freq_index(channel, chwidth));
        strlcat(output, buffer, output_size);
        if (unsol_pr_resp_interval) {
            snprintf(buffer, sizeof(buffer), "unsol_bcast_probe_resp_interval=%d\n", unsol_pr_resp_interval);
            strlcat(output, buffer, output_size);
        } else {
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
            // allow hostapd to start without fils for ANDROID and MDM.
            indigo_logger(LOG_LEVEL_DEBUG, "Unset fils_discovery_max_interval from hostapd.conf");
#else
            strlcat(output, "fils_discovery_max_interval=20\n", output_size);
#endif
        }
        /* Enable bss_color and country IE */
        strlcat(output, "he_bss_color=19\n", output_size);
        strlcat(output, "ieee80211d=1\n", output_size);
#ifndef HOSTAPD_DEFAULT_COUNTRY_VLP
        strlcat(output, "country_code=US\n", output_size);
#else
        if (strlen(country) == 0) {
            strlcat(output, "country_code="HOSTAPD_DEFAULT_COUNTRY_VLP"\n", output_size);
            cld_drivercmd(wlanp->ifname, "COUNTRY "HOSTAPD_DEFAULT_COUNTRY_VLP);
        }
#endif
    } else if (strstr(band, "a")) {
        if (is_ht40plus_chan(channel))
            strlcat(output, "ht_capab=[HT40+]\n", output_size);
        else if (is_ht40minus_chan(channel))
            strlcat(output, "ht_capab=[HT40-]\n", output_size);
        else // Ch 165 and avoid hostapd configuration error
            chwidth = 0;
        if (chwidth > 0) {
            int center_freq = get_center_freq_index(channel, chwidth);
#ifndef _WTS_OPENWRT_
            if (chwidth == 2) {
                /* 160M: Need to enable 11h for DFS and enable 11d for 11h */
                strlcat(output, "ieee80211d=1\n", output_size);
                strlcat(output, "ieee80211h=1\n", output_size);
#ifndef HOSTAPD_DEFAULT_COUNTRY_VLP
                strlcat(output, "country_code=US\n", output_size);
#else
                if (strlen(country) == 0) {
                    strlcat(output, "country_code="HOSTAPD_DEFAULT_COUNTRY_VLP"\n", output_size);
                    cld_drivercmd(wlanp->ifname, "COUNTRY "HOSTAPD_DEFAULT_COUNTRY_VLP);
                }
#endif
            }
#endif
            if (enable_ac) {
                if (vht_chwidthset == 0) {
                    snprintf(buffer, sizeof(buffer), "vht_oper_chwidth=%d\n", chwidth);
                    strlcat(output, buffer, output_size);
                }
                snprintf(buffer, sizeof(buffer), "vht_oper_centr_freq_seg0_idx=%d\n", center_freq);
                strlcat(output, buffer, output_size);
#ifndef _WTS_OPENWRT_
                if (chwidth == 2) {
                    strlcat(output, "vht_capab=[VHT160]\n", output_size);
                }
#endif
            }
            if (enable_ax) {
#ifndef _WTS_OPENWRT_
                if (chwidthset == 0) {
                    snprintf(buffer, sizeof(buffer), "he_oper_chwidth=%d\n", chwidth);
                    strlcat(output, buffer, output_size);
                }
                snprintf(buffer, sizeof(buffer), "he_oper_centr_freq_seg0_idx=%d\n", center_freq);
                strlcat(output, buffer, output_size);
#endif
            }
        }
    }

    if (enable_muedca) {
        strlcat(output, "he_mu_edca_qos_info_queue_request=1\n", output_size);
        strlcat(output, "he_mu_edca_ac_be_aifsn=0\n", output_size);
        strlcat(output, "he_mu_edca_ac_be_ecwmin=15\n", output_size);
        strlcat(output, "he_mu_edca_ac_be_ecwmax=15\n", output_size);
        strlcat(output, "he_mu_edca_ac_be_timer=255\n", output_size);
        strlcat(output, "he_mu_edca_ac_bk_aifsn=0\n", output_size);
        strlcat(output, "he_mu_edca_ac_bk_aci=1\n", output_size);
        strlcat(output, "he_mu_edca_ac_bk_ecwmin=15\n", output_size);
        strlcat(output, "he_mu_edca_ac_bk_ecwmax=15\n", output_size);
        strlcat(output, "he_mu_edca_ac_bk_timer=255\n", output_size);
        strlcat(output, "he_mu_edca_ac_vi_ecwmin=15\n", output_size);
        strlcat(output, "he_mu_edca_ac_vi_ecwmax=15\n", output_size);
        strlcat(output, "he_mu_edca_ac_vi_aifsn=0\n", output_size);
        strlcat(output, "he_mu_edca_ac_vi_aci=2\n", output_size);
        strlcat(output, "he_mu_edca_ac_vi_timer=255\n", output_size);
        strlcat(output, "he_mu_edca_ac_vo_aifsn=0\n", output_size);
        strlcat(output, "he_mu_edca_ac_vo_aci=3\n", output_size);
        strlcat(output, "he_mu_edca_ac_vo_ecwmin=15\n", output_size);
        strlcat(output, "he_mu_edca_ac_vo_ecwmax=15\n", output_size);
        strlcat(output, "he_mu_edca_ac_vo_timer=255\n", output_size);
    }

#if defined(_OPENWRT_) && !defined(_WTS_OPENWRT_)
    /* Make sure AP include power constranit element even in non DFS channel */
    if (enable_11h) {
        strlcat(output, "spectrum_mgmt_required=1\n", output_size);
        strlcat(output, "local_pwr_constraint=3\n", output_size);
    }
#endif
    if (enable_hs20) {
        strlcat(output, "hs20_release=3\n", output_size);
        strlcat(output, "manage_p2p=1\n", output_size);
        strlcat(output, "allow_cross_connection=0\n", output_size);
        strlcat(output, "bss_load_update_period=100\n", output_size);
        strlcat(output, "hs20_deauth_req_timeout=3\n", output_size);
    }

    if (has_owe_trans || has_multi_ap) {
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
        indigo_logger(LOG_LEVEL_DEBUG, "add bridge config for multi ap or owe trans");
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "bridge=%s\n", BRIDGE_WLANS);
        strlcat(output, buffer, output_size);
        strlcat(output, "use_driver_iface_addr=1\n", output_size);
#endif
    }

    /* vendor specific config, not via hostapd */
    configure_ap_radio_params(band, country, channel, chwidth);

    return strlen(output);
}
#endif

// RESP: {<ResponseTLV.STATUS: 40961>: '0',<ResponseTLV.MESSAGE: 40960>: 'DUT configured as AP : Configuration file created'}
int configure_ap_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int len = 0;
    char buffer[L_BUFFER_LEN], ifname[TLV_VALUE_SIZE];
    struct tlv_hdr *tlv;
    char *message = "DUT configured as AP : Configuration file created";
    int bss_identifier = 0, band;
    struct interface_info* wlan = NULL;
    char bss_identifier_str[16], hw_mode_str[8];
    struct bss_identifier_info bss_info;

    memset(buffer, 0, sizeof(buffer));
    memset(ifname, 0, sizeof(ifname));
    memset(&bss_info, 0, sizeof(bss_info));

#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
    /* TLV: Interface Name */
    tlv = find_wrapper_tlv_by_id(req, TLV_INTERFACE_NAME);
    if (tlv) {
        memcpy(ifname, tlv->value, tlv->len);
    }
#endif

    /* TLV: Bss Identifier */
    tlv = find_wrapper_tlv_by_id(req, TLV_BSS_IDENTIFIER);
    if (tlv) {
        /* Multiple wlans configure must carry TLV_BSS_IDENTIFIER */
        memset(bss_identifier_str, 0, sizeof(bss_identifier_str));
        memcpy(bss_identifier_str, tlv->value,
               MIN(tlv->len, sizeof(bss_identifier_str) - 1));
        bss_identifier = atoi(bss_identifier_str);
        parse_bss_identifier(bss_identifier, &bss_info);
        wlan = get_wireless_interface_info(bss_info.band, bss_info.identifier);
        if (NULL == wlan) {
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
            wlan = assign_pref_wireless_interface_info(&bss_info, ifname);
#else
            wlan = assign_wireless_interface_info(&bss_info);
#endif
        }
        if (wlan && bss_info.mbssid_enable) {
            configure_ap_enable_mbssid();
            if (bss_info.transmitter) {
                band_transmitter[bss_info.band] = wlan;
            }
        }
        indigo_logger(LOG_LEVEL_DEBUG, "TLV_BSS_IDENTIFIER 0x%x band %d multiple_bssid %d transmitter %d identifier %d\n",
               bss_identifier,
               bss_info.band,
               bss_info.mbssid_enable,
               bss_info.transmitter,
               bss_info.identifier
               );
#ifdef _OPENWRT_QTI_
        if (bss_info.band == 0)
                dut.ap_interface_2g = 1;
        else if (bss_info.band == 1)
                dut.ap_interface_5g = 1;
        else
                dut.ap_interface_6g = 1;

        if (dut.ap_interface_5g && dut.ap_interface_2g)
                dut.ap_is_dual = 1;
        else if (dut.ap_interface_6g && dut.ap_interface_5g)
                dut.ap_is_dual = 1;

        dut.ap_bss_info.identifier= bss_info.identifier;
        dut.ap_bss_info.band= bss_info.band;
#endif
    } else {
        /* Single wlan case */
        tlv = find_wrapper_tlv_by_id(req, TLV_HW_MODE);
        if (tlv)
        {
            memset(hw_mode_str, 0, sizeof(hw_mode_str));
            memcpy(hw_mode_str, tlv->value, tlv->len);
            if (find_wrapper_tlv_by_id(req, TLV_HE_6G_ONLY)) {
                band = BAND_6GHZ;
#ifdef _OPENWRT_QTI_
                dut.ap_interface_6g = 1;
#endif
            } else if (!strncmp(hw_mode_str, "a", 1)) {
                band = BAND_5GHZ;
#ifdef _OPENWRT_QTI_
                dut.ap_interface_5g = 1;
#endif
            } else {
                band = BAND_24GHZ;
#ifdef _OPENWRT_QTI_
                dut.ap_interface_2g = 1;
#endif
            }
            /* Single wlan use ID 1 */
            bss_info.band = band;
            bss_info.identifier = 1;
            wlan = assign_wireless_interface_info(&bss_info);
        }
#ifdef _OPENWRT_QTI_
        /**** PLACEHOLDER CONDITION ****/
        if (dut.ap_interface_5g && dut.ap_interface_2g)
            dut.ap_is_dual = 1;;
#endif
    }
    if (wlan) {
#if _OPENWRT_QTI_
        len = generate_wireless_config(buffer, sizeof(buffer),
                req, wlan, &dut);
#else
        indigo_logger(LOG_LEVEL_DEBUG, "ifname %s hostapd conf file %s\n",
               wlan ? wlan->ifname : "n/a",
               wlan ? wlan->hapd_conf_file: "n/a"
               );
        len = generate_hostapd_config(buffer, sizeof(buffer), req, wlan);
#endif
        if (len)
        {
#if HOSTAPD_SUPPORT_MBSSID
            if (bss_info.mbssid_enable && !bss_info.transmitter) {
                if (band_transmitter[bss_info.band]) {
                    indigo_logger(LOG_LEVEL_DEBUG, "Append bss conf to %s", band_transmitter[bss_info.band]->hapd_conf_file);
                    append_file(band_transmitter[bss_info.band]->hapd_conf_file, buffer, len);
                }
                memset(wlan->hapd_conf_file, 0, sizeof(wlan->hapd_conf_file));
            } else
#endif
            {
                indigo_logger(LOG_LEVEL_DEBUG, "%s: hostapd_conf:\n %s", __func__, buffer);
                write_file(wlan->hapd_conf_file, buffer, len);
            }

        if (!band_first_wlan[bss_info.band]) {
            /* For the first configured ap */
            band_first_wlan[bss_info.band] = wlan;
        }
        }
    }
    show_wireless_interface_info();

    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
#ifdef _OPENWRT_QTI_
    fill_wrapper_tlv_byte(resp, TLV_STATUS, TLV_VALUE_STATUS_OK);
#else
    fill_wrapper_tlv_byte(resp, TLV_STATUS, len > 0 ? TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
#endif
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

#ifdef HOSTAPD_SUPPORT_MBSSID_WAR
extern int use_openwrt_wpad;
#endif
// RESP: {<ResponseTLV.STATUS: 40961>: '0', <ResponseTLV.MESSAGE: 40960>: 'AP is up : Hostapd service is active'}

int start_ap_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    char *message = TLV_VALUE_HOSTAPD_START_OK;
    char buffer[S_BUFFER_LEN];
    int len;
    int swap_hostapd = 0;

#ifdef _WTS_OPENWRT_
#ifndef _OPENWRT_QTI_
    openwrt_apply_radio_config();
    // DFS wait again if set wlan params after hostapd starts
    iterate_all_wlan_interfaces(start_ap_set_wlan_params);
#endif
#endif

    memset(buffer, 0, sizeof(buffer));
#if (defined(ANDROID) || defined(MDM))
    snprintf(buffer, sizeof(buffer), "%s -t -g %s %s%s %s &",
        get_hapd_full_exec_path(),
        get_hapd_ctrl_path_no_iface(),
        get_hostapd_debug_arguments(),
        get_hapd_log_file_arguments(),
        get_all_hapd_conf_files(&swap_hostapd));
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, buffer);
    len = system(buffer);
    sleep(5);
#else
#ifndef _OPENWRT_QTI_
    snprintf(buffer, sizeof(buffer), "%s -B -t -P /var/run/hostapd.pid -g %s %s -f /var/log/hostapd.log %s",
        get_hapd_full_exec_path(),
        get_hapd_global_ctrl_path(),
        get_hostapd_debug_arguments(),
        get_all_hapd_conf_files(&swap_hostapd));
    len = system(buffer);
    sleep(1);
#endif
#endif

    /* Bring up VAPs with MBSSID disable using WFA hostapd */
    if (swap_hostapd) {
#ifdef HOSTAPD_SUPPORT_MBSSID_WAR
        indigo_logger(LOG_LEVEL_INFO, "Use WFA hostapd for MBSSID disable VAPs with RNR");
        system("cp /overlay/hostapd /usr/sbin/hostapd");
        use_openwrt_wpad = 0;
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "%s -B -t -P /var/run/hostapd_1.pid %s -f /var/log/hostapd_1.log %s",
                get_hapd_full_exec_path(),
                get_hostapd_debug_arguments(),
                get_all_hapd_conf_files(&swap_hostapd));
        len = system(buffer);
        sleep(1);
#endif
    }

#ifdef _OPENWRT_QTI_
    /* Configure Radio & VAP, commit the config */
    len = bring_up_ap_interface();
#else
#ifndef _WTS_OPENWRT_
    iterate_all_wlan_interfaces(start_ap_set_wlan_params);
#endif
#endif

    bridge_init(get_wlans_bridge());

    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
#ifdef _OPENWRT_QTI_
    fill_wrapper_tlv_byte(resp, TLV_STATUS, len >= 0 ? TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
#else
    fill_wrapper_tlv_byte(resp, TLV_STATUS, len == 0 ? TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
#endif
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

// RESP: {<ResponseTLV.STATUS: 40961>: '0', <ResponseTLV.MESSAGE: 40960>: 'Configure and start wsc ap successfully. (Configure and start)'}
int configure_ap_wsc_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int len_1 = -1, len_2 = 0, len_3 = -1;
    char buffer[L_BUFFER_LEN], ifname[S_BUFFER_LEN];
    struct tlv_hdr *tlv;
    char *message = NULL;
    int bss_identifier = 0, band;
    struct interface_info* wlan = NULL;
    char bss_identifier_str[16], hw_mode_str[8];
    struct bss_identifier_info bss_info;
    char *parameter[] = {"pidof", get_hapd_exec_file(), NULL};
    int swap_hostapd = 0;

    /* Stop hostapd [Begin] */
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "killall %s 1>/dev/null 2>/dev/null", get_hapd_exec_file());
    system(buffer);
    sleep(2);

#ifdef _OPENWRT_
#else
    len_1 = system("rfkill unblock wlan");
    if (len_1) {
        indigo_logger(LOG_LEVEL_DEBUG, "Failed to run rfkill unblock wlan");
    }
    sleep(1);
#endif

    memset(buffer, 0, sizeof(buffer));
    len_1 = pipe_command(buffer, sizeof(buffer), "/bin/pidof", parameter);
    if (len_1) {
        message = TLV_VALUE_HOSTAPD_STOP_NOT_OK;
        goto done;
    }
    /* Stop hostapd [End] */

    /* Generate hostapd configuration file [Begin] */
    memset(buffer, 0, sizeof(buffer));
    tlv = find_wrapper_tlv_by_id(req, TLV_BSS_IDENTIFIER);
    memset(ifname, 0, sizeof(ifname));
    memset(&bss_info, 0, sizeof(bss_info));
    if (tlv) {
        /* Multiple wlans configure must carry TLV_BSS_IDENTIFIER */
        memset(bss_identifier_str, 0, sizeof(bss_identifier_str));
        memcpy(bss_identifier_str, tlv->value,
               MIN(tlv->len, sizeof(bss_identifier_str) - 1));
        bss_identifier = atoi(bss_identifier_str);
        parse_bss_identifier(bss_identifier, &bss_info);
        wlan = get_wireless_interface_info(bss_info.band, bss_info.identifier);
        if (NULL == wlan) {
            wlan = assign_wireless_interface_info(&bss_info);
        }
        if (wlan && bss_info.mbssid_enable) {
            configure_ap_enable_mbssid();
            if (bss_info.transmitter) {
                band_transmitter[bss_info.band] = wlan;
            }
        }
        indigo_logger(LOG_LEVEL_DEBUG, "TLV_BSS_IDENTIFIER 0x%x band %d multiple_bssid %d transmitter %d identifier %d\n",
               bss_identifier,
               bss_info.band,
               bss_info.mbssid_enable,
               bss_info.transmitter,
               bss_info.identifier
               );
    } else {
        /* Single wlan case */
        /* reset interfaces info */
        clear_interfaces_resource();

        tlv = find_wrapper_tlv_by_id(req, TLV_HW_MODE);
        if (tlv)
        {
            memset(hw_mode_str, 0, sizeof(hw_mode_str));
            memcpy(hw_mode_str, tlv->value, tlv->len);
            if (find_wrapper_tlv_by_id(req, TLV_HE_6G_ONLY)) {
                band = BAND_6GHZ;
#ifdef _OPENWRT_QTI_
                dut.ap_interface_6g = 1;
#endif
            } else if (!strncmp(hw_mode_str, "a", 1)) {
                band = BAND_5GHZ;
#ifdef _OPENWRT_QTI_
                dut.ap_interface_5g = 1;
#endif
            } else {
                band = BAND_24GHZ;
#ifdef _OPENWRT_QTI_
                dut.ap_interface_2g = 1;
#endif
            }
            /* Single wlan use ID 1 */
            bss_info.band = band;
            bss_info.identifier = 1;
            wlan = assign_wireless_interface_info(&bss_info);
        }
#ifdef _OPENWRT_QTI_
        /**** PLACEHOLDER CONDITION ****/
        if (dut.ap_interface_5g && dut.ap_interface_2g)
            dut.ap_is_dual = 1;;
#endif
    }
    if (wlan) {
#if _OPENWRT_QTI_
        len_2 = generate_wireless_config(buffer, sizeof(buffer),
                req, wlan, &dut);
#else
        indigo_logger(LOG_LEVEL_DEBUG, "ifname %s hostapd conf file %s\n",
               wlan ? wlan->ifname : "n/a",
               wlan ? wlan->hapd_conf_file: "n/a"
               );
        len_2 = generate_hostapd_config(buffer, sizeof(buffer), req, wlan);
#endif
        if (len_2)
        {
#if HOSTAPD_SUPPORT_MBSSID
            if (bss_info.mbssid_enable && !bss_info.transmitter) {
                if (band_transmitter[bss_info.band]) {
                    append_file(band_transmitter[bss_info.band]->hapd_conf_file, buffer, len_2);
                }
                memset(wlan->hapd_conf_file, 0, sizeof(wlan->hapd_conf_file));
            }
            else
#endif
                write_file(wlan->hapd_conf_file, buffer, len_2);
        } else {
            message = "Failed to generate hostapd configuration file.";
            goto done;
        }
    }
    show_wireless_interface_info();
    /* Generate hostapd configuration file [End] */

    tlv = find_wrapper_tlv_by_id(req, TLV_WSC_CONFIG_ONLY);
    if (tlv) {
        /* Configure only (without starting hostapd) */
        message = "Configure wsc ap successfully. (Configure only)";
        goto done;
    }

    /* Start hostapd [Begin] */
#ifdef _WTS_OPENWRT_
    openwrt_apply_radio_config();
    // DFS wait again if set wlan params after hostapd starts
    iterate_all_wlan_interfaces(start_ap_set_wlan_params);
#endif

    memset(buffer, 0, sizeof(buffer));
#if (defined(ANDROID) || defined(MDM))
    snprintf(buffer, sizeof(buffer), "%s -t -g %s %s %s &",
        get_hapd_full_exec_path(),
        get_hapd_ctrl_path_no_iface(),
        get_hostapd_debug_arguments(),
        get_all_hapd_conf_files(&swap_hostapd));
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, buffer);
    len_3 = system(buffer);
    sleep(5);
#else
#ifndef _OPENWRT_QTI_
    snprintf(buffer, sizeof(buffer), "%s -B -t -P /var/run/hostapd.pid -g %s %s -f /var/log/hostapd.log %s",
        get_hapd_full_exec_path(),
        get_hapd_global_ctrl_path(),
        get_hostapd_debug_arguments(),
        get_all_hapd_conf_files(&swap_hostapd));
    len_3 = system(buffer);
    sleep(1);
#endif
#endif
    /* Bring up VAPs with MBSSID disable using WFA hostapd */
    if (swap_hostapd) {
#ifdef HOSTAPD_SUPPORT_MBSSID_WAR
        indigo_logger(LOG_LEVEL_INFO, "Use WFA hostapd for MBSSID disable VAPs with RNR");
        system("cp /overlay/hostapd /usr/sbin/hostapd");
        use_openwrt_wpad = 0;
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "%s -B -t -P /var/run/hostapd_1.pid %s -f /var/log/hostapd_1.log %s",
                get_hapd_full_exec_path(),
                get_hostapd_debug_arguments(),
                get_all_hapd_conf_files(&swap_hostapd));
        len_3 = system(buffer);
        sleep(1);
#endif
    }

#ifndef _WTS_OPENWRT_
    iterate_all_wlan_interfaces(start_ap_set_wlan_params);
#endif

    bridge_init(get_wlans_bridge());
    if (len_3 == 0)
        message = "Confiugre and start wsc ap successfully. (Configure and start)";
    else
        message = "Failed to start hostapd.";
    /* Start hostapd [End] */

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, (len_1 == 0 || len_2 > 0 || len_3 == 0) ? TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}
/* deprecated */
int create_bridge_network_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int err = 0;
    char static_ip[S_BUFFER_LEN];
    struct tlv_hdr *tlv;
    char *message = TLV_VALUE_CREATE_BRIDGE_OK;

    /* TLV: TLV_STATIC_IP */
    memset(static_ip, 0, sizeof(static_ip));
    tlv = find_wrapper_tlv_by_id(req, TLV_STATIC_IP);
    if (tlv) {
        memcpy(static_ip, tlv->value, tlv->len);
    } else {
        message = TLV_VALUE_CREATE_BRIDGE_NOT_OK;
        err = -1;
        goto response;
    }

    /* Create new bridge */
    create_bridge(get_wlans_bridge());

    add_all_wireless_interface_to_bridge(get_wlans_bridge());

    set_interface_ip(get_wlans_bridge(), static_ip);

    response:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, err >= 0 ? TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

// Bytes to DUT : 01 50 06 00 ed ff ff 00 55 0c 31 39 32 2e 31 36 38 2e 31 30 2e 33
// RESP :{<ResponseTLV.STATUS: 40961>: '0', <ResponseTLV.MESSAGE: 40960>: 'Static Ip successfully assigned to wireless interface'}
int assign_static_ip_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    char buffer[64];
    struct tlv_hdr *tlv = NULL;
#ifndef _OPENWRT_QTI_
    char *ifname = NULL;
    int len = 0;
#endif
    char *message = TLV_VALUE_ASSIGN_STATIC_IP_OK;

    memset(buffer, 0, sizeof(buffer));
    tlv = find_wrapper_tlv_by_id(req, TLV_STATIC_IP);
    if (tlv) {
        memcpy(buffer, tlv->value, tlv->len);
    } else {
        message = "Failed.";
        goto response;
    }

#ifndef _OPENWRT_QTI_
    if (is_bridge_created()) {
        ifname = get_wlans_bridge();
    } else {
        ifname = get_wireless_interface();
    }
    /* Release IP address from interface */
    reset_interface_ip(ifname);
    /* Bring up interface */
    control_interface(ifname, "up");
    /* Set IP address with network mask */
    strlcat(buffer, "/24", sizeof(buffer));
    len = set_interface_ip(ifname, buffer);
    if (len) {
        message = TLV_VALUE_ASSIGN_STATIC_IP_NOT_OK;
    }
#endif
    response:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
#ifdef _OPENWRT_QTI_
    fill_wrapper_tlv_byte(resp, TLV_STATUS, TLV_VALUE_STATUS_OK);
#else
    fill_wrapper_tlv_byte(resp, TLV_STATUS, len == 0 ? TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
#endif
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

// Bytes to DUT : 01 50 01 00 ee ff ff
// RESP: {<ResponseTLV.STATUS: 40961>: '0', <ResponseTLV.MESSAGE: 40960>: '9c:b6:d0:19:40:c7', <ResponseTLV.DUT_MAC_ADDR: 40963>: '9c:b6:d0:19:40:c7'}
int get_mac_addr_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    struct tlv_hdr *tlv;
    struct wpa_ctrl *w = NULL;

    int status = TLV_VALUE_STATUS_NOT_OK;
    size_t resp_len = 0;
    char *message = TLV_VALUE_NOT_OK;

    char cmd[16];
    char response[L_BUFFER_LEN];

    char band[S_BUFFER_LEN];
    char ssid[S_BUFFER_LEN];
    char role[S_BUFFER_LEN];

    char connected_freq[S_BUFFER_LEN];
    char connected_ssid[S_BUFFER_LEN];
    char mac_addr[S_BUFFER_LEN] = {0};
    int bss_identifier = 0;
    struct interface_info* wlan = NULL;
    char bss_identifier_str[16];
    struct bss_identifier_info bss_info;
    char buff[S_BUFFER_LEN];
#ifdef _OPENWRT_QTI_
    char *ifname;
#endif

    if (req->tlv_num == 0) {
        get_mac_address(mac_addr, sizeof(mac_addr), get_wireless_interface());
        status = TLV_VALUE_STATUS_OK;
        message = TLV_VALUE_OK;

        goto done;
    } else {
        /* TLV: TLV_ROLE */
        memset(role, 0, sizeof(role));
        tlv = find_wrapper_tlv_by_id(req, TLV_ROLE);
        if (tlv) {
            memcpy(role, tlv->value, tlv->len);
        }

        /* TLV: TLV_BAND */
        memset(band, 0, sizeof(band));
        tlv = find_wrapper_tlv_by_id(req, TLV_BAND);
        if (tlv) {
            memcpy(band, tlv->value, tlv->len);
        }

        /* TLV: TLV_SSID */
        memset(ssid, 0, sizeof(ssid));
        tlv = find_wrapper_tlv_by_id(req, TLV_SSID);
        if (tlv) {
            memcpy(ssid, tlv->value, tlv->len);
        }

        memset(&bss_info, 0, sizeof(bss_info));
        tlv = find_wrapper_tlv_by_id(req, TLV_BSS_IDENTIFIER);
        if (tlv) {
            memset(bss_identifier_str, 0, sizeof(bss_identifier_str));
            memcpy(bss_identifier_str, tlv->value,
                   MIN(tlv->len, sizeof(bss_identifier_str) - 1));
            bss_identifier = atoi(bss_identifier_str);
            parse_bss_identifier(bss_identifier, &bss_info);

            indigo_logger(LOG_LEVEL_DEBUG, "TLV_BSS_IDENTIFIER 0x%x identifier %d band %d\n",
                    bss_identifier,
                    bss_info.identifier,
                    bss_info.band
                    );
        } else {
            bss_info.identifier = -1;
        }
    }

    if (atoi(role) == DUT_TYPE_STAUT) {
        w = wpa_ctrl_open(get_wpas_ctrl_path());
    } else if (atoi(role) == DUT_TYPE_P2PUT) {
        /* Get P2P GO/Client or Device MAC */
        if (get_p2p_mac_addr(mac_addr, sizeof(mac_addr))) {
            indigo_logger(LOG_LEVEL_INFO, "Can't find P2P Device MAC. Use wireless IF MAC");
            get_mac_address(mac_addr, sizeof(mac_addr), get_wireless_interface());
        }
        status = TLV_VALUE_STATUS_OK;
        message = TLV_VALUE_OK;
        goto done;
    } else {
        wlan = get_wireless_interface_info(bss_info.band, bss_info.identifier);
        if (!wlan) {
            indigo_logger(LOG_LEVEL_ERROR, "Failed to get wireless intf info");
            status = TLV_VALUE_STATUS_NOT_OK;
            message = TLV_VALUE_NOT_OK;
            goto done;
        }
#ifdef _OPENWRT_QTI_
        ifname = owrt_get_ap_wireless_interface();

        if (dut.ap_is_dual) {
            if (bss_info.band == 0) {
               /* band id is '0'for 2.4G and '1' for 5G*/
               snprintf(buff, sizeof(buff), "/var/run/hostapd-wifi%d/ath%d",
                        bss_info.band + 1, bss_info.band + 1);
            } else if (bss_info.band == 1) {
               snprintf(buff, sizeof(buff), "/var/run/hostapd-wifi%d/ath%d",
                        bss_info.band - 1, bss_info.band - 1);
            } else {
               snprintf(buff, sizeof(buff), "/var/run/hostapd-wifi%d/ath%d",
                        bss_info.band, bss_info.band);
            }
        } else {
            if (bss_info.identifier > 1) {
               snprintf(buff, sizeof(buff), "%s%d", ifname, (bss_info.identifier - 1));
            } else {
               snprintf(buff, sizeof(buff), "%s", ifname);
            }
            get_mac_address(mac_addr, sizeof(mac_addr), buff);
            status = TLV_VALUE_STATUS_OK;
            message = TLV_VALUE_OK;
            goto done;
        }
        w = wpa_ctrl_open(buff);
#else
        w = wpa_ctrl_open(get_hapd_ctrl_path_by_id(wlan));
#endif
    }

    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to %s", atoi(role) == DUT_TYPE_STAUT ? "wpa_supplicant" : "hostapd");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_NOT_OK;
        goto done;
    }

    /* Assemble hostapd command */
    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "STATUS");
    /* Send command to hostapd UDS socket */
    resp_len = sizeof(response) - 1;
    memset(response, 0, sizeof(response));
    wpa_ctrl_request(w, cmd, strlen(cmd), response, &resp_len, NULL);

    /* Check response */
    get_key_value(connected_freq, response, "freq");

    memset(mac_addr, 0, sizeof(mac_addr));
    if (atoi(role) == DUT_TYPE_STAUT) {
        get_key_value(connected_ssid, response, "ssid");
        get_key_value(mac_addr, response, "address");
    } else {
#if HOSTAPD_SUPPORT_MBSSID
        if(bss_info.identifier >= 0) {
            if (!wlan) {
                indigo_logger(LOG_LEVEL_ERROR, "wlan intf is NULL");
                status = TLV_VALUE_STATUS_NOT_OK;
                message = TLV_VALUE_NOT_OK;
                goto done;
            }
            snprintf(buff, sizeof(buff), "ssid[%d]", wlan->hapd_bss_id);
            get_key_value(connected_ssid, response, buff);
            snprintf(buff, sizeof(buff), "bssid[%d]", wlan->hapd_bss_id);
            get_key_value(mac_addr, response, buff);
        } else {
            get_key_value(connected_ssid, response, "ssid[0]");
            get_key_value(mac_addr, response, "bssid[0]");
        }
#else
        get_key_value(connected_ssid, response, "ssid[0]");
        get_key_value(mac_addr, response, "bssid[0]");
#endif
    }

    if (bss_info.identifier >= 0) {
        indigo_logger(LOG_LEVEL_DEBUG, "Get mac_addr %s\n", mac_addr);
        status = TLV_VALUE_STATUS_OK;
        message = TLV_VALUE_OK;
        goto done;
    }

    /* Check band and connected freq*/
    if (strlen(band)) {
        int band_id = 0;

        if (strcmp(band, "2.4GHz") == 0)
            band_id = BAND_24GHZ;
        else if (strcmp(band, "5GHz") == 0)
            band_id = BAND_5GHZ;
        else if (strcmp(band, "6GHz") == 0)
            band_id = BAND_6GHZ;
        if (verify_band_from_freq(atoi(connected_freq), band_id) == 0) {
            status = TLV_VALUE_STATUS_OK;
            message = TLV_VALUE_OK;
        } else {
            status = TLV_VALUE_STATUS_NOT_OK;
            message = "Unable to get mac address associated with the given band";
            goto done;
        }
    }

    /* Check SSID and connected SSID */
    if (strlen(ssid)) {
        if (strcmp(ssid, connected_ssid) == 0) {
            status = TLV_VALUE_STATUS_OK;
            message = TLV_VALUE_OK;
        } else {
            status = TLV_VALUE_STATUS_NOT_OK;
            message = "Unable to get mac address associated with the given ssid";
            goto done;
        }
    }

    /* TODO: BSSID */

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    if (status == TLV_VALUE_STATUS_OK) {
        fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(mac_addr), mac_addr);
        fill_wrapper_tlv_bytes(resp, TLV_DUT_MAC_ADDR, strlen(mac_addr), mac_addr);
    } else {
        fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    }
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int start_loopback_server(struct packet_wrapper *req, struct packet_wrapper *resp) {
    char local_ip[256];
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_LOOPBACK_SVR_START_NOT_OK;
    char tool_udp_port[16] = {0};
    char if_name[32] = {0};

    /* Find network interface. If P2P Group or bridge exists, then use it. Otherwise, it uses the initiation value. */
    memset(local_ip, 0, sizeof(local_ip));
    if (get_p2p_group_if(if_name, sizeof(if_name)) == 0 && find_interface_ip(local_ip, sizeof(local_ip), if_name)) {
        indigo_logger(LOG_LEVEL_DEBUG, "use %s", if_name);
    } else if (find_interface_ip(local_ip, sizeof(local_ip), get_wlans_bridge())) {
        indigo_logger(LOG_LEVEL_DEBUG, "use %s", get_wlans_bridge());
    } else if (find_interface_ip(local_ip, sizeof(local_ip), get_wireless_interface())) {
        indigo_logger(LOG_LEVEL_DEBUG, "use %s", get_wireless_interface());
// #ifdef __TEST__
    } else if (find_interface_ip(local_ip, sizeof(local_ip), "eth0")) {
        indigo_logger(LOG_LEVEL_DEBUG, "use %s", "eth0");
// #endif /* __TEST__ */
    } else {
        indigo_logger(LOG_LEVEL_ERROR, "No available interface");
        goto done;
    }
    /* Start loopback */
    if (!loopback_server_start(local_ip, tool_udp_port, LOOPBACK_TIMEOUT)) {
        status = TLV_VALUE_STATUS_OK;
        message = TLV_VALUE_LOOPBACK_SVR_START_OK;
    }
done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    fill_wrapper_tlv_bytes(resp, TLV_LOOP_BACK_SERVER_PORT, strlen(tool_udp_port), tool_udp_port);

    return 0;
}

// RESP: {<ResponseTLV.STATUS: 40961>: '0', <ResponseTLV.MESSAGE: 40960>: 'Loopback server in idle state'}
int stop_loop_back_server_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    /* Stop loopback */
    if (loopback_server_status()) {
        loopback_server_stop();
    }
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, TLV_VALUE_STATUS_OK);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(TLV_VALUE_LOOP_BACK_STOP_OK), TLV_VALUE_LOOP_BACK_STOP_OK);

    return 0;
}

int send_ap_disconnect_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int status = TLV_VALUE_STATUS_NOT_OK;
    char buffer[S_BUFFER_LEN];
    char *message = NULL;
#ifndef _OPENWRT_QTI_
    size_t resp_len;
    struct tlv_hdr *tlv = NULL;
    struct wpa_ctrl *w = NULL;
    char *parameter[] = {"pidof", get_hapd_exec_file(), NULL};
    char address[32];
    char response[S_BUFFER_LEN];
    int len;
#endif

    /* Check hostapd status. TODO: it may use UDS directly */
    memset(buffer, 0, sizeof(buffer));
#ifndef _OPENWRT_QTI_
    len = pipe_command(buffer, sizeof(buffer), "/bin/pidof", parameter);
    if (len == 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to find hostapd PID");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_HOSTAPD_NOT_OK;
        goto done;
    }
    /* Open hostapd UDS socket */
    w = wpa_ctrl_open(get_hapd_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to hostapd");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_HOSTAPD_CTRL_NOT_OK;
        goto done;
    }
    /* ControlApp on DUT */
    /* TLV: TLV_ADDRESS */
    memset(address, 0, sizeof(address));
    tlv = find_wrapper_tlv_by_id(req, TLV_ADDRESS);
    if (tlv) {
        memcpy(address, tlv->value, tlv->len);
    } else {
        indigo_logger(LOG_LEVEL_ERROR, "Missed TLV:Address");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_INSUFFICIENT_TLV;
        goto done;
    }
    /* Assemble hostapd command */
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "DISASSOCIATE %s reason=1", address);
    /* Send command to hostapd UDS socket */
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command. Response: %s", response);
        message = TLV_VALUE_HOSTAPD_RESP_NOT_OK;
        goto done;
    }
#else
    run_system("wifi down");
    sleep(1);
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_HOSTAPD_STOP_OK;
    goto done;
#endif
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_HOSTAPD_STOP_OK;
done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
#ifndef _OPENWRT_QTI_
    if (w) {
        wpa_ctrl_close(w);
    }
#endif
    return 0;
}

int set_ap_parameter_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = NULL;
#ifndef _OPENWRT_QTI_
    char response[1024];
    char param_name[32];
    char param_value[256];
    size_t resp_len;
    char buffer[8192];
    struct tlv_hdr *tlv = NULL;
#endif
    struct wpa_ctrl *w = NULL;
#ifdef _OPENWRT_QTI_
    set_ap_params(req);

    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;
    goto done;
#else

    /* Open hostapd UDS socket */
    w = wpa_ctrl_open(get_hapd_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to hostapd");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_HOSTAPD_CTRL_NOT_OK;
        goto done;
    }

    /* ControlApp on DUT */
    /* TLV: MBO_ASSOC_DISALLOW or GAS_COMEBACK_DELAY */
    memset(param_value, 0, sizeof(param_value));
    tlv = find_wrapper_tlv_by_id(req, TLV_MBO_ASSOC_DISALLOW);
    if (!tlv) {
        tlv = find_wrapper_tlv_by_id(req, TLV_GAS_COMEBACK_DELAY);
    }
    if (tlv && find_tlv_config_name(tlv->id) != NULL) {
        strlcpy(param_name, find_tlv_config_name(tlv->id), sizeof(param_name));
        memcpy(param_value, tlv->value, tlv->len);
    } else {
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_INSUFFICIENT_TLV;
        indigo_logger(LOG_LEVEL_ERROR, "Missed TLV: TLV_MBO_ASSOC_DISALLOW or TLV_GAS_COMEBACK_DELAY");
        goto done;
    }
    /* Assemble hostapd command */
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "SET %s %s", param_name, param_value);
    /* Send command to hostapd UDS socket */
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command. Response: %s", response);
        message = TLV_VALUE_HOSTAPD_RESP_NOT_OK;
        goto done;
    }
#endif
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;
done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int send_ap_btm_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = NULL;
    struct tlv_hdr *tlv = NULL;
    struct wpa_ctrl *w = NULL;
#ifndef _OPENWRT_QTI_
    char response[4096];
    char buffer[1024];
    size_t resp_len;
    char request[4096];
#endif
    char bssid[256];
    char disassoc_imminent[256];
    char disassoc_timer[256];
    char candidate_list[256];
    char reassoc_retry_delay[256];
    char bss_term_bit[256];
    char bss_term_tsf[256];
    char bss_term_duration[256];

    memset(bssid, 0, sizeof(bssid));
    memset(disassoc_imminent, 0, sizeof(disassoc_imminent));
    memset(disassoc_timer, 0, sizeof(disassoc_timer));
    memset(candidate_list, 0, sizeof(candidate_list));
    memset(reassoc_retry_delay, 0, sizeof(reassoc_retry_delay));
    memset(bss_term_bit, 0, sizeof(bss_term_bit));
    memset(bss_term_tsf, 0, sizeof(bss_term_tsf));
    memset(bss_term_duration, 0, sizeof(bss_term_duration));

    /* ControlApp on DUT */
    /* TLV: BSSID (required) */
    tlv = find_wrapper_tlv_by_id(req, TLV_BSSID);
    if (tlv) {
        memcpy(bssid, tlv->value, tlv->len);
    }
    /* DISASSOC_IMMINENT            disassoc_imminent=%s */
    tlv = find_wrapper_tlv_by_id(req, TLV_DISASSOC_IMMINENT);
    if (tlv) {
        memcpy(disassoc_imminent, tlv->value, tlv->len);
#ifdef _OPENWRT_QTI_
        dut.ap_btmreq_disassoc_imnt = atoi(disassoc_imminent);
#endif
    }
    /* DISASSOC_TIMER               disassoc_timer=%s */
    tlv = find_wrapper_tlv_by_id(req, TLV_DISASSOC_TIMER);
    if (tlv) {
        memcpy(disassoc_timer, tlv->value, tlv->len);
#ifdef _OPENWRT_QTI_
        dut.ap_disassoc_timer = atoi(disassoc_timer);
#endif
    }
    /* REASSOCIAITION_RETRY_DELAY   mbo=0:{}:0 */
    tlv = find_wrapper_tlv_by_id(req, TLV_REASSOCIAITION_RETRY_DELAY);
    if (tlv) {
        memcpy(reassoc_retry_delay, tlv->value, tlv->len);
    }
    /* CANDIDATE_LIST              pref=1 */
    tlv = find_wrapper_tlv_by_id(req, TLV_CANDIDATE_LIST);
    if (tlv) {
        memcpy(candidate_list, tlv->value, tlv->len);
    }
    /* BSS_TERMINATION              bss_term_bit */
    tlv = find_wrapper_tlv_by_id(req, TLV_BSS_TERMINATION);
    if (tlv) {
        memcpy(bss_term_bit, tlv->value, tlv->len);
#ifdef _OPENWRT_QTI_
        dut.ap_btmreq_term_bit = atoi(bss_term_bit);
#endif
    }
    /* BSS_TERMINATION_TSF          bss_term_tsf */
    tlv = find_wrapper_tlv_by_id(req, TLV_BSS_TERMINATION_TSF);
    if (tlv) {
        memcpy(bss_term_tsf, tlv->value, tlv->len);
#ifdef _OPENWRT_QTI_
        dut.ap_btmreq_bss_term_tsf = atoi(bss_term_tsf);
#endif
    }
    /* BSS_TERMINATION_DURATION     bss_term_duration */
    tlv = find_wrapper_tlv_by_id(req, TLV_BSS_TERMINATION_DURATION);
    if (tlv) {
        memcpy(bss_term_duration, tlv->value, tlv->len);
#ifdef _OPENWRT_QTI_
        dut.ap_btmreq_bss_term_dur = atoi(bss_term_duration);
#endif
    }

#ifdef _OPENWRT_QTI_
    send_ap_btm_req(bssid);
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;
    goto done;
#else
    /* Assemble hostapd command for BSS_TM_REQ */
    memset(request, 0, sizeof(request));
    snprintf(request, sizeof(request), "BSS_TM_REQ %s", bssid);
    /*  disassoc_imminent=%s */
    if (strlen(disassoc_imminent)) {
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), " disassoc_imminent=%s", disassoc_imminent);
        strlcat(request, buffer, sizeof(request));
    }
    /* disassoc_timer=%s */
    if (strlen(disassoc_timer)) {
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), " disassoc_timer=%s", disassoc_timer);
        strlcat(request, buffer, sizeof(request));
    }
    /* reassoc_retry_delay=%s */
    if (strlen(reassoc_retry_delay)) {
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), " mbo=0:%s:0", reassoc_retry_delay);
        strlcat(request, buffer, sizeof(request));
    }
    /* if bss_term_bit && bss_term_tsf && bss_term_duration, then bss_term={bss_term_tsf},{bss_term_duration} */
    if (strlen(bss_term_bit) && strlen(bss_term_tsf) && strlen(bss_term_duration) ) {
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), " bss_term=%s,%s", bss_term_tsf, bss_term_duration);
        strlcat(request, buffer, sizeof(request));
    }
    /* candidate_list */
    if (strlen(candidate_list) && atoi(candidate_list) == 1) {
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), " pref=1");
        strlcat(request, buffer, sizeof(request));
    }
    indigo_logger(LOG_LEVEL_DEBUG, "cmd:%s", request);

    /* Open hostapd UDS socket */
    w = wpa_ctrl_open(get_hapd_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to hostapd");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_HOSTAPD_CTRL_NOT_OK;
        goto done;
    }
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, request, strlen(request), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command. Response: %s", response);
        message = TLV_VALUE_HOSTAPD_RESP_NOT_OK;
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;
#endif
done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int trigger_ap_channel_switch(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = NULL;
    struct tlv_hdr *tlv = NULL;
    struct wpa_ctrl *w = NULL;
    char request[S_BUFFER_LEN];
#ifndef _OPENWRT_QTI_
    char response[S_BUFFER_LEN];
    size_t resp_len;
#else
    char buf[L_BUFFER_LEN];
    int width = 0;
    char *ifname;
#endif
    char channel[64];
    char frequency[64];
    int freq, center_freq, offset;

    memset(channel, 0, sizeof(channel));
    memset(frequency, 0, sizeof(frequency));

    /* ControlApp on DUT */
    /* TLV: TLV_CHANNEL (required) */
    tlv = find_wrapper_tlv_by_id(req, TLV_CHANNEL);
    if (tlv) {
        memcpy(channel, tlv->value, MIN(tlv->len, sizeof(channel) - 1));
    } else {
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_INSUFFICIENT_TLV;
        indigo_logger(LOG_LEVEL_ERROR, "Missed TLV: TLV_CHANNEL");
        goto done;
    }
    /* TLV_FREQUENCY (required) */
    tlv = find_wrapper_tlv_by_id(req, TLV_FREQUENCY);
    if (tlv) {
        memcpy(frequency, tlv->value, MIN(tlv->len, sizeof(frequency) - 1));
    } else {
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_INSUFFICIENT_TLV;
        indigo_logger(LOG_LEVEL_ERROR, "Missed TLV: TLV_FREQUENCY");
    }

    center_freq = 5000 + get_center_freq_index(atoi(channel), 1) * 5;
    freq = atoi(frequency);
    if ((center_freq == freq + 30) || (center_freq == freq - 10))
        offset = 1;
    else
        offset = -1;
    /* Assemble hostapd command for channel switch */
    memset(request, 0, sizeof(request));
    snprintf(request, sizeof(request), "CHAN_SWITCH 10 %s center_freq1=%d sec_channel_offset=%d bandwidth=80 vht", frequency, center_freq, offset);
    indigo_logger(LOG_LEVEL_INFO, "%s", request);

#ifdef _OPENWRT_QTI_
    if (dut.ap_chwidth == AP_20)
        width = 20;
    else if (dut.ap_chwidth == AP_40)
        width = 40;
    else if (dut.ap_chwidth == AP_80)
        width = 80;

    ifname = owrt_get_ap_wireless_interface();
    snprintf(buf, sizeof(buf), "cfg80211tool %s doth_ch_chwidth %d 3 %d", ifname, atoi(channel), width);
    run_system(buf);
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;
    goto done;
#else
    /* Open hostapd UDS socket */
    w = wpa_ctrl_open(get_hapd_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to hostapd");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_HOSTAPD_CTRL_NOT_OK;
        goto done;
    }
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, request, strlen(request), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command. Response: %s", response);
        message = TLV_VALUE_HOSTAPD_RESP_NOT_OK;
        goto done;
    }
#endif
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;
done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int get_ip_addr_handler(struct packet_wrapper *req,
                               struct packet_wrapper *resp)
{
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = NULL;
    char buffer[64] = {0};
    struct tlv_hdr *tlv = NULL;
    char value[16] = {0}, if_name[32] = {0};
    int role = 0;

    memset(value, 0, sizeof(value));
    tlv = find_wrapper_tlv_by_id(req, TLV_ROLE);
    if (tlv) {
            memcpy(value, tlv->value, MIN(tlv->len, sizeof(value) - 1));
            role = atoi(value);
    }

    if (role == DUT_TYPE_P2PUT &&
        get_p2p_group_if(if_name, sizeof(if_name)) == 0 &&
        find_interface_ip(buffer, sizeof(buffer), if_name)) {
        status = TLV_VALUE_STATUS_OK;
        message = TLV_VALUE_OK;
    } else if (find_interface_ip(buffer, sizeof(buffer), get_wlans_bridge())) {
        status = TLV_VALUE_STATUS_OK;
        message = TLV_VALUE_OK;
    } else if (find_interface_ip(buffer, sizeof(buffer),
               get_wireless_interface())) {
        status = TLV_VALUE_STATUS_OK;
        message = TLV_VALUE_OK;
    } else {
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_NOT_OK;
    }
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (status == TLV_VALUE_STATUS_OK) {
        fill_wrapper_tlv_bytes(resp, TLV_DUT_WLAN_IP_ADDR,
                               strlen(buffer), buffer);
    }
    return 0;
}

int stop_sta_handler(struct packet_wrapper *req,
                            struct packet_wrapper *resp)
{
    int len = 0, reset = 0;
    char buffer[S_BUFFER_LEN], reset_type[16];
    char *parameter[] = {"pidof", get_wpas_exec_file(), NULL};
    char *message = NULL;
    struct tlv_hdr *tlv = NULL;

    /* TLV: RESET_TYPE */
    tlv = find_wrapper_tlv_by_id(req, TLV_RESET_TYPE);
    memset(reset_type, 0, sizeof(reset_type));
    if (tlv) {
        memcpy(reset_type, tlv->value, MIN(tlv->len, sizeof(reset_type) - 1));
        reset = atoi(reset_type);
        indigo_logger(LOG_LEVEL_DEBUG, "Reset Type: %d", reset);
    }

    if (reset == RESET_TYPE_INIT) {
        open_tc_app_log();
        /* clean the log */
#ifndef ANDROID
        system("rm -rf /var/log/supplicant.log >/dev/null 2>/dev/null");
#endif

        /* remove pac file if needed */
        if (strlen(pac_file_path)) {
            remove_pac_file(pac_file_path);
            memset(pac_file_path, 0, sizeof(pac_file_path));
        }
    }

    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "killall %s 1>/dev/null 2>/dev/null",
             get_wpas_exec_file());
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, buffer);
    system(buffer);
    sleep(2);
    sta_configured = 0;
    sta_started = 0;

    len = reset_interface_ip(get_wireless_interface());
    if (len) {
        indigo_logger(LOG_LEVEL_DEBUG, "Failed to free IP address");
    }
    sleep(1);

    len = pipe_command(buffer, sizeof(buffer), "/bin/pidof", parameter);
    if (len) {
        message = TLV_VALUE_WPA_S_STOP_NOT_OK;
    } else {
        message = TLV_VALUE_WPA_S_STOP_OK;
    }

    /* Test case teardown case */
    if (reset == RESET_TYPE_TEARDOWN) {
        close_tc_app_log();
    }

    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, len == 0 ?
                          TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

#ifdef _RESERVED_
/* The function is reserved for the defeault wpas config */
#define WPAS_DEFAULT_CONFIG_SSID                    "QuickTrack"
#define WPAS_DEFAULT_CONFIG_WPA_KEY_MGMT            "WPA-PSK"
#define WPAS_DEFAULT_CONFIG_PROTO                   "RSN"
#define HOSTAPD_DEFAULT_CONFIG_RSN_PAIRWISE         "CCMP"
#define WPAS_DEFAULT_CONFIG_WPA_PASSPHRASE          "12345678"

static void append_wpas_network_default_config(struct packet_wrapper *wrapper)
{
    if (find_wrapper_tlv_by_id(wrapper, TLV_SSID) == NULL) {
        add_wrapper_tlv(wrapper, TLV_SSID, strlen(WPAS_DEFAULT_CONFIG_SSID),
                        WPAS_DEFAULT_CONFIG_SSID);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_WPA_KEY_MGMT) == NULL) {
        add_wrapper_tlv(wrapper, TLV_WPA_KEY_MGMT,
                        strlen(WPAS_DEFAULT_CONFIG_WPA_KEY_MGMT),
                        WPAS_DEFAULT_CONFIG_WPA_KEY_MGMT);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_PROTO) == NULL) {
        add_wrapper_tlv(wrapper, TLV_PROTO, strlen(WPAS_DEFAULT_CONFIG_PROTO),
                        WPAS_DEFAULT_CONFIG_PROTO);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_RSN_PAIRWISE) == NULL) {
        add_wrapper_tlv(wrapper, TLV_RSN_PAIRWISE,
                        strlen(HOSTAPD_DEFAULT_CONFIG_RSN_PAIRWISE),
                        HOSTAPD_DEFAULT_CONFIG_RSN_PAIRWISE);
    }
    if (find_wrapper_tlv_by_id(wrapper, TLV_WPA_PASSPHRASE) == NULL) {
        add_wrapper_tlv(wrapper, TLV_WPA_PASSPHRASE,
                        strlen(WPAS_DEFAULT_CONFIG_WPA_PASSPHRASE),
                        WPAS_DEFAULT_CONFIG_WPA_PASSPHRASE);
    }
}
#endif /* _RESERVED_ */

static int generate_wpas_config(char *buffer, int buffer_size,
                                struct packet_wrapper *wrapper)
{
    int i;
    char value[S_BUFFER_LEN], cfg_item[2*S_BUFFER_LEN], buf[S_BUFFER_LEN];
    int ieee80211w_configured = 0;
    int transition_mode_enabled = 0;
    int owe_configured = 0;
    int sae_only = 0;
    struct tlv_to_config_name* cfg = NULL;

#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
    snprintf(buffer, buffer_size, "ctrl_interface=%s\nap_scan=1\npmf=1\n",
             get_wpas_ctrl_path_no_iface());
#else
    snprintf(buffer, buffer_size, "ctrl_interface=%s\nap_scan=1\npmf=1\n",
             get_wpas_ctrl_path());
#endif

    for (i = 0; i < wrapper->tlv_num; i++) {
        cfg = find_wpas_global_config_name(wrapper->tlv[i]->id);
        if (cfg) {
            memset(value, 0, sizeof(value));
            memcpy(value, wrapper->tlv[i]->value, wrapper->tlv[i]->len);
            snprintf(cfg_item, sizeof(cfg_item), "%s=%s\n",
                     cfg->config_name, value);
            strlcat(buffer, cfg_item, buffer_size);
        }
    }
    strlcat(buffer, "network={\n", buffer_size);

#ifdef _RESERVED_
    /* The function is reserved for the defeault wpas config */
    append_wpas_network_default_config(wrapper);
#endif /* _RESERVED_ */

    for (i = 0; i < wrapper->tlv_num; i++) {
        cfg = find_tlv_config(wrapper->tlv[i]->id);
        if (cfg && find_wpas_global_config_name(wrapper->tlv[i]->id) == NULL) {
            memset(value, 0, sizeof(value));
            memcpy(value, wrapper->tlv[i]->value, wrapper->tlv[i]->len);

            if ((wrapper->tlv[i]->id == TLV_IEEE80211_W) ||
                (wrapper->tlv[i]->id == TLV_STA_IEEE80211_W)) {
                ieee80211w_configured = 1;
            } else if (wrapper->tlv[i]->id == TLV_KEY_MGMT) {
                if (strstr(value, "WPA-PSK") && strstr(value, "SAE")) {
                    transition_mode_enabled = 1;
                }
                if (!strstr(value, "WPA-PSK") && strstr(value, "SAE")) {
                    sae_only = 1;
                }

                if (strstr(value, "OWE")) {
                    owe_configured = 1;
                }
            } else if ((wrapper->tlv[i]->id == TLV_CA_CERT) &&
                       strcmp("DEFAULT", value) == 0) {
#ifdef ANDROID
                /* When "ca_cert=DEFAULT", Android platform system has no
                 * specific ca-certificates.crt, use ca_path parameter
                 * instead of ca_cert to indicate certificates path.
                 */
                strlcat(buffer, "ca_path=\"/system/etc/security/cacerts\"\n",
                        buffer_size);
                continue;
#else
                snprintf(value, sizeof(value),
                         "/etc/ssl/certs/ca-certificates.crt");
#endif
            } else if (wrapper->tlv[i]->id == TLV_PAC_FILE) {
                memset(pac_file_path, 0, sizeof(pac_file_path));
                snprintf(pac_file_path, sizeof(pac_file_path), "%s", value);
            } else if (wrapper->tlv[i]->id == TLV_SERVER_CERT) {
                memset(buf, 0, sizeof(buf));
                get_server_cert_hash(value, buf, sizeof(buf));
                memcpy(value, buf, sizeof(buf));
            }

            if (cfg->quoted) {
                snprintf(cfg_item, sizeof(cfg_item), "%s=\"%s\"\n",
                         cfg->config_name, value);
                strlcat(buffer, cfg_item, buffer_size);
            } else {
                snprintf(cfg_item, sizeof(cfg_item), "%s=%s\n",
                         cfg->config_name, value);
                strlcat(buffer, cfg_item, buffer_size);
            }
        }
    }

    if (ieee80211w_configured == 0) {
        if (transition_mode_enabled) {
            strlcat(buffer, "ieee80211w=1\n", buffer_size);
        } else if (sae_only) {
            strlcat(buffer, "ieee80211w=2\n", buffer_size);
        } else if (owe_configured) {
            strlcat(buffer, "ieee80211w=2\n", buffer_size);
        }
    }

    /* TODO: merge another file */
    /* python source code:
        if merge_config_file:
        appended_supplicant_conf_str = ""
        existing_conf = StaCommandHelper.get_existing_supplicant_conf()
        wpa_supplicant_dict = StaCommandHelper.__convert_config_str_to_dict(config = wps_config)
        for each_key in existing_conf:
            if each_key not in wpa_supplicant_dict:
                wpa_supplicant_dict[each_key] = existing_conf[each_key]

        for each_supplicant_conf in wpa_supplicant_dict:
            appended_supplicant_conf_str += each_supplicant_conf + "=" + wpa_supplicant_dict[each_supplicant_conf] + "\n"
        wps_config = appended_supplicant_conf_str.rstrip()
    */

    strlcat(buffer, "}\n", buffer_size);
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SUPPLICANT_CONF:\n %s\n", __func__, buffer);
    return strlen(buffer);
}

int configure_sta_handler(struct packet_wrapper *req,
                                 struct packet_wrapper *resp)
{
    int len;
    char buffer[L_BUFFER_LEN];
    char *message = "DUT configured as STA : Configuration file created";

    memset(buffer, 0, sizeof(buffer));
    len = generate_wpas_config(buffer, sizeof(buffer), req);
    if (len) {
        sta_configured = 1;
        write_file(get_wpas_conf_file(), buffer, len);
    }

    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, len > 0 ?
                          TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

int associate_sta_handler(struct packet_wrapper *req,
                                 struct packet_wrapper *resp)
{
    char *message = TLV_VALUE_WPA_S_START_UP_NOT_OK;
    char buffer[256];
    int status = TLV_VALUE_STATUS_NOT_OK;
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
    struct wpa_ctrl *w = NULL;
    size_t resp_len;
    char response[1024];
#endif

#ifdef _OPENWRT_
#else
    system("rfkill unblock wlan");
    sleep(1);
#endif

    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "killall %s 1>/dev/null 2>/dev/null",
             get_wpas_exec_file());
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, buffer);
    system(buffer);
    sleep(5);

    /* Start WPA supplicant */
    memset(buffer, 0 ,sizeof(buffer));
#if defined(ANDROID) || defined(MDM)
    snprintf(buffer, sizeof(buffer), "%s -B -t -c %s %s -i %s%s",
             get_wpas_full_exec_path(),
             get_wpas_conf_file(),
             get_wpas_debug_arguments(),
             get_wireless_interface(),
             get_wpas_log_file_arguments());
#else
    snprintf(buffer, sizeof(buffer),
             "%s -B -t -c %s %s -i %s -f /var/log/supplicant.log",
             get_wpas_full_exec_path(),
             get_wpas_conf_file(),
             get_wpas_debug_arguments(),
             get_wireless_interface());
#endif
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, buffer);
    system(buffer);
    sleep(4);

#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_START_UP_NOT_OK;
        goto done;
    }
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "SCAN");
    memset(response, 0, sizeof(response));
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    sleep(5);
#endif
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_WPA_S_START_UP_OK;

#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
done:
#endif
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    return 0;
}

int send_sta_disconnect_handler(struct packet_wrapper *req,
                                       struct packet_wrapper *resp)
{
    struct wpa_ctrl *w = NULL;
    char *message = TLV_VALUE_WPA_S_DISCONNECT_NOT_OK;
    char buffer[256], response[1024];
    int status;
    size_t resp_len;

    /* Open WPA supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_DISCONNECT_NOT_OK;
        goto done;
    }
    /* Send command to hostapd UDS socket */
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "DISCONNECT");
    memset(response, 0, sizeof(response));
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR,
                      "Failed to execute the command. Response: %s", response);
        status = TLV_VALUE_STATUS_NOT_OK;
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_WPA_S_DISCONNECT_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int send_sta_reconnect_handler(struct packet_wrapper *req,
                                      struct packet_wrapper *resp)
{
    struct wpa_ctrl *w = NULL;
    char *message = TLV_VALUE_WPA_S_RECONNECT_NOT_OK;
    char buffer[256], response[1024];
    int status;
    size_t resp_len;

    /* Open WPA supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_RECONNECT_NOT_OK;
        goto done;
    }
    /* Send command to hostapd UDS socket */
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "RECONNECT");
    memset(response, 0, sizeof(response));
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR,
                      "Failed to execute the command. Response: %s", response);
        status = TLV_VALUE_STATUS_NOT_OK;
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_WPA_S_RECONNECT_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int set_sta_parameter_handler(struct packet_wrapper *req,
                                     struct packet_wrapper *resp)
{
    int status = TLV_VALUE_STATUS_NOT_OK, i;
    size_t resp_len;
    char *message = NULL;
    char buffer[BUFFER_LEN];
    char response[BUFFER_LEN];
    char param_name[32];
    char param_value[256];
    struct tlv_hdr *tlv = NULL;
    struct wpa_ctrl *w = NULL;

    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }

    for (i = 0; i < req->tlv_num; i++) {
        memset(param_name, 0, sizeof(param_name));
        memset(param_value, 0, sizeof(param_value));
        tlv = req->tlv[i];
        if (tlv && find_tlv_config_name(tlv->id) != NULL) {
            strlcpy(param_name, find_tlv_config_name(tlv->id), sizeof(param_name));
            memcpy(param_value, tlv->value, tlv->len);
        } else {
            status = TLV_VALUE_STATUS_NOT_OK;
            message = TLV_VALUE_INSUFFICIENT_TLV;
            indigo_logger(LOG_LEVEL_ERROR, "Missed TLV: tlv is missing");
            goto done;
        }

        /* Assemble wpa_supplicant command */
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "SET %s %s", param_name, param_value);
        /* Send command to wpa_supplicant UDS socket */
        resp_len = sizeof(response) - 1;
        wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
        /* Check response */
        if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
            indigo_logger(LOG_LEVEL_ERROR,
                          "Failed to execute the command. Response: %s",
                          response);
            message = TLV_VALUE_WPA_SET_PARAMETER_NO_OK;
            goto done;
        }
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;
done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int send_sta_btm_query_handler(struct packet_wrapper *req,
                                      struct packet_wrapper *resp)
{
    int status = TLV_VALUE_STATUS_NOT_OK;
    size_t resp_len;
    char *message = TLV_VALUE_WPA_S_BTM_QUERY_NOT_OK;
    char buffer[1024];
    char response[1024];
    char reason_code[256];
    char candidate_list[256] = {0};
    struct tlv_hdr *tlv = NULL;
    struct wpa_ctrl *w = NULL;

    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }
    /* TLV: BTMQUERY_REASON_CODE */
    tlv = find_wrapper_tlv_by_id(req, TLV_BTMQUERY_REASON_CODE);
    if (tlv) {
        memcpy(reason_code, tlv->value, tlv->len);
    } else {
        goto done;
    }

    /* TLV: TLV_CANDIDATE_LIST */
    tlv = find_wrapper_tlv_by_id(req, TLV_CANDIDATE_LIST);
    if (tlv) {
        memcpy(candidate_list, tlv->value, tlv->len);
    }

    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "WNM_BSS_QUERY %s", reason_code);
    if (strcmp(candidate_list, "1") == 0) {
        strlcat(buffer, " list", sizeof(buffer));
    }

    /* Send command to wpa_supplicant UDS socket */
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR,
                      "Failed to execute the command. Response: %s", response);
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int send_sta_anqp_query_handler(struct packet_wrapper *req,
                                       struct packet_wrapper *resp)
{
    int len, status = TLV_VALUE_STATUS_NOT_OK, i;
    char *message = TLV_VALUE_WPA_S_BTM_QUERY_NOT_OK;
    char buffer[1024];
    char response[1024];
    char bssid[256];
    char anqp_info_id[256];
    struct tlv_hdr *tlv = NULL;
    struct wpa_ctrl *w = NULL;
    size_t resp_len;
    char *token = NULL;
    char *delimit = ";";
    char realm[S_BUFFER_LEN] = {0};
    char *save_ptr;

    /* It may need to check whether to just scan */
    memset(buffer, 0, sizeof(buffer));
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
    len = snprintf(buffer, sizeof(buffer), "ctrl_interface=%s\nap_scan=1\n",
                   get_wpas_ctrl_path_no_iface());
#else
    len = snprintf(buffer, sizeof(buffer), "ctrl_interface=%s\nap_scan=1\n",
                   get_wpas_ctrl_path());
#endif
    if (len) {
        write_file(get_wpas_conf_file(), buffer, len);
    }

    memset(buffer, 0 ,sizeof(buffer));
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
    snprintf(buffer, sizeof(buffer), "%s -B -t -c %s %s -i %s%s",
             get_wpas_full_exec_path(),
             get_wpas_conf_file(),
             get_wpas_debug_arguments(),
             get_wireless_interface(),
             get_wpas_log_file_arguments());
#else
    snprintf(buffer, sizeof(buffer),
             "%s -B -t -c %s -i %s -f /var/log/supplicant.log",
             get_wpas_full_exec_path(),
             get_wpas_conf_file(),
             get_wireless_interface());
#endif
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, buffer);
    len = system(buffer);
    sleep(2);

    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }
    // SCAN
    memset(buffer, 0, sizeof(buffer));
    memset(response, 0, sizeof(response));
    snprintf(buffer, sizeof(buffer), "SCAN");
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR,
                      "Failed to execute the command. Response: %s", response);
        goto done;
    }
    sleep(10);

    /* TLV: BSSID */
    tlv = find_wrapper_tlv_by_id(req, TLV_BSSID);
    if (tlv) {
        memset(bssid, 0, sizeof(bssid));
        memcpy(bssid, tlv->value, tlv->len);
    } else {
        goto done;
    }

    memset(anqp_info_id, 0, sizeof(anqp_info_id));
    /* TLV: ANQP_INFO_ID */
    tlv = find_wrapper_tlv_by_id(req, TLV_ANQP_INFO_ID);
    if (tlv) {
        memcpy(anqp_info_id, tlv->value, tlv->len);
    }

    if (strcmp(anqp_info_id, "NAIHomeRealm") == 0) {
        /* TLV: REALM */
        memset(realm, 0, sizeof(realm));
        tlv = find_wrapper_tlv_by_id(req, TLV_REALM);
        if (tlv) {
            memcpy(realm, tlv->value, tlv->len);
            snprintf(buffer, sizeof(buffer),
                     "HS20_GET_NAI_HOME_REALM_LIST %s realm=%s", bssid, realm);
        } else {
            goto done;
        }
    } else {
        token = strtok_r(anqp_info_id, delimit, &save_ptr);
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "ANQP_GET %s ", bssid);
        while(token != NULL) {
            for (i = 0; i < sizeof(anqp_maps)/sizeof(struct anqp_tlv_to_config_name); i++) {
                if (strcmp(token, anqp_maps[i].element) == 0) {
                    strlcat(buffer, anqp_maps[i].config, sizeof(buffer));
                }
            }

            token = strtok_r(NULL, delimit, &save_ptr);
            if (token != NULL) {
                strlcat(buffer, ",", sizeof(buffer));
            }
        }
    }
    /* Send command to wpa_supplicant UDS socket */
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);

    indigo_logger(LOG_LEVEL_DEBUG, "%s -> resp: %s\n", buffer, response);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR,
                      "Failed to execute the command. Response: %s", response);
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int start_up_p2p_handler(struct packet_wrapper *req,
                                struct packet_wrapper *resp) {
    char *message = TLV_VALUE_WPA_S_START_UP_NOT_OK;
    char buffer[S_BUFFER_LEN] = {0};
    int len, status = TLV_VALUE_STATUS_NOT_OK;

#ifdef _OPENWRT_
#else
    system("rfkill unblock wlan");
    sleep(1);
#endif

    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "killall %s 1>/dev/null 2>/dev/null",
             get_wpas_exec_file());
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, buffer);
    system(buffer);
    sleep(5);

    /* Generate P2P config file */
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
    snprintf(buffer, sizeof(buffer), "ctrl_interface=%s\n",
             get_wpas_ctrl_path_no_iface());
#else
    snprintf(buffer, sizeof(buffer), "ctrl_interface=%s\n",
             WPAS_CTRL_PATH_DEFAULT);
#endif
    /* Add Device name and Device type */
    strlcat(buffer, "device_name=WFA P2P Device\n", sizeof(buffer));
    strlcat(buffer, "device_type=1-0050F204-1\n", sizeof(buffer));
    /* Add config methods */
    strlcat(buffer, "config_methods=keypad display push_button\n",
            sizeof(buffer));
    len = strlen(buffer);

    if (len) {
        write_file(get_wpas_conf_file(), buffer, len);
    }

    /* Start WPA supplicant */
    memset(buffer, 0 ,sizeof(buffer));
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
    snprintf(buffer, sizeof(buffer), "%s -B -t -c %s %s -i %s",
             get_wpas_full_exec_path(),
             get_wpas_conf_file(),
             get_wpas_debug_arguments(),
             get_wireless_interface());
#else
    snprintf(buffer, sizeof(buffer),
             "%s -B -t -c %s %s -i %s -f /var/log/supplicant.log",
             get_wpas_full_exec_path(),
             get_wpas_conf_file(),
             get_wpas_debug_arguments(),
             get_wireless_interface());
#endif
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, buffer);
    system(buffer);
    sleep(4);

    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_WPA_S_START_UP_OK;

    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

int p2p_find_handler(struct packet_wrapper *req,
                            struct packet_wrapper *resp)
{
    struct wpa_ctrl *w = NULL;
    char buffer[S_BUFFER_LEN], response[BUFFER_LEN];
    size_t resp_len;
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_P2P_FIND_NOT_OK;

    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }
    // P2P_FIND
    memset(buffer, 0, sizeof(buffer));
    memset(response, 0, sizeof(response));
    snprintf(buffer, sizeof(buffer), "P2P_FIND");
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR,
                      "Failed to execute the command. Response: %s", response);
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int p2p_listen_handler(struct packet_wrapper *req,
                              struct packet_wrapper *resp)
{
    struct wpa_ctrl *w = NULL;
    char buffer[S_BUFFER_LEN], response[BUFFER_LEN];
    size_t resp_len;
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_P2P_LISTEN_NOT_OK;

    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }
    // P2P_LISTEN
    memset(buffer, 0, sizeof(buffer));
    memset(response, 0, sizeof(response));
    snprintf(buffer, sizeof(buffer), "P2P_LISTEN");
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR,
                      "Failed to execute the command. Response: %s", response);
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int add_p2p_group_handler(struct packet_wrapper *req,
                                 struct packet_wrapper *resp)
{
    struct wpa_ctrl *w = NULL;
    char buffer[S_BUFFER_LEN], response[BUFFER_LEN];
    char freq[64], he[16];
    size_t resp_len;
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_P2P_ADD_GROUP_NOT_OK;
    struct tlv_hdr *tlv = NULL;

    memset(freq, 0, sizeof(freq));
    /* TLV_FREQUENCY (required) */
    tlv = find_wrapper_tlv_by_id(req, TLV_FREQUENCY);
    if (tlv) {
        memcpy(freq, tlv->value, tlv->len);
    } else {
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_INSUFFICIENT_TLV;
        indigo_logger(LOG_LEVEL_ERROR, "Missed TLV: TLV_FREQUENCY");
        goto done;
    }

    memset(he, 0, sizeof(he));
    tlv = find_wrapper_tlv_by_id(req, TLV_IEEE80211_AX);
    if (tlv)
        snprintf(he, sizeof(he), " he");

    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }

    memset(buffer, 0, sizeof(buffer));
    memset(response, 0, sizeof(response));
    snprintf(buffer, sizeof(buffer), "P2P_GROUP_ADD freq=%s%s", freq, he);
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR,
                      "Failed to execute the command. Response: %s", response);
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int stop_p2p_group_handler(struct packet_wrapper *req,
                                  struct packet_wrapper *resp)
{
    struct wpa_ctrl *w = NULL;
    char buffer[S_BUFFER_LEN], response[BUFFER_LEN];
    int persist = 0;
    size_t resp_len;
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_P2P_ADD_GROUP_NOT_OK;
    struct tlv_hdr *tlv = NULL;
    char if_name[32], p2p_dev_if[32];

    tlv = find_wrapper_tlv_by_id(req, TLV_PERSISTENT);
    if (tlv) {
        persist = 1;
    }

    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }

    if (get_p2p_group_if(if_name, sizeof(if_name)) != 0) {
        message = "Failed to get P2P Group Interface";
        goto done;
    }
    memset(buffer, 0, sizeof(buffer));
    memset(response, 0, sizeof(response));
    snprintf(buffer, sizeof(buffer), "P2P_GROUP_REMOVE %s", if_name);
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR,
                      "Failed to execute the command. Response: %s", response);
        goto done;
    }
    if (w) {
        wpa_ctrl_close(w);
        w = NULL;
    }

    if (persist == 1) {
        /* Can use global ctrl if global ctrl is initialized */
        get_p2p_dev_if(p2p_dev_if, sizeof(p2p_dev_if));
        indigo_logger(LOG_LEVEL_DEBUG, "P2P Dev IF: %s", p2p_dev_if);
        /* Open wpa_supplicant UDS socket */
        w = wpa_ctrl_open(get_wpas_if_ctrl_path(p2p_dev_if));
        if (!w) {
            indigo_logger(LOG_LEVEL_ERROR,
                          "Failed to connect to wpa_supplicant");
            status = TLV_VALUE_STATUS_NOT_OK;
            message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
            goto done;
        }

        /* Clear the persistent group with id 0 */
        memset(buffer, 0, sizeof(buffer));
        memset(response, 0, sizeof(response));
        snprintf(buffer, sizeof(buffer), "REMOVE_NETWORK 0");
        resp_len = sizeof(response) - 1;
        wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
        /* Check response */
        if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
            indigo_logger(LOG_LEVEL_ERROR,
                          "Failed to execute the command. Response: %s",
                          response);
            goto done;
        }
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int p2p_start_wps_handler(struct packet_wrapper *req,
                                 struct packet_wrapper *resp)
{
    struct wpa_ctrl *w = NULL;
    char buffer[S_BUFFER_LEN], response[BUFFER_LEN];
    char pin_code[64], if_name[32];
    size_t resp_len;
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_P2P_START_WPS_NOT_OK;
    struct tlv_hdr *tlv = NULL;

    memset(buffer, 0, sizeof(buffer));
    tlv = find_wrapper_tlv_by_id(req, TLV_PIN_CODE);
    if (tlv) {
        memset(pin_code, 0, sizeof(pin_code));
        memcpy(pin_code, tlv->value, tlv->len);
        snprintf(buffer, sizeof(buffer), "WPS_PIN any %s", pin_code);
    } else {
        snprintf(buffer, sizeof(buffer), "WPS_PBC");
    }

    /* Open wpa_supplicant UDS socket */
    if (get_p2p_group_if(if_name, sizeof(if_name))) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to get P2P group interface");
        goto done;
    }
    indigo_logger(LOG_LEVEL_DEBUG, "P2P group interface: %s", if_name);
    w = wpa_ctrl_open(get_wpas_if_ctrl_path(if_name));
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }

    memset(response, 0, sizeof(response));
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    if (strncmp(response, WPA_CTRL_FAIL, strlen(WPA_CTRL_FAIL)) == 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command(%s).",
                      buffer);
        goto done;
    }

    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int sta_scan_handler(struct packet_wrapper *req,
                            struct packet_wrapper *resp)
{
    int len, status = TLV_VALUE_STATUS_NOT_OK, i;
    char *message = TLV_VALUE_WPA_S_SCAN_NOT_OK;
    char buffer[1024];
    char response[1024];
    struct tlv_hdr *tlv = NULL;
    struct wpa_ctrl *w = NULL;
    size_t resp_len;
    struct tlv_to_config_name* cfg = NULL;
    char value[TLV_VALUE_SIZE], cfg_item[2*S_BUFFER_LEN];

    memset(buffer, 0, sizeof(buffer));
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
    snprintf(buffer, sizeof(buffer), "ctrl_interface=%s\nap_scan=1\n",
             get_wpas_ctrl_path_no_iface());
#else
    snprintf(buffer, sizeof(buffer), "ctrl_interface=%s\nap_scan=1\n",
             WPAS_CTRL_PATH_DEFAULT);
#endif
    tlv = find_wrapper_tlv_by_id(req, TLV_STA_IEEE80211_W);
    if (tlv) {
        memset(value, 0, sizeof(value));
        memcpy(value, tlv->value, tlv->len);
        snprintf(cfg_item, sizeof(cfg_item), "pmf=%s\n", value);
        strlcat(buffer, cfg_item, sizeof(buffer));
    }
    for (i = 0; i < req->tlv_num; i++) {
        cfg = find_wpas_global_config_name(req->tlv[i]->id);
        if (cfg) {
            memset(value, 0, sizeof(value));
            memcpy(value, req->tlv[i]->value, req->tlv[i]->len);
            snprintf(cfg_item, sizeof(cfg_item), "%s=%s\n", cfg->config_name,
                     value);
            strlcat(buffer, cfg_item, sizeof(buffer));
        }
    }
    len = strlen(buffer);
    if (len) {
        write_file(get_wpas_conf_file(), buffer, len);
    }

    memset(buffer, 0 ,sizeof(buffer));
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
    snprintf(buffer, sizeof(buffer), "%s -B -t -c %s %s -i %s",
             get_wpas_full_exec_path(),
             get_wpas_conf_file(),
             get_wpas_debug_arguments(),
             get_wireless_interface());
#else
    snprintf(buffer, sizeof(buffer),
             "%s -B -t -c %s %s -i %s -f /var/log/supplicant.log",
             get_wpas_full_exec_path(),
             get_wpas_conf_file(),
             get_wpas_debug_arguments(),
             get_wireless_interface());
#endif
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, buffer);
    system(buffer);
    sleep(4);

    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }
    // SCAN
    memset(buffer, 0, sizeof(buffer));
    memset(response, 0, sizeof(response));
    snprintf(buffer, sizeof(buffer), "SCAN");
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR,
                      "Failed to execute the command. Response: %s", response);
        goto done;
    }
    indigo_logger(LOG_LEVEL_DEBUG, "%s -> resp: %s\n", buffer, response);
    sleep(10);

    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int set_sta_hs2_associate_handler(struct packet_wrapper *req,
                                         struct packet_wrapper *resp)
{
    int status = TLV_VALUE_STATUS_NOT_OK;
    size_t resp_len;
    char *message = TLV_VALUE_WPA_S_ASSOC_NOT_OK;
    char buffer[BUFFER_LEN];
    char response[BUFFER_LEN];
    char bssid[256];
    struct tlv_hdr *tlv = NULL;
    struct wpa_ctrl *w = NULL;

    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }

    memset(bssid, 0, sizeof(bssid));
    tlv = find_wrapper_tlv_by_id(req, TLV_BSSID);
    if (tlv) {
        memset(bssid, 0, sizeof(bssid));
        memcpy(bssid, tlv->value, tlv->len);
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "INTERWORKING_CONNECT %s", bssid);
    } else {
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "INTERWORKING_SELECT auto");
    }

    /* Send command to wpa_supplicant UDS socket */
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR,
                      "Failed to execute the command %s.\n Response: %s",
                      buffer, response);
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;
done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int sta_add_credential_handler(struct packet_wrapper *req,
                                      struct packet_wrapper *resp)
{
    char *message = TLV_VALUE_WPA_S_ADD_CRED_NOT_OK;
    char buffer[BUFFER_LEN];
    int len, status = TLV_VALUE_STATUS_NOT_OK, i, cred_id, wpa_ret;
    size_t resp_len;
    char response[BUFFER_LEN];
    char param_value[256];
    struct tlv_hdr *tlv = NULL;
    struct wpa_ctrl *w = NULL;
    struct tlv_to_config_name* cfg = NULL;

    if (sta_configured == 0) {
        sta_configured = 1;
#ifdef _OPENWRT_
#else
        system("rfkill unblock wlan");
        sleep(1);
#endif
        memset(buffer, 0, sizeof(buffer));
        snprintf(buffer, sizeof(buffer), "killall %s 1>/dev/null 2>/dev/null",
                 get_wpas_exec_file());
        system(buffer);
        sleep(3);

        memset(buffer, 0, sizeof(buffer));
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
        len = snprintf(buffer, sizeof(buffer), "ctrl_interface=%s\nap_scan=1\n",
                       get_wpas_ctrl_path_no_iface());
#else
        len = snprintf(buffer, sizeof(buffer), "ctrl_interface=%s\nap_scan=1\n",
                       WPAS_CTRL_PATH_DEFAULT);
#endif
        if (len) {
            write_file(get_wpas_conf_file(), buffer, len);
        }
    }
    if (sta_started == 0) {
        sta_started = 1;
        /* Start WPA supplicant */
        memset(buffer, 0 ,sizeof(buffer));
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
        snprintf(buffer, sizeof(buffer), "%s -B -t -c %s %s -i %s",
                 get_wpas_full_exec_path(),
                 get_wpas_conf_file(),
                 get_wpas_debug_arguments(),
                 get_wireless_interface());
#else
        snprintf(buffer, sizeof(buffer),
                 "%s -B -t -c %s %s -i %s -f /var/log/supplicant.log",
                 get_wpas_full_exec_path(),
                 get_wpas_conf_file(),
                 get_wpas_debug_arguments(),
                 get_wireless_interface());
#endif
        indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, buffer);
        system(buffer);
        sleep(4);
    }

    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }
    /* Assemble wpa_supplicant command */
    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "ADD_CRED");
    resp_len = sizeof(response) - 1;
    wpa_ret = wpa_ctrl_request(w, buffer, strlen(buffer), response,
                               &resp_len, NULL);
    if (wpa_ret < 0) {
        indigo_logger(LOG_LEVEL_ERROR,
                      "Failed to execute the command ADD_CRED. Response: %s",
                      response);
        goto done;
    }
    cred_id = atoi(response);

    for (i = 0; i < req->tlv_num; i++) {
        memset(param_value, 0, sizeof(param_value));
        tlv = req->tlv[i];
        cfg = find_tlv_config(tlv->id);
        if (!cfg) {
            continue;
        }
        memcpy(param_value, tlv->value, tlv->len);

        /* Assemble wpa_supplicant command */
        memset(buffer, 0, sizeof(buffer));

        if (cfg->quoted) {
            snprintf(buffer, sizeof(buffer), "SET_CRED %d %s \"%s\"",
                     cred_id, cfg->config_name, param_value);
        } else {
            snprintf(buffer, sizeof(buffer), "SET_CRED %d %s %s",
                     cred_id, cfg->config_name, param_value);
        }
        indigo_logger(LOG_LEVEL_DEBUG, "Execute the command: %s", buffer);
        /* Send command to wpa_supplicant UDS socket */
        resp_len = sizeof(response) - 1;
        wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
        /* Check response */
        if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
            indigo_logger(LOG_LEVEL_ERROR,
                          "Failed to execute the command. Response: %s",
                          response);
            message = TLV_VALUE_WPA_SET_PARAMETER_NO_OK;
            goto done;
        }
    }

    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_WPA_S_ADD_CRED_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    return 0;
}

static int run_hs20_osu_client(const char *params)
{
    char buf[BUFFER_LEN], cmd[S_BUFFER_LEN];
    int res;

#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
    res = snprintf(cmd, sizeof(cmd),
                   "%s -w \"%s\" -r hs20-osu-client.res -dddKt",
                   HS20_OSU_CLIENT,
                   get_wpas_full_exec_path(), "/");
#else
    res = snprintf(cmd, sizeof(cmd),
                   "%s -w \"%s\" -r hs20-osu-client.res -dddKt -f /var/log/hs20-osu-client.log",
                   HS20_OSU_CLIENT, WPAS_CTRL_PATH_DEFAULT "/");
#endif
    if (res < 0 || res >= (int) sizeof(cmd))
        return -1;

    res = snprintf(buf, sizeof(buf), "%s %s", cmd, params);
    if (res < 0 || res >= (int) sizeof(buf))
        return -1;

    indigo_logger(LOG_LEVEL_DEBUG, "Run: %s", buf);

    if (system(buf) != 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to run: %s", buf);
        return -1;
    }

    return 0;
}

int set_sta_install_ppsmo_handler(struct packet_wrapper *req,
                                         struct packet_wrapper *resp)
{
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_HS2_INSTALL_PPSMO_NOT_OK;
    int len;
    char buffer[L_BUFFER_LEN], ppsmo_file[S_BUFFER_LEN];
    struct tlv_hdr *tlv;
    char *fqdn = NULL;
    char fqdn_buf[S_BUFFER_LEN];
    memset(buffer, 0, sizeof(buffer));

#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
    len = snprintf(buffer, sizeof(buffer), "ctrl_interface=%s\nap_scan=1\n",
                   get_wpas_ctrl_path_no_iface());
#else
    len = snprintf(buffer, sizeof(buffer), "ctrl_interface=%s\nap_scan=1\n",
                   WPAS_CTRL_PATH_DEFAULT);
#endif
    if (len) {
        write_file(get_wpas_conf_file(), buffer, len);
    }

#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
    snprintf(buffer, sizeof(buffer), "%s -B -t -c %s %s -i %s",
             get_wpas_full_exec_path(), get_wpas_conf_file(),
             get_wpas_debug_arguments(),
             get_wireless_interface());
#else
    snprintf(buffer, sizeof(buffer),
             "%s -B -t -c %s %s -i %s -f /var/log/supplicant.log",
             get_wpas_full_exec_path(),
             get_wpas_conf_file(),
             get_wpas_debug_arguments(),
             get_wireless_interface());
#endif
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, buffer);
    if (system(buffer)) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to run wpa_supplicant.");
        goto done;
    }
    sleep(4);

    tlv = find_wrapper_tlv_by_id(req, TLV_PPSMO_FILE);
    if (tlv) {
        memset(ppsmo_file, 0, sizeof(ppsmo_file));
        memcpy(ppsmo_file, tlv->value, tlv->len);
    } else {
        goto done;
    }

    /* TODO */
    unlink("pps-tnds.xml");
    snprintf(buffer, sizeof(buffer), "wget -T 10 -t 3 -O pps-tnds.xml '%s'",
             ppsmo_file);
    indigo_logger(LOG_LEVEL_DEBUG, "RUN: %s\n", buffer);
    if (system(buffer) != 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to download PPS MO from %s\n",
                      ppsmo_file);
        goto done;
    }

    snprintf(buffer, sizeof(buffer), "from_tnds pps-tnds.xml pps.xml");
    if (run_hs20_osu_client(buffer) < 0)
        goto done;
    sleep(2);

    memset(fqdn_buf, 0, sizeof(fqdn_buf));
    if (run_hs20_osu_client("get_fqdn pps.xml") == 0) {
        FILE *f = fopen("pps-fqdn", "r");
        if (f) {
            if (fgets(fqdn_buf, sizeof(fqdn_buf), f)) {
                fqdn_buf[sizeof(fqdn_buf) - 1] = '\0';
                fqdn = fqdn_buf;
                if (fqdn)
                    indigo_logger(LOG_LEVEL_DEBUG, "FQDN: %s", fqdn);
                else {
                    indigo_logger(LOG_LEVEL_ERROR, "Get FQDN ERROR" );
                    goto done;
                }
            } else {
                indigo_logger(LOG_LEVEL_ERROR, "Get FQDN failed" );
                goto done;
            }
            fclose(f);
        } else {
                indigo_logger(LOG_LEVEL_ERROR, "pps-fqdn open failed" );
                goto done;
        }
    } else {
        indigo_logger(LOG_LEVEL_ERROR, " get_fqdn pps.xml failed" );
        goto done;
    }

    mkdir("SP", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    snprintf(buffer, sizeof(buffer), "SP/%s", fqdn);
    mkdir(buffer, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

    snprintf(buffer, sizeof(buffer), "dl_aaa_ca pps.xml SP/%s/aaa-ca.pem",
             fqdn);
    if (run_hs20_osu_client(buffer) < 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to download AAA CA cert");
        goto done;
    }

    snprintf(buffer, sizeof(buffer), "set_pps pps.xml");
    if (run_hs20_osu_client(buffer) < 0) {
        indigo_logger(LOG_LEVEL_ERROR,
              "errorCode,Failed to configure credential from PPSMO");
        goto done;
    }

    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_HS2_INSTALL_PPSMO_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

int p2p_connect_handler(struct packet_wrapper *req,
                               struct packet_wrapper *resp)
{
    struct wpa_ctrl *w = NULL;
    char buffer[S_BUFFER_LEN], response[BUFFER_LEN];
    char pin_code[64];
    char method[16], mac[32], type[16];
    size_t resp_len;
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_P2P_CONNECT_NOT_OK;
    struct tlv_hdr *tlv = NULL;
    char go_intent[32], he[16], persist[32];
    int intent_value = P2P_GO_INTENT;

    memset(buffer, 0, sizeof(buffer));
    memset(mac, 0, sizeof(mac));
    memset(method, 0, sizeof(method));
    memset(type, 0, sizeof(type));
    memset(he, 0, sizeof(he));
    memset(persist, 0, sizeof(persist));
    tlv = find_wrapper_tlv_by_id(req, TLV_ADDRESS);
    if (tlv) {
        memcpy(mac, tlv->value, tlv->len);
    } else {
        indigo_logger(LOG_LEVEL_ERROR, "Missed TLV: TLV_ADDRESS");
        goto done;
    }
    tlv = find_wrapper_tlv_by_id(req, TLV_GO_INTENT);
    memset(go_intent, 0, sizeof(go_intent));
    if (tlv) {
        memcpy(go_intent, tlv->value, MIN(tlv->len, sizeof(go_intent) - 1));
        intent_value = atoi(go_intent);
    }
    tlv = find_wrapper_tlv_by_id(req, TLV_P2P_CONN_TYPE);
    if (tlv) {
        memcpy(type, tlv->value, MIN(tlv->len, sizeof(type) - 1));
        if (atoi(type) == P2P_CONN_TYPE_JOIN) {
            snprintf(type, sizeof(type), " join");
            memset(go_intent, 0, sizeof(go_intent));
        } else if (atoi(type) == P2P_CONN_TYPE_AUTH) {
            snprintf(type, sizeof(type), " auth");
            snprintf(go_intent, sizeof(go_intent), " go_intent=%d",
                     intent_value);
        }
    } else {
            snprintf(go_intent, sizeof(go_intent), " go_intent=%d",
                     intent_value);
    }
    tlv = find_wrapper_tlv_by_id(req, TLV_IEEE80211_AX);
    if (tlv) {
            snprintf(he, sizeof(he), " he");
    }
    tlv = find_wrapper_tlv_by_id(req, TLV_PERSISTENT);
    if (tlv) {
            snprintf(persist, sizeof(persist), " persistent");
    }
    tlv = find_wrapper_tlv_by_id(req, TLV_PIN_CODE);
    if (tlv) {
        memset(pin_code, 0, sizeof(pin_code));
        memcpy(pin_code, tlv->value, tlv->len);
        tlv = find_wrapper_tlv_by_id(req, TLV_PIN_METHOD);
        if (tlv) {
            memcpy(method, tlv->value, tlv->len);
        } else {
            indigo_logger(LOG_LEVEL_ERROR, "Missed TLV PIN_METHOD???");
        }
        snprintf(buffer, sizeof(buffer), "P2P_CONNECT %s %s %s%s%s%s%s", mac,
                 pin_code, method, type, go_intent, he, persist);
    } else {
        tlv = find_wrapper_tlv_by_id(req, TLV_WSC_METHOD);
        if (tlv) {
            memcpy(method, tlv->value, tlv->len);
        } else {
            indigo_logger(LOG_LEVEL_ERROR, "Missed TLV WSC_METHOD");
        }
        snprintf(buffer, sizeof(buffer), "P2P_CONNECT %s %s%s%s%s%s", mac,
                 method, type, go_intent, he, persist);
    }
    indigo_logger(LOG_LEVEL_DEBUG, "Command: %s", buffer);

    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }

    memset(response, 0, sizeof(response));
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR,
                      "Failed to execute the command. Response: %s", response);
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}


int start_dhcp_handler(struct packet_wrapper *req,
                              struct packet_wrapper *resp)
{
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_START_DHCP_NOT_OK;
    char buffer[S_BUFFER_LEN];
    char ip_addr[32], role[8];
    struct tlv_hdr *tlv = NULL;
    char if_name[32];

    memset(role, 0, sizeof(role));
    tlv = find_wrapper_tlv_by_id(req, TLV_ROLE);
    if (tlv) {
        memcpy(role, tlv->value, MIN(tlv->len, sizeof(role) - 1));
        if (atoi(role) == DUT_TYPE_P2PUT) {
            get_p2p_group_if(if_name, sizeof(if_name));
        } else {
            indigo_logger(LOG_LEVEL_ERROR, "DHCP only supports in P2PUT");
            goto done;
        }
    } else {
        indigo_logger(LOG_LEVEL_ERROR, "Missed TLV: TLV_ROLE");
        goto done;
    }

    /* TLV: TLV_STATIC_IP */
    memset(ip_addr, 0, sizeof(ip_addr));
    tlv = find_wrapper_tlv_by_id(req, TLV_STATIC_IP);
    if (tlv) { /* DHCP Server */
        strlcpy(ip_addr, (char *)tlv->value, sizeof(ip_addr));
        if (!strcmp("0.0.0.0", ip_addr)) {
            snprintf(ip_addr, sizeof(ip_addr), DHCP_SERVER_IP);
        }
        snprintf(buffer, sizeof(buffer), "%s/24", ip_addr);
        set_interface_ip(if_name, buffer);
        start_dhcp_server(if_name, ip_addr);
    } else { /* DHCP Client */
        start_dhcp_client(if_name);
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}

int stop_dhcp_handler(struct packet_wrapper *req,
                             struct packet_wrapper *resp)
{
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_NOT_OK;
    char role[8];
    struct tlv_hdr *tlv = NULL;
    char if_name[32];

    memset(role, 0, sizeof(role));
    tlv = find_wrapper_tlv_by_id(req, TLV_ROLE);
    if (tlv) {
        memcpy(role, tlv->value, MIN(tlv->len, sizeof(role) - 1));
        if (atoi(role) == DUT_TYPE_P2PUT) {
            if (!get_p2p_group_if(if_name, sizeof(if_name)))
                reset_interface_ip(if_name);
        } else {
            indigo_logger(LOG_LEVEL_ERROR, "DHCP only supports in P2PUT");
            goto done;
        }
    } else {
        indigo_logger(LOG_LEVEL_ERROR, "Missed TLV: TLV_ROLE");
        goto done;
    }

    /* TLV: TLV_STATIC_IP */
    tlv = find_wrapper_tlv_by_id(req, TLV_STATIC_IP);
    if (tlv) { /* DHCP Server */
        stop_dhcp_server();
    } else { /* DHCP Client */
        stop_dhcp_client();
    }

    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}


int get_wsc_pin_handler(struct packet_wrapper *req,
                               struct packet_wrapper *resp)
{
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_NOT_OK;
    char buffer[64], response[S_BUFFER_LEN];
    struct tlv_hdr *tlv = NULL;
    char value[16];
    int role = 0;
    struct wpa_ctrl *w = NULL;
    size_t resp_len;

    memset(value, 0, sizeof(value));
    tlv = find_wrapper_tlv_by_id(req, TLV_ROLE);
    if (tlv) {
            memcpy(value, tlv->value, MIN(tlv->len, sizeof(value) - 1));
            role = atoi(value);
    } else {
        indigo_logger(LOG_LEVEL_ERROR, "Missed TLV: TLV_ROLE");
        goto done;
    }

    if (role == DUT_TYPE_APUT) {
        // TODO
        snprintf(buffer, sizeof(buffer), "WPS_AP_PIN get");
        w = wpa_ctrl_open(get_hapd_ctrl_path());
        if (!w) {
            indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to hostapd");
            status = TLV_VALUE_STATUS_NOT_OK;
            message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
            goto done;
        }
    } else if (role == DUT_TYPE_STAUT || role == DUT_TYPE_P2PUT) {
        snprintf(buffer, sizeof(buffer), "WPS_PIN get");
        w = wpa_ctrl_open(get_wpas_ctrl_path());
        if (!w) {
            indigo_logger(LOG_LEVEL_ERROR,
                          "Failed to connect to wpa_supplicant");
            status = TLV_VALUE_STATUS_NOT_OK;
            message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
            goto done;
        }
    } else {
        indigo_logger(LOG_LEVEL_ERROR, "Invalid value in TLV_ROLE");
        goto done;
    }

    memset(response, 0, sizeof(response));
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    if (strncmp(response, WPA_CTRL_FAIL, strlen(WPA_CTRL_FAIL)) == 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command(%s).",
                      buffer);
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (status == TLV_VALUE_STATUS_OK) {
        fill_wrapper_tlv_bytes(resp, TLV_WSC_PIN_CODE, strlen(response),
                               response);
    }
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int get_p2p_intent_value_handler(struct packet_wrapper *req,
                                        struct packet_wrapper *resp)
{
    int status = TLV_VALUE_STATUS_OK;
    char *message = TLV_VALUE_OK;
    char response[S_BUFFER_LEN];

    memset(response, 0, sizeof(response));
    snprintf(response, sizeof(response), "%d", P2P_GO_INTENT);

    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (status == TLV_VALUE_STATUS_OK) {
        fill_wrapper_tlv_bytes(resp, TLV_P2P_INTENT_VALUE, strlen(response),
                               response);
    }

    return 0;
}

int start_wps_ap_handler(struct packet_wrapper *req,
                                struct packet_wrapper *resp)
{
    struct wpa_ctrl *w = NULL;
    char buffer[S_BUFFER_LEN], response[BUFFER_LEN];
    char pin_code[64];
    size_t resp_len;
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_AP_START_WPS_NOT_OK;
    struct tlv_hdr *tlv = NULL;

    memset(buffer, 0, sizeof(buffer));
    tlv = find_wrapper_tlv_by_id(req, TLV_PIN_CODE);
    if (tlv) {
        memset(pin_code, 0, sizeof(pin_code));
        memcpy(pin_code, tlv->value, tlv->len);

        /* Please implement the wsc pin validation function to
         * identify the invalid PIN code and DONOT start wps.
         * */
        #define WPS_PIN_VALIDATION_FILE "/tmp/pin_checksum.sh"
        int len = 0;
        char pipebuf[S_BUFFER_LEN];
        char *parameter[] = {"sh", WPS_PIN_VALIDATION_FILE, pin_code, NULL};
        memset(pipebuf, 0, sizeof(pipebuf));
        if (0 == access(WPS_PIN_VALIDATION_FILE, F_OK)) {
            len = pipe_command(pipebuf, sizeof(pipebuf) - 1, "/bin/sh",
                               parameter);
            if (len && atoi(pipebuf)) {
                indigo_logger(LOG_LEVEL_INFO, "Valid PIN Code: %s", pin_code);
            } else {
                indigo_logger(LOG_LEVEL_INFO, "Invalid PIN Code: %s", pin_code);
                message = TLV_VALUE_AP_WSC_PIN_CODE_NOT_OK;
                goto done;
            }
        }
        /*
         * End of wsc pin validation function
         */
        snprintf(buffer, sizeof(buffer), "WPS_PIN any %s", pin_code);
    } else {
        snprintf(buffer, sizeof(buffer), "WPS_PBC");
    }

    /* Open hostapd UDS socket */
    w = wpa_ctrl_open(get_hapd_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to hostapd");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }

    memset(response, 0, sizeof(response));
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    if (strncmp(response, WPA_CTRL_FAIL, strlen(WPA_CTRL_FAIL)) == 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command(%s).",
                      buffer);
        goto done;
    }

    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int start_wps_sta_handler(struct packet_wrapper *req,
                                 struct packet_wrapper *resp)
{
    struct wpa_ctrl *w = NULL;
    char buffer[S_BUFFER_LEN], response[BUFFER_LEN];
    char pin_code[64];
    size_t resp_len;
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_AP_START_WPS_NOT_OK;
    struct tlv_hdr *tlv = NULL;
    int use_dynamic_pin = 0;

    memset(buffer, 0, sizeof(buffer));
    tlv = find_wrapper_tlv_by_id(req, TLV_PIN_CODE);
    if (tlv) {
        memset(pin_code, 0, sizeof(pin_code));
        memcpy(pin_code, (char *)tlv->value, MIN(tlv->len, sizeof(pin_code) - 1));
        if (strlen(pin_code) == 1 && atoi(pin_code) == 0) {
            snprintf(buffer, sizeof(buffer), "WPS_PIN any");
            use_dynamic_pin = 1;
        } else if (strlen(pin_code) == 4 || strlen(pin_code) == 8){
            snprintf(buffer, sizeof(buffer), "WPS_PIN any %s", pin_code);
        } else {
            /* Please implement the function to strip the extraneous
            *  hyphen(dash) attached with 4 or 8-digit PIN code, then
            *  start WPS PIN Registration with stripped PIN code.
            * */
            indigo_logger(LOG_LEVEL_ERROR, "Unrecognized PIN: %s", pin_code);
            goto done;
        }
    } else {
        snprintf(buffer, sizeof(buffer), "WPS_PBC");
    }

    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }

    memset(response, 0, sizeof(response));
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    if (strncmp(response, WPA_CTRL_FAIL, strlen(WPA_CTRL_FAIL)) == 0) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to execute the command(%s).",
                      buffer);
        goto done;
    }

    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (status == TLV_VALUE_STATUS_OK && use_dynamic_pin) {
        fill_wrapper_tlv_bytes(resp, TLV_WSC_PIN_CODE, strlen(response),
                               response);
    }
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

struct _cfg_cred {
    char *key;
    char *tok;
    char val[S_BUFFER_LEN];
    unsigned short tid;
};

int get_wsc_cred_handler(struct packet_wrapper *req,
                                struct packet_wrapper *resp)
{
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_NOT_OK;
    char *pos = NULL, *data = NULL, value[16];
    int i, len, count = 0, role = 0;
    struct tlv_hdr *tlv = NULL;
    struct _cfg_cred *p_cfg = NULL;

    memset(value, 0, sizeof(value));
    tlv = find_wrapper_tlv_by_id(req, TLV_ROLE);
    if (tlv) {
            memcpy(value, tlv->value, MIN(tlv->len, sizeof(value) - 1));
            role = atoi(value);
    } else {
        indigo_logger(LOG_LEVEL_ERROR, "Missed TLV: TLV_ROLE");
        goto done;
    }

    if (role == DUT_TYPE_APUT) {
        // APUT
        struct _cfg_cred cfg_creds[] = {
            {"ssid", "ssid=", {0}, TLV_WSC_SSID},
            {"wpa_passphrase", "wpa_passphrase=", {0}, TLV_WSC_WPA_PASSPHRASE},
            {"wpa_key_mgmt", "wpa_key_mgmt=", {0}, TLV_WSC_WPA_KEY_MGMT}
        };
        count = sizeof(cfg_creds)/sizeof(struct _cfg_cred);
        p_cfg = cfg_creds;
        tlv = find_wrapper_tlv_by_id(req, TLV_BSS_IDENTIFIER);
        struct interface_info *wlan = NULL;
        if (tlv) {
            /* mbss: TBD */
        } else {
            /* single wlan  */
            wlan = get_first_configured_wireless_interface_info();
        }
        if (!wlan)
            goto done;
        data = read_file(wlan->hapd_conf_file);
        if (!data) {
            indigo_logger(LOG_LEVEL_ERROR, "Fail to read file: %s",
                          wlan->hapd_conf_file);
            goto done;
        }
    } else if (role == DUT_TYPE_STAUT) {
        // STAUT
        struct _cfg_cred cfg_creds[] = {
            {"ssid", "ssid=", {0}, TLV_WSC_SSID},
            {"psk", "psk=", {0}, TLV_WSC_WPA_PASSPHRASE},
            {"key_mgmt", "key_mgmt=", {0}, TLV_WSC_WPA_KEY_MGMT}
        };
        count = sizeof(cfg_creds)/sizeof(struct _cfg_cred);
        p_cfg = cfg_creds;
        data = read_file(get_wpas_conf_file());
        if (!data) {
            indigo_logger(LOG_LEVEL_ERROR, "Fail to read file: %s",
                          get_wpas_conf_file());
            goto done;
        }
    } else {
        indigo_logger(LOG_LEVEL_ERROR, "Invalid value in TLV_ROLE");
        goto done;
    }

    for (i = 0; i < count; i++) {
        pos = strstr(data, p_cfg[i].tok);
        if (pos) {
            pos += strlen(p_cfg[i].tok);
            if (*pos == '"') {
                /* Handle with the format aaaaa="xxxxxxxx" */
                pos++;
                len = strchr(pos, '"') - pos;
            } else {
                /* Handle with the format bbbbb=yyyyyyyy */
                len = strchr(pos, '\n') - pos;
            }
            memcpy(p_cfg[i].val, pos, len);
            indigo_logger(LOG_LEVEL_INFO, "Get %s: %s\n",
                          p_cfg[i].key, p_cfg[i].val);
        } else {
            indigo_logger(LOG_LEVEL_INFO, "Cannot find the setting: %s\n",
                          p_cfg[i].key);
            //goto done;
        }
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    if (data)
        free(data);
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (status == TLV_VALUE_STATUS_OK) {
        for (i = 0; i < count; i++) {
            fill_wrapper_tlv_bytes(resp, p_cfg[i].tid, strlen(p_cfg[i].val),
                                   p_cfg[i].val);
        }
    }
    return 0;
}

int p2p_invite_handler(struct packet_wrapper *req,
                              struct packet_wrapper *resp)
{
    struct wpa_ctrl *w = NULL;
    char buffer[S_BUFFER_LEN], response[BUFFER_LEN];
    char addr[32], if_name[16], persist[32], p2p_dev_if[32];
    char freq[16], he[16];
    size_t resp_len;
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_P2P_INVITE_NOT_OK;
    struct tlv_hdr *tlv = NULL;

    memset(addr, 0, sizeof(addr));
    /* TLV_ADDRESS (required) */
    tlv = find_wrapper_tlv_by_id(req, TLV_ADDRESS);
    if (tlv) {
        memcpy(addr, tlv->value, tlv->len);
    } else {
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_INSUFFICIENT_TLV;
        indigo_logger(LOG_LEVEL_ERROR, "Missed TLV: TLV_ADDRESS");
        goto done;
    }

    memset(persist, 0, sizeof(persist));
    memset(if_name, 0, sizeof(if_name));
    tlv = find_wrapper_tlv_by_id(req, TLV_PERSISTENT);
    if (tlv) {
        /* Assume persistent group id is 0 */
        snprintf(persist, sizeof(persist), "persistent=0");
    } else if (get_p2p_group_if(if_name, sizeof(if_name)) != 0) {
        message = "Failed to get P2P Group Interface";
        goto done;
    }

    /* Can use global ctrl if global ctrl is initialized */
    get_p2p_dev_if(p2p_dev_if, sizeof(p2p_dev_if));
    indigo_logger(LOG_LEVEL_DEBUG, "P2P Dev IF: %s", p2p_dev_if);
    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_if_ctrl_path(p2p_dev_if));
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }

    memset(buffer, 0, sizeof(buffer));
    memset(response, 0, sizeof(response));
    if (persist[0] != 0) {
        memset(he, 0, sizeof(he));
        tlv = find_wrapper_tlv_by_id(req, TLV_IEEE80211_AX);
        if (tlv)
            snprintf(he, sizeof(he), " he");

        tlv = find_wrapper_tlv_by_id(req, TLV_FREQUENCY);
        if (tlv) {
            memset(freq, 0, sizeof(freq));
            memcpy(freq, tlv->value, tlv->len);
            snprintf(buffer, sizeof(buffer), "P2P_INVITE %s peer=%s%s freq=%s",
                     persist, addr, he, freq);
        } else {
            snprintf(buffer, sizeof(buffer), "P2P_INVITE %s peer=%s%s",
                     persist, addr, he);
        }
    } else {
        snprintf(buffer, sizeof(buffer), "P2P_INVITE group=%s peer=%s",
                 if_name, addr);
    }
    indigo_logger(LOG_LEVEL_DEBUG, "Command: %s", buffer);
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR,
                      "Failed to execute the command. Response: %s", response);
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int set_p2p_serv_disc_handler(struct packet_wrapper *req,
                                     struct packet_wrapper *resp)
{
    struct wpa_ctrl *w = NULL;
    char buffer[BUFFER_LEN], response[BUFFER_LEN];
    char addr[32], p2p_dev_if[32];
    size_t resp_len;
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_P2P_SET_SERV_DISC_NOT_OK;
    struct tlv_hdr *tlv = NULL;

    memset(addr, 0, sizeof(addr));
    tlv = find_wrapper_tlv_by_id(req, TLV_ADDRESS);
    if (tlv) {
        /* Send Service Discovery Req */
        memcpy(addr, tlv->value, tlv->len);
    } else {
        /* Set Services case */
        /* Add bonjour and upnp Service */
    }

    /* Can use global ctrl if global ctrl is initialized */
    get_p2p_dev_if(p2p_dev_if, sizeof(p2p_dev_if));
    indigo_logger(LOG_LEVEL_DEBUG, "P2P Dev IF: %s", p2p_dev_if);
    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_if_ctrl_path(p2p_dev_if));
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }

    memset(buffer, 0, sizeof(buffer));
    memset(response, 0, sizeof(response));
    if (addr[0] != 0) {
        snprintf(buffer, sizeof(buffer), "P2P_SERV_DISC_REQ %s 02000001", addr);
        indigo_logger(LOG_LEVEL_DEBUG, "Command: %s", buffer);
    } else {
        snprintf(buffer, sizeof(buffer),
                 "P2P_SERVICE_ADD bonjour 096d797072696e746572045f697070c00c001001 09747874766572733d311a70646c3d6170706c69636174696f6e2f706f7374736372797074");
    }
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);

    if (addr[0] == 0) {
        if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
            indigo_logger(LOG_LEVEL_ERROR,
                          "Failed to execute the command. Response: %s",
                          response);
            goto done;
        }
        snprintf(buffer, sizeof(buffer), "P2P_SERVICE_ADD upnp 10 uuid:5566d33e-9774-09ab-4822-333456785632::urn:schemas-upnp-org:service:ContentDirectory:2");
        wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
        if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
            indigo_logger(LOG_LEVEL_ERROR,
                          "Failed to execute the command. Response: %s",
                          response);
            goto done;
        }
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int set_p2p_ext_listen_handler(struct packet_wrapper *req,
                                      struct packet_wrapper *resp)
{
    struct wpa_ctrl *w = NULL;
    char buffer[S_BUFFER_LEN], response[BUFFER_LEN];
    size_t resp_len;
    int status = TLV_VALUE_STATUS_NOT_OK;
    char *message = TLV_VALUE_P2P_SET_EXT_LISTEN_NOT_OK;

    /* Open wpa_supplicant UDS socket */
    w = wpa_ctrl_open(get_wpas_ctrl_path());
    if (!w) {
        indigo_logger(LOG_LEVEL_ERROR, "Failed to connect to wpa_supplicant");
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_WPA_S_CTRL_NOT_OK;
        goto done;
    }
    memset(buffer, 0, sizeof(buffer));
    memset(response, 0, sizeof(response));
    snprintf(buffer, sizeof(buffer), "P2P_EXT_LISTEN 1000 4000");
    resp_len = sizeof(response) - 1;
    wpa_ctrl_request(w, buffer, strlen(buffer), response, &resp_len, NULL);
    /* Check response */
    if (strncmp(response, WPA_CTRL_OK, strlen(WPA_CTRL_OK)) != 0) {
        indigo_logger(LOG_LEVEL_ERROR,
                      "Failed to execute the command. Response: %s", response);
        goto done;
    }
    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_OK;

done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    if (w) {
        wpa_ctrl_close(w);
    }
    return 0;
}

int enable_wsc_sta_handler(struct packet_wrapper *req,
                                  struct packet_wrapper *resp)
{
    char *message = TLV_VALUE_WPA_S_START_UP_NOT_OK;
    char buffer[L_BUFFER_LEN];
    char value[S_BUFFER_LEN], cfg_item[2*S_BUFFER_LEN];
    int i, len = 0, status = TLV_VALUE_STATUS_NOT_OK;
    struct tlv_hdr *tlv = NULL;
    struct tlv_to_config_name* cfg = NULL;

#ifdef _OPENWRT_
#else
    system("rfkill unblock wlan");
    sleep(1);
#endif

    memset(buffer, 0, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "killall %s 1>/dev/null 2>/dev/null",
             get_wpas_exec_file());
    system(buffer);
    sleep(3);

    /* Generate configuration */
    memset(buffer, 0, sizeof(buffer));
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
    snprintf(buffer, sizeof(buffer), "ctrl_interface=%s\nap_scan=1\n",
             get_wpas_ctrl_path_no_iface());
#else
    snprintf(buffer, sizeof(buffer), "ctrl_interface=%s\nap_scan=1\n",
             WPAS_CTRL_PATH_DEFAULT);
#endif

    for (i = 0; i < req->tlv_num; i++) {
        cfg = find_wpas_global_config_name(req->tlv[i]->id);
        if (cfg) {
            memset(value, 0, sizeof(value));
            memcpy(value, req->tlv[i]->value, req->tlv[i]->len);
            snprintf(cfg_item, sizeof(cfg_item), "%s=%s\n",
                     cfg->config_name, value);
            strlcat(buffer, cfg_item, sizeof(buffer));
        }
    }

    /* wps settings */
    tlv = find_wrapper_tlv_by_id(req, TLV_WPS_ENABLE);
    if (tlv) {
        memset(value, 0, sizeof(value));
        memcpy(value, tlv->value, tlv->len);
        /* To get STA wps vendor info */
        wps_setting *s = get_vendor_wps_settings(WPS_STA);
        if (!s) {
            indigo_logger(LOG_LEVEL_WARNING,
                          "Failed to get STAUT WPS settings");
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
            if (atoi(value) == WPS_ENABLE_NORMAL) {
                snprintf(cfg_item, sizeof(cfg_item),
                         "config_methods=virtual_display virtual_push_button keypad\npmf=1\n");
                strlcat(buffer, cfg_item, sizeof(buffer));
                indigo_logger(LOG_LEVEL_DEBUG,
                              "configure default STAUT WPS settings");
        }
#endif
        } else if (atoi(value) == WPS_ENABLE_NORMAL) {
            for (i = 0; i < STA_SETTING_NUM; i++) {
                memset(cfg_item, 0, sizeof(cfg_item));
                snprintf(cfg_item, sizeof(cfg_item), "%s=%s\n",
                         s[i].wkey, s[i].value);
                strlcat(buffer, cfg_item,sizeof(buffer));
            }
            indigo_logger(LOG_LEVEL_INFO, "STAUT Configure WPS");
        } else {
            indigo_logger(LOG_LEVEL_ERROR,
                          "Invalid WPS TLV value: %d (TLV ID 0x%04x)",
                          atoi(value), tlv->id);
        }
    } else {
        indigo_logger(LOG_LEVEL_WARNING,
                      "No WSC TLV found. Failed to append STA WSC data");
    }

    len = strlen(buffer);

    if (len) {
        write_file(get_wpas_conf_file(), buffer, len);
    }

    /* Start wpa supplicant */
    memset(buffer, 0 ,sizeof(buffer));
#if defined(ANDROID) || defined(MDM) || defined(LINUX_WCN)
    snprintf(buffer, sizeof(buffer), "%s -B -t -c %s %s -i %s",
             get_wpas_full_exec_path(),
             get_wpas_conf_file(),
             get_wpas_debug_arguments(),
             get_wireless_interface());
#else
    snprintf(buffer, sizeof(buffer),
             "%s -B -t -c %s %s -i %s -f /var/log/supplicant.log",
             get_wpas_full_exec_path(),
             get_wpas_conf_file(),
             get_wpas_debug_arguments(),
             get_wireless_interface());
#endif
    indigo_logger(LOG_LEVEL_DEBUG, "%s: SYSTEM_CMD: %s", __func__, buffer);
    system(buffer);
    sleep(4);

    status = TLV_VALUE_STATUS_OK;
    message = TLV_VALUE_WPA_S_START_UP_OK;

    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    return 0;
}
