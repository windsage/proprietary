/*
 * Copyright (c) 2012,2017-2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * 2012 Qualcomm Atheros, Inc..
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 *
 * =====================================================================================
 *
 *       Filename:  icm.h
 *
 *    Description:  Intelligent Channel Manager
 *
 *        Version:  1.0
 *        Created:  04/19/2012 01:17:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  S.Karthikeyan (),
 *
 * =====================================================================================
 */


#ifndef __ICM_H__
#define __ICM_H__

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <string.h>
#include <net/if.h>
#include <linux/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/netlink.h>
#include <netinet/in.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#ifdef WLAN_SPECTRAL_ENABLE
#ifndef __packed
#define __packed __attribute__((packed))
#endif
#include "spec_msg_proto.h"
#endif

/*
 * Linux uses __BIG_ENDIAN and __LITTLE_ENDIAN while BSD uses _foo and an
 * explicit _BYTE_ORDER. We define things the BSD way since BSD got to this
 * first, and for compliance with rest of the QCA framework.
 */
#ifndef _LITTLE_ENDIAN
#define _LITTLE_ENDIAN 1234 /* LSB first */
#endif
#ifndef _BIG_ENDIAN
#define _BIG_ENDIAN 4321 /* MSB first */
#endif
#include <asm/byteorder.h>
#if defined(__LITTLE_ENDIAN)
#define _BYTE_ORDER _LITTLE_ENDIAN
#elif defined(__BIG_ENDIAN)
#define _BYTE_ORDER _BIG_ENDIAN
#else
#error "Please fix asm/byteorder.h"
#endif

#ifdef ICM_RTR_DRIVER
#include "ieee80211_external.h"
#endif /* ICM_RTR_DRIVER */
#ifdef WLAN_SPECTRAL_ENABLE
#include "spectral_ioctl.h"
#include "spectral_data.h"
#endif /* WLAN_SPECTRAL_ENABLE */

#ifdef ICM_RTR_DRIVER
#include "if_athioctl.h"
#include "a_types.h"
#include "ol_ath_ucfg.h"
#include "spectral.h"
#endif /* ICM_RTR_DRIVER */
#include "wireless_copy.h"
#include "common.h"
#include "driver_nl80211.h"
#include "rtr_driver_defs.h"
#define __bool_already_defined__
#include "cdp_txrx_stats_struct.h"

#define not_yet()   icm_printf("Not Yet : %s : %d\n", __func__,  __LINE__)
#define no_third_radio_support()    icm_printf("No third radio support yet\n")

#define LINESTR     "-----------------------------------------------------------\n"

#ifndef NETLINK_ATHEROS
#define NETLINK_ATHEROS     (17)
#endif /* NETLINK_ATHEROS */

#ifndef NETLINK_GENERIC
#define NETLINK_GENERIC     (16)
#endif  /* NETLINK_GENERIC */

#define NBBY    8           /* number of bits/byte */

#define RTNETLINKBUFSIZ         (4096)

/* IFNAMSIZ definition */
#ifndef IFNAMSIZ
#define IFNAMSIZ    16
#endif

#ifdef ICM_RTR_DRIVER
#define OL_ATH_PARAM_SHIFT     0x1000

#ifndef MAX
#define MAX(a, b)    (((a) > (b)) ? (a) : (b))
#endif /* MAX */

#ifndef MIN
#define MIN(a, b)    (((a) < (b)) ? (a) : (b))
#endif /* MIN */

/* Vendor command definitions used in RTR driver */

#define QCA_NL80211_VENDOR_SUBCMD_WIFI_PARAMS                         (200)
#define QCA_NL80211_VENDOR_SUBCMD_GET_PRI20_BLOCKCHANLIST             (286)
#define QCA_NL80211_VENDOR_SUBCMD_GET_CHAN_RF_CHARACTERIZATION_INFO   (292)
#define QCA_NL80211_VENDOR_SUBCMD_GET_ACS_PRECAC_SUPPORT              (321)
#define NL80211_ATTR_MAX_INTERNAL                                     (256)

#ifndef BAND_2_4GHZ_FREQ_BASE
#define BAND_2_4GHZ_FREQ_BASE             (2407)
#endif /* BAND_2_4GHZ_FREQ_BASE */

#ifndef BAND_2_4GHZ_FREQ_MIN
#define BAND_2_4GHZ_FREQ_MIN              (2412)
#endif /* BAND_2_4GHZ_FREQ_MIN */

#ifndef BAND_2_4GHZ_FREQ_MAX
#define BAND_2_4GHZ_FREQ_MAX              (2484)
#endif /* BAND_2_4GHZ_FREQ_MAX */

#ifndef BAND_PUBLIC_SAFETY_BASE
#define BAND_PUBLIC_SAFETY_BASE           (4940)
#endif /* BAND_PUBLIC_SAFETY_BASE */

#ifndef BAND_PUBLIC_SAFETY_EDGE
#define BAND_PUBLIC_SAFETY_EDGE           (4990)
#endif /* BAND_PUBLIC_SAFETY_EDGE */

#ifndef IEEE80211J_JP_FREQ_BASE
#define IEEE80211J_JP_FREQ_BASE           (4000)
#endif /* IEEE80211J_JP_FREQ_BASE */

#ifndef IEEE80211J_JP_FREQ_MIN
#define IEEE80211J_JP_FREQ_MIN            (4900)
#endif /* IEEE80211J_JP_FREQ_MIN */

#ifndef WLAN_CHAN_15_FREQ
#define WLAN_CHAN_15_FREQ                 (2512)
#endif /* WLAN_CHAN_15_FREQ */

#ifndef BAND_5GHZ_FREQ_BASE
#define BAND_5GHZ_FREQ_BASE               (5000)
#endif /* BAND_5GHZ_FREQ_BASE */

#ifndef BAND_5GHZ_FREQ_MIN
#define BAND_5GHZ_FREQ_MIN                (5180)
#endif /* BAND_5GHZ_FREQ_MIN */

#ifndef BAND_5GHZ_FREQ_MAX
#define BAND_5GHZ_FREQ_MAX                (5920)
#endif /* BAND_5GHZ_FREQ_MAX */

#ifndef BAND_5GHZ_FREQ_CHAN_NUM_132
#define BAND_5GHZ_FREQ_CHAN_NUM_132       (5660)
#endif

#ifndef BAND_6GHZ_FREQ_BASE
#define BAND_6GHZ_FREQ_BASE               (5925)
#endif /* BAND_6GHZ_FREQ_BASE */

#ifndef BAND_6GHZ_FREQ_BASE_NON_ORPHAN
#define BAND_6GHZ_FREQ_BASE_NON_ORPHAN    (5950)
#endif /* BAND_6GHZ_FREQ_BASE_NON_ORPHAN */

#ifndef BAND_6GHZ_FREQ_MIN
#define BAND_6GHZ_FREQ_MIN                (5935)
#endif /* BAND_6GHZ_FREQ_MIN */

#ifndef BAND_6GHZ_FREQ_MIN_NON_ORPHAN
#define BAND_6GHZ_FREQ_MIN_NON_ORPHAN     (5955)
#endif /* BAND_6GHZ_FREQ_MIN_NON_ORPHAN */

#ifndef BAND_6GHZ_FREQ_MAX
#define BAND_6GHZ_FREQ_MAX                (7115)
#endif /* BAND_6GHZ_FREQ_MAX */

#ifndef BAND_6GHZ_FREQ_ORPHAN_CHAN_2
#define BAND_6GHZ_FREQ_ORPHAN_CHAN_2      (5935)
#endif

#ifndef BAND_6GHZ_ORPHAN_CHAN_NUM
#define BAND_6GHZ_ORPHAN_CHAN_NUM         (2)
#endif


#define IS_CHAN_IN_PUBLIC_SAFETY_BAND(_f) \
    ((_f) > BAND_PUBLIC_SAFETY_BASE && (_f) < BAND_PUBLIC_SAFETY_EDGE)

enum qca_wlan_genric_data {
	QCA_WLAN_VENDOR_ATTR_PARAM_INVALID = 0,
	QCA_WLAN_VENDOR_ATTR_PARAM_DATA,
	QCA_WLAN_VENDOR_ATTR_PARAM_LENGTH,
	QCA_WLAN_VENDOR_ATTR_PARAM_FLAGS,

	/* keep last */
	QCA_WLAN_VENDOR_ATTR_PARAM_LAST,
	QCA_WLAN_VENDOR_ATTR_PARAM_MAX =
		QCA_WLAN_VENDOR_ATTR_PARAM_LAST - 1
};
#endif /* ICM_RTR_DRIVER */

/* Bit map related macros. */
#define setbit(a,i) ((a)[(i)/NBBY] |= 1<<((i)%NBBY))
#define clrbit(a,i) ((a)[(i)/NBBY] &= ~(1<<((i)%NBBY)))
#define isset(a,i)  ((a)[(i)/NBBY] & (1<<((i)%NBBY)))
#define isclr(a,i)  (((a)[(i)/NBBY] & (1<<((i)%NBBY))) == 0)

#define SUCCESS                           (1)
#define FAILURE                           !(SUCCESS)
#ifndef MAX_SSID_LEN
#define MAX_SSID_LEN                      (32)
#endif
#define MAX_SCAN_ENTRIES                  (128)
#define MAX_NUM_CHANNEL                   (256)
#define CMD_BUF_SIZE                      (1024)
#define CHAN1FREQ_MHZ                     (2412)
#define PERCHANWIDTH_MHZ                  (5)
#define MAX_2_4GHZ_CHANNEL                (14)
/* Channel dwell interval for Spectral Scan */
#define CHANNEL_DWELL_INTERVAL            (10)
/* Short channel dwell interval for channels which do not have MWO
   interference */
#define CHANNEL_DWELL_INTERVAL_SHORT      (4)
#define MAX_NUM_SPECTRAL_SCAN_COUNT       (1)
#define ENABLE_CLASSIFIER_PRINT           (1)
#define ATHPORT                           8001
#define BACKLOG                           10
#define INVALID_FD                        (-1)
#define INVALID_CHANNEL                   (-1)
#define MAX_DEV_NUM                       (4)
#define MAX_VAP_NUM                       (17)
/* Dwell time in milliseconds to be used for vendor scan */
#define ICM_VENDOR_SCAN_COMMON_DWELL_TIME_MS (300)

#define PATH_SYSNET                       "/sys/class/net/"
/* Extra bytes to provision for estimation buffering purposes in max expected
 * path size for VAP parent entry */
#define PATH_SYSNET_PARENT_EXTRASZ        (64)
/* Max expected path size for VAP parent entry */
#define PATH_SYSNET_PARENT_SZ             (sizeof(PATH_SYSNET) + IFNAMSIZ + \
                                           sizeof("/parent") + \
                                           PATH_SYSNET_PARENT_EXTRASZ)

#define LINK_PATH_SZ                      (50)
#ifdef WLAN_SPECTRAL_ENABLE
/* Maximum percentage of free memory we allow ourselves to approximately utilize
 * while setting socket receive buffer size. This is a value in the range of
 * 0 - 100.
 *
 * Note that the kernel may double the value we request for, to account for
 * bookkeeping overhead. This needs to be taken into account when changing the
 * value.
 *
 * This value we actually request may be bound by other limitations as well.
 */
#define ICM_MAX_FREEMEM_UTIL_PERCENT       (30U)

/* Max number of Spectral samples to buffer per second. Can be tied to Spectral
 * configuration aspects in the future depending on complexity.
 */
#define ICM_SPECTRALSAMP_MAX_BUFF_PERSEC   (5000)

/* Max number of seconds for which we expect to be able to buffer */
#define ICM_SPECTRALSAMP_MAX_BUFFTIME_SEC  (CHANNEL_DWELL_INTERVAL)

/* Max number of Spectral samples to buffer */
#define ICM_SPECTRALSAMP_MAX_BUFF          (ICM_SPECTRALSAMP_MAX_BUFF_PERSEC * \
    ICM_SPECTRALSAMP_MAX_BUFFTIME_SEC)

/* Max number of Spectral samples over which we would be considered to be
 * re-syncing with the kernel after a Spectral stop/start. This can be due to
 * various factors such as application processing between transitions from one
 * Spectral scan session to another, etc.
 */
#define ICM_SPECTRALSAMP_RESYNC_THRESHOLD  (10)

#endif /* WLAN_SPECTRAL_ENABLE */

/* Maximum number of 80 MHz segments currently available across all regulatory
   domains */
#define ICM_MAX_80MHZ_SEG_REG             (15)

/* Buffer to be applied while provisioning for processing of 80 MHz segments */
#define ICM_NUM_80MHZ_SEG_BUFFER          (10)

/* Maximum number of 80 MHz segments to be processed, inclusive of buffer */
#define ICM_MAX_80MHZ_SEG                 (ICM_MAX_80MHZ_SEG_REG + \
        ICM_NUM_80MHZ_SEG_BUFFER)

#define ICM_MAX_PCL_WEIGHT       (255)

/* This is only needed on M2M platforms where a fake NF value seems to be
 * received, or on uncalibrated chipsets in initial development stages.
 */
#define ICM_SUPPORT_HIGH_NF_WAR           (0)

#if ICM_SUPPORT_HIGH_NF_WAR
#define ATH_DEFAULT_NF_THRESHOLD          (-20)
#else
#define ATH_DEFAULT_NF_THRESHOLD          (-85)
#endif /* ICM_SUPPORT_HIGH_NF_WAR */
#define ATH_DEFAULT_NOMINAL_NF            (-115)    // Verify this value
#define ATH_DEFAULT_CW_NOISEFLOOR_DELTA   (30)      // CW Noisefloor Delta
#define ATH_DEFAULT_NF_CORRECTION         (-7)      // Correction for NF variation due to receive sensitivity for higher freq

/* Default value of anticipated usage factor for each bandwidth, in 80 MHz mode.
 * The sum across bandwidths must be 100.
 */
#define ICM_DEFAULT_USAGE_FACTOR_80_BW40            (50)
#define ICM_DEFAULT_USAGE_FACTOR_80_BW80            (50)

/* Default value of anticipated usage factor for each bandwidth, in 160 MHz
 * mode.
 * The sum across bandwidths must be 100.
 */
#define ICM_DEFAULT_USAGE_FACTOR_160_BW40           (35)
#define ICM_DEFAULT_USAGE_FACTOR_160_BW80           (35)
#define ICM_DEFAULT_USAGE_FACTOR_160_BW160          (30)

/* Default value of anticipated usage factor for each bandwidth, in 320 MHz
 * mode for 6GHz band.
 * The sum across bandwidths must be 100.
 */
#define ICM_DEFAULT_USAGE_FACTOR_6GHZ_320_BW40           (10)
#define ICM_DEFAULT_USAGE_FACTOR_6GHZ_320_BW80           (15)
#define ICM_DEFAULT_USAGE_FACTOR_6GHZ_320_BW160          (25)
#define ICM_DEFAULT_USAGE_FACTOR_6GHZ_320_BW320          (50)

/* Default value of anticipated usage factor for each bandwidth, in 320 MHz
 * mode for 5 GHz band.
 * The sum across bandwidths must be 100.
 */
#define ICM_DEFAULT_USAGE_FACTOR_5GHZ_320_BW40           (50)
#define ICM_DEFAULT_USAGE_FACTOR_5GHZ_320_BW80           (20)
#define ICM_DEFAULT_USAGE_FACTOR_5GHZ_320_BW160          (15)
#define ICM_DEFAULT_USAGE_FACTOR_5GHZ_320_BW320          (15)

/* Default value of anticipated weightage of clients for legacy, OFDMA
 * and NON-OFDMA.
 * The sum must be 100.
 */
#define ICM_DEFAULT_LEGACY_CLIENT_WEIGHTAGE             (70)
#define ICM_DEFAULT_OFDMA_CLIENT_WEIGHTAGE              (15)
#define ICM_DEFAULT_NONOFDMA_CLIENT_WEIGHTAGE           (15)

/* Default value of anticipated usage factor for each bandwidth, for primary 80
 * MHz segment in 80+80 MHz mode. Usage factor for 80+80 MHz PPDU transmissions
 * is not considered separately, currently. See notes in selection algorithm
 * documentation.
 * The sum across bandwidths must be 100.
 */
#define ICM_DEFAULT_USAGE_FACTOR_80p80_BW40          (50)
#define ICM_DEFAULT_USAGE_FACTOR_80p80_BW80          (50)

/* Penalization factors for 802.11ax OBSS APs which use SRP, and which might
 * allow client-side use of SRP, OBSS PD.
 * Do NOT define any of these as 0 (unless the standard evolves and this becomes
 * necessary, in which case code which uses these will also require
 * re-adjustments).
 *
 * 11AX TODO: This is an initial implementation. To be refined as 802.11ax
 * Spatial Reuse specifications evolve. May also be made configurable in later
 * phases.
 */
