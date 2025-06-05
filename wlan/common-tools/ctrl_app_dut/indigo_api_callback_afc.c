
/*
 * Copyright (c) 2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/* Copyright (c) 2020 Wi-Fi Alliance                                           */

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
#ifdef _OPENWRT_QTI_
#include "vendor_specific_openwrt.h"
#endif

#ifdef _OPENWRT_QTI_
extern char *indigo_radio_ifname[];
extern char *indigo_radio_ifname_ap[];
extern int num_radio;
int run_system(const char *cmd);
extern struct indigo_dut dut;
int generate_wireless_config(char *output, int output_size,
            struct packet_wrapper *wrapper,
            struct interface_info* wlanp, struct indigo_dut *dut);
#define BUFF_SIZE 1024
#endif

/* Save TLVs in afcd_configure and Send in afcd_operation */
char server_url[64];
char geo_area[8];

int afcd_get_info_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    int freq = 0 , channel = 0;
    char response[S_BUFFER_LEN];
    int status = TLV_VALUE_STATUS_OK;
    char *message = TLV_VALUE_OK;
#ifdef _OPENWRT_QTI_
    char buf[512];
    char buff[BUFF_SIZE];
    int id;
    id = (int)indigo_radio_ifname[0][strlen(indigo_radio_ifname[0]) - 1] - 48;

    snprintf(buf, sizeof(buf),
             "iwlist ath%d chan | grep Current | cut -c 48-50 | grep -Eo '[0-9]{1,3}'> /tmp/channel",
              id);

    if (run_system(buf) != 0)
        printf("Retrieve Channel failed");

    snprintf(buf, sizeof(buf), "/tmp/channel");
    FILE *fp = fopen(buf, "r");
    if (!fp) {
        printf("Failed to open %s", buf);
        status = TLV_VALUE_STATUS_NOT_OK;
        message = TLV_VALUE_OK;
        goto done;
    }
    fgets(buff, BUFF_SIZE, fp);
    fclose(fp);
    channel = atoi(buff);
#endif
    freq = 5950 + 5*channel;
done:
    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
    fill_wrapper_tlv_byte(resp, TLV_STATUS, status);
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);

    snprintf(response, sizeof(response), "%d", freq);
    fill_wrapper_tlv_bytes(resp, TLV_AFC_OPER_FREQ, strlen(response), response);
    snprintf(response, sizeof(response), "%d", channel);
    fill_wrapper_tlv_bytes(resp, TLV_AFC_OPER_CHANNEL, strlen(response), response);
    return 0;
}

#define ELLIPSE 0
#define LINEARPOLYGON 1
#define RADIALPOLYGON 2
int afcd_configure_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    char *message = TLV_VALUE_OK;
    struct tlv_hdr *tlv;
    int i = 0;
    char security[8];
    char wpa_passphrase[101];
    char ssid[33];
    char afc_bandwidth[8];
#ifdef _OPENWRT_QTI_
    int len = -1;
    char buf[512];
    char buff[BUFF_SIZE];
    char *pos;
    int id;
    id = (int)indigo_radio_ifname[0][strlen(indigo_radio_ifname[0]) - 1] - 48;
