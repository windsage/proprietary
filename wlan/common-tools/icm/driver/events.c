/*
 * Copyright (c) 2017-2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * this is to process nl80211 events that are called by driver_nl80211.c
 */

#include "icm.h"
#include "icm_internal.h"

#define ENUM2STRING(x) case x: return #x;
// if MBL flags is enabled, process enums as per mbl driver definitions
#ifndef ICM_RTR_DRIVER
/* copy of MBL driver enum */
typedef enum {
    /* 11a/b/g only, no HT, no proprietary */
    eCSR_DOT11_MODE_abg = 0x0001,
    eCSR_DOT11_MODE_11a = 0x0002,
    eCSR_DOT11_MODE_11b = 0x0004,
    eCSR_DOT11_MODE_11g = 0x0008,
    eCSR_DOT11_MODE_11n = 0x0010,
    eCSR_DOT11_MODE_11g_ONLY = 0x0020,
    eCSR_DOT11_MODE_11n_ONLY = 0x0040,
    eCSR_DOT11_MODE_11b_ONLY = 0x0080,
    eCSR_DOT11_MODE_11ac = 0x0100,
    eCSR_DOT11_MODE_11ac_ONLY = 0x0200,
    /*
     * This is for WIFI test. It is same as eWNIAPI_MAC_PROTOCOL_ALL
     * except when it starts IBSS in 11B of 2.4GHz
     * It is for CSR internal use
     */
    eCSR_DOT11_MODE_AUTO = 0x0400,

    /* specify the number of maximum bits for phyMode */
    eCSR_NUM_PHY_MODE = 16,
} eCsrPhyMode;

typedef enum {
    eCSR_BAND_ALL,
    eCSR_BAND_24,
    eCSR_BAND_5G,
    eCSR_BAND_MAX,
} eCsrBand;

ICM_CH_BW_T convert_mbl_to_RTR_chan_width(enum phy_ch_width mbl_ch_width)
{
    switch (mbl_ch_width){
        case CH_WIDTH_20MHZ:
            return ICM_CH_BW_20;
        case CH_WIDTH_40MHZ:
            return ICM_CH_BW_40;
        case CH_WIDTH_80MHZ:
            return ICM_CH_BW_80;
        case CH_WIDTH_160MHZ:
            return ICM_CH_BW_160;
        case CH_WIDTH_80P80MHZ:
            return ICM_CH_BW_80_PLUS_80;
        default:
            return ICM_CH_BW_INVALID;
    }

    return ICM_CH_BW_INVALID;
}

int convert_RTR_to_mbl_chan_width(ICM_CH_BW_T rtr_ch_width)
{
    switch (rtr_ch_width){
        case ICM_CH_BW_20:
            return CH_WIDTH_20MHZ;
        case ICM_CH_BW_40MINUS:
        case ICM_CH_BW_40PLUS:
        case ICM_CH_BW_40:
            return CH_WIDTH_40MHZ;
        case ICM_CH_BW_80:
            return CH_WIDTH_80MHZ;
        case ICM_CH_BW_160:
            return CH_WIDTH_160MHZ;
        case ICM_CH_BW_80_PLUS_80:
            return CH_WIDTH_80P80MHZ;
        default:
            return CH_WIDTH_INVALID;
    }

    return CH_WIDTH_INVALID;
}

ICM_PHY_SPEC_T convert_mbl_to_RTR_phy_mode(eCsrPhyMode mbl_phy_mode, ICM_BAND_T scan_band)
{
    switch (mbl_phy_mode){
        case eCSR_DOT11_MODE_abg:
            return ICM_PHY_SPEC_ANY;
        case eCSR_DOT11_MODE_11a:
            return ICM_PHY_SPEC_11A;
        case eCSR_DOT11_MODE_11b:
        case eCSR_DOT11_MODE_11b_ONLY:
            return ICM_PHY_SPEC_11B;
        case eCSR_DOT11_MODE_11g:
        case eCSR_DOT11_MODE_11g_ONLY:
            return ICM_PHY_SPEC_11G;
        case eCSR_DOT11_MODE_11n_ONLY:
        case eCSR_DOT11_MODE_11n:
        case eCSR_DOT11_MODE_AUTO:
            if (scan_band == ICM_BAND_2_4G)
                return ICM_PHY_SPEC_11NG;
            else
                return ICM_PHY_SPEC_11NA;
        case eCSR_DOT11_MODE_11ac:
        case eCSR_DOT11_MODE_11ac_ONLY:
            return ICM_PHY_SPEC_11AC;
        /* 11AX TODO: Add MBL conversions for 11ax here */
        default:
            return ICM_PHY_SPEC_INVALID;
    }

    return ICM_PHY_SPEC_INVALID;
}