#define ICM_SRP_PENALIZATION_FACTOR                  (02)
#define ICM_CLIENT_SRP_PENALIZATION_FACTOR           (01)
#define ICM_CLIENT_OBSSPD_PENALIZATION_FACTOR        (01)

/* Default scaling factor to be used for 20 MHz width when comparing between
 * bands
 */
#define ICM_DEFAULT_MULTIBAND_SCALING_FACTOR_BW20     (20)

/* Default scaling factor to be used for 40 MHz width when comparing between
 * bands
 */
#define ICM_DEFAULT_MULTIBAND_SCALING_FACTOR_BW40     (40)

/* Default scaling factor to be used for 80 MHz width when comparing between
 * bands
 */
#define ICM_DEFAULT_MULTIBAND_SCALING_FACTOR_BW80     (80)

/* Default scaling factor to be used for 160 MHz width when comparing between
 * bands. This is kept at less than the max to account for issues such as
 * greater potential interference at higher widths, some STAs having maxNSS of
 * 160 MHz the same as that for <= 80 MHz, etc.
 */
#define ICM_DEFAULT_MULTIBAND_SCALING_FACTOR_BW160    (120)

/* Default scaling factor to be used for 320 MHz width when comparing between
 * bands. This is kept at less than the max to account for issues such as
 * greater potential interference at higher widths.
 */
#define ICM_DEFAULT_MULTIBAND_SCALING_FACTOR_BW320    (150)

/* Default scaling factor to be used for 80+80 MHz MHz width when comparing
 * between bands. This is kept at less than the max to account for issues such
 * as greater potential interference at higher widths, some STAs having maxNSS
 * of 160 MHz the same as that for <= 80 MHz, etc.
 */
#define ICM_DEFAULT_MULTIBAND_SCALING_FACTOR_BW80P80  (120)

#define ICM_DEFAULT_MULTIBAND_SCALING_FACTOR_MAX      (160)

/* Default scaled usability margin to be considered when comparing between bands.
 * If the scaled usabilities of two bands fall within this margin, then the max
 * regulatory power of their respective best channels are considered.
 */
#define ICM_DEFAULT_MULTIBAND_SCALED_USABILITY_MARGIN_MAXREGPWR (2000)

/* Penalization factor in percentage for non-PSC 6 GHz channels. The usability
 * is reduced to this percentage of its original value.
 */
#define ICM_6GHZNONPSC_PENALIZATION_FACTOR           (75)

#define DEFAULT_SELDBG_FILENAME      "/tmp/icmseldebug.csv"

#define streq(a,b) (strcasecmp(a,b) == 0)

#define info(fmt, args...) do {\
    icm_printf("icm : " fmt "\n", ## args); \
} while (0)

#define err(fmt, args...) do {\
    icm_printf("icm : %s %d " fmt "\n", __func__, __LINE__, ## args); \
} while (0)

#define ICM_ASSERT(e)       ((e)?((void)0):assert(e))

#define MIN_INT(a,b)        ((a) < (b) ? (a) : (b))
#define MAX_INT(a,b)        ((a) > (b) ? (a) : (b))

#define ARRAY_LEN(x)        (sizeof(x)/sizeof(x[0]))

/* We avoid using CHAR_BIT since this may not be 8 on all platforms */
#ifndef NUM_BITS_IN_BYTE
#define NUM_BITS_IN_BYTE    (8)
#endif /* NUM_BITS_IN_BYTE */

/*
 * Spetral param data structure with sane names for data memembers
 * XXX : This is copy of what is present in ah.h
 * Can't avoid this dupication now as the app and kernel share
 * this data structure and I do not want to make the build infrastruture
 * as simple as possible
 */
#define MAX_CHAINS  4
#define HAL_PHYERR_PARAM_NOVAL  65355
#define HAL_PHYERR_PARAM_ENABLE 0x8000

/*
 * Value for KILO for purposes of frequency comparison.
 */
#define KILO_VAL            (1e3)

#ifndef MILLISEC_TO_MICROSEC
#define MILLISEC_TO_MICROSEC    1000U
#endif /* MILLISEC_TO_MICROSEC */

/*
 * Number of operating modes.
 */
#define ICM_NUM_OPER_MODE   (7)
#ifdef ICM_RTR_DRIVER
/* Macro used for diagnostic prints in ICM */
#define ICM_DPRINTF(pdev, flags, level, module_id, ...) do { \
    if ((pdev) && ((level) >= ((pdev)->conf.dbg_level)) && (((pdev)->conf.dbg_module_bitmap) & (module_id))) { \
        if (!((flags) & ICM_PRCTRL_FLAG_NO_PREFIX)) icm_printf("icm: "); \
        icm_printf(__VA_ARGS__); \
    } \
}while(0);
#else
/* Macro used for diagnostic prints in ICM */
#define ICM_DPRINTF(pdev, flags, level, module_id, ...) do { \
    if ((pdev) && ((level) >= ((pdev)->conf.dbg_level)) && (((pdev)->conf.dbg_module_bitmap) & (module_id))) { \
        icm_android_log_helper(__VA_ARGS__); \
    } \
}while(0);
#endif

/* Max length of short descriptive string */
#define SHORTSTRWIDTH           (5)

/* Max length of short descriptive string, but wider version for info requiring
 * more space.
 */
#define SHORTSTRWIDTH_WIDE      (10)

/* Return whether a given 80+80 MHz pri80 centre index + sec80 centre index
 * combination is allowed when the 80+80 EMI WAR is enabled.
 */
#define ICM_IS_EMIWAR80P80_COMB_ALLOWED(pri80cindex, sec80cindex) \
    (!(((pri80cindex) == 155) && ((sec80cindex) == 42)))

extern const char *icm_bool_str[];
extern const char *icm_bool_shortstr[];

/* Flags used to control ICM_DPRINTF() print behaviour */
typedef enum {
    ICM_PRCTRL_FLAG_NONE      = 0,
    ICM_PRCTRL_FLAG_NO_PREFIX = 0x01,  /* Do not add the "icm: " prefix */
} ICM_PRCTRL_FLAG_T;

/* Debug level macros used to set the debug level*/
typedef enum {
    ICM_DEBUG_LEVEL_MINOR = 1,
    ICM_DEBUG_LEVEL_DEFAULT,
    ICM_DEBUG_LEVEL_MAJOR,
    ICM_DEBUG_LEVEL_CRITICAL,
    ICM_DEBUG_LEVEL_INVALID,        /* This should always be the last
                                       member */
}ICM_DEBUG_LEVEL_T;

/* wireless abstraction interface path  macros*/
typedef enum {
    ICM_WAL_IOCTL = 1,
    ICM_WAL_CFG,
    ICM_WAL_INVALID,        /* This should always be the last
                               member */
}ICM_WAL_FLAG_T;

/* Constants to set the debug mask */
typedef enum {
    ICM_MODULE_ID_NONE      = 0x0000,
    ICM_MODULE_ID_MAIN      = 0x0001,
    ICM_MODULE_ID_SCAN      = 0x0002,
    ICM_MODULE_ID_SELECTOR  = 0x0004,
    ICM_MODULE_ID_UTIL      = 0x0008,
    ICM_MODULE_ID_TEST      = 0x0010,
    ICM_MODULE_ID_SOCKET    = 0x0020,
    ICM_MODULE_ID_SPECTRAL  = 0x0040,
    ICM_MODULE_ID_CMDS      = 0x0080,
    ICM_MODULE_ID_ALL       = 0x00FF, /* This should always be the last
                                         member, and should be the OR
                                         of all ID bit positions defined. */
}ICM_MODULE_ID_T;

/* Enable scan dissection debugging */
//#define ICM_SCAN_DISSECTION_DEBUG (1)

/* Enable event stream parse debugging */
//#define ICM_EV_STREAM_DEBUG     (1)

/* Standard ioctl mapping for getting radio private params */
#define ATH_IOCTL_GETPARAM  (SIOCIWFIRSTPRIV + 1)

/* Standard ioctl mapping for setting radio private params */
#define ATH_IOCTL_SETPARAM  (SIOCIWFIRSTPRIV + 0)

/*
 * Structure used for holding context while
 * parsing scan related event stream.
 */
typedef struct _icm_event_parse_context
{
    char      *curr_ev;           /* Pointer to current event being processed */
    u_int32_t curr_ev_val_offset; /* Offset to current value, in current
                                     event being processed */
    char      *stream_end;        /* End of event stream */
    u_int8_t  is_valid;           /* Is this event stream valid */
} ICM_EVENT_PARSE_CONTEXT_T;

/* Event types */
typedef enum _icm_ev_type {
    ICM_EV_TYPE_NULL,
    ICM_EV_TYPE_FREQ,
    ICM_EV_TYPE_ADDR,
    ICM_EV_TYPE_CHAR,
    ICM_EV_TYPE_UINT,
    ICM_EV_TYPE_POINT,
    ICM_EV_TYPE_QUALITY,
    ICM_EV_TYPE_PARAM,
} ICM_EV_TYPE_T;

/*
 * Structure providing meta-data about an event.
 * This is based upon standard Wireless Extensions definitions.
 */
typedef struct _icm_event_descriptor
{
    ICM_EV_TYPE_T ev_type;         /* Type of event */
    u_int32_t     ev_len;          /* Event length */
    u_int8_t      ev_unit_size;    /* Unit size of event payload */
    u_int16_t     ev_min_units;    /* Minimum no. of event payload units */
    u_int16_t     ev_max_units;    /* Maximum no. of event payload units */
    u_int8_t      ev_is_nolimit;   /* Whether there is no limit on req size */
} ICM_EVENT_DESCRIPTOR_T;

/* Policy regarding usage of representative Tx power - specifies whether to use
 * representative Tx power, and if so, which factor to optimize for.
 */
typedef enum icm_rep_txpower_policy {
    ICM_REP_TXPOWER_POLICY_DENY,
    ICM_REP_TXPOWER_POLICY_OPTIMIZE_THROUGHPUT,
    ICM_REP_TXPOWER_POLICY_OPTIMIZE_RANGE,

    ICM_REP_TXPOWER_POLICY_AFTER_LAST,
    ICM_REP_TXPOWER_POLICY_MAX =
        ICM_REP_TXPOWER_POLICY_AFTER_LAST - 1,
} ICM_REP_TXPOWER_POLICY_T;

/* Default usability margin within which the top ranked channels will be
 * re-assessed based on representative Tx power.
 */
#define ICM_DEFAULT_USABILITY_MARGIN_REP_TXPOWER               (2000)

/* Default usability margin within which the top ranked channels will be
 * re-assessed based on their presence within U-NII-3 band (802.11ax only).
 */
#define ICM_DEFAULT_USABILITY_MARGIN_11AX_UNII3_PREFERENCE     (2000)

/*
 * Structure providing limited, adjusted meta-data about an event (taking
 * into account Wireless Extension version, failures, etc)
 * This is based upon standard Wireless Extensions definitions.
 */
typedef struct _icm_event_descriptor_adj
{
    u_int32_t     ev_len;          /* Event length */
    /* TODO: Add any other meta-data that might require adjusting
       in the future */
} ICM_EVENT_DESCRIPTOR_ADJ_T;

/* Event parsing result codes */
typedef enum _icm_ev_parse_result {
    ICM_EV_PARSE_RESULT_SUCCESS,     /* Success - found an event */
    ICM_EV_PARSE_RESULT_INVAL_CTX,   /* Invalid context */
    ICM_EV_PARSE_RESULT_INVAL_ARG,   /* Invalid argument */
    ICM_EV_PARSE_RESULT_STREAM_END,  /* End of stream */
    ICM_EV_PARSE_RESULT_UNKNOWN_EV,  /* Unknown event */
    /* This is not a hard error
       - can continue */
    ICM_EV_PARSE_RESULT_INVAL_EVLEN, /* Invalid event length */
    /* This is a hard error
       - cannot continue */
} ICM_EV_PARSE_RESULT_T;

/* Channel comparison result codes */
typedef enum _icm_chan_cmp_result {
    ICM_CHAN_CMP_RESULT_CHAN1_BETTER,     /* Channel 1 is better */
    ICM_CHAN_CMP_RESULT_CHAN2_BETTER,     /* Channel 2 is better */
    ICM_CHAN_CMP_RESULT_BOTH_BAD,         /* Both channels are bad
                                             (usability <=0 ) */
    ICM_CHAN_CMP_RESULT_BOTH_EQUAL,       /* Both channels are equal */
    ICM_CHAN_CMP_RESULT_FAILURE,          /* General failure */
} ICM_CHAN_CMP_RESULT_T;

/* AFC Power modes */
typedef enum icm_afc_power_mode {
    ICM_LOW_POWER_MODE,                /* Low power/Indoor power mode */
    ICM_STANDARD_POWER_MODE,           /* Standard power mode */
    ICM_VERY_LOW_POWER_MODE,           /* Very low power mode */
    ICM_MAX_POWER_MODE,
} ICM_AFC_POWER_MODE_T;

/* Operating bands.
   These *must* correspond to the values
   defined in the TCA Design.
   We don't support 4.9 GHz, currently.
   Note:
   - Do NOT introduce holes in the below enumeration, ensure all unique values
   are contiguous integers equal to or greater than 0. Two enum definitions
   corresponding to the same value are permisibile for markers such as max,
   invalid, etc.
   - Coordinate with TCA maintainers in case of any changes in definitions for
   2.4 GHz and 5 GHz.*/
typedef enum icm_band {
    ICM_BAND_2_4G,       /* 2.4 GHz */
    ICM_BAND_5G,         /* 5 GHz */
    ICM_BAND_6G,         /* 6 GHz */
    ICM_BAND_MAX,
    ICM_BAND_INVALID = ICM_BAND_MAX,
} ICM_BAND_T;

extern const char *icm_band_str[];

/* Channel widths.
   These *must* correspond to the values
   defined in the TCA Design.
Note: Do NOT introduce holes in the below enumeration, and coordinate
with TCA maintainers for the same.*/
typedef enum icm_ch_bw {
    ICM_CH_BW_20,
    ICM_CH_BW_40MINUS,
    ICM_CH_BW_40PLUS,
    ICM_CH_BW_40,
    ICM_CH_BW_80,
    ICM_CH_BW_160,
    ICM_CH_BW_80_PLUS_80,
    ICM_CH_BW_320,
    ICM_CH_BW_INVALID
} ICM_CH_BW_T;

extern const char *icm_ch_bw_str[];

/* PHY specification to be used.
Note: Do NOT introduce holes in the below enumeration. */
typedef enum icm_phy_spec {
    ICM_PHY_SPEC_11A = 0,
    ICM_PHY_SPEC_11B,
    ICM_PHY_SPEC_11G,
    ICM_PHY_SPEC_FH,
    ICM_PHY_SPEC_TURBO_A,
    ICM_PHY_SPEC_TURBO_G,
    ICM_PHY_SPEC_11NA,
    ICM_PHY_SPEC_11NG,
    ICM_PHY_SPEC_11AC,
    ICM_PHY_SPEC_11AXA,
    ICM_PHY_SPEC_11AXG,
    ICM_PHY_SPEC_11BEG,
    ICM_PHY_SPEC_11BEA,
    ICM_PHY_SPEC_ANY,
    ICM_PHY_SPEC_INVALID
} ICM_PHY_SPEC_T;

extern const char *icm_phy_spec_str[];

/* Max size of string which would represent a channel
   width (e.g. "40MINUS") including terminating NULL */
#define ICM_MAX_CH_BW_STR_SIZE    (8)

/* Max size of string which would represent a channel
   width (e.g. "LPI") including terminating NULL */
#define ICM_MAX_CH_POWER_MODE_SIZE (4)

/* Max size of string which would represent a PHY
   spec (e.g. "11NA") including terminating NULL */
#define ICM_PHY_SPEC_STR_SIZE    (8)

/* Max size of string which would represent a PHY mode
   (e.g. "11ACVHT40MINUS") including terminating NULL */
#define ICM_MAX_PHYMODE_STR_SIZE  (15)


/* Channel selection modes - honored only in server mode.
   These *must* correspond to the values
   defined in the TCA Design. */
typedef enum icm_ch_selection_mode {
    ICM_CH_SELECTION_MODE_NONE,
    ICM_CH_SELECTION_MODE_AUTO,
    ICM_CH_SELECTION_MODE_AUTO_WITH_EXCLUSIONS,
    ICM_CH_SELECTION_MODE_MANUAL,
    ICM_CH_SELECTION_MODE_INVALID
} ICM_CH_SELECTION_MODE_T;

/*
 * RROP availability information - Specifies whether Representative RF Operating
 * Parameter (RROP) information is available, and if so, at which point in the
 * application-driver interaction sequence it can be retrieved by the
 * application from the driver. This point may vary by architecture and other
 * factors. This applies to the cfg80211 interface.
 */
