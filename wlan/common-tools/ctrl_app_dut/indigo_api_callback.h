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

#ifndef _INDIGO_API_CALLBACK
#define _INDIGO_API_CALLBACK


#define LOOPBACK_TIMEOUT 180

/*Shared between AFC and DUT apis */

struct tlv_to_config_name {
    unsigned short tlv_id;
    char config_name[NAME_SIZE];
    int quoted;
};

struct anqp_tlv_to_config_name {
    char element[NAME_SIZE];
    char config[NAME_SIZE];
};

extern struct tlv_to_config_name maps[];
extern struct tlv_to_config_name semicolon_list[];
extern struct anqp_tlv_to_config_name anqp_maps[];
char* find_tlv_config_name(int tlv_id);
struct tlv_to_config_name* find_tlv_config(int tlv_id);
extern struct tlv_to_config_name wpas_global_maps[];
struct tlv_to_config_name* find_wpas_global_config_name(int tlv_id);
struct tlv_to_config_name* find_generic_tlv_config(int tlv_id, struct tlv_to_config_name* arr, int arr_size);

/* Basic */
int get_control_app_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int start_loopback_server(struct packet_wrapper *req, struct packet_wrapper *resp);
int stop_loop_back_server_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int send_loopback_data_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int stop_loopback_data_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int create_bridge_network_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int assign_static_ip_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int get_mac_addr_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int get_ip_addr_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int reset_device_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int start_dhcp_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int stop_dhcp_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int get_wsc_pin_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int get_wsc_cred_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
/* AP */
int stop_ap_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int configure_ap_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int start_ap_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int send_ap_disconnect_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int set_ap_parameter_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int send_ap_btm_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int trigger_ap_channel_switch(struct packet_wrapper *req, struct packet_wrapper *resp);
int send_ap_arp_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int start_wps_ap_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int configure_ap_wsc_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
/* STA */
int stop_sta_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int configure_sta_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int associate_sta_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int start_up_sta_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int send_sta_disconnect_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int send_sta_reconnect_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int send_sta_btm_query_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int send_sta_anqp_query_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int sta_scan_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int set_sta_parameter_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int set_sta_hs2_associate_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int sta_add_credential_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int set_sta_install_ppsmo_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int set_sta_phy_mode_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int set_sta_channel_width_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int set_sta_power_save_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int start_wps_sta_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int enable_wsc_sta_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
/* P2P */
int start_up_p2p_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int p2p_find_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int p2p_listen_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int add_p2p_group_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int stop_p2p_group_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int p2p_start_wps_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int p2p_connect_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int get_p2p_intent_value_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int p2p_invite_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int set_p2p_serv_disc_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int set_p2p_ext_listen_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
/* AFC */
int afcd_configure_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int afcd_operation_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
int afcd_get_info_handler(struct packet_wrapper *req, struct packet_wrapper *resp);
#endif // __INDIGO_API_CALLBACK