ICM_BAND_T convert_mbl_to_RTR_scan_band(eCsrBand mbl_scan_band)
{
    switch (mbl_scan_band){
        case eCSR_BAND_24:
            return ICM_BAND_2_4G;
        case eCSR_BAND_5G:
            return ICM_BAND_5G;
        default:
            return ICM_BAND_INVALID;
    }

    return ICM_BAND_INVALID;
}
#else /* ICM_RTR_DRIVER */
/*
 * convert_bandID_to_icm_band - Convert wlan_band_id to ICM_BAND_T
 *
 * @band: wlan_band_id value giving the operational band
 * Return: ICM_BAND_T value corresponding to @band
 */
ICM_BAND_T convert_bandID_to_icm_band(enum wlan_band_id band)
{
    ICM_BAND_T icm_band;

    switch (band) {
        case WLAN_BAND_2GHZ:
            icm_band = ICM_BAND_2_4G;
            break;
        case WLAN_BAND_5GHZ:
            icm_band = ICM_BAND_5G;
            break;
        case WLAN_BAND_6GHZ:
            icm_band = ICM_BAND_6G;
            break;
        default:
            icm_band = ICM_BAND_INVALID;
            break;
    }

    return icm_band;
}
#endif /* ICM_RTR_DRIVER */

const char * icm_band_to_string(ICM_BAND_T band)
{
    switch (band) {
        ENUM2STRING(ICM_BAND_2_4G)
        ENUM2STRING(ICM_BAND_5G)
        ENUM2STRING(ICM_BAND_6G)
        default:
            return "ICM_BAND_INVALID";
    }
}

const char * icm_chan_width_to_string(ICM_CH_BW_T chan_width)
{
    switch (chan_width) {
        ENUM2STRING(ICM_CH_BW_20)
        ENUM2STRING(ICM_CH_BW_40MINUS)
        ENUM2STRING(ICM_CH_BW_40PLUS)
        ENUM2STRING(ICM_CH_BW_40)
        ENUM2STRING(ICM_CH_BW_80)
        ENUM2STRING(ICM_CH_BW_160)
        ENUM2STRING(ICM_CH_BW_80_PLUS_80)
        ENUM2STRING(ICM_CH_BW_320)
        default:
            return "ICM_CH_BW_INVALID";
    }
}

const char * icm_phy_mode_to_string(ICM_PHY_SPEC_T phy_mode)
{
    switch (phy_mode) {
        ENUM2STRING(ICM_PHY_SPEC_11A)
        ENUM2STRING(ICM_PHY_SPEC_11B)
        ENUM2STRING(ICM_PHY_SPEC_11G)
        ENUM2STRING(ICM_PHY_SPEC_FH)
        ENUM2STRING(ICM_PHY_SPEC_TURBO_A)
        ENUM2STRING(ICM_PHY_SPEC_TURBO_G)
        ENUM2STRING(ICM_PHY_SPEC_11NA)
        ENUM2STRING(ICM_PHY_SPEC_11NG)
        ENUM2STRING(ICM_PHY_SPEC_11AC)
        ENUM2STRING(ICM_PHY_SPEC_11AXG)
        ENUM2STRING(ICM_PHY_SPEC_11AXA)
        ENUM2STRING(ICM_PHY_SPEC_11BEG)
        ENUM2STRING(ICM_PHY_SPEC_11BEA)
        ENUM2STRING(ICM_PHY_SPEC_ANY)
        default:
            return "ICM_PHY_SPEC_INVALID";
    }
}