typedef enum icm_rropavail_info {
    /* RROP information is unavailable. */
    ICM_RROPAVAIL_INFO_UNAVAILABLE = 0,
    /* RROP information is available and the application can retrieve the
     * information after receiving an QCA_NL80211_VENDOR_SUBCMD_EXTERNAL_ACS
     * event from the driver.
     */
    ICM_RROPAVAIL_INFO_EXTERNAL_ACS_START,
    /* RROP information is available only after a vendor specific scan
     * (requested using QCA_NL80211_VENDOR_SUBCMD_TRIGGER_SCAN) has
     * successfully completed. The application can retrieve the information
     * after receiving the QCA_NL80211_VENDOR_SUBCMD_SCAN_DONE event from
     * the driver.
     */
    ICM_RROPAVAIL_INFO_VSCAN_END,
} ICM_RROPAVAIL_INFO_T;

typedef struct icm_dev_config {
    char radio_ifname[IFNAMSIZ + 1];
    char dev_ifname[IFNAMSIZ + 1];
    ICM_BAND_T  band;
    ICM_PHY_SPEC_T phy_spec;
    ICM_CH_BW_T channel_width;
    u_int8_t def_channel;
}ICM_DEV_CONFIG_T;

#define SEC_CHANNEL_OFFSET_ABOVE_IDX     (4)    /* Jump by 20 MHz front */
#define SEC_CHANNEL_OFFSET_BELOW_IDX    (-4)    /* Jump by 20 MHz back */
#define SEC_CHANNEL_OFFSET_ABOVE        (1)     /* Indicates Extension channel is above */
#define SEC_CHANNEL_OFFSET_BELOW        (3)     /* Indicates Extension channel is below */
#define CHANNEL_OFFSET_160MHZ_IN_320MHZ    (16)     /* Indicates the channel offset to calculate 160 MHz center
                                                       freq when channel width is 320 MHz */

#define VHT_CHANNEL_WIDTH_20MHZ_OR_40MHZ   (0)
#define VHT_CHANNEL_WIDTH_80MHZ            (1)
#define VHT_CHANNEL_WIDTH_160_MHZ          (2)
#define VHT_CHANNEL_WIDTH_80MHZ_PLUS_80MHZ (3)

#define CHANNEL_BOUNDARY_OFFSET_160MHZ         (14)
#define CHANNEL_BOUNDARY_OFFSET_320MHZ         (30)

#define HE_6GHZ_CHANNEL_WIDTH_20MHZ            (0)
#define HE_6GHZ_CHANNEL_WIDTH_40MHZ            (1)
#define HE_6GHZ_CHANNEL_WIDTH_80MHZ            (2)
#define HE_6GHZ_CHANNEL_WIDTH_160_80P80_MHZ    (3)

#define EHT_CHANNEL_WIDTH_20_MHZ            (0)
#define EHT_CHANNEL_WIDTH_40_MHZ            (1)
#define EHT_CHANNEL_WIDTH_80_MHZ            (2)
#define EHT_CHANNEL_WIDTH_160_MHZ           (3)
#define EHT_CHANNEL_WIDTH_320_MHZ           (4)

#define ICM_WEXT_DRV_MAX_NUM_CHANPROP_CHANNELS  (255)

#define CHWIDTH_20_MHZ            (20)
#define CHWIDTH_40_MHZ            (40)
#define CHWIDTH_80_MHZ            (80)
#define CHWIDTH_160_MHZ           (160)
#define CHWIDTH_320_MHZ           (320)

typedef struct icm_channel_properties {
#ifdef WLAN_SPECTRAL_ENABLE
    int cycle_count;
    int channel_free_time;
#else
    u64 cycle_count;
    u64 channel_free_time;
#endif /* WLAN_SPECTRAL_ENABLE */
    int per;
    int noisefloor;
    u_int16_t comp_usablity;
    int8_t maxregpower;
    u_int16_t comp_usablity_sec80;
    int8_t maxregpower_sec80;
    int32_t channel_tx_power_tput;
    int32_t channel_tx_power_range;
    /*
     * Channel performance grade indicated by driver/target if this channel is
     * used as the primary 20 MHz of the BSS. The operating width of the BSS can
     * be higher than 20 MHz.
     */
    u_int8_t pri20channel_grade;
    u_int8_t channel_ocac_status;
    u_int16_t puncture_bitmap;             /* Puncture bitmap. This will be used only for EHT phymodes */
} ICM_CHANNEL_PROPERTIES_T;

/* Channel properties list indexed by channel number */
typedef struct icm_channel_properties_list {
    ICM_CHANNEL_PROPERTIES_T chprop[MAX_NUM_CHANNEL];
} ICM_CHANNEL_PROPERTIES_LIST_T;

/* Holds channel related information */
typedef struct icm_channel {
    double freq;        /* frequency */
    int channel;        /* channel */
    int flags;           /* properties */
    int num_wnw;        /* number of wireless network */
    int num_wnw_20;     /* number of wireless networks in this channel, having width 20 MHz */
    int num_wnw_40;     /* number of wireless networks in this channel, having width 40 MHz */
    int num_wnw_80;     /* number of wireless networks in this channel, having width 80 MHz */
    int num_wnw_160;    /* number of wireless networks in this channel, having width 160 MHz */
    int num_wnw_320;    /* number of wireless networks in this channel, having width 320 MHz */
    int num_wnw_80_80;  /* number of wireless networks in this channel, having width 80+80 MHz */

    /* Wireless preference factors which includes RSSI per BSS */
    double num_wpf;       /* number of wireless preference factor */
    double num_wpf_20;    /* number of wireless preference factor in this channel, having width 20 MHz */
    double num_wpf_40;    /* number of wireless preference factor in this channel, having width 40 MHz */
    double num_wpf_80;    /* number of wireless preference factor in this channel, having width 80 MHz */
    double num_wpf_160;   /* number of wireless preference factor in this channel, having width 160 MHz */
    double num_wpf_320;   /* number of wireless preference factor in this channel, having width 320 MHz */
    double num_wpf_80_80; /* number of wireless preference factor in this channel, having width 80+80 MHz */

    /* 11AX TODO: Refine the below 802.11ax counts based on how Spatial Reuse
     * support evolves in the standard.
     */

    /* Number of Wireless Networks with this channel as primary, which support
     * 802.11ax SRP */
    int num_wnw_srp_supported;

    /* Number of Wireless Networks with this channel as primary, which support
     * use of 802.11ax SRP at associated non-AP STAs */
    int num_wnw_client_srp_allowed;

    /* Number of Wireless Networks with this channel as primary, which support
     * use of 802.11ax OBSS PD at associated non-AP STAs */
    int num_wnw_client_obsspd_allowed;

    u_int8_t non_overlapping_ch; /* Is this a center channel */
    ICM_BAND_T band;    /* band */
    u_int16_t usablity; /* Measure of free time in the air */
    u_int16_t used_as_secondary_20;    /* indicates how many BSS are using this channel as extension channel for 20MHz */
    u_int16_t used_as_160_80p80_secondary_40;  /* indicates how many 160/80+80 MHz BSS are using this channel as secondary 40MHz */
    u_int16_t used_as_160_secondary_80;  /* indicates how many 160 MHz BSS are using this channel as secondary 80 MHz */
    u_int16_t used_as_80p80_secondary_80;  /* indicates how many 80+80 MHz BSS are using this channel as secondary 80 MHz */
    u_int16_t used_as_320_secondary_40;  /* indicates how many 320 MHz BSS are using this channel as secondary 40MHz */
    u_int16_t used_as_320_secondary_80;  /* indicates how many 320 MHz BSS are using this channel as secondary 80 MHz */
    u_int16_t used_as_320_secondary_160;  /* indicates how many 320 MHz BSS are using this channel as secondary 160 MHz */

    u_int64_t   ic_flags;
    u_int16_t   ic_flagext;
    u_int8_t    ic_ieee;
    int8_t      ic_maxregpower;
    u_int16_t   supp_power_modes;
    int32_t     afc_tx_power[ICM_MAX_POWER_MODE];
    int8_t      ic_maxpower;
    int8_t      ic_minpower;
    u_int8_t    ic_regClassId;
    u_int8_t    ic_antennamax;             /* antenna gain max from regulatory */
    u_int8_t    ic_ch_freq_seg1;     /* For 80 MHz, indicates the channel
                                              center frequency index.
                                              For 80+80 MHz, indicates the
                                              channel center frequency index for
                                              segment 1.

                                              Note: Since for a given primary
                                              channel, this value will be
                                              different for 160 MHz and 80+80
                                              MHz (but the same for 80 MHz and
                                              80+80 MHz), this value is used
                                              only for 80 MHz and 80+80 MHz. A
                                              separate value is defined for 160
                                              MHz.
                                            */
    u_int8_t    ic_ch_freq_seg1_160;       /* Indicates the channel center
                                              frequency for 160 MHz.

                                              See note for
                                              ic_ch_freq_seg1.
                                            */

    u_int8_t    ic_ch_freq_seg2;           /* For 80+80 MHz, indicates the
                                              channel center frequency index for
                                              segment 2.
                                            */
    u_int8_t    ic_ch_freq_seg1_320[2];    /* Indicates the channel center frequency
                                              for 320 MHz.
                                              There can be 2 possbile 320 MHz channel
                                              bondings, hence an array of the center
                                              frequency is needed.
                                            */
    u_int16_t puncture_bitmap;             /* Puncture bitmap. This will be used only for EHT phymodes */
}ICM_CHANNEL_T;

#define ICM_DEFAULT_MIN_RSSI (-100)
#define ICM_DEFAULT_MAX_RSSI (0)
#define ICM_DEFAULT_RSSI_WEIGHTAGE (0.5)
#define ICM_DEFAULT_BSS_COUNT_WEIGHTAGE (0.5)


/* capability information from driver */

struct pcl_list {
    size_t len;
    u32 policy;
    u32 *list;
    u8 *weight;
};

/* AFC Channel information with supported power modes and power values */
typedef struct afc_channel_info {
    u_int16_t    supp_power_modes;
    int32_t     tx_power[ICM_MAX_POWER_MODE];
} ICM_AFC_CHANNEL_INFO_T;

typedef struct icm_capability_info {
    u8 config_reason;
    bool spectral_capable;
    bool offload_enabled; /* 11ac offload */
    bool add_chan_stat_supported;
    bool afc_capable;
    ICM_RROPAVAIL_INFO_T rropavail_info;
    bool vap_status;
    u8 vap_mode;
    u8 chan_width;
    u32 *freq_list;
    u32 phy_mode;
    size_t freq_list_len;
    struct pcl_list pcl;
    struct ieee80211_ath_channel chan_info[MAX_NUM_CHANNEL];
    ICM_AFC_CHANNEL_INFO_T afc_info[MAX_NUM_CHANNEL];
} ICM_CAPABILITY_INFO_T;

/* List of channels */
typedef struct icm_channel_list {
    int count;                              /* Number of channel in the list */
    int channel_index;                      /* Points to current active channel */
    ICM_CHANNEL_T ch[MAX_NUM_CHANNEL];      /* List of channels */
}ICM_CHANNEL_LIST_T;

/*
 * Holds information relating to HT Info
 * Can be optimized with bit fields.
 */
typedef struct icm_htinfo {
    bool is_valid;
    u_int32_t control_channel;
    u_int32_t ext_channel_offset;
    u_int32_t tx_channel_width;
    u_int32_t obss_nonht_present;
    u_int32_t tx_burst_limit;
    u_int32_t non_gf_present;
}ICM_HTINFO_T;

/*
 * Holds VHT Operation Information
 */
typedef struct icm_vhtop {
    bool is_valid;
    u_int32_t channel_width;
    u_int32_t channel_cf_seg1;
    u_int32_t channel_cf_seg2;
}ICM_VHTOP_T;

/*
 * Holds HE Capabilities.
 * Note: Only information of interest to ICM is held here.
 */
typedef struct icm_hecap {
    bool is_valid;
    u_int8_t srp_supported;
} ICM_HECAP_T;

/*
 * Holds HE Operation - VHT Operation field information
 * Note: This is currently held in pre non-0 CCFS1 signalling format for ease of
 * processing.
 */
typedef struct icm_heop_vhtopinfo {
    bool is_valid;
    u_int32_t channel_width;
    u_int32_t channel_cf_seg1;
    u_int32_t channel_cf_seg2;
} ICM_HEOP_VHTOPINFO_T;

/*
 * Holds HE Operation - 6 GHz Operation field information
 */
typedef struct icm_heop_6ghzopinfo {
    bool is_valid;
    u_int32_t primary_channel;
    u_int32_t channel_width;
    u_int32_t channel_cf_seg1;
    u_int32_t channel_cf_seg2;
} ICM_HEOP_6GHZOPINFO_T;

/*
 * Holds HE Operation Info
 * Note: Only information of interest to ICM is held here.
 */
typedef struct icm_heop {
    bool is_valid;
    ICM_HEOP_VHTOPINFO_T heop_vhtopinfo;
    ICM_HEOP_6GHZOPINFO_T heop_6ghzopinfo;
} ICM_HEOP_T;

/*
 * Holds Spatial Reuse Parameter Set information.
 * Note: Only information of interest to ICM is held here.
 */
typedef struct icm_srp {
    bool is_valid;
    u_int8_t client_srp_disallowed;
    u_int8_t client_obsspd_disallowed;
} ICM_SRP_T;

/*
 * Holds EHT Operation - EHT Operation field information
 */
typedef struct icm_ehtop {
    bool is_valid;
    u_int32_t channel_width;
    u_int32_t channel_cf_seg1;
    u_int32_t channel_cf_seg2;
} ICM_EHTOP_T;

/* Scan info, contains BSS related information */
typedef struct icm_scan_info {
    u_char  bssid[ETH_ALEN];    /* BSSID of the network */
    double      freq;               /* Operating frequency */
    u_int32_t   channel;            /* Operating channel */
    ICM_BAND_T  band;               /* Band */
    u_int32_t   mode;               /* Operating mode */
    u_char      ssid[MAX_SSID_LEN]; /* SSID of network */
    u_int8_t    ssid_len;           /* Length of SSID */
    int8_t      rssi;               /* RSSI of network */
    bool        rssi_valid;         /* Whether RSSI is valid */
    bool        valid;              /* Validity Indicator */
    ICM_HTINFO_T htinfo;            /* HT Info Element */
    ICM_VHTOP_T vhtop;              /* VHT Info Element */
    ICM_HECAP_T hecap;              /* HE Capabilities Element */
    ICM_HEOP_T  heop;               /* HE Op Info Element */
    ICM_SRP_T   srp;                /* Spatial Reuse Parameter Set info */
    ICM_EHTOP_T ehtop;
}ICM_SCAN_INFO_T;


/* Scan data base list */
typedef struct icm_scan_list {
    ICM_SCAN_INFO_T elem[MAX_SCAN_ENTRIES];     /* List of BSS Info */
} ICM_SCAN_LIST_T;

/* Definitions for Scan Bands.
   These should be kept in sync with the definitions in osif_private.h,
   which shouldn't be included from here. In any case, the band definitions
   are very slow to be added and this isn't much of a concern. */
#define SCAN_BAND_ALL            (0)
#define SCAN_BAND_2G_ONLY        (1)
#define SCAN_BAND_5G_ONLY        (2)
#define ICM_DEFAULT_SCAN_TIMEOUT (30000000) /* 30 seconds */

/* Scan Band Management Context */
typedef struct icm_sbandmgmt_ctx {
    bool is_valid;          /* Whether this structure is valid. It is to be
                               marked valid once used in a configuration, and 
                               marked invalid once a restoration has occurred */
    int oprtv_scanband;     /* Operative value of Scan Band configured for the
                               device */
    int req_scanband;       /* Scan Band value required for our scan, using the
                               device */
} ICM_SBANDMGMT_CTX_T;


#define MAX_USABILITY       ((1<<16)-1)

/* Per-channel penalization/exclusion record */
typedef struct icm_penalization {
    bool   exclude;                 /* Whether to exclude this channel,
                                       as dictated by internal/external
                                       entity. */
    bool   opri20_exclude;          /* Whether to exclude this channel only for
                                       use as pri20 as dictated by
                                       internal/external entity. It may still be
                                       used as sec20/sec40/sec80 if other
                                       exclusions are not requested. */
    bool pwr_mode_exclude[ICM_MAX_POWER_MODE + 1];   /* Whether to exclude this channel
                                                        for the specified power mode as
                                                        as dictated by internal/external
                                                        entity. */
    u_int16_t baseline_usability;   /* Baseline usability imposed by
                                       internal/external entity to penalize this
                                       channel for presence of an SSID. */
} ICM_PENALIZATION_T;

/* Penalization/exclusion record listing indexed by channel number */
typedef struct icm_penalization_list {
    ICM_PENALIZATION_T  chpnlz[MAX_NUM_CHANNEL];
} ICM_PENALIZATION_LIST_T;