#endif

    for (i = 0; i < req->tlv_num; i++) {
        struct indigo_tlv *i_tlv;
        char tlv_value[64];
        i_tlv = get_tlv_by_id(req->tlv[i]->id);
        if (i_tlv) {
                memset(tlv_value, 0, sizeof(tlv_value));
                memcpy(tlv_value, req->tlv[i]->value, req->tlv[i]->len);
                indigo_logger(LOG_LEVEL_DEBUG, "TLV: %s - %s", i_tlv->name, tlv_value);
        }
    }

    tlv = find_wrapper_tlv_by_id(req, TLV_AFC_SERVER_URL);
    if (tlv) {
        memset(server_url, 0, sizeof(server_url));
        memcpy(server_url, tlv->value, (tlv->len < sizeof(server_url)) ? tlv->len : (sizeof(server_url) - 1));
#ifdef _OPENWRT_QTI_
        snprintf(buff, sizeof(buff), "afc_base_url");
        snprintf(buf, sizeof(buf), "/etc/afc/afc_ipq.conf");
        FILE *fp = fopen(buf, "a+");
        if (fp != NULL) {
            int found = 0;
            while(fgets(buff, BUFF_SIZE, fp)) {
                char fpar[40] = {'\0'};

                if((pos = strchr(buff, '=')) != NULL)
                    strlcpy(fpar, buff, (pos - buff) * sizeof(char));

                if (strcmp(fpar, buff) == 0) {
                    pos++;
                    fputs(server_url, fp);
                    found=1;
                    break;
                }
            }
            if(!found) {
               fputs("afc_base_url=", fp);
               fputs(server_url, fp);
            }
            len = 1;
            fclose(fp);
            run_system("/etc/init.d/afc restart");
        } else {
            indigo_logger(LOG_LEVEL_DEBUG, "File /etc/afc/afc_ipq.conf not found");
        }
#endif
    }

    tlv = find_wrapper_tlv_by_id(req, TLV_AFC_BANDWIDTH);
    if (tlv) {
        memset(afc_bandwidth, 0, sizeof(afc_bandwidth));
        memcpy(afc_bandwidth, tlv->value, MIN(tlv->len, sizeof(afc_bandwidth) - 1));
#ifdef _OPENWRT_QTI_
        if (atoi(afc_bandwidth) == 0) {
            indigo_logger(LOG_LEVEL_DEBUG, "Configure DUT to 20MHz bandwidth");
            owrt_ap_set_radio(id, "htmode", "HT20");
	} else if (atoi(afc_bandwidth) == 1) {
            indigo_logger(LOG_LEVEL_DEBUG, "Configure DUT to 40MHz bandwidth");
            owrt_ap_set_radio(id, "htmode", "HT40");
	} else if (atoi(afc_bandwidth) == 2) {
            indigo_logger(LOG_LEVEL_DEBUG, "Configure DUT to 80MHz bandwidth");
            owrt_ap_set_radio(id, "htmode", "HT80");
	} else if (atoi(afc_bandwidth) == 3) {
            indigo_logger(LOG_LEVEL_DEBUG, "Configure DUT to 160MHz bandwidth");
            owrt_ap_set_radio(id, "htmode", "HT160");
	}
#endif
    }

    /* BSS Configurations: SSID, Security, Passphrase */
    tlv = find_wrapper_tlv_by_id(req, TLV_AFC_SECURITY_TYPE);
    if (tlv) {
        memset(security, 0, sizeof(security));
        memcpy(security, tlv->value, MIN(tlv->len, sizeof(security) - 1));
        if (atoi(security) == 0) {
            indigo_logger(LOG_LEVEL_DEBUG, "Configure SAE");
#ifdef _OPENWRT_QTI_
            snprintf(buf, sizeof(buf), "psk2+ccmp");
            owrt_ap_set_vap(id, "encryption", buf);
            owrt_ap_set_vap(id, "sae", security);
#endif
        }
    }

    tlv = find_wrapper_tlv_by_id(req, TLV_AFC_TEST_SSID);
    if (tlv) {
        memset(ssid, 0, sizeof(ssid));
        memcpy(ssid, tlv->value, tlv->len);
#ifdef _OPENWRT_QTI_
        owrt_ap_set_radio(id, "disabled", "0");
        owrt_ap_set_radio(id, "band", "3");
        owrt_ap_set_radio(id, "radio_qdf_cv_lvl", "0x94000a");
        owrt_ap_set_radio(id, "regdomain", "0x90000");
        owrt_ap_set_radio(id, "channel", "auto");
        owrt_ap_set_iniwifi("enable_6ghz_sp_pwrmode_supp", "1");
        snprintf(buf, sizeof(buf), "\"%s\"", "ACS_WiFi-Test");
        owrt_ap_set_vap(id, "ssid", buf);
        owrt_ap_set_vap(id, "en_6g_sec_comp", "0");
        snprintf(buf, sizeof(buf), "\'%s\'", "WPA-PSK SAE");
        owrt_ap_set_vap(id, "wpa_key_mgmt", buf);
#endif
    }

    tlv = find_wrapper_tlv_by_id(req, TLV_WPA_PASSPHRASE);
    if (tlv) {
        memset(wpa_passphrase, 0, sizeof(wpa_passphrase));
        memcpy(wpa_passphrase, tlv->value, tlv->len);
#ifdef _OPENWRT_QTI_
        snprintf(buf, sizeof(buf), "\"%s\"", wpa_passphrase);
        owrt_ap_set_vap(id, "key", buf);
        owrt_ap_set_list_vap(id, "sae_password", buf);
#endif
    }

    /* Mandatory Registration Configurations */
    tlv = find_wrapper_tlv_by_id(req, TLV_AFC_LOCATION_GEO_AREA);
    if (tlv) {
        memset(geo_area, 0, sizeof(geo_area));
        memcpy(geo_area, tlv->value, MIN(tlv->len, sizeof(geo_area) - 1));
        if (atoi(geo_area) == ELLIPSE) {
            tlv = find_wrapper_tlv_by_id(req, TLV_AFC_ELLIPSE_CENTER);
            tlv = find_wrapper_tlv_by_id(req, TLV_AFC_ELLIPSE_MAJOR_AXIS);
            tlv = find_wrapper_tlv_by_id(req, TLV_AFC_ELLIPSE_MINOR_AXIS);
            tlv = find_wrapper_tlv_by_id(req, TLV_AFC_ELLIPSE_ORIENTATION);
        } else if (atoi(geo_area) == LINEARPOLYGON) {
            tlv = find_wrapper_tlv_by_id(req, TLV_AFC_LINEARPOLY_BOUNDARY);
        } else if (atoi(geo_area) == RADIALPOLYGON){
            tlv = find_wrapper_tlv_by_id(req, TLV_AFC_RADIALPOLY_CENTER);
            tlv = find_wrapper_tlv_by_id(req, TLV_AFC_RADIALPOLY_BOUNDARY);
        }
    }

    /* AFCD vendors should have their own freq_range or global op_class + channel CFI */