const char * icm_acs_policy_to_string(u32 policy)
{
    switch (policy) {
        case QCA_WLAN_VENDOR_ATTR_EXTERNAL_ACS_POLICY_PCL_PREFERRED:
            return "PCL_PREFERRED";
        case QCA_WLAN_VENDOR_ATTR_EXTERNAL_ACS_POLICY_PCL_MANDATORY:
            return "PCL_MANDATORY";
        default:
            return "Invalid/No Policy";
    }
}

u64 get_channel_free_time(u64 time, u64 time_busy)
{
    u64 chan_free_time = 0;

    if (time) {
        chan_free_time = ((time - time_busy) * MAX_USABILITY) / time;
    } else {
        /* Cycle count cannot be zero. But if so,
         * set the chan_free_time to zero */
        chan_free_time = 0;
    }
    return chan_free_time;
}

// get scan results and process here
static void event_scan_results(ICM_INFO_T *picm)
{
    // Call to get kernel bss table here.
    struct scan_results *res;
    struct scan_res *r;
    ICM_SCAN_INFO_T *elem;
    int i, k;
    u8 *pos;
    const u8 *ssid;
    struct iwscan_state iw_state; // Only for passing AP index in scanlist
    ICM_DEV_INFO_T* pdev = get_pdev();
    ICM_CHANNEL_LIST_T *pchlist = NULL;
    int chn_idx = -1;

    /* handle aborted case here. Close ICM */
    if(picm->drv.aborted) {
        icm_cleanup(pdev);
        return;
    }

    res = driver_nl80211_get_scan_results(picm);

    if (res == NULL) {
        icm_printf("Failed to get scan results\n");
        return;
    }

    if (picm->rropavail_info == ICM_RROPAVAIL_INFO_VSCAN_END) {
        if (icm_wal_get_chan_rropinfo(picm)) {
            icm_printf("Failed to get RROP info\n");
            return;
        }
    }

#ifdef ICM_RTR_DRIVER
    if (icm_wal_get_channel_status(picm)) {
        icm_printf("Failed to get channel status info\n");
        return;
    }
#endif /* ICM_RTR_DRIVER */

    // We got scan_res here. send/fill corresponding ICM structure now
    // map scan_results to icm_scan_list and scan_res to icm_scan_info
    icm_printf("Scan Results received:\n");
    for (i = 0; i < res->num; i++) {
        ICM_BAND_T band = ICM_BAND_INVALID;

        if (i >= MAX_SCAN_ENTRIES) {
            icm_printf("max limit reached. can't store more than %d\n", MAX_SCAN_ENTRIES);
            break;
        }

        r = res->res[i];

        band = icm_get_band_from_freq(r->freq);
        if  (ICM_BAND_INVALID == band) {
            icm_printf("Unable to map frequency %u to recognized band. Skipping scan entry.\n",
                    r->freq);
            continue;
        }

        elem = &picm->slist.elem[i];
        memcpy(elem->bssid, r->bssid, ETH_ALEN);
        elem->freq = (double) r->freq;
        elem->channel = icm_convert_mhz2channel(r->freq);
        elem->band = band;
        /* what is mode in ICM_SCAN_INFO_T */
        elem->rssi = r->level;
        elem->rssi_valid = true;
        elem->valid = true;

        /* get SSID from scan ie */
        pos = (u8 *) (r + 1);
        ssid = get_ie(pos, r->ie_len, WLAN_EID_SSID);
        if (ssid != NULL) {
            /* TYPE | LEN | VALUE */
            elem->ssid_len = *(ssid + 1);
            memcpy(elem->ssid, ssid + 2, elem->ssid_len);
            elem->ssid[elem->ssid_len] = '\0';
        } else {
            elem->ssid[0] = '\0';
            elem->ssid_len = 0;
        }

        /* Get the relevant operating band */
        pchlist = ICM_GET_BAND_CHANNEL_LIST_PTR(picm, band);
        ICM_ASSERT(pchlist != NULL);

        /* update OBSS for 20 MHz bandwidth */
        chn_idx = icm_get_channel_index(pchlist, elem->channel);
        if (chn_idx != -1) {
            pchlist->ch[chn_idx].num_wnw++;
            pchlist->ch[chn_idx].num_wnw_20++;

            /* Update Wireless Preference factor for this bss */
            picm->wpf = icm_get_wpf(r->level);
            pchlist->ch[chn_idx].num_wpf += picm->wpf;
            pchlist->ch[chn_idx].num_wpf_20 += picm->wpf;
        }

        iw_state.ap_num = i;
        icm_parse_gen_ie(picm, &iw_state, pos, r->ie_len);

        icm_printf("\tSSID: %s, BSSID: "MACSTR", freq: %d, rssi: %d wpf: %lf\n",
                elem->ssid, MAC2STR(r->bssid), r->freq, r->level, picm->wpf);
    }

    if (res->info.count) {

        /* Only fill noisefloor, cycle_count and channel_free_time,
         * rest are calucated and stored dynamically */
        for (k = 0; k < res->info.count; k++) {
            ICM_BAND_T band = ICM_BAND_INVALID;

            band = icm_get_band_from_freq(res->info.freq[k]);
            if  (ICM_BAND_INVALID == band) {
                icm_printf("Unable to map frequency %u to recognized band. Skipping noise info entry.\n",
                        res->info.freq[k]);
                continue;
            }

            chn_idx = icm_convert_mhz2channel(res->info.freq[k]);

            ICM_SET_CHANNEL_NOISEFLOOR(picm, band, chn_idx,
                    (signed char)res->info.noise[k]);
            ICM_SET_CHANNEL_CYCLE_COUNT(picm, band, chn_idx,
                    res->info.time[k]);
            ICM_SET_CHANNEL_FREE_TIME(picm, band, chn_idx,
                    get_channel_free_time(res->info.time[k],
                        res->info.time_busy[k]));

#ifdef ICM_RTR_DRIVER
            icm_printf("\t freq %u, noisefloor: %d, ",
                       res->info.freq[k],
                       ICM_GET_CHANNEL_NOISEFLOOR(picm, band, chn_idx));
            icm_printf("time [cycle_count] %u, time_busy %llu, "
                       "channel free time: %u\n",
                       ICM_GET_CHANNEL_CYCLE_COUNT(picm, band, chn_idx),
                       res->info.time_busy[k],
                       ICM_GET_CHANNEL_FREE_TIME(picm, band, chn_idx));
#else
            icm_printf("\t freq %u, noisefloor: %d, "
                       "time [cycle_count] %lu, time_busy %lu, "
                       "channel free time: %d, wpf: %lf\n",
                       res->info.freq[k],
                       ICM_GET_CHANNEL_NOISEFLOOR(picm, band, chn_idx),
                       ICM_GET_CHANNEL_CYCLE_COUNT(picm, band, chn_idx),
                       res->info.time_busy[k],
                       ICM_GET_CHANNEL_FREE_TIME(picm, band, chn_idx),
                       picm->chlist.ch[i].num_wpf);
#endif
        }
    }

    /* Free res here */
    scan_results_free(res);

    /* start channel selection */
    icm_cfg_select_channel(picm, TRUE);
}