#ifdef WLAN_SPECTRAL_ENABLE
/* Spectral info */
typedef struct icm_spectral_info {
    struct ath_diag atd;            /* Atheros Socket */
    struct ifreq ifr;               /* request io */
    int dwell_interval;             /* how long to stay in given channel */
    int dwell_interval_short;       /* shortened interval to stay in a given 
                                       channel, for channels which do not have
                                       MWO interference */
    int spectral_scan_count;        /* how long to loop to channels doing spectral scan */
    int current_channel;            /* current channel in spectral is active */
    int log_mode;
}ICM_SPECTRAL_INFO_T;

/* Spectral Configuration Management Context */
typedef struct icm_spectralcfgmgmt_ctx {
    bool is_valid;               /* Whether this structure is valid. It is to be
                                    marked valid once used in a configuration,
                                    and marked invalid once a restoration has
                                    occurred */
    struct spectral_config saved_sp;  /* Saved values of Spectral Configuration
                                    Parameters*/
} ICM_SPECTRALCFGMGMT_CTX_T;
#endif /* WLAN_SPECTRAL_ENABLE */

typedef enum sock_type {
    SOCK_TYPE_TCP,
    SOCK_TYPE_UDP,
}SOCK_TYPE_T;

#define CONFIGURED_SOCK_TYPE(pdev)     ((pdev)->conf.sock_type)

typedef struct icm_inet {
    int listener;
    int on;
    int client_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    socklen_t addrlen;
    SOCK_TYPE_T type;
}ICM_INET_T;

#define ICM_GET_ADDR_OF_INETINFO(pdev) (&(pdev)->inet_sock_info)

typedef struct icm_nlsock {
    int sock_fd;
    struct sockaddr_nl src_addr;
    struct sockaddr_nl dst_addr;
    int rbuff_sz_def;
}ICM_NLSOCK_T;

#define ICM_GET_ADDR_OF_NLSOCK_INFO(pdev) (&(pdev)->nl_sock_info)
#define ICM_GET_ADDR_OF_RTNLSOCK_INFO(pdev) (&(pdev)->rtnl_sock_info)
#define ICM_GET_ADDR_OF_NL80211_SOCK(pdev) (pdev->nl80211->nl)
#define ICM_GET_ADDR_OF_NL80211_SOCK_EVENT(pdev) (pdev->nl80211->nl_event)

typedef struct icm_iocsock {
    int sock_fd;
} ICM_IOCSOCK_T;

#define ICM_GET_ADDR_OF_IOCSOCK_INFO(pdev) (&(pdev)->ioc_sock_info)

typedef struct icm_file {
    FILE *file;
} ICM_FILE_T;

#define ICM_GET_ADDR_OF_SELDBGFILE_INFO(pdev) (&(pdev)->seldbg_file_info)


/* ICM application internal events. */
typedef enum icm_event {
    ICM_EVENT_SPECTRAL_SWEEP_STARTED,
    ICM_EVENT_SPECTRAL_SWEEP_DONE,
    ICM_EVENT_SCAN_JOB_DONE,
    ICM_EVENT_INVALID,
} ICM_EVENT_T;

/* ICM application internal events recipients */
typedef enum icm_event_recipient {
    ICM_EVENT_RECIPIENT_MAIN,
    ICM_EVENT_RECIPIENT_SPECTRAL_LOOP,
    ICM_EVENT_RECIPIENT_INVALID,
} ICM_EVENT_RECIPIENT_T;

typedef struct icm_pipe {
    int pfds[2];
}ICM_PIPE_T;

#define ICM_GET_ADDR_OF_MNPIPE_INFO(pdev) (&(pdev)->mn_pipe_info)
#define ICM_GET_ADDR_OF_SRPIPE_INFO(pdev) (&(pdev)->sr_pipe_info)


/* Application states.
   These *must* correspond to the state values
   defined in the TCA Design. */
typedef enum icm_state {
    ICM_STATE_INITIALIZING,
    ICM_STATE_IDLE,
    ICM_STATE_SPECTRAL_SCAN,
    ICM_STATE_CHANNEL_SCAN,
    ICM_STATE_INVALID
} ICM_STATE_T;

#define ICM_SCAN_TYPE_CHANNEL_SCAN    (0x01)
#define ICM_SCAN_TYPE_SPECTRAL_SCAN   (0x02)

typedef enum icm_scan_select_status {
    ICM_SCAN_SELECT_STATUS_SUCCESS,
    ICM_SCAN_SELECT_STATUS_NOPREVINFO,
    ICM_SCAN_SELECT_STATUS_SCAN_CANCELLED,
    ICM_SCAN_SELECT_STATUS_FAILURE,
    ICM_SCAN_SELECT_STATUS_INVALID
} ICM_SCAN_SELECT_STATUS_T;

/* Relationship of a channel with the primary 20 MHz
   in a given channel bonding span.
Note: Do NOT introduce holes in the below enumeration. */
typedef enum icm_chan_relationship {
    ICM_CHAN_RELATIONSHIP_SELF,
    ICM_CHAN_RELATIONSHIP_ADJ,   /* Outside bonding span, but adjacent */
    ICM_CHAN_RELATIONSHIP_OVLP,  /* Overlapping channel */
    ICM_CHAN_RELATIONSHIP_SEC20, /* Secondary 20 MHz */
    ICM_CHAN_RELATIONSHIP_SEC40, /* In secondary 40 MHz */
    ICM_CHAN_RELATIONSHIP_SEC80, /* In secondary 80 MHz */
    ICM_CHAN_RELATIONSHIP_INVALID,
} ICM_CHAN_RELATIONSHIP_T;

extern const char *icm_chan_relationship_str[];
extern const char *icm_chan_relationship_shortstr[];

/* Scan configurations that can be requested by an
   external entity.
   These correspond to values in the GET_RANKED_CHANNELS_REQ
   request. See TCA Design. */
typedef struct icm_scan_config {
    ICM_CH_BW_T channel_config;
    u_int8_t    scan_type;
    u_int8_t    max_channels;
    u_int8_t    max_aps_per_channel;
} ICM_SCAN_CONFIG_T;

/* Request parameters received from peer in
   GET_RANKED_CHANNELS_REQ, apart from the core
   scan parameters. These are required for deferred
   response to the peer.
   See TCA Design for details of the parameters. */
typedef struct icm_reqmsg_params {
    u_int8_t    transactionId;
    /* Add future parameters here */
} ICM_REQMSG_PARAMS_T;

/* Polling interval in milliseconds to find out if device is up,
   before setting channel. Should not be set to more than 999 */
#define ICM_CHANSET_POLL_INTERVAL_MS    (300)

/* Timeout in seconds to wait for a device to come up
   after receiving channel set command. This should
   be lower than the timeout used by the peer entity */
#define ICM_CHANSET_DEVUP_TIMEOUT_S      (18)


/* Linked List of Channel Set Job requests */
typedef struct icm_chanset_job {
    struct icm_chanset_job     *next;
    struct icm_chanset_job     *prev;
    u_int8_t                   transactionId;
    char                       dev_ifname[IFNAMSIZ];
    struct timeval             timeout;
} ICM_CHANSET_JOB_T;

typedef struct icm_chanset_job_queue {
    ICM_CHANSET_JOB_T *head;
    ICM_CHANSET_JOB_T *tail;
} ICM_CHANSET_JOB_LL_T;

/* Per-band channel information */
typedef struct icm_perband_chinfo {
    /* Whether this band is a candidate for channel selection */
    bool is_candidate_band;

    /* List of channels in this band */
    ICM_CHANNEL_LIST_T  chlist;

    /* Channel properties list */
    ICM_CHANNEL_PROPERTIES_LIST_T proplist;

    /* Channel penalization list */
    ICM_PENALIZATION_LIST_T pnlzlist;
} ICM_PERBAND_CHINFO_T;

/* Selection results information */
typedef struct icm_selection_results_info
{
    /* Selected band */
    ICM_BAND_T selected_best_band;

    /* Selected best primary channel index */
    int selected_best_chan_idx;

    /* Selected best secondary 80 MHz center index (applicable only for
     * 80+80 MHz)
     */
    int selected_best_cfreq2_chan_idx;

    /* Selected width */
    ICM_CH_BW_T selected_width;

    /* Usability for the combination */
    u_int16_t usability;

    /* Max regulatory power */
    int8_t maxregpower;

    /* selected power mode */
    ICM_AFC_POWER_MODE_T selected_pwr_mode;

    /* puncture bitmap of the best channel selected */
    u_int16_t selected_punc_bitmap;
} ICM_SELECTION_RESULTS_INFO_T;

/* Multi-band scaling factor information */
typedef struct multiband_scaling_factor_info
{
    /* Scaling factor to be used for 20 MHz width when comparing between
     * bands
     */
    u_int8_t      multiband_scaling_factor_bw20;

    /* Scaling factor to be used for 40 MHz width when comparing between
     * bands
     */
    u_int8_t      multiband_scaling_factor_bw40;

    /* Scaling factor to be used for 80 MHz width when comparing between
     * bands
     */
    u_int8_t      multiband_scaling_factor_bw80;

    /* Scaling factor to be used for 160 MHz width when comparing between
     * bands
     */
    u_int8_t      multiband_scaling_factor_bw160;

    /* Scaling factor to be used for 320 MHz width when comparing between
     * bands
     */
    u_int8_t      multiband_scaling_factor_bw320;

    /* Scaling factor to be used for 80+80 MHz width when comparing between
     * bands
     */
    u_int8_t      multiband_scaling_factor_bw80p80;

    /* Max scaling factor when comparing between bands */
    u_int8_t      multiband_scaling_factor_max;

   /* Scaled usability margin to be considered when comparing between bands. If
    * the scaled usabilities of two bands fall within this margin, then the max
    * regulatory power of their respective best channels are considered.
    */
    u_int16_t     multiband_scaled_usability_margin_maxregpwr;
} ICM_MULTIBAND_SCALING_FACTOR_INFO_T;

/* Main icm info structure */
typedef struct icm_info {
    char dev_ifname[IFNAMSIZ];
    char*  radio_ifname;
    u_int8_t numdevs;   /* Number of VAPs under this radio*/
    char dev_ifnames_list[MAX_VAP_NUM][IFNAMSIZ];
    u_int8_t radio_addr[ETH_ALEN];
#ifdef WLAN_SPECTRAL_ENABLE
    ICM_SPECTRAL_INFO_T sinfo;
    ICM_SPECTRALCFGMGMT_CTX_T scfgmgmt_ctx;
#endif /* WLAN_SPECTRAL_ENABLE */
    ICM_SCAN_LIST_T slist;
    ICM_CHANNEL_LIST_T  chlist;
    ICM_PERBAND_CHINFO_T pbchinfo[ICM_BAND_MAX];
    ICM_CHANNEL_T   selected_channel;
    ICM_PHY_SPEC_T phy_spec;
    ICM_CH_BW_T   channel_width;
    ICM_CH_BW_T   selected_channel_width; /* In case external entity wants us
                                             to carry out a selection */

    /* Profile or historical data can be used to fine-tune the below usage
     * factors.
     */
    u_int8_t      usage_factor_80_bw40;       /* Anticipated usage factor in
                                                 percentage for 20/40 MHz PPDUs,
                                                 in 80 MHz mode. */
    u_int8_t      usage_factor_80_bw80;       /* Anticipated usage factor in
                                                 percentage for 80 MHz PPDUs,
                                                 in 80 MHz mode. */

    u_int8_t      usage_factor_160_bw40;      /* Anticipated usage factor in
                                                 percentage for 20/40 MHz PPDUs,
                                                 in 160 MHz mode. */
    u_int8_t      usage_factor_160_bw80;      /* Anticipated usage factor in
                                                 percentage for 80 MHz PPDUs,
                                                 in 160 MHz mode. */
    u_int8_t      usage_factor_160_bw160;     /* Anticipated usage factor in
                                                 percentage for 160 MHz PPDUs,
                                                 in 160 MHz mode. */

    u_int8_t      usage_factor_6g_320_bw40;   /* Anticipated usage factor in
                                                 percentage for 20/40 MHz PPDUs,
                                                 in 6 GHz band 320 MHz mode. */
    u_int8_t      usage_factor_6g_320_bw80;   /* Anticipated usage factor in
                                                 percentage for 80 MHz PPDUs,
                                                 in 6 GHz band 320 MHz mode. */
    u_int8_t      usage_factor_6g_320_bw160;  /* Anticipated usage factor in
                                                 percentage for 160 MHz PPDUs,
                                                 in 6 GHz band 320 MHz mode. */
    u_int8_t      usage_factor_6g_320_bw320;  /* Anticipated usage factor in
                                                 percentage for 320 MHz PPDUs,
                                                 in 6 GHz band 320 MHz mode. */

    u_int8_t      usage_factor_5g_320_bw40;   /* Anticipated usage factor in
                                                 percentage for 20/40 MHz PPDUs,
                                                 in 5GHz band 320 MHz mode. */
    u_int8_t      usage_factor_5g_320_bw80;   /* Anticipated usage factor in
                                                 percentage for 80 MHz PPDUs,
                                                 in 5GHz band 320 MHz mode. */
    u_int8_t      usage_factor_5g_320_bw160;  /* Anticipated usage factor in
                                                 percentage for 160 MHz PPDUs,
                                                 in 5GHz band 320 MHz mode. */
    u_int8_t      usage_factor_5g_320_bw320;  /* Anticipated usage factor in
                                                 percentage for 320 MHz PPDUs,
                                                 in 5GHz band 320 MHz mode. */

    u_int8_t      usage_factor_80p80_bw40;    /* Anticipated usage factor in
                                                 percentage for 20/40 MHz PPDUs,
                                                 in 80+80 MHz mode. */
    u_int8_t      usage_factor_80p80_bw80;    /* Anticipated usage factor in
                                                 percentage for 80 MHz PPDUs,
                                                 in 80+80 MHz mode. */

    /* Multi-band scaling factor info */
    ICM_MULTIBAND_SCALING_FACTOR_INFO_T mb_scaling_factor_info;

    int rfreg_domain;
    enum nl80211_dfs_regions dfs_domain; /* This is similar to HAL_DFS_DOMAIN in rtr driver definition */
    u_int16_t sort_chan_list[MAX_NUM_CHANNEL];
    u_int16_t sort_chan_list_sec80[MAX_NUM_CHANNEL];
    u_int16_t num_usable_ch;
    u_int16_t num_usable_ch_sec80;
    u_int16_t cur_ch_idx;
    u_int16_t cur_ch_idx_sec80;
    int current_channel;
    int init_classifier;
    u_int16_t scan_in_progress;
    int def_channel;
    ICM_BAND_T best_band;
    int best_channel;
    int best_cfreq2_channel;
    int8_t dev_index;
    ICM_STATE_T substate; /* Per-icm state */
    ICM_CH_SELECTION_MODE_T ch_selection_mode;
    ICM_SCAN_CONFIG_T scan_config;
    ICM_SBANDMGMT_CTX_T sbmgmt_ctx;
    ICM_REQMSG_PARAMS_T reqmsg_params;
    bool            is_prev_scaninfo_available;

    /* Scanner thread */
    pthread_t       scanner_thread;
    pthread_attr_t  scanner_thread_attr;
    pthread_mutex_t scanner_thread_mutex;
    bool            is_scanner_thread_active;
    bool            is_80211scan_done;
    bool            is_80211scan_cancel_requested;
    int             scanner_thread_status; /* Exit status for latest invocation
                                              of scanner thread */
    /* Channel Set thread */
    pthread_t           chanset_thread;
    pthread_attr_t      chanset_thread_attr;
    pthread_mutex_t     chanset_thread_mutex;
    bool                is_chanset_thread_active;
    int                 chanset_thread_status; /* Exit status for latest
                                                  invocation of channel
                                                  set thread */
    ICM_CHANSET_JOB_LL_T  main_list;
    ICM_CHANSET_JOB_LL_T  repeat_list;
    int get_nominal_nf; /* Get Nominal NF from hardware */
    struct nl80211_data drv; /* nl80211 driver data per interface */
    bool offload_enabled; /* Get this info from driver */
    /* Representative Tx power usage policy */
    ICM_REP_TXPOWER_POLICY_T rep_txpower_policy;
    /* Usability margin within which the top ranked channels will be re-assessed
     * based on representative Tx power */
    uint16_t usability_margin_rep_txpower;
    /* Whether to enable preferential use of U-NII-3 for 802.11ax */
    bool enable_11ax_unii3_pref;
    /* Usability margin within which the top ranked channels will be re-assessed
     * based on presence in U-NII-3 band (802.11ax only)
     */
    uint16_t usability_margin_11ax_unii3_pref;

    /* Whether to enable use of channel grading */
    bool enable_chan_grade_usage;

    bool spectral_capable; /* Get this info from driver */
    bool afc_capable; /* Get this info from driver */
    uint8_t puncture_legacy_weightage;
    uint8_t puncture_ofdma_weightage;
    uint8_t puncture_nofdma_weightage;
    ICM_RROPAVAIL_INFO_T rropavail_info; /* RROP availability info */
    struct pcl_list *pcl;      /* preferred channel list as send by driver */
    uint8_t reselect_reason;   /* reselection reason */

    double wpf;               /* wireless presence factor. This is just to pass around wpf value
                                 using icm pointe so as to avoid changing function signature.
                                 Do we have better way? */
#ifdef WLAN_SPECTRAL_ENABLE
    uint32_t spectral_hw_gen;  /* Spectral hardware generation */
#endif /* WLAN_SPECTRAL_ENABLE */
#ifdef ICM_RTR_DRIVER
    struct nl80211_user_chanlist pri20_blockchanlist;
    struct nl80211_chan_rf_characterization_info_list chan_rf_characterization;
#endif /* ICM_RTR_DRIVER */
    char* dev_ifname_global_event; /* Interface name received in global event */
    bool acs_in_progress;       /* Whether ACS is in progress */
    bool init_complete; /* Whether initialization is completed */
    uint8_t link_id; /* Link id to identify the interface */
}ICM_INFO_T;