#ifdef _OPENWRT_QTI_
    run_system("uci commit wireless");
    len = run_system("wifi");
#endif

    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
#ifdef _OPENWRT_QTI_
    fill_wrapper_tlv_byte(resp, TLV_STATUS, len >= 0 ? TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
#else
    fill_wrapper_tlv_byte(resp, TLV_STATUS, TLV_VALUE_STATUS_OK);
#endif
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    return 0;
}


int afcd_operation_handler(struct packet_wrapper *req, struct packet_wrapper *resp) {
    struct tlv_hdr *tlv;
    char req_type[8];
    char frame_bw[8];
    char *message = TLV_VALUE_OK;
#ifdef _OPENWRT_QTI_
    int len = -1;
    int id;
    id = (int)indigo_radio_ifname[0][strlen(indigo_radio_ifname[0]) - 1] - 48;
#endif
    tlv = find_wrapper_tlv_by_id(req, TLV_AFC_DEVICE_RESET);
    if (tlv) {
        indigo_logger(LOG_LEVEL_DEBUG, "Device reset");
        /* Vendor specific: add in vendor_specific_afc.c */
#ifdef _OPENWRT_QTI_
        len = run_system("wifi load");
        sleep(1);
#endif
    }
    tlv = find_wrapper_tlv_by_id(req, TLV_AFC_SEND_SPECTRUM_REQ);
    if (tlv) {
        memset(req_type, 0, sizeof(req_type));
        memcpy(req_type, tlv->value, MIN(tlv->len, sizeof(req_type) - 1));
        if (atoi(req_type) == 0) {
            indigo_logger(LOG_LEVEL_DEBUG, "Send Spectrum request with Channel and Frequency based");
#ifdef _OPENWRT_QTI_
           len = send_afc_spectrum_req(id);
#endif
        } else if (atoi(req_type) == 1) {
            indigo_logger(LOG_LEVEL_DEBUG, "Send Spectrum request with Channel based");
        } else if (atoi(req_type) == 2) {
            indigo_logger(LOG_LEVEL_DEBUG, "Send Spectrum request with Frequency based");
        }
    }
    tlv = find_wrapper_tlv_by_id(req, TLV_AFC_POWER_CYCLE);
    if (tlv) {
        indigo_logger(LOG_LEVEL_DEBUG, "Trigger power cycle");
        /* Vendor specific: add in vendor_specific_afc.c */
#ifdef _OPENWRT_QTI_
        len = run_system("wifi load");
#endif
    }

    tlv = find_wrapper_tlv_by_id(req, TLV_AFC_SEND_TEST_FRAME);
    if (tlv) {
        memset(frame_bw, 0, sizeof(frame_bw));
        memcpy(frame_bw, tlv->value, (tlv->len < sizeof(frame_bw)) ? tlv->len : (sizeof(frame_bw) - 1));
#ifdef _OPENWRT_QTI_
        if (atoi(frame_bw) == 0) {
           indigo_logger(LOG_LEVEL_DEBUG, "Trigger DUT to send test frames for 20MHz bandwidth");
           len = send_afc_test_frame(id, 0);
        } else if (atoi(frame_bw) == 1) {
           indigo_logger(LOG_LEVEL_DEBUG, "Trigger DUT to send test frames for 40MHz bandwidth");
           len = send_afc_test_frame(id, 1);
        } else if (atoi(frame_bw) == 2) {
           indigo_logger(LOG_LEVEL_DEBUG, "Trigger DUT to send test frames for 80MHz bandwidth");
           len = send_afc_test_frame(id, 2);
        } else if (atoi(frame_bw) == 3) {
           indigo_logger(LOG_LEVEL_DEBUG, "Trigger DUT to send test frames for 160MHz bandwidth");
           len = send_afc_test_frame(id, 3);
        }
        indigo_logger(LOG_LEVEL_DEBUG, "Triggered Test Frame");

#endif
    }

    fill_wrapper_message_hdr(resp, API_CMD_RESPONSE, req->hdr.seq);
#ifdef _OPENWRT_QTI_
    fill_wrapper_tlv_byte(resp, TLV_STATUS, len >= 0 ? TLV_VALUE_STATUS_OK : TLV_VALUE_STATUS_NOT_OK);
#else
    fill_wrapper_tlv_byte(resp, TLV_STATUS, TLV_VALUE_STATUS_OK);
#endif
    fill_wrapper_tlv_bytes(resp, TLV_MESSAGE, strlen(message), message);
    return 0;
}