static void event_start_acs(ICM_INFO_T *picm, void *event_data)
{
    ICM_CAPABILITY_INFO_T *acs_data = event_data;
    ICM_DEV_INFO_T* pdev = get_pdev();
    int i = 0, j = 0;
#ifdef ICM_RTR_DRIVER
    int ret = -1;
    ICM_BAND_T band = ICM_BAND_INVALID;
    int chan_idx = -1;
#endif /* ICM_RTR_DRIVER */

    if (acs_data == NULL) {
        icm_printf("No data received\n");
        return;
    }
#ifndef ICM_RTR_DRIVER
    /* call phy_mode conversion only after scan_band is mapped */
    acs_data->phy_mode = convert_mbl_to_RTR_phy_mode(acs_data->phy_mode, acs_data->scan_band);
    acs_data->chan_width = convert_mbl_to_RTR_chan_width(acs_data->chan_width);
#else
    acs_data->chan_width = icm_get_radio_channel_width(acs_data->phy_mode);
    acs_data->phy_mode = convert_phymode_to_icm_physpec(acs_data->phy_mode);
#endif /* ICM_RTR_DRIVER */

    if (icm_vap_init_with_cap_info(pdev, acs_data, picm->dev_index)
            != SUCCESS) {
        return;
    }

    if (picm->rropavail_info == ICM_RROPAVAIL_INFO_EXTERNAL_ACS_START) {
        if (icm_wal_get_chan_rropinfo(picm)) {
            icm_printf("Failed to get RROP info\n");
            return;
        }
    }

#ifdef ICM_RTR_DRIVER
    picm->pri20_blockchanlist.num_freq = 0;
    if ((ret = icm_wal_get_radio_pri20_blockchanlist(picm)) != 0) {
        icm_printf("Failed to get pri20 blockchanlist.\n");
        return;
    }

    if (picm->pri20_blockchanlist.num_freq > 0) {
        for (i = 0; i < picm->pri20_blockchanlist.num_freq; i++) {
            band = icm_get_band_from_freq(\
                        picm->pri20_blockchanlist.freq[i]);
            if  (ICM_BAND_INVALID == band) {
                icm_printf("Unable to map frequency %u to recognized band. Skipping pri20 blocked chanlist entry.\n",
                        picm->pri20_blockchanlist.freq[i]);
                continue;
            }

            chan_idx = icm_convert_mhz2channel(\
                    picm->pri20_blockchanlist.freq[i]);

            ICM_SET_OPRI20CHANNEL_EXCLUDE(picm, band, chan_idx);

            icm_printf("Frequency %u blocked for use as pri20. May still be used as sec20/sec40/sec80 where applicable if other exlusions do not apply.\n",
                       picm->pri20_blockchanlist.freq[i]);
        }
    }
#endif /* ICM_RTR_DRIVER */

    /*
     * All pri20 channels are assumed to have the max primary channel metric
     * unless the driver/target provides a lower value.
     */
    for (i = 0; i < ICM_BAND_MAX; i++) {
        for (j = 0; j < MAX_NUM_CHANNEL; j++) {
            ICM_SET_PRI20CHANNEL_GRADE(picm, i, j,
                    NL80211_RF_CHARACTERIZATION_CHAN_METRIC_MAX);
        }
    }

#ifdef ICM_RTR_DRIVER
    picm->chan_rf_characterization.num_prichannels = 0;
    if ((ret = icm_wal_get_radio_chan_rf_characterization_info(picm)) != 0) {
        icm_printf("Failed to get RF characterization info.\n");
        return;
    }

    if (picm->chan_rf_characterization.num_prichannels > 0) {
        for (i = 0; i < picm->chan_rf_characterization.num_prichannels; i++) {
            if (picm->chan_rf_characterization.prichaninfo[i].bw !=
                                                        CH_WIDTH_20MHZ) {
                icm_printf("Freq=%hu: Chan BW %hhu is not handled "
                           "currently. Skipping.\n",
                       picm->chan_rf_characterization.prichaninfo[i].freq,
                       picm->chan_rf_characterization.prichaninfo[i].bw);
                continue;
            }

            band = icm_get_band_from_freq(\
                        picm->chan_rf_characterization.prichaninfo[i].freq);
            if  (ICM_BAND_INVALID == band) {
                icm_printf("Unable to map frequency %hu to recognized band. Skipping RF characterization entry.\n",
                        picm->chan_rf_characterization.prichaninfo[i].freq);
                continue;
            }

            chan_idx = icm_convert_mhz2channel(\
                    picm->chan_rf_characterization.prichaninfo[i].freq);

            ICM_SET_PRI20CHANNEL_GRADE(picm, band, chan_idx,
                picm->chan_rf_characterization.prichaninfo[i].chan_metric);
        }
    }
#endif /* ICM_RTR_DRIVER */

    icm_wal_do_80211_scan(picm);
}

// Process all NL80211 driver events that are sent to ICM application
void process_nl80211_events(void *ctx, enum event_type event, void *event_data)
{
    ICM_INFO_T *picm = ctx;

    switch (event) {
        case EVENT_SCAN_RESULTS:
            event_scan_results(picm);
            break;
        case EVENT_START_ACS:
            event_start_acs(picm, event_data);
            break;
        default:
            icm_printf("ICM: unknown event_type\n");
            break;
    }
}
#undef ENUM2STRING