/* Start of attribution: Code taken from QCA apstats */
/* Structure to hold radio and VAP names read from /proc */
typedef struct
{
    char **ifnames;
    int max_size;
    int curr_size;
} sys_ifnames;

/* Debug information generated by the selector algorithm. It is kept segregated
   from other data structures to provide a specific and cohesive set of fields
   for display purposes, freshly determined during every pass of the selection
   algorithm and ready to use by the display helper(s). */

#define ICM_MAX_NUM_ADJ_CHAN        (2)

/* The below is as of the 802.11ac standard */
#define ICM_MAX_NUM_BONDING_CHAN    (8)

#define ICM_MAX_NUM_2_4GHZ_CHAN     (14)

/* Max number of channels related to a given primary 20 MHz channel.
   This includes the primary itself. */
#define ICM_MAX_NUM_REL_CHAN        (MAX_INT((ICM_MAX_NUM_ADJ_CHAN +     \
                ICM_MAX_NUM_BONDING_CHAN), \
            ICM_MAX_NUM_2_4GHZ_CHAN))

#define ICM_SELDBG_INTERCOL_PADDING (2)

/* Field status information.
   Note: Do NOT introduce holes in the below enumeration. */
typedef enum icm_seldbg_fstatus {
    ICM_SELDBG_FSTATUS_NOTCOMP = 0,  /* Not computed, and not valid.
                                        MUST be 0. */
    ICM_SELDBG_FSTATUS_NOTAPPL,      /* Not applicable */
    ICM_SELDBG_FSTATUS_NOTREQ,       /* Not required */
    ICM_SELDBG_FSTATUS_NOTUSED,      /* Not used */
    ICM_SELDBG_FSTATUS_SET,          /* Is set and valid. */
    ICM_SELDBG_FSTATUS_INVALID,      /* Invalid enumeration. */
} ICM_SELDBG_FSTATUS_T;

extern const char *icm_seldbg_fstatus_str[];
extern const char *icm_seldbg_fstatus_shortstr[];

#define ICM_SELDBG_MARKFIELD_NC(field)      { field##_fstatus = \
    ICM_SELDBG_FSTATUS_NOTCOMP; }
#define ICM_SELDBG_MARKFIELD_NA(field)      { field##_fstatus = \
    ICM_SELDBG_FSTATUS_NOTAPPL; }
#define ICM_SELDBG_MARKFIELD_NR(field)      { field##_fstatus = \
    ICM_SELDBG_FSTATUS_NOTREQ; }
#define ICM_SELDBG_MARKFIELD_NU(field)      { field##_fstatus = \
    ICM_SELDBG_FSTATUS_NOTUSED; }
#define ICM_SELDBG_SETFIELD(field, val)     { field = (val); \
    field##_fstatus = \
    ICM_SELDBG_FSTATUS_SET; }
#define ICM_SELDBG_GETFSTATUS(field)        (field##_fstatus)

/* Rejection reason.
Note: Do NOT introduce holes in the below enumeration. */
typedef enum icm_seldbg_rejcode {
    ICM_SELDBG_REJCODE_NOTREJ = 0,   /* Not (yet) rejected. MUST be 0. */
    ICM_SELDBG_REJCODE_PRI_EXCLD,    /* Primary excluded by external entity. */
    ICM_SELDBG_REJCODE_BOND_EXCLD,   /* Bonding channel excluded by external
                                        entity. */
    ICM_SELDBG_REJCODE_OVLP_EXCLD,   /* Overlapping channel excluded by external
                                        entity. */
    ICM_SELDBG_REJCODE_PRI_INCAP,    /* Primary channel incapable of PHY spec
                                        and/or width requested. */
    ICM_SELDBG_REJCODE_PRI_SEC20,    /* Primary channel falls on secondary
                                        20 MHz of OBSS. */
    ICM_SELDBG_REJCODE_PRI_SEC40,    /* Primary channel falls on secondary
                                        40 MHz of 160/80+80 MHz OBSS. */
    ICM_SELDBG_REJCODE_BOND_PRI20,   /* Bonding channel falls on primary
                                        20 MHz of OBSS. */
    ICM_SELDBG_REJCODE_SEC20_OCC,    /* Secondary 20 MHz is already occupied by
                                        primary 20 MHz of OBSS. */
    ICM_SELDBG_REJCODE_SEC40_OCC,    /* Secondary 40 MHz of desired 160/80+80
                                        MHz BSS is already occupied by primary
                                        20 MHz of OBSS. */
    ICM_SELDBG_REJCODE_OVLP_PRI20,   /* Overlapping channel falls on primary
                                        20 MHz of OBSS. */
    ICM_SELDBG_REJCODE_PRI_ETSIWTH,  /* Primary channel is a weather radar
                                        channel in the ETSI domain.*/
    ICM_SELDBG_REJCODE_BOND_ETSIWTH, /* Bonding channel is a weather radar
                                        channel in the ETSI domain.*/
    ICM_SELDBG_REJCODE_PRI_CW,       /* Primary channel suffers CW
                                        interference. */
    ICM_SELDBG_REJCODE_BOND_CW,      /* Bonding channel suffers CW
                                        interference. */
    ICM_SELDBG_REJCODE_PRI_OCAC,     /* Primary channel has not completed
                                      * off-channel CAC */
    ICM_SELDBG_REJCODE_BOND_OCAC,    /* Bonding channel has not completed
                                      * off-channel CAC */
    ICM_SELDBG_REJCODE_INVALID,
} ICM_SELDBG_REJCODE_T;

extern  const char *icm_seldbg_rejcode_str[];
extern  const char *icm_seldbg_rejcode_shortstr[];

typedef enum icm_rejection_policy {
    ICM_REJECT_POLICY_INVALID                = 0,    /* invalid value for policy */
    ICM_REJECT_POLICY_STRICT                 = 1,    /* strictly reject if applicable */
    ICM_REJECT_POLICY_SKIP                   = 2,    /* always skip this rejection*/

    /* Do not exceed this enum beyond 4 bits value. Anything above 0x0f would not be considered */
    ICM_REJECT_POLICY_SIZE,                          /* Keep this last */
} icm_rej_policy;

#define ICM_REJECT_POLICY_MASK_DEFAULT 0x1111111111111111

/* Per-channel debug information generated by the selector algorithm. This must
   be used in conjunction with the containing ICM_SELDBG_DECISION_INFO_T
   structure. 
   Note that each member has a companion field status which indicates whether
   it is not computed, not applicable, set to a valid value, etc.
 */
typedef struct icm_seldbg_chan_info {
    /* Whether this structure is valid. If is_valid_fstatus is
       ICM_SELDBG_FSTATUS_NOTCOMP, the structure is considered invalid by
       default. */
    ICM_SELDBG_FSTATUS_T        is_valid_fstatus;
    bool                        is_valid;

    ICM_SELDBG_FSTATUS_T        chan_num_fstatus;
    u_int16_t                   chan_num;

    ICM_SELDBG_FSTATUS_T        freq_fstatus;
    double                      freq;

    ICM_SELDBG_FSTATUS_T        center_chan_num_fstatus;
    u_int16_t                   center_chan_num;

    ICM_SELDBG_FSTATUS_T        center_freq_fstatus;
    double                      center_freq;

    /* Relationship with proposed primary 20 MHz channel (available from
       containing ICM_SELDBG_DECISION_INFO_T). */
    ICM_SELDBG_FSTATUS_T        relation_fstatus;
    ICM_CHAN_RELATIONSHIP_T     relation;

    /* Whether this channel is incapable of the PHY spec and width requested
       (available from containing ICM_SELDBG_DECISION_INFO_T). */
    ICM_SELDBG_FSTATUS_T        is_incapable_fstatus;
    bool                        is_incapable;

    /* Whether this channel has been excluded by an external entity. */
    ICM_SELDBG_FSTATUS_T        is_excluded_fstatus;
    bool                        is_excluded;

    /* Whether this channel is the primary channel for an OBSS (valid only if
       .relation is ICM_CHAN_RELATIONSHIP_SEC20, ICM_CHAN_RELATIONSHIP_SEC40 or
       ICM_CHAN_RELATIONSHIP_OVLP).
     */
    ICM_SELDBG_FSTATUS_T        is_obss_pri20_fstatus;
    bool                        is_obss_pri20;

    /* Number of OBSS APs found with their primary corresponding to this
       channel. */
    ICM_SELDBG_FSTATUS_T        num_obss_aps_fstatus;
    int                         num_obss_aps;

    /* Whether this channel is the secondary 20 MHz channel for an OBSS (valid
       only if .relation is ICM_CHAN_RELATIONSHIP_SELF)*/
    ICM_SELDBG_FSTATUS_T        is_obss_sec20_fstatus;
    bool                        is_obss_sec20;

    /* Whether this channel is on the secondary 40 MHz channel for a
       160/80+80 MHz OBSS (valid only if .relation is
       ICM_CHAN_RELATIONSHIP_SELF) */
    ICM_SELDBG_FSTATUS_T        is_obss_sec40_fstatus;
    bool                        is_obss_sec40;

    ICM_SELDBG_FSTATUS_T        noise_floor_fstatus;
    int                         noise_floor;

    ICM_SELDBG_FSTATUS_T        noise_floor_thresh_fstatus;
    int                         noise_floor_thresh;

    ICM_SELDBG_FSTATUS_T        maxregpower_fstatus;
    int8_t                      maxregpower;

    /* Whether this channel is a weather radar channel in the ETSI domain. */
    ICM_SELDBG_FSTATUS_T        is_etsi_weather_fstatus;
    bool                        is_etsi_weather;

    /* Whether CW interference was found on this channel. */
    ICM_SELDBG_FSTATUS_T        is_cw_intf_fstatus;
    bool                        is_cw_intf;

    /* Whether Microwave interference was found on this channel. */
    ICM_SELDBG_FSTATUS_T        is_mwo_intf_fstatus;
    bool                        is_mwo_intf;

    /* Whether FHSS interference was found on this channel. */
    ICM_SELDBG_FSTATUS_T        is_fhss_intf_fstatus;
    bool                        is_fhss_intf;

    /* Whether this channel is pending off-channel CAC */
    ICM_SELDBG_FSTATUS_T        is_ocac_pending_fstatus;
    bool                        is_ocac_pending;
}ICM_SELDBG_CHAN_INFO_T;

/* Selection decision debug information generated by the selector algorithm.
   This is per candidate primary channel. */
typedef struct icm_seldbg_decision_info {
    ICM_SELDBG_FSTATUS_T    band_fstatus;
    ICM_BAND_T              band;

    ICM_SELDBG_FSTATUS_T    physpec_fstatus;
    ICM_PHY_SPEC_T          physpec;

    ICM_SELDBG_FSTATUS_T    width_fstatus;
    ICM_CH_BW_T             width;

    /* Information for primary channel */
    ICM_SELDBG_CHAN_INFO_T  prichaninfo;

    /* Information for related channels.
       For simplicity:
       2.4 GHz: These will be indexed by channel number - 1.
       5 GHz: These will be indexed with reference to value of adj_check_min_adj
       determined in the selector algorithm.
       For both bands, the slot corresponding to the primary channel will remain
       vacant (marked invalid).
     */
    ICM_SELDBG_CHAN_INFO_T  relchaninfo[ICM_MAX_NUM_REL_CHAN];

    /* FHSS degradation factor used. */
    ICM_SELDBG_FSTATUS_T    fhss_degrade_factor_fstatus;
    u_int8_t                fhss_degrade_factor;

    /* MWO degradation factor used. */
    ICM_SELDBG_FSTATUS_T    mwo_degrade_factor_fstatus;
    u_int8_t                mwo_degrade_factor;

    /* Whether all related channels have been processed. */
    ICM_SELDBG_FSTATUS_T    is_allrelchanprocd_fstatus;
    bool                    is_allrelchanprocd;

    ICM_SELDBG_FSTATUS_T    baseline_usability_fstatus;
    u_int16_t               baseline_usability;

    /* Total number of APs affecting candidacy of our potential primary
       channel.
       This is for widths lesser than 80 MHz, only */
    ICM_SELDBG_FSTATUS_T    tot_num_aps_fstatus;
    int                     tot_num_aps;

    /* Total number of APs affecting candidacy of our potential primary
       channel for 20/40 MHz PPDU transmissions, when configured for 80 MHz
       width.
       This includes our own BSS.
     */
    ICM_SELDBG_FSTATUS_T    tot_num_aps_80w_40ppdu_fstatus;
    int                     tot_num_aps_80w_40ppdu;

    /* Total number of APs affecting candidacy of our potential primary
       channel for 80 MHz PPDU transmissions, when configured for 80 MHz width.
       This includes our own BSS.
     */
    ICM_SELDBG_FSTATUS_T    tot_num_aps_80w_80ppdu_fstatus;
    int                     tot_num_aps_80w_80ppdu;

    /* Total number of APs affecting candidacy of our potential primary
       channel for 20/40 MHz PPDU transmissions, when configured for 160 MHz
       width.
       This includes our own BSS.
     */
    ICM_SELDBG_FSTATUS_T    tot_num_aps_160w_40ppdu_fstatus;
    int                     tot_num_aps_160w_40ppdu;

    /* Total number of APs affecting candidacy of our potential primary
       channel for 80 MHz PPDU transmissions, when configured for 160 MHz width.
       This includes our own BSS.
     */
    ICM_SELDBG_FSTATUS_T    tot_num_aps_160w_80ppdu_fstatus;
    int                     tot_num_aps_160w_80ppdu;

    /* Total number of APs affecting candidacy of our potential primary
       channel for 160 MHz PPDU transmissions, when configured for 160 MHz width.
       This includes our own BSS.
     */
    ICM_SELDBG_FSTATUS_T    tot_num_aps_160w_160ppdu_fstatus;
    int                     tot_num_aps_160w_160ppdu;

    /* Total number of APs affecting candidacy of our potential primary
       channel for 20/40 MHz PPDU transmissions, when configured for 320 MHz
       width.
       This includes our own BSS.
     */
    ICM_SELDBG_FSTATUS_T    tot_num_aps_320w_40ppdu_fstatus;
    int                     tot_num_aps_320w_40ppdu;

    /* Total number of APs affecting candidacy of our potential primary
       channel for 80 MHz PPDU transmissions, when configured for 320 MHz width.
       This includes our own BSS.
     */
    ICM_SELDBG_FSTATUS_T    tot_num_aps_320w_80ppdu_fstatus;
    int                     tot_num_aps_320w_80ppdu;

    /* Total number of APs affecting candidacy of our potential primary
       channel for 160 MHz PPDU transmissions, when configured for 320 MHz width.
       This includes our own BSS.
     */
    ICM_SELDBG_FSTATUS_T    tot_num_aps_320w_160ppdu_fstatus;
    int                     tot_num_aps_320w_160ppdu;

    /* Total number of APs affecting candidacy of our potential primary
       channel for 320 MHz PPDU transmissions, when configured for 320 MHz width.
       This includes our own BSS.
     */
    ICM_SELDBG_FSTATUS_T    tot_num_aps_320w_320ppdu_fstatus;
    int                     tot_num_aps_320w_320ppdu;

    /* Total number of APs affecting candidacy of our potential primary
       channel for 20/40 MHz PPDU transmissions, when configured for 80+80 MHz
       width.
       This applies to the selection process for primary 80 MHz.
       This count includes our own BSS.
     */
    ICM_SELDBG_FSTATUS_T    tot_num_aps_80p80w_40ppdu_fstatus;
    int                     tot_num_aps_80p80w_40ppdu;

    /* Total number of APs affecting candidacy of our potential primary
       channel for 80 MHz PPDU transmissions, when configured for 80+80 MHz width.
       This applies to the selection process for primary 80 MHz.
       This count includes our own BSS.
     */
    ICM_SELDBG_FSTATUS_T    tot_num_aps_80p80w_80ppdu_fstatus;
    int                     tot_num_aps_80p80w_80ppdu;

    /* See documentation in selector code for info on the next three
       usabilities */
    ICM_SELDBG_FSTATUS_T    usability_40ppdu_fstatus;
    u_int16_t               usability_40ppdu;

    ICM_SELDBG_FSTATUS_T    usability_80ppdu_fstatus;
    u_int16_t               usability_80ppdu;

    ICM_SELDBG_FSTATUS_T    usability_160ppdu_fstatus;
    u_int16_t               usability_160ppdu;

    ICM_SELDBG_FSTATUS_T    usability_320ppdu_fstatus;
    u_int16_t               usability_320ppdu;

    ICM_SELDBG_FSTATUS_T    usage_factor_80_bw40_fstatus;
    u_int8_t                usage_factor_80_bw40;

    ICM_SELDBG_FSTATUS_T    usage_factor_80_bw80_fstatus;
    u_int8_t                usage_factor_80_bw80;

    ICM_SELDBG_FSTATUS_T    usage_factor_160_bw40_fstatus;
    u_int8_t                usage_factor_160_bw40;

    ICM_SELDBG_FSTATUS_T    usage_factor_160_bw80_fstatus;
    u_int8_t                usage_factor_160_bw80;

    ICM_SELDBG_FSTATUS_T    usage_factor_160_bw160_fstatus;
    u_int8_t                usage_factor_160_bw160;

    ICM_SELDBG_FSTATUS_T    usage_factor_320_bw40_fstatus;
    u_int8_t                usage_factor_320_bw40;

    ICM_SELDBG_FSTATUS_T    usage_factor_320_bw80_fstatus;
    u_int8_t                usage_factor_320_bw80;

    ICM_SELDBG_FSTATUS_T    usage_factor_320_bw160_fstatus;
    u_int8_t                usage_factor_320_bw160;

    ICM_SELDBG_FSTATUS_T    usage_factor_320_bw320_fstatus;
    u_int8_t                usage_factor_320_bw320;

    ICM_SELDBG_FSTATUS_T    usage_factor_80p80_bw40_fstatus;
    u_int8_t                usage_factor_80p80_bw40;

    ICM_SELDBG_FSTATUS_T    usage_factor_80p80_bw80_fstatus;
    u_int8_t                usage_factor_80p80_bw80;

    /*
     * Computed usability prior to application of primary 20 MHz channel
     * performance grade
     */
    ICM_SELDBG_FSTATUS_T    prepri20grading_usability_fstatus;
    u_int16_t               prepri20grading_usability;

    /* Primary 20 MHz channel performance grade */
    ICM_SELDBG_FSTATUS_T    pri20channel_grade_fstatus;
    u_int8_t                pri20channel_grade;

    /*
     * Computed usability prior to application of 6 GHz non-PSC channel
     * penalization.
     */
    ICM_SELDBG_FSTATUS_T    pre6ghznonpscpnl_usability_fstatus;
    u_int16_t               pre6ghznonpscpnl_usability;

    /*
     * 6 GHz non-PSC channel penalization factor in percentage. The usability is
     * reduced to this percentage of its original value.
     */
    ICM_SELDBG_FSTATUS_T    pre6ghznonpscpnl_factor_fstatus;
    u_int8_t                pre6ghznonpscpnl_factor;

    /* Computed usability prior to adjustment against measured usability. */
    ICM_SELDBG_FSTATUS_T    preadj_usability_fstatus;
    u_int16_t               preadj_usability;

    /* Measured usability prior to penalization */
    ICM_SELDBG_FSTATUS_T    prepnl_measured_usability_fstatus;
    u_int16_t               prepnl_measured_usability;

    ICM_SELDBG_FSTATUS_T    is_penalized_fstatus;
    bool                    is_penalized;

    /* Measured usability after application of penalization, if applicable. */
    ICM_SELDBG_FSTATUS_T    measured_usability_fstatus;
    u_int16_t               measured_usability;

    /* Measured usability on extension channel prior to penalization.
       Currently used on only 2.4 GHz*/
    ICM_SELDBG_FSTATUS_T    prepnl_measured_usability_ext_fstatus;
    u_int16_t               prepnl_measured_usability_ext;

    ICM_SELDBG_FSTATUS_T    is_penalized_ext_fstatus;
    bool                    is_penalized_ext;

    /* Measured usability on extension channel, after application of
       penalization if applicable. Currently used on only 2.4 GHz. */
    ICM_SELDBG_FSTATUS_T    measured_usability_ext_fstatus;
    u_int16_t               measured_usability_ext;

    /* Measured usability of 80+80 MHz secondary 80 MHz segment prior to
       penalization.
     */
    ICM_SELDBG_FSTATUS_T    prepnl_measured_usability_sec80_fstatus;
    u_int16_t               prepnl_measured_usability_sec80;

    ICM_SELDBG_FSTATUS_T    is_penalized_sec80_fstatus;
    bool                    is_penalized_sec80;

    /* Measured usability of 80+80 MHz secondary 80 MHz segment after
       application of penalization, if applicable. */
    ICM_SELDBG_FSTATUS_T    measured_usability_sec80_fstatus;
    u_int16_t               measured_usability_sec80;

    ICM_SELDBG_FSTATUS_T    usability_fstatus;
    u_int16_t               usability;

    ICM_SELDBG_FSTATUS_T    rejreason_fstatus;
    ICM_SELDBG_REJCODE_T    rejreason;

    ICM_SELDBG_FSTATUS_T    is_bestchan_fstatus;
    bool                    is_bestchan;
}ICM_SELDBG_DECISION_INFO_T;

typedef struct icm_config {
    ICM_DEV_CONFIG_T dev_config[MAX_DEV_NUM];
    int num_radios;
    int sock_type;
    int server_mode;
    int walflag;
    bool spectral_enab;
    int chanload_enab;
    int nf_enab;
    int run_tests;
    int daemon;
    int get_nominal_nf;
    int enable_seldbg_dump;
    char *seldbg_filename;
    int dbg_level;
    u_int16_t dbg_module_bitmap;
    ICM_REP_TXPOWER_POLICY_T rep_txpower_policy;
    bool enable_11ax_unii3_pref;
    u64 rej_policy_bitmask;
    bool icm_rejection_rule [ICM_SELDBG_REJCODE_INVALID][ICM_REJECT_POLICY_SIZE];
    int min_rssi;
    int max_rssi;
    double rssi_weightage;
    double bss_count_weightage;
    bool enable_chan_grade_usage;
}ICM_CONFIG_T;

typedef struct icm_dev_info {
    ICM_CONFIG_T    conf;
    ICM_INET_T      inet_sock_info;
    ICM_NLSOCK_T    nl_sock_info;
#ifdef WLAN_SPECTRAL_ENABLE
    /* Reference count for Spectral NL socket operations */
    int             spectral_nlsockop_ref;

    /* Mutex to protect accesses to spectral_nlsockop_ref */
    pthread_mutex_t spectral_nlsockop_ref_lock;

    /* Count of Spectral samples received after any Spectral state change
     * (i.e. Spectral stop/start currently).
     */
    u_int64_t       spectral_poststatechange_count;
#endif /* WLAN_SPECTRAL_ENABLE */
    ICM_NLSOCK_T    rtnl_sock_info;
    ICM_IOCSOCK_T   ioc_sock_info;
    ICM_PIPE_T      sr_pipe_info;     /* Pipe used for communicating events to the
                                         Spectral receive loop entity */
    ICM_PIPE_T      mn_pipe_info;     /* Pipe used for communicating events to the
                                         main ICM loop entity  */
    ICM_FILE_T      seldbg_file_info;
    struct nlmsghdr *nlh;
    struct msghdr   msg;
    struct iovec    iov;
    SOCK_TYPE_T     sock_type;
    u_int8_t        dev_flags;
    u_int8_t        active_device;
    ICM_STATE_T     state; /* Main state */
    ICM_INFO_T      icm[MAX_VAP_NUM];
    struct nl80211_global   *nl80211;
}ICM_DEV_INFO_T;


/* Helper macros to print fields.
   These have some limitations since they are merely helpers:
   i) They assume some formatting intelligence at the caller - refer to details
   of individual macros for more information on this. 
   ii)Some of the arguments, especially the field to be printed, must be passed
   directly and not as an expression.

XXX: If ever required in the future, look at the possibility of making them
more generic.
 */

/* Print a Selection Decision Debug Info field if it has been set, else print
   the status of the field. */
#define ICM_SELDBG_PRINTENTRY(file, field, formatstr)                        \
{                                                                        \
    if (field##_fstatus == ICM_SELDBG_FSTATUS_SET) {                     \
        fprintf((file), (formatstr), field);                             \
    } else {                                                             \
        fprintf((file), "%s,", icm_seldbg_fstatus_str[field##_fstatus]); \
    }                                                                    \
}

/* Similar to ICM_SELDBG_PRINTENTRY(), but print to console/standard output
   and using short versions of representative strings.
   The user of this helper function MUST note that the field width will be
   an argument passed by the macro. */
#define ICM_SELDBG_PRINTENTRY_CONS(pdev, mod_id, field, formatstr)           \
{                                                                        \
    if (field##_fstatus == ICM_SELDBG_FSTATUS_SET) {                     \
        ICM_DPRINTF((pdev),                                              \
                ICM_PRCTRL_FLAG_NO_PREFIX,                           \
                ICM_DEBUG_LEVEL_DEFAULT,                             \
                (mod_id),                                            \
                formatstr"|",                                        \
                SHORTSTRWIDTH,                                       \
                field);                                              \
    } else {                                                             \
        ICM_DPRINTF((pdev),                                              \
                ICM_PRCTRL_FLAG_NO_PREFIX,                           \
                ICM_DEBUG_LEVEL_DEFAULT,                             \
                (mod_id),                                            \
                "%-*s|",                                             \
                SHORTSTRWIDTH,                                       \
                icm_seldbg_fstatus_shortstr[field##_fstatus]);       \
    }                                                                    \
}

/* Print a Selection Decision Debug Info field if it has been set, else print
   the status of the field.
   The field is first mapped into a string describing its value, using a string
   array fieldstrarray. */
#define ICM_SELDBG_PRINTENTRY_STRCONV(file, field, fieldstrarray)            \
{                                                                        \
    if (field##_fstatus == ICM_SELDBG_FSTATUS_SET) {                     \
        fprintf((file), "%s,", fieldstrarray[field]);                    \
    } else {                                                             \
        fprintf((file), "%s,", icm_seldbg_fstatus_str[field##_fstatus]); \
    }                                                                    \
}

/* Similar to ICM_SELDBG_PRINTENTRY_STRCONV(), but print to console/standard
   output and using short versions of representative strings. */
#define ICM_SELDBG_PRINTENTRY_STRCONV_CONS(pdev, mod_id, field, fieldstrarray)\
{                                                                        \
    if (field##_fstatus == ICM_SELDBG_FSTATUS_SET) {                     \
        ICM_DPRINTF((pdev),                                              \
                ICM_PRCTRL_FLAG_NO_PREFIX,                           \
                ICM_DEBUG_LEVEL_DEFAULT,                             \
                (mod_id),                                            \
                "%-*s|",                                             \
                SHORTSTRWIDTH,                                       \
                fieldstrarray[field]);                               \
    } else {                                                             \
        ICM_DPRINTF((pdev),                                              \
                ICM_PRCTRL_FLAG_NO_PREFIX,                           \
                ICM_DEBUG_LEVEL_DEFAULT,                             \
                (mod_id),                                            \
                "%-*s|",                                             \
                SHORTSTRWIDTH,                                       \
                icm_seldbg_fstatus_shortstr[field##_fstatus]);       \
    }                                                                    \
}

/* Similar to ICM_SELDBG_PRINTENTRY_STRCONV_CONS(), but prints wider columns
 * for short info requiring more characters than usual. */
#define ICM_SELDBG_PRINTENTRY_STRCONV_CONS_WIDE(pdev, mod_id, field,         \
        fieldstrarray)               \
{                                                                        \
    if (field##_fstatus == ICM_SELDBG_FSTATUS_SET) {                     \
        ICM_DPRINTF((pdev),                                              \
                ICM_PRCTRL_FLAG_NO_PREFIX,                           \
                ICM_DEBUG_LEVEL_DEFAULT,                             \
                (mod_id),                                            \
                "%-*s|",                                             \
                SHORTSTRWIDTH_WIDE,                                  \
                fieldstrarray[field]);                               \
    } else {                                                             \
        ICM_DPRINTF((pdev),                                              \
                ICM_PRCTRL_FLAG_NO_PREFIX,                           \
                ICM_DEBUG_LEVEL_DEFAULT,                             \
                (mod_id),                                            \
                "%-*s|",                                             \
                SHORTSTRWIDTH_WIDE,                                  \
                icm_seldbg_fstatus_shortstr[field##_fstatus]);       \
    }                                                                    \
}


/* End of debug information generated by the selector algorithm. */

#define VHT_CHANNEL_WIDTH_REVSIG_160_MHZ            (1)
#define VHT_CHANNEL_WIDTH_REVSIG_80_80_MHZ          (1)

#ifdef ICM_RTR_DRIVER
/*
 * Check if revised signalling is being used for VHT160 in vhtop
 */
#define ICM_IS_REVSIG_VHT160(vhtop) (((vhtop)->vht_op_chwidth == VHT_CHANNEL_WIDTH_REVSIG_160_MHZ) && \
        ((vhtop)->vht_op_ch_freq_seg2 != 0) && \
        (abs((vhtop)->vht_op_ch_freq_seg2 - (vhtop)->vht_op_ch_freq_seg1) == 8))   \
/*
 * Check if revised signalling is being used for VHT80p80 in vhtop
 */
#define ICM_IS_REVSIG_VHT80_80(vhtop) (((vhtop)->vht_op_chwidth == VHT_CHANNEL_WIDTH_REVSIG_80_80_MHZ)  && \
        ((vhtop)->vht_op_ch_freq_seg2 != 0) && \
        (abs((vhtop)->vht_op_ch_freq_seg2 - (vhtop)->vht_op_ch_freq_seg1) > 8))

#else /* ICM_RTR_DRIVER */
/* TODO: check if this is part of standard and make it available for both RTR/MBL */
/*
 * Check if revised signalling is being used for VHT160 in vhtop
 */
#define ICM_IS_REVSIG_VHT160(vhtop) (((vhtop)->vht_op_info_chwidth == VHT_CHANNEL_WIDTH_REVSIG_160_MHZ) && \
        ((vhtop)->vht_op_info_chan_center_freq_seg1_idx != 0) && \
        (abs((vhtop)->vht_op_info_chan_center_freq_seg1_idx - (vhtop)->vht_op_info_chan_center_freq_seg0_idx) == 8))   \
/*
 * Check if revised signalling is being used for VHT80p80 in vhtop
 */
#define ICM_IS_REVSIG_VHT80_80(vhtop) (((vhtop)->vht_op_info_chwidth == VHT_CHANNEL_WIDTH_REVSIG_80_80_MHZ)  && \
        ((vhtop)->vht_op_info_chan_center_freq_seg1_idx != 0) && \
        (abs((vhtop)->vht_op_info_chan_center_freq_seg1_idx - (vhtop)->vht_op_info_chan_center_freq_seg0_idx) > 8))

#endif /* ICM_RTR_DRIVER */

/*
 * Check if revised signalling is being used for HE160 in HE VHTOP
 */
#define ICM_IS_REVSIG_HE160(hevhtop)                                           \
            (((hevhtop)->vht_op_info_chwidth ==                                \
                    VHT_CHANNEL_WIDTH_REVSIG_160_MHZ) &&                       \
             ((hevhtop)->vht_op_info_chan_center_freq_seg1_idx != 0) &&        \
             (abs((hevhtop)->vht_op_info_chan_center_freq_seg1_idx -           \
                    (hevhtop)->vht_op_info_chan_center_freq_seg0_idx) == 8))
/*
 * Check if revised signalling is being used for HE80p80 in HE VHTOP
 */
#define ICM_IS_REVSIG_HE80_80(hevhtop)                                         \
            (((hevhtop)->vht_op_info_chwidth ==                                \
                    VHT_CHANNEL_WIDTH_REVSIG_80_80_MHZ)  &&                    \
             ((hevhtop)->vht_op_info_chan_center_freq_seg1_idx != 0) &&        \
             (abs((hevhtop)->vht_op_info_chan_center_freq_seg1_idx -           \
                    (hevhtop)->vht_op_info_chan_center_freq_seg0_idx) > 8))


/* If 11BE feature not enabled, define these macros locally as 0 */
#ifndef WLAN_FEATURE_11BE
#define IEEE80211_IS_CHAN_11BEA_EHT40PLUS(_c)     (0)
#define IEEE80211_IS_CHAN_11BEA_EHT40MINUS(_c)    (0)
#endif

/*
 * For PHYs which allow a bandwidth greater than or equal to 80 MHz, check
 * whether the given IEEE channel number _bondspan_chan_num is the secondary 20
 * MHz of a given ICM_CHANNEL_T instance _ch
 */
#define ICM_IS_GRTEQ80BW_CHAN_RELATION_SEC20(_picm,                            \
        _ch, _bondspan_chan_num)                                               \
      ((((((_picm)->phy_spec == ICM_PHY_SPEC_11AC) &&                          \
              IEEE80211_IS_CHAN_11AC_VHT40PLUS((_ch))) ||                      \
         (((_picm)->phy_spec == ICM_PHY_SPEC_11AXA) &&                         \
              IEEE80211_IS_CHAN_11AXA_HE40PLUS((_ch))) ||                      \
         (((_picm)->phy_spec == ICM_PHY_SPEC_11BEA) &&                         \
              IEEE80211_IS_CHAN_11BEA_EHT40PLUS((_ch)))) &&                    \
        ((_bondspan_chan_num) == ((_ch)->channel + 4))) ||                     \
       (((((_picm)->phy_spec == ICM_PHY_SPEC_11AC) &&                          \
              IEEE80211_IS_CHAN_11AC_VHT40MINUS((_ch))) ||                     \
         (((_picm)->phy_spec == ICM_PHY_SPEC_11AXA) &&                         \
              IEEE80211_IS_CHAN_11AXA_HE40MINUS((_ch))) ||                     \
         (((_picm)->phy_spec == ICM_PHY_SPEC_11BEA) &&                         \
              IEEE80211_IS_CHAN_11BEA_EHT40MINUS((_ch)))) &&                   \
        ((_bondspan_chan_num) == ((_ch)->channel - 4))))

/*
 * For PHYs which allow contiguous 160 MHz operation, check whether the given
 * IEEE channel number _bondspan_chan_num is the secondary 40 MHz of a given
 * ICM_CHANNEL_T instance _ch
 * XXX - Transition this to revised 160 MHz signalling.
 */
#define ICM_IS_160BW_CHAN_RELATION_SEC40(_picm,                                \
        _ch, _bondspan_chan_num)                                               \
      (((((_ch)->channel < (_ch)->ic_ch_freq_seg1_160) &&                      \
         ((_bondspan_chan_num) < (_ch)->ic_ch_freq_seg1_160)) ||               \
        (((_ch)->channel > (_ch)->ic_ch_freq_seg1_160) &&                      \
         ((_bondspan_chan_num) > (_ch)->ic_ch_freq_seg1_160))) &&              \
       !ICM_IS_GRTEQ80BW_CHAN_RELATION_SEC20((_picm), (_ch),                   \
          (_bondspan_chan_num)))


#define PATH_SYSCLASS_NET    "/sys/class/net"

/* End of attribution: Code taken from QCA apstats */

#define INVALID_DEVICE_ID   (-1)

#define IS_DEV_ACTIVE(pdev, devid)  (((pdev)->dev_flags & (1 << devid))?1:0)

#define SET_DEV_ACTIVE(pdev, devid) ((pdev)->dev_flags |= (1 << devid))
#define CLR_DEV_ACTIVE(pdev, devid) ((pdev)->dev_flags &= ~(1 << devid))

#define MAX_RAW_SPECT_DATA_SZ   (150)
#define NUM_RAW_DATA_TO_CAP     (1000)
#define MAX_PAYLOAD             (1024)

#ifdef ICM_RTR_DRIVER
#define DEFAULT_CONFIG_FILE     "/etc/ath/icm.conf"
#else
#define DEFAULT_CONFIG_FILE     "/vendor/etc/wifi/icm.conf"
#endif /* ICM_RTR_DRIVER */

#define DEFAULT_DEV_IFNAME      "none"
#define DEFAULT_RADIO_IFNAME    "none"

#define GET_NETLINK_SOCK_FD(_picm)   ((_picm)->sock_fd)
#define GET_MSG_HDR(_picm)           ((_picm)->nlh)
#define GET_ATH_DIAG_PTR(_picm)      (&(_picm)->atd))
#define GET_DEV_IFNAME(_picm)        ((_picm)->dev_ifname)
#define GET_RADIO_IFNAME(_picm)      ((_picm)->radio_ifname)

#define SIZEOF_IFNAME               (IFNAMSIZ + 1)


#define ICM_GET_OPERATING_BAND(_picm)            ((_picm)->band)

#define ICM_GET_2_4GHZ_CHANNEL_LIST_PTR(_picm) \
    (&((_picm)->pbchinfo[ICM_BAND_2_4G].chlist))
#define ICM_GET_5GHZ_CHANNEL_LIST_PTR(_picm) \
    (&((_picm)->pbchinfo[ICM_BAND_5G].chlist))
#define ICM_GET_6GHZ_CHANNEL_LIST_PTR(_picm) \
    (&((_picm)->pbchinfo[ICM_BAND_6G].chlist))
#define ICM_GET_BAND_CHANNEL_LIST_PTR(_picm, _band) \
    (&((_picm)->pbchinfo[(_band)].chlist))

#define ICM_GET_NUM_OF_CHANNELS_IN_2_4GHZ_BAND(_picm) \
    ((_picm)->pbchinfo[ICM_BAND_2_4G].chlist.count)
#define ICM_GET_NUM_OF_CHANNELS_IN_5GHZ_BAND(_picm) \
    ((_picm)->pbchinfo[ICM_BAND_5G].chlist.count)
#define ICM_GET_NUM_OF_CHANNELS_IN_6GHZ_BAND(_picm) \
    ((_picm)->pbchinfo[ICM_BAND_6G].chlist.count)
#define ICM_GET_NUM_OF_CHANNELS_IN_BAND(_picm, _band) \
    ((_picm)->pbchinfo[(_band)].chlist.count)

#define ICM_SET_CHANNEL_PUNCTURE_BITMAP(_picm, _band, _chan, _val) \
    ((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].puncture_bitmap = \
    (_val))
#define ICM_GET_CHANNEL_PUNCTURE_BITMAP(_picm, _band, _chan) \
    ((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].puncture_bitmap)

#define ICM_IS_2_4GHZ_CANDIDATE_BAND(_picm) \
    ((_picm)->pbchinfo[ICM_BAND_2_4G].is_candidate_band)
#define ICM_SET_2_4GHZ_AS_CANDIDATE_BAND(_picm) \
    (((_picm)->pbchinfo[ICM_BAND_2_4G].is_candidate_band) = true)
#define ICM_CLEAR_2_4GHZ_AS_CANDIDATE_BAND(_picm) \
    (((_picm)->pbchinfo[ICM_BAND_2_4G].is_candidate_band) = false)

#define ICM_IS_5GHZ_CANDIDATE_BAND(_picm) \
    ((_picm)->pbchinfo[ICM_BAND_5G].is_candidate_band)
#define ICM_SET_5GHZ_AS_CANDIDATE_BAND(_picm) \
    (((_picm)->pbchinfo[ICM_BAND_5G].is_candidate_band) = true)
#define ICM_CLEAR_5GHZ_AS_CANDIDATE_BAND(_picm) \
    (((_picm)->pbchinfo[ICM_BAND_5G].is_candidate_band) = false)

#define ICM_IS_6GHZ_CANDIDATE_BAND(_picm) \
    ((_picm)->pbchinfo[ICM_BAND_6G].is_candidate_band)
#define ICM_SET_6GHZ_AS_CANDIDATE_BAND(_picm) \
    (((_picm)->pbchinfo[ICM_BAND_6G].is_candidate_band) = true)
#define ICM_CLEAR_6GHZ_AS_CANDIDATE_BAND(_picm) \
    (((_picm)->pbchinfo[ICM_BAND_6G].is_candidate_band) = false)

#define ICM_IS_CANDIDATE_BAND(_picm, _band) \
    ((_picm)->pbchinfo[(_band)].is_candidate_band)
#define ICM_SET_AS_CANDIDATE_BAND(_picm, _band) \
    (((_picm)->pbchinfo[(_band)].is_candidate_band) = true)
#define ICM_CLEAR_AS_CANDIDATE_BAND(_picm, _band) \
    (((_picm)->pbchinfo[(_band)].is_candidate_band) = false)

#define ICM_GET_NUM_CANDIDATE_BANDS(_picm) \
    icm_get_num_candidate_bands((_picm))

#define ICM_GET_SINGLE_CANDIDATE_BAND(_picm) \
    icm_get_single_candidate_band((_picm))

#define ICM_SET_SINGLE_CANDIDATE_BAND(_picm, _band) \
    icm_set_single_candidate_band((_picm), (_band))

#define ICM_GET_SCAN_LIST_PTR(_picm)     (&(_picm)->slist)
#define ICM_GET_SCAN_ELEM_CHANNEL(_picm, _idx) \
    ((_picm)->slist.elem[(_idx)].channel)
#define ICM_GET_SCAN_ELEM_SSID(_picm, _idx) \
    ((_picm)->slist.elem[(_idx)].ssid)
#define ICM_GET_SCAN_ELEM_SSID_LEN(_picm, _idx) \
    ((_picm)->slist.elem[(_idx)].ssid_len)
#define ICM_GET_SCAN_ELEM(_picm, _idx) \
    (&(_picm)->slist.elem[(_idx)])
#define IS_SCAN_ELEM_VALID(_picm, _idx) \
    ((_picm)->slist.elem[(_idx)].valid ? 1:0)

#define ICM_GET_CHANNEL_PROPERTIES_PTR(_picm, _band, _chan) \
    (&((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)]))

#define ICM_GET_CHANNEL_CYCLE_COUNT(_picm, _band, _chan) \
    ((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].cycle_count)
#define ICM_SET_CHANNEL_CYCLE_COUNT(_picm, _band, _chan, _val) \
    (((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].cycle_count) = \
        (_val))

#define ICM_GET_CHANNEL_FREE_TIME(_picm, _band, _chan) \
    ((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].channel_free_time)
#define ICM_SET_CHANNEL_FREE_TIME(_picm, _band, _chan, _val) \
    (((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].channel_free_time) = \
        (_val))

#define ICM_GET_CHANNEL_PER(_picm, _band, _chan) \
    ((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].per)
#define ICM_SET_CHANNEL_PER(_picm, _band, _chan, _val) \
    (((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].per) = (_val))

#define ICM_GET_CHANNEL_NOISEFLOOR(_picm, _band, _chan) \
    ((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].noisefloor)
#define ICM_SET_CHANNEL_NOISEFLOOR(_picm, _band, _chan, _val) \
    (((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].noisefloor) = \
        (_val))

#define ICM_GET_CHANNEL_OCAC_STATUS(_picm, _band, _chan) \
    ((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].channel_ocac_status)
#define ICM_SET_CHANNEL_OCAC_STATUS(_picm, _band, _chan, _val)  \
    (((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].channel_ocac_status) \
        = (_val))

#define ICM_GET_CHANNEL_TX_POWER_TPUT(_picm, _band, _chan) \
    ((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].channel_tx_power_tput)
#define ICM_SET_CHANNEL_TX_POWER_TPUT(_picm, _band, _chan, _val)  \
    (((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].channel_tx_power_tput)\
        = (_val))

#define ICM_GET_CHANNEL_TX_POWER_RANGE(_picm, _band, _chan) \
    ((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].channel_tx_power_range)
#define ICM_SET_CHANNEL_TX_POWER_RANGE(_picm, _band, _chan, _val) \
    (((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].channel_tx_power_range) \
        = (_val))

#define ICM_GET_PRI20CHANNEL_GRADE(_picm, _band, _chan) \
    ((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].pri20channel_grade)
#define ICM_SET_PRI20CHANNEL_GRADE(_picm, _band, _chan, _val) \
    (((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].pri20channel_grade) = \
        (_val))

#define ICM_GET_CHANNEL_USABLITY(_picm, _band, _chan) \
    ((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].comp_usablity)
#define ICM_GET_CHANNEL_USABLITY_SEC80(_picm, _band, _chan) \
    ((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].comp_usablity_sec80)
#define ICM_SET_CHANNEL_USABLITY(_picm, _band, _chan, _val) \
    (((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].comp_usablity) = \
        (_val))
#define ICM_SET_CHANNEL_USABLITY_SEC80(_picm, _band, _chan, _val) \
    (((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].comp_usablity_sec80) = \
        (_val))

#define ICM_GET_CHANNEL_MAX_REGPWR(_picm, _band, _chan) \
    ((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].maxregpower)
#define ICM_GET_CHANNEL_MAX_REGPWR_SEC80(_picm, _band, _chan) \
    ((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].maxregpower_sec80)
#define ICM_SET_CHANNEL_MAX_REGPWR(_picm, _band,_chan, _val) \
    (((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].maxregpower) = \
        (_val))
#define ICM_SET_CHANNEL_MAX_REGPWR_SEC80(_picm, _band, _chan, _val) \
    (((_picm)->pbchinfo[(_band)].proplist.chprop[(_chan)].maxregpower_sec80) = \
        (_val))

#define ICM_GET_CHANNEL_EXCLUDE(_picm, _band, _chan) \
    ((_picm)->pbchinfo[(_band)].pnlzlist.chpnlz[(_chan)].exclude)
#define ICM_SET_CHANNEL_EXCLUDE(_picm, _band, _chan) \
    (((_picm)->pbchinfo[(_band)].pnlzlist.chpnlz[(_chan)].exclude) = \
        TRUE)
#define ICM_CLEAR_CHANNEL_EXCLUDE(_picm, _band, _chan) \
    (((_picm)->pbchinfo[(_band)].pnlzlist.chpnlz[(_chan)].exclude) = \
        FALSE)

#define ICM_GET_CHANNEL_PWR_MODE_EXCLUDE(_picm, _band, _chan, _pwr_type) \
    ((_picm)->pbchinfo[(_band)].pnlzlist.chpnlz[(_chan)].pwr_mode_exclude[_pwr_type])
#define ICM_SET_CHANNEL_PWR_MODE_EXCLUDE(_picm, _band, _chan, _pwr_type) \
    (((_picm)->pbchinfo[(_band)].pnlzlist.chpnlz[(_chan)].pwr_mode_exclude[_pwr_type]) = \
        TRUE)
#define ICM_CLEAR_CHANNEL_PWR_MODE_EXCLUDE(_picm, _band, _chan, _pwr_type) \
    (((_picm)->pbchinfo[(_band)].pnlzlist.chpnlz[(_chan)].pwr_mode_exclude[_pwr_type]) = \
        FALSE)

#define ICM_GET_OPRI20CHANNEL_EXCLUDE(_picm, _band, _chan) \
    ((_picm)->pbchinfo[(_band)].pnlzlist.chpnlz[(_chan)].opri20_exclude)
#define ICM_SET_OPRI20CHANNEL_EXCLUDE(_picm, _band, _chan) \
    (((_picm)->pbchinfo[(_band)].pnlzlist.chpnlz[(_chan)].opri20_exclude) = \
        TRUE)
#define ICM_CLEAR_OPRI20CHANNEL_EXCLUDE(_picm, _band, _chan) \
    (((_picm)->pbchinfo[(_band)].pnlzlist.chpnlz[(_chan)].opri20_exclude) = \
        FALSE)

#define ICM_GET_CHANNEL_BLUSABILITY(_picm, _band, _chan) \
    ((_picm)->pbchinfo[(_band)].pnlzlist.chpnlz[(_chan)].baseline_usability)
#define ICM_SET_CHANNEL_BLUSABILITY(_picm, _band, _chan, _val) \
    (((_picm)->pbchinfo[(_band)].pnlzlist.chpnlz[(_chan)].baseline_usability) = \
        (_val))

#define ICM_IS_PUNCTURING_SUPPORTED_FOR_CHAN_WIDTH(_chanbw) \
    ((_chanbw) == ICM_CH_BW_80 || \
     (_chanbw) == ICM_CH_BW_160 || \
     (_chanbw) == ICM_CH_BW_320)

ICM_INFO_T* get_picm(int devid);
int icm_init_nl80211_socket(ICM_DEV_INFO_T* pdev);
int icm_init_netlink_socket(ICM_DEV_INFO_T* pdev);
int icm_init_rtnetlink_socket(ICM_DEV_INFO_T* pdev);
int icm_init_ioctl_socket(ICM_DEV_INFO_T* pdev);
int icm_cleanup_socket(ICM_DEV_INFO_T* pdev);
int icm_vap_init_with_cap_info(ICM_DEV_INFO_T* pdev, ICM_CAPABILITY_INFO_T *cap_info, int dev_index);
#ifdef WLAN_SPECTRAL_ENABLE
int icm_init_spectral(ICM_INFO_T* picm);
int icm_get_raw_spectral_data(ICM_INFO_T* picm);
int icm_is_spectral_enab(ICM_INFO_T* picm);
int icm_is_spectral_active(ICM_INFO_T* picm);
int start_spectral_scan(ICM_INFO_T* picm);
int stop_spectral_scan(ICM_INFO_T* picm);
int icm_get_spectral_threholds(ICM_INFO_T* picm);
int icm_set_spectral(ICM_INFO_T* picm, int type, int  value);
int icm_do_spectral_scan(ICM_INFO_T* picm, ICM_BAND_T band,
        bool restore_channel);
int icm_start_spectral_scan(ICM_INFO_T* picm);
int icm_init_spectral_nlsock_operation(ICM_DEV_INFO_T *pdev);
int icm_deinit_spectral_nlsock_operation(ICM_DEV_INFO_T *pdev);
int icm_test_spectral_enab(ICM_INFO_T *picm);
int icm_recv_spectral_data(ICM_INFO_T* picm);
int icm_process_spectral_msg(ICM_INFO_T* picm, struct spectral_samp_msg* msg);
int icm_stop_spectral_scan(ICM_INFO_T* picm);
int icm_start_spectral_scan(ICM_INFO_T* picm);
int icm_clear_spectral_chan_properties(ICM_INFO_T* picm);
int icm_trim_spectral_scan_ch_list(ICM_INFO_T* picm);
int icm_handle_spectral_data(ICM_DEV_INFO_T* pdev, ICM_INFO_T *picm);
int icm_get_spectral_params(ICM_INFO_T *picm, struct spectral_config *sp);
int icm_set_spectral_params(ICM_INFO_T *picm, struct spectral_config *sp);
int icm_configure_spectral_params(ICM_INFO_T *picm);
int icm_restore_spectral_params(ICM_INFO_T *picm);
#endif /* WLAN_SPECTRAL_ENABLE */
int icm_do_test(ICM_INFO_T* picm);
int icm_test_scan(ICM_INFO_T* picm);
int icm_do_80211_scan(ICM_INFO_T* picm);
int icm_cancel_80211_scan(ICM_INFO_T * picm);
int icm_get_supported_channels(ICM_INFO_T* picm);
int icm_do_test_channel(ICM_INFO_T* picm);
int icm_get_reg_domain(ICM_INFO_T* picm);
int icm_set_icm_active(ICM_INFO_T *picm, u_int32_t val);
int icm_disable_dcs(ICM_INFO_T* picm);
int icm_enable_dcs(ICM_INFO_T* picm);
int icm_get_channel_vendorsurvey_info(ICM_INFO_T* picm, ICM_BAND_T band);
int icm_display_chan_properties(ICM_INFO_T* picm, ICM_BAND_T band);
int icm_get_wireless_nw_in_channel(ICM_INFO_T* picm, int channel);
int icm_convert_mhz2channel(u_int32_t freq);
u_int32_t icm_convert_ieee2mhz(int chan, ICM_BAND_T band);
ICM_BAND_T icm_get_band_from_freq(u_int32_t freq);
int icm_get_num_candidate_bands(ICM_INFO_T* picm);
ICM_BAND_T icm_get_single_candidate_band(ICM_INFO_T* picm);
int icm_set_single_candidate_band(ICM_INFO_T* picm, ICM_BAND_T band);
int icm_selector_init(ICM_INFO_T* picm);
ICM_SCAN_SELECT_STATUS_T
icm_scan_and_select_channel(ICM_INFO_T* picm, bool setchannel);
ICM_SCAN_SELECT_STATUS_T icm_cfg_select_channel(ICM_INFO_T* picm, bool setchannel);
int icm_select_home_channel_perband_per_pwrmode(ICM_INFO_T *picm, ICM_BAND_T band,
        ICM_CH_BW_T desired_width, ICM_SELECTION_RESULTS_INFO_T *selresults);
int icm_select_home_channel_multiband(ICM_INFO_T* picm,
        ICM_CH_BW_T desired_width, ICM_SELECTION_RESULTS_INFO_T *selresults);
int icm_init_seldbg_dump(ICM_DEV_INFO_T* pdev);
void icm_deinit_seldbg_dump(ICM_DEV_INFO_T* pdev);
int icm_init_streaminet_socket(ICM_DEV_INFO_T* pdev);
int icm_init_dgraminet_socket(ICM_DEV_INFO_T* pdev);
bool icm_update_channel_ieee_info(ICM_INFO_T* picm, int freq, const struct ieee80211_ath_channel *c);
bool icm_append_160MHz_channel_ieee_info(ICM_INFO_T* picm, int freq,
        const struct ieee80211_ath_channel *c);
int icm_accept_new_connection(ICM_DEV_INFO_T* pdev);
int icm_handle_client_data(ICM_DEV_INFO_T* pdev, int fd);
size_t os_strlcpy(char *dest, const char *src, size_t siz);

void print_usage(void);
void icm_init_scan_params(ICM_INFO_T* picm);
void icm_display_channels(ICM_INFO_T* picm);
void alarm_handler(ICM_DEV_INFO_T *pdev);
int icm_switch_channel(ICM_INFO_T* picm, bool is_first);
void icm_init_channel_params(ICM_INFO_T* picm);
void init_bandinfo(struct ss *plwrband, struct ss *puprband, int print_enable);
void icm_display_scan_db(ICM_INFO_T* picm);
void icm_display_interference(int flags);
void icm_update_wnw_in_channel_list(ICM_INFO_T* picm, ICM_BAND_T band);
int icm_get_ieee_chaninfo(ICM_INFO_T* picm);
void icm_print_chaninfo(ICM_INFO_T* picm, ICM_BAND_T  band);
void icm_change_channel(ICM_INFO_T* picm, enum dcs_int_type int_type, u_int16_t dcs_enabled);
void icm_cleanup(ICM_DEV_INFO_T* pdev);
void icm_print_dev_info(ICM_DEV_INFO_T* pdev);

const char* icm_ether_sprintf(const uint8_t mac[6]);
ICM_DEV_INFO_T* get_pdev(void);

int     icm_get_paramrange(ICM_INFO_T *picm, struct iw_range *range);
double  icm_freq_to_float(const struct iw_freq *freq);
int     icm_freq_to_channel(double freq, const struct iw_range *range);
int     icm_init_event_parse_context(ICM_EVENT_PARSE_CONTEXT_T *ctx,
        char *evbuf,
        int evbuflen);
ICM_EV_PARSE_RESULT_T
icm_get_next_event(ICM_EVENT_PARSE_CONTEXT_T *ctx,
        struct iw_event *event,
        int we_ver);

int icm_parse_client_cmd(ICM_DEV_INFO_T* pdev, void *buf, int buflen);

int icm_get_currchan(ICM_INFO_T* picm);

int icm_init_scanner_thread(ICM_INFO_T *picm);
int icm_cleanup_scanner_thread(ICM_INFO_T *picm);
int icm_activate_scanner_thread(ICM_INFO_T *picm);

void icm_set_scanner_thread_active(ICM_INFO_T *picm, bool value);
bool icm_is_scanner_thread_active(ICM_INFO_T *picm);
void icm_set_80211scan_done(ICM_INFO_T *picm, bool value);
bool icm_is_80211scan_done(ICM_INFO_T *picm);
void icm_set_80211scan_cancel_requested(ICM_INFO_T *picm, bool value);
bool icm_is_80211scan_cancel_requested(ICM_INFO_T *picm);
void* icm_process_scan_job(void *arg);

int icm_init_chanset_thread(ICM_INFO_T *picm);
int icm_cleanup_chanset_thread(ICM_INFO_T *picm);
void icm_set_chanset_thread_active(ICM_INFO_T *picm, bool value);
bool icm_is_chanset_thread_active(ICM_INFO_T *picm);
int icm_activate_chanset_thread(ICM_INFO_T *picm);
void* icm_process_chanset_job(void *arg);

void icm_chanset_ll_add(ICM_INFO_T *picm,
        ICM_CHANSET_JOB_LL_T *list,
        ICM_CHANSET_JOB_T *chanset);

ICM_CHANSET_JOB_T* icm_chanset_ll_del(ICM_INFO_T *picm,
        ICM_CHANSET_JOB_LL_T *list);

int icm_get_iface_addr(ICM_DEV_INFO_T* pdev,
        const char* ifname,
        u_int8_t *ifaddr);

int icm_phy_spec_to_str(ICM_PHY_SPEC_T physpec,
        char* str,
        int strbufflen);

int icm_pwr_level_to_str(ICM_AFC_POWER_MODE_T power_mode,
                         char *str, int strbufflen);

int icm_ch_bw_to_str(ICM_CH_BW_T bw, char* str, int strbufflen);

int get_radio_priv_int_param(ICM_DEV_INFO_T* pdev,
        const char *ifname,
        int param,
        int32_t *val);

int set_radio_priv_int_param(ICM_DEV_INFO_T* pdev,
        const char *ifname,
        int param,
        int val);

int set_vap_priv_int_param(ICM_DEV_INFO_T* pdev,
        const char *ifname,
        int param,
        int val);

int get_vap_priv_int_param(ICM_DEV_INFO_T* pdev,
        const char *ifname,
        int param);
int is_11ac_offload(ICM_INFO_T* picm);
int is_emiwar80p80_enab(ICM_INFO_T* picm);
char* icm_compose_phymode_str(ICM_PHY_SPEC_T physpec,
        ICM_CH_BW_T width,
        char *phymode,
        int phymodelen);
bool icm_is_modulebitmap_valid(const char* bitmapstr);
bool icm_is_debuglevel_valid(const char* dgblevelstr);
bool icm_is_walflag_valid(const char* walflagstr);
bool icm_is_numericalbool_valid(const char* numericalboolstr);
bool icm_is_rep_txpower_policy_valid(const char* reptxpowerpolicystr);
#ifdef WLAN_SPECTRAL_ENABLE
enum ieee80211_cwm_width icm_get_channel_width(ICM_INFO_T* picm);
#endif /* WLAN_SPECTRAL_ENABLE */
int icm_init_event_mechanism(ICM_DEV_INFO_T* pdev);
void icm_deinit_event_mechanism(ICM_DEV_INFO_T* pdev);
int icm_register_event_recipient(ICM_DEV_INFO_T* pdev,
        ICM_EVENT_RECIPIENT_T recipient);
int icm_deregister_event_recipient(ICM_DEV_INFO_T* pdev,
        ICM_EVENT_RECIPIENT_T recipient);
int icm_send_event(ICM_DEV_INFO_T* pdev,
        ICM_EVENT_RECIPIENT_T recipient,
        ICM_EVENT_T event);

int icm_get_event(ICM_DEV_INFO_T* pdev,
        ICM_EVENT_RECIPIENT_T recipient,
        ICM_EVENT_T *pevent);

int icm_is_dev_ifname_present(ICM_INFO_T* picm,
        char *dev_ifname,
        bool *ispresent);

int icm_set_width_and_channel(ICM_INFO_T *picm, char *dev_ifname);

int icm_is_dev_ifname_valid(const char *ifname);
#ifdef SINGLE_WIPHY_ICM
int icm_is_mld_ifname_valid(const char *ifname);
int icm_is_link_ifname_valid(const char *ifname);
bool icm_is_mld_link_radiochild(const char *mldlinkname, const char *radioname);
int icm_get_picm_index(ICM_DEV_INFO_T* pdev, const char *radio_listing_ifname);
#endif /* SINGLE_WIPHY_ICM */
int icm_get_nominal_noisefloor(ICM_INFO_T *picm);
int icm_parse_netlink_msg(ICM_DEV_INFO_T* pdev);
int icm_parse_rtnetlink_msg(ICM_DEV_INFO_T* pdev);
int icm_configure_radio_iface(ICM_DEV_INFO_T* pdev);
int icm_init_radio_iface(ICM_INFO_T* picm, const char *radio_ifname);
int icm_get_channel_index(ICM_CHANNEL_LIST_T *pchlist, u_int32_t channel);
u16 icm_get_pcl_adjusted_usability(ICM_INFO_T* picm, int channel, u16 usability);
bool icm_is_chan_unii3(int channel, ICM_BAND_T band);
#ifdef WLAN_SPECTRAL_ENABLE
int icm_get_free_mem(size_t *free_mem_bytes);
#endif /* WLAN_SPECTRAL_ENABLE */

void icm_printf(const char *fmt, ...);
void icm_android_log_helper(const char *fmt, ...);

int icm_set_ext_acs_inprogress(ICM_DEV_INFO_T *pdev, ICM_INFO_T *picm);
int icm_clear_ext_acs_inprogress(ICM_DEV_INFO_T *pdev, ICM_INFO_T *picm);
int icm_clear_radio_ext_acs_request_in_progress(ICM_INFO_T *picm);

/* wireless abstraction layer API list */
#ifdef WLAN_SPECTRAL_ENABLE
int icm_wal_is_spectral_enab(ICM_INFO_T* picm);
int icm_wal_get_spectral_params(ICM_INFO_T *picm, struct spectral_config *sp);
int icm_wal_set_spectral_params(ICM_INFO_T *picm, struct spectral_config *sp);
int icm_wal_get_spectral_capabilities(ICM_INFO_T *picm, struct spectral_caps *scaps);
int icm_wal_start_spectral_scan(ICM_INFO_T* picm);
int icm_wal_stop_spectral_scan(ICM_INFO_T* picm);
int icm_wal_set_spectral_debug(ICM_INFO_T* picm, int dbglevel);
int icm_wal_clear_spectral_chan_properties(ICM_INFO_T* picm);
#endif /* WLAN_SPECTRAL_ENABLE */
int icm_wal_get_channel_vendorsurvey_info(ICM_INFO_T* picm, ICM_BAND_T band);
int icm_wal_set_icm_active(ICM_INFO_T *picm, u_int32_t val);
int icm_wal_get_nominal_noisefloor(ICM_INFO_T *picm);

int icm_wal_do_80211_scan(ICM_INFO_T* picm);
int icm_wal_cancel_80211_scan(ICM_INFO_T * picm);
int icm_wal_set_channel(ICM_INFO_T* picm, struct nl80211_channel_config *chan_config);
int
icm_wal_do_80211_priv(ICM_INFO_T *picm, struct iwreq *iwr, const char *ifname, int op, void *data, size_t len);
int icm_wal_get_currdomain(ICM_INFO_T* picm);
void icm_wal_init_channel_params(ICM_INFO_T * picm);
int icm_wal_get_reg_domain(ICM_INFO_T* picm);
int icm_wal_get_currchan(ICM_INFO_T* picm);
int icm_wal_get_ieee_chaninfo(ICM_INFO_T* picm);
int icm_wal_get_paramrange(ICM_INFO_T *picm, struct iw_range *range);

int icm_wal_is_dev_up(ICM_DEV_INFO_T* pdev, char *dev_ifname, bool *isup);
int icm_wal_is_dev_ap(ICM_DEV_INFO_T* pdev, char *dev_ifname, bool *isap);

int icm_wal_get_radio_priv_int_param(ICM_DEV_INFO_T* pdev, const char *ifname,
        int param, int32_t *val);
int icm_wal_set_radio_priv_int_param(ICM_DEV_INFO_T* pdev, const char *ifname,
        int param, int val);
int icm_wal_get_vap_priv_int_param(ICM_DEV_INFO_T* pdev, const char *ifname, int param);
int icm_wal_set_vap_priv_int_param(ICM_DEV_INFO_T* pdev,
        const char *ifname,
        int param,
        int32_t val);

#ifdef ICM_RTR_DRIVER
int icm_wal_get_channel_status(ICM_INFO_T * picm);
#endif
int icm_wal_get_chan_rropinfo(ICM_INFO_T * picm);

int icm_wal_get_radio_pri20_blockchanlist(ICM_INFO_T * picm);
int icm_wal_get_radio_chan_rf_characterization_info(ICM_INFO_T *picm);
int icm_wal_clear_radio_ext_acs_request_in_progress(ICM_INFO_T *picm);

enum ieee80211_cwm_width icm_wal_get_channel_width(ICM_INFO_T* picm);
int icm_wal_set_width_and_channel(ICM_INFO_T *picm, char *dev_ifname);
const char * icm_band_to_string(ICM_BAND_T band);
const char * icm_chan_width_to_string(ICM_CH_BW_T chan_width);
const char * icm_phy_mode_to_string(ICM_PHY_SPEC_T phy_mode);
const char * icm_acs_policy_to_string(u32 policy);

#ifndef ICM_RTR_DRIVER
int convert_RTR_to_mbl_chan_width(ICM_CH_BW_T rtr_ch_width);
#endif /* ICM_RTR_DRIVER */

ICM_BAND_T icm_get_radio_band(int phymode);
int icm_get_radio_channel_width(int phymode);
enum ieee80211_phymode icm_get_phy_mode(ICM_PHY_SPEC_T phy_spec, ICM_CH_BW_T channel_width);
enum icm_phy_spec convert_phymode_to_icm_physpec(int phymode);
bool icm_is_vap_radiochild(const char *vapname, const char *radioname);
double icm_get_wpf(int rssi);
#ifdef ICM_RTR_DRIVER
enum ieee80211_cwm_width convert_to_RTR_driver_chan_width(ICM_CH_BW_T ch_width);
#endif /* ICM_RTR_DRIVER */
uint32_t convert_to_mhz_chan_width(ICM_CH_BW_T ch_width);
#endif  /* __ICM_H__ */
