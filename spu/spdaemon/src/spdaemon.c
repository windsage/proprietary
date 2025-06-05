/*=============================================================================
Copyright (c) 2016-2022,2024 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/**
 * @file spdaemon.c
 * @brief - Secure Processor Daemon (spdaemon)
 *
 * This driver is responsible for loading SP Applications that
 * doesn't have an owner HLOS Application.
 */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include <stdlib.h>    // malloc()
#include <stdio.h>     // fopen()
#include <fcntl.h>     // O_RDONLY
#include <unistd.h>    // sleep() / usleep()
#include <errno.h>     // ENODEV
#include <memory.h>
#include <pthread.h>
#include <time.h>

#include <log/log.h>        // SLOGE()

#include <spcomlib.h>
#include <sp_uim_remote.h>
#include "mdm_detect.h"
#include "pm-service.h"
#include <cutils/properties.h> // property_get()
#include <hardware_legacy/power.h>

#include <linux/spss_utils.h>   // SPSS_IOC_SET_SSR_STATE

#include "sp_iar_lib.h"

// protected API
extern int spcom_wait_for_nvm_ready(uint32_t timeout_sec);
extern int spcom_signal_spu_ready(void);
extern int spcom_signal_pil_called(void);
extern void spcom_reset_all_app_loaded_prop(void);
extern int spcom_ioctl(unsigned long cmd_id, void *cmd);

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#ifdef PRINT_LOG_TO_STDOUT
    #define LOGD(fmt, x...) printf("spdaemon: dbg: %s: " fmt "\n", __func__,##x)
    #define LOGI(fmt, x...) printf("spdaemon: dbg: %s: " fmt "\n", __func__,##x)
    #define LOGE(fmt, x...) printf("spdaemon: err: %s: " fmt "\n", __func__,##x)
#else // print to system log a.k.a logcat
    #undef LOG_TAG
    #undef LOGD
    #undef LOGI
    #undef LOGE
    #define LOG_TAG "spdaemon"
    #define LOGD SLOGD
    #define LOGI SLOGI
    #define LOGE SLOGE
#endif

#define SZ_1K (1024)
#define BUILD_MAJOR(build_version) (build_version >> 16)
#define BUILD_MINOR(build_version) (build_version & 0xFFFF)

#ifndef OFFSET_OF
    #define OFFSET_OF(field, base) ((int)((char*)(field) - (char*)(base)))
#endif

#define FILE_PATH_SIZE 256

#define SP_APP_LOAD_FLAG_NONE           0x00000000
#define SP_APP_LOAD_FLAG_OPTIONAL       0x00000001
#define SP_APP_LOAD_FLAG_SILENT_LOAD    0x00000002
#define SP_APP_LOAD_FLAG_UEFI_MANDATORY 0x00000004 // must be loaded during UEFI stage
#define SP_APP_LOAD_FLAG_UEFI_OPTIONAL  0x00000008 // can be loaded during UEFI stage or by spdaemon

#define WAIT_FOR_NVM_READY_TIMEOUT_SEC  30
#if (SPSS_TARGET != sxr2330p)
#define SPSS_WAKE_LOCK_NAME "spss-boot"   // Wakelock interface is not up in matrix presil,enable it in BU
#endif

#define UNDEFINED_VER_VALUE 0xFFFFFFFF
#define UPDATE_LOAD_STATUS_TIMEOUT_MS 100
#define UPDATE_LOAD_STATUS_DELAY_MS 30
#define IAR_DB_PERMISSIONS  (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

#define STRERROR_ERR_BUF_SIZE 80

/*-------------------------------------------------------------------------
 * Structures and enums
 * ----------------------------------------------------------------------*/

typedef struct _sp_app_info_t {
    const char*     ch_name;
    const char*     base_name; // FAT 8.3 chars format
    int             swap_size;
    uint32_t        id;
    uint32_t        flags;
} sp_app_info_t;

typedef enum _spss_firmware_t {
    SPSS_FW_TYPE_TEST = 't',
    SPSS_FW_TYPE_PROD = 'p',
} spss_firmware_t;


// Note: The request and response structure format should be packed

/*-------------------------------------------------------------------------
 * Global Variables
 * ----------------------------------------------------------------------*/

static spss_firmware_t g_firmware_type = SPSS_FW_TYPE_TEST;
static bool g_is_boot_sequence = false;
static const char *spunvm_iar_db_path  = "/mnt/vendor/spunvm/iar_db";

// SPSS Apps
static sp_app_info_t g_apps_to_load[] = {
    {
        .ch_name = "asym_cryptoapp",
        .base_name = "asym",
        .swap_size = 256 * SZ_1K, // Unused
        .id = 0x3D86A887,
        .flags =
            SP_APP_LOAD_FLAG_UEFI_MANDATORY, // App loaded on UEFI
    },

    {
        .ch_name = "cryptoapp",
        .base_name = "crypt",
        .swap_size = 256 * SZ_1K, // Unused
        .id = 0xEF80345E,
        .flags =
            SP_APP_LOAD_FLAG_UEFI_MANDATORY, // App loaded on UEFI
    },

    {
        .ch_name = "sp_keymaster",
        .base_name = "keym",
        .swap_size = 256 * SZ_1K, // Unused
        .id = 0x24500E2F,
        .flags =
            SP_APP_LOAD_FLAG_UEFI_MANDATORY, // App loaded on UEFI
    },

	{
        .ch_name = "sp_nvm",
        .base_name = "nvm",
        .swap_size = 256 * SZ_1K, // Unused
        .id = 0x697B1837,
        .flags =
            SP_APP_LOAD_FLAG_UEFI_MANDATORY, // App loaded on UEFI
    },

    {
        .ch_name = "macchiato",
        .base_name = "macch",
        .swap_size = 256 * SZ_1K, // Unused
        .id = 0x09068E40,
        .flags = // App loaded by spdaemon, potentially can be loaded on UEFI
            SP_APP_LOAD_FLAG_UEFI_OPTIONAL,
    },
};

// The spss_utils kernel driver sysfs path depends on the linux kernel version
static const char* g_firmware_name_path = "./sys/devices/platform/soc/soc:qcom,spss_utils/firmware_name";

static const char* g_iar_db_path = "/mnt/vendor/persist/iar_db/";

static struct spcom_client* g_ssr_spcom_client = NULL;

static pthread_mutex_t g_pthread_mutex;
static pthread_cond_t g_pthread_cond;
static int g_soc_version = 0;
static int g_thread_resumed = 0;

/*-------------------------------------------------------------------------
 * Function Implementations
 * ----------------------------------------------------------------------*/

static inline void msleep(int msec)
{
    usleep(msec * 1000);
}

static void get_app_file_path(char* path, const char* base_name, spss_firmware_t fw_type, int soc_version)
{
    const char* base_path = "/vendor/firmware_mnt/image/";
    char type[2] = {0};

    type[0]= (char) fw_type;

    if (iar_is_iar_state_activated()) {
        strlcpy(path, g_iar_db_path, FILE_PATH_SIZE);
    } else {
        strlcpy(path, base_path, FILE_PATH_SIZE);
    }

    strlcat(path, base_name, FILE_PATH_SIZE);
    if (soc_version == 1)
        strlcat(path, "1", FILE_PATH_SIZE);
    else
        strlcat(path, "2", FILE_PATH_SIZE);

    strlcat(path, type, FILE_PATH_SIZE);
    strlcat(path, ".sig", FILE_PATH_SIZE);
}

static spss_firmware_t get_firmware_type(void)
{
    int fd;
    int ret;
    char name[10] = {}; // up to 8 chars , example "spss2t".
    const char* path = g_firmware_name_path;
    char soc_ver;
    char fw_type;

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        LOGD("open() file [%s] Failed, ret =%d.\n", path, fd);
        LOGD("Can't get soc version, assuming v1\n");
        g_soc_version = 1;
        return SPSS_FW_TYPE_TEST;
    }

    ret = read(fd, name, sizeof(name) - 1);
    if (ret < 0) {
        LOGE("read() file [%s] Failed, ret =%d.\n", path, ret);
    } else {
        char* p;

        //Zero the last byte of the array to make sure the string is valid
        name[sizeof(name) - 1] = 0;

        p = strstr(name, "\n");
        if (p != NULL)
            *p = 0; // remove new line char
        LOGD("Firmware Name [%s].\n", name);
        if (strstr(name, "spss") == 0) {
            LOGE("Not SPSS firmware name.\n");
            return SPSS_FW_TYPE_TEST;
        }

        soc_ver = name[4];
        fw_type = name[5];

        if (soc_ver == '1' || soc_ver == '2')
            g_soc_version = soc_ver - '0';

        if (fw_type == 't') {
            LOGD("Test FW.\n");
            return SPSS_FW_TYPE_TEST;
        }
        if (fw_type == 'p') {
            LOGD("Prod FW.\n");
            return SPSS_FW_TYPE_PROD;
        }
    }

    return SPSS_FW_TYPE_TEST;
}

static void suspend_me(void)
{
    pthread_mutex_lock(&g_pthread_mutex);

    // wait for a flag here
    // If resume_me() is called before suspend_me() we will release the wait immediately
    while (g_thread_resumed == 0) {
        pthread_cond_wait(&g_pthread_cond, &g_pthread_mutex);
    }

    //reset the flag here to prepare for the next ssr
    g_thread_resumed = 0;
    pthread_mutex_unlock(&g_pthread_mutex);
}

static void resume_me(void)
{
    pthread_mutex_lock(&g_pthread_mutex);
    g_thread_resumed = 1;
    pthread_cond_signal(&g_pthread_cond);
    pthread_mutex_unlock(&g_pthread_mutex);
}

// Read the app's version and hash-
// if successful, it means that the app is loaded - update system properties if needed and return 0
// if not, return error
static int update_load_status(sp_app_info_t* info, struct spcom_client** service_client) {
    if (*service_client == NULL) {
        // Since first loaded App is responsible for handling all the service
        // requests, initialize the client here and let upper layers to release
        // it after
        int ret = spcom_connect_service_client(service_client);
        if (ret) {
            LOGE("spcom_connect_service_client failed, retval = %d\n", ret);
            return ret;
        }
    }

    uint32_t build_version = UNDEFINED_VER_VALUE;
    uint32_t build_version_hash = UNDEFINED_VER_VALUE;

    int time_msec = 0;
    while (time_msec < UPDATE_LOAD_STATUS_TIMEOUT_MS) {
        if (!spcom_sp_sysparam_read_ext(SP_SYSPARAM_ID_APP_BUILD_VERSION, info->id, 0, &build_version, *service_client) &&
            !spcom_sp_sysparam_read_ext(SP_SYSPARAM_ID_APP_BUILD_VERSION_HASH, info->id, 0, &build_version_hash, *service_client)) {

            // app's version is not reported if SILENT_LOAD flag is enabled
            if (!(info->flags & SP_APP_LOAD_FLAG_SILENT_LOAD)) {
                LOGI("%s, v%d.%d.%08X\n",
                info->ch_name,
                (UNDEFINED_VER_VALUE == build_version) ? 0 : BUILD_MAJOR(build_version),
                (UNDEFINED_VER_VALUE == build_version) ? 0 : BUILD_MINOR(build_version),
                (UNDEFINED_VER_VALUE == build_version_hash) ? 0 : build_version_hash);
            }

            if (!spcom_is_app_loaded(info->ch_name)) {
                if (!spcom_set_app_load_status(info->ch_name)) {
                    LOGE("spcom_set_app_load_status failed\n");
                    return -EINVAL;
                }
            }

            return 0;
        } else {
            msleep(UPDATE_LOAD_STATUS_DELAY_MS);
            time_msec += UPDATE_LOAD_STATUS_DELAY_MS;
        }
    }

    return -EPERM;
}

static int load_app(sp_app_info_t* info, struct spcom_client** service_client)
{
    int ret = 0;
    bool is_loaded = false;
    int timeout_msec = 60 * 1000;
    int time_msec = 0;
    const char* ch_name = info->ch_name;
    char file_path[FILE_PATH_SIZE] = {};

    // check if the App is already loaded
    is_loaded = spcom_is_app_loaded(ch_name);
    if (!is_loaded) {

        // Check If the App was loaded during UEFI stage
        if (info->flags & SP_APP_LOAD_FLAG_UEFI_MANDATORY || info->flags & SP_APP_LOAD_FLAG_UEFI_OPTIONAL) {

            // If SP is started in UEFI stage, SP itself loads the UEFI apps(cryptoapp, asymm_crypto, keymaster).
            // Load status of the UEFI apps are retrieved from SP and system properties are updated.
            ret = update_load_status(info, service_client);
            if (!ret) {
                return 0; // the app is loaded and its load status is updated, no need to continue
            }

            // if update_load_status() failed and the app must be loaded during UEFI stage - don't continue and return error
            if (info->flags & SP_APP_LOAD_FLAG_UEFI_MANDATORY) {
                return -EPERM;
            }
        }

        LOGD("Load SP App [%s].\n", ch_name);
        get_app_file_path(file_path, info->base_name, g_firmware_type, g_soc_version);

        LOGD("Load [%s] from path [%s].\n", ch_name, file_path);

        // Load the app
        ret = spcom_load_app(ch_name,  file_path, info->swap_size);
        if (ret < 0) {
            LOGE("Loading SP App [%s] failed. ret [%d].\n", ch_name, ret);
            return ((info->flags & SP_APP_LOAD_FLAG_OPTIONAL)) ? 0 : ret;
        }

        while (!is_loaded) {
            is_loaded = spcom_is_app_loaded(ch_name);
            msleep(10);
            time_msec += 10;
            if (time_msec >= timeout_msec) {
                LOGE("Timeout wait for char dev creation.\n");
                return ((info->flags & SP_APP_LOAD_FLAG_OPTIONAL)) ? 0 : -ETIMEDOUT;
            }
        }

        LOGD("SP App [%s] is loaded successfully.\n", ch_name);
    }
    else {
        LOGD("SP App [%s] already loaded.\n", ch_name);
    }

    if ((info->flags & SP_APP_LOAD_FLAG_SILENT_LOAD)) {
        // App is loaded silently without reporting version
        return 0;
    }

    if (!g_is_boot_sequence) {
        // We got version on boot, no need to read it on SSR, wasting time.
        return 0;
    }

    return update_load_status(info, service_client);
}

/**
 * spcom_notify_ssr_cb() - a callback to notify on SP SubSystem-Reset (SSR).
 *
 * The spcom shall notify upon SP reset.
 * This callback should wait until the SP is up again (LINK is UP),
 * and then re-load the SP Applications and do any init sequence required.
 */
static void spcom_notify_ssr_cb(void)
{
    LOGD("SP subsystem reset detected.\n");

    resume_me();
}

static int register_ssr_callback(void)
{
    struct spcom_client *client = NULL;
    struct spcom_client_info reg_info = {};
    const char* ch_name = "spdaemon_ssr"; // use known node for sepolicy

    reg_info.ch_name = ch_name;
    reg_info.notify_ssr_cb = spcom_notify_ssr_cb;

    // register to spcom for sending request
    client = spcom_register_client(&reg_info);
    if (client == NULL) {
        LOGE("spcom register failed.\n");
        return -EFAULT;
    }

    // Note1: don't unregister spcom for SSR awareness

    LOGD("SSR callback registered ok.\n");
    g_ssr_spcom_client = client;

    return 0;
}

static int unregister_ssr_callback(void)
{
    spcom_unregister_client(g_ssr_spcom_client);
    g_ssr_spcom_client = NULL;

    return 0;
}

/**
 *  Wait until SP is up and running.
 */
static int wait_for_sp_link_up(int timeout_sec)
{
    bool sp_is_up = false;
    int timeout_msec = timeout_sec * 1000;
    int time_msec = 0;

    while (!sp_is_up) {
        sp_is_up = spcom_is_sp_subsystem_link_up();
        msleep(10);
        time_msec += 10;
        if (time_msec >= timeout_msec) {
            LOGE("Timeout wait for SP link UP.\n");
            return -ETIMEDOUT;
        }
    }

    LOGD("SP LINK is UP in [%d] msec.\n", time_msec);

    return 0;
}

/**
 * Load SP Apps
 */
static int load_sp_apps(sp_app_info_t apps[], size_t apps_num, struct spcom_client** service_client)
{
    int ret = 0;
    int link_up_timeout_sec = 60;

    // Wait for link up
    LOGD("Wait for sp link up.\n");
    ret = wait_for_sp_link_up(link_up_timeout_sec);
    if (ret < 0) {
        LOGE("load_sp_apps: wait_for_sp_link_up failed. ret [%d].\n", ret);
        return ret;
    }

    // Load Apps
    for (size_t index = 0; index < apps_num; index++) {
        ret = load_app(&apps[index], service_client);
        if (ret < 0) {
            LOGE("Loading SP %s App failed. ret [%d].\n", apps[index].ch_name, ret);
            break;
        }
    }

    if (!ret)
      LOGD("SP Apps were loaded successfully.\n");

    return ret;
}

/**
 *  re-load SP App after SP reset
 */
static int handle_sp_out_of_reset(void)
{
    int ret = 0;
    struct stat st = {0};
    char err_buf[STRERROR_ERR_BUF_SIZE] = {0};
    // The client will be initialized by the first loaded App
    struct spcom_client* service_client = NULL;
    const int link_up_timeout_sec = 60;

    //Do not reset the app properties during boot.
    if(!g_is_boot_sequence){
        spcom_reset_all_app_loaded_prop();
    }

    ret = spcom_wait_for_nvm_ready(WAIT_FOR_NVM_READY_TIMEOUT_SEC);
    if (ret < 0) {
        strerror_r(ret, err_buf, sizeof(err_buf));
        LOGE("spcom_wait_for_nvm_ready() failed. ret [%d] [%s].\n", ret, err_buf);
        return ret;
    }

    // Re-register SSR callback before loading Apps to handle SSR
    // events in case there is any issue with App loading
    if (g_ssr_spcom_client) {
        unregister_ssr_callback();
    }

    ret = register_ssr_callback();
    if (ret < 0) {
        LOGE("SSR callback registration failed. ret [%d].\n", ret);
        return ret;
    }
	
    ret = stat(spunvm_iar_db_path, &st);
    if (ret < 0) {
        /* Create iar_db dir if its missing. */
        if(errno == ENOENT) {
            LOGE("iar-db directory [%s] is missing, create it.", spunvm_iar_db_path);
            ret = mkdir(spunvm_iar_db_path, IAR_DB_PERMISSIONS);
            if (ret < 0) {
                int err_val = errno;
                strerror_r(err_val, err_buf, sizeof(err_buf));
                LOGE("Creating directory %s failed (%d), [%d] [%s]",
                    spunvm_iar_db_path, ret, err_val, err_buf);
                return -err_val;
            }
            LOGD("iar-db directory [%s] created OK.", spunvm_iar_db_path);
        }
        /* For every other errno, return from here.
           Because if IAR is activated and iar_db dir cannot be created,
           we would incorrectly flag the IAR state as disabled. */
        else {
            LOGE("iar-db directory could not be created. ret [%d]", ret);
            return ret;
        }
    }

    // IAR-Recovery MUST be after glink LINK-UP, to ask SPU about IAR-state via check-health.
    // Better before load_sp_apps() to detect Macchiato cmac fix needed.
    // Must recover iar-state before call to iar_check_spu_pbl_ce(), which will fix firmware and uefi apps cmac.
    if (iar_is_recovery_needed()) {
        LOGI("IAR recovery is needed");
        // fix iar-state to active and create dummy hash and cmac files.
        ret = iar_perform_recovery();
        if (ret != 0) {
            LOGE("perform_iar_recovery Failed. ret [%d].\n", ret);
        }
    }

    ret = load_sp_apps(g_apps_to_load,
            sizeof(g_apps_to_load)/sizeof(g_apps_to_load[0]), &service_client);
    if (ret < 0) {
        LOGE("Loading SP Apps failed. ret [%d].\n", ret);
    }

    // If IAR activated and IAR PBL CE occurred -
    // save new SPU fw and UEFI apps cmac
    // read calc cmacs only after apploader loaded UEFI apps
    // if not loading any app - check apploader is ready
    if (iar_is_iar_state_activated()) {
        iar_check_spu_pbl_ce();
    }

    // All Apps loaded - Get SP Build version and Health parameters
    // read it once on boot only, don't waste time on SSR
    if (service_client && g_is_boot_sequence) {
        int retval = 0;
        sp_health_status_data status_data;
        uint32_t build_version = UNDEFINED_VER_VALUE;
        uint32_t build_version_hash = UNDEFINED_VER_VALUE;
        uint32_t hw_version = UNDEFINED_VER_VALUE;

        if (!spcom_sp_sysparam_read_ext(SP_SYSPARAM_ID_BUILD_VERSION, 0, 0, &build_version, service_client) &&
                !spcom_sp_sysparam_read_ext(SP_SYSPARAM_ID_BUILD_VERSION_HASH, 0, 0, &build_version_hash, service_client)) {

            retval = spcom_sp_sysparam_read_ext(SP_SYSPARAM_ID_HW_VERSION, 0, 0, &hw_version, service_client);
            if (retval < 0) {
                LOGE("Failed to read HW Version. [%d]\n", retval);
            }

            LOGI("SP Build v%d.%d.%08X, HW v%08X\n",
                    (UNDEFINED_VER_VALUE == build_version) ? 0 : BUILD_MAJOR(build_version),
                    (UNDEFINED_VER_VALUE == build_version) ? 0 : BUILD_MINOR(build_version),
                    (UNDEFINED_VER_VALUE == build_version_hash) ? 0 : build_version_hash,
                    hw_version);
        }
        else {
            LOGE("Failed to read SP Build Version.\n");
        }

        retval = spcom_check_sp_health_ext(SP_HEALTH_ARI_LIFECYCLE_STATE, &status_data, service_client);
        if (!retval) {
            LOGI("Sensors Calibration: %s, ARI lifecycle: %d, ARI Gauge: %d%%",
                    (status_data.sensors_calibrated)?"Ok":"None",
                    status_data.ari_lifecycle_state,
                    status_data.ari_fuse_gauge);
            if (status_data.ari_lifecycle_state == ARI_LIFECYCLE_TEST_MODE ||
                      status_data.ari_lifecycle_state == ARI_LIFECYCLE_OPERATIONAL) {
                      LOGI("POR indicator: %d, POR counter %d, normal_boot_count_since_por %d, commit_count_since_por %d",
                                 status_data.por_indicator,
                                 status_data.por_data.por_counter,
                                 status_data.por_data.normal_boot_count_since_por,
                                 status_data.por_data.commit_count_since_por);
            }
            LOGI("NVM result: %08X\n", status_data.nvm_result);
        }
        else {
            LOGE("Failed to read SP Health Info.[%d]\n", retval);
        }

        uint32_t fips_override = 0;

        retval = spcom_sp_sysparam_read_ext(SP_SYSPARAM_ID_FIPS_OVERRIDE, 0, 0, &fips_override, service_client);
        if (!retval) {
            LOGI("FIPS: %s, FIPS override: %s",
                    (status_data.fips_enabled)?"on":"off",
                    (fips_override)?"on":"off");
        }
        else {
            LOGE("Failed to read FIPS Config.[%d]\n", retval);
        }
    }

    if (service_client) {
        if (spcom_disconnect_service_client(service_client)) {
            LOGD("Failed to disconnect service client.\n");
        }
    }

    spcom_signal_spu_ready();

    return ret;
}

static bool wait_for_spu_kernel_drivers(void)
{
    int timeout_msec = 10 * 1000;
    int time_msec = 0;
    char *file_path = "/dev/spcom";
    struct stat st; /* file status */
    int fd = 0; /* file descriptor */

    // 1. Wait for /dev/spcom
    LOGD("Start wait for [%s] device node.\n", file_path);
    while (1) {
        if (stat(file_path, &st) == 0) {
                if (S_ISCHR(st.st_mode))
                    break; /* exit while loop */
        }
        msleep(10);
        time_msec += 10;
        if (time_msec >= timeout_msec) {
                LOGE("file [%s] NOT exist!\n", file_path);
                return false;
        }
    }
    LOGD("file [%s] exist!\n", file_path);

    while (1) {
        int err = 0;

        fd = open(file_path, O_RDWR);
        if (fd >= 0)
            break; /* exit while loop */
        err = errno; /* save errno before msleep() syscall */
        msleep(10);
        time_msec += 10;
        if (time_msec >= timeout_msec) {
            LOGE("Failed to open file [%s] ret [%d] errno [%d].\n", file_path, fd, (int) err);
            return false;
        }
    }
    LOGD("file [%s] is ready in [%d] msec.\n", file_path, time_msec);
    close(fd);

    // 2. Wait for /dev/sp_kernel - is used for loading spu apps from hlos
    file_path = "/dev/sp_kernel";
    while (1) {
        int err = 0;

        fd = open(file_path, O_RDWR);
        if (fd >= 0)
            break; /* exit while loop */
        err = errno; /* save errno before msleep() syscall */
        msleep(10);
        time_msec += 10;
        if (time_msec >= timeout_msec) {
            LOGE("Failed to open file [%s] ret [%d] errno [%d].\n", file_path, fd, (int) err);
            return false;
        }
    }
    LOGD("file [%s] is ready in [%d] msec.\n", file_path, time_msec);
    close(fd);

    return true;
}

static uint32_t get_rmb_error()
{
  struct spcom_rmb_error_info rmb_info = {0};

  int ret = spcom_ioctl(SPCOM_GET_RMB_ERROR, (void *)&rmb_info);
  if (ret < 0)
      LOGE("can't get rmb_error, ret [%d]\n", ret);

  return rmb_info.rmb_error;
}

int main(void)
{
    int ret = -1;
    bool is_fw_update = false;
    char bootmode[PROPERTY_VALUE_MAX];

#if (SPSS_TARGET != sxr2330p)
    // This property is not set in XR targets. Make changes when its enabled
    // In FFBM mode - don't load spss
    property_get("ro.bootmode", bootmode, NULL);
    if (!strncmp(bootmode, "ffbm", 4)) {
        LOGD("FFBM mode, SPSS will not be loaded\n");
        goto load_app_error;
    }
#endif
    g_is_boot_sequence = true;

#if (SPSS_TARGET != sxr2330p)
    // Prevent system suspend flow up to spcom_signal_spu_ready() called
    acquire_wake_lock(PARTIAL_WAKE_LOCK, SPSS_WAKE_LOCK_NAME);
#endif
    if (!wait_for_spu_kernel_drivers()) {
        goto load_app_error;
    }

    g_firmware_type = get_firmware_type();
    LOGD("firmware_type [%c].\n", (int) g_firmware_type);

    if (iar_is_iar_state_activated()) {
        LOGI("IAR is activated\n");

        ret = iar_maintain_database_sanity();
        if (ret < 0) {
            LOGE("Error while trying to maintain IAR database sanity, ret [%d]. Continue anyway\n", ret);
        }

        ret = iar_load_spu_firmware_cmac();
        if (0 != ret) {
            LOGE("Loading SPU FW CMAC failed, ret [%d]. Continue anyway\n", ret);
        }

        ret = iar_check_if_fw_update(&is_fw_update);
        if (ret < 0) {
            LOGE("Fail to check if spu images was updated %d\n", ret);
        }

        LOGD("Firmware/UEFI apps update indication: %d\n", (is_fw_update ? 1 : 0));

    } else {
        LOGI("IAR is not activated\n");
    }

    LOGD("Going to start SP From kernel.\n");

    // SPSS  started already in UEFI stage.
    // Remoteproc subsystem enables SPSS.
    ret = spcom_restart_sp_subsystem(true /*is_uefi*/, is_fw_update);

    LOGD("spcom supports PBL error handling. ret [%d].\n", ret);
    if (ret == -ENODEV) {
        // Unrecoverable PBL error or state (TERMINATED, etc)
        // Request rmb_error value to understand what is the reason
        // it is fatal error SPSS is not supported
        // depending on specific PBL error could be applied different
        // handling logic: retry or other.
        // Currently all PBL errors considered fatal and no recovery
        // attempt done. The daemon go to sleep forever.
        LOGE("Fatal SPSS error (after UEFI), rmb_error = [0x%x], ret [%d]\n", get_rmb_error(), ret);
        goto rproc_error;
    } else if (ret == -ETIMEDOUT) {
      LOGE("SPSS start request (after UEFI) timeout, retry\n");
      ret = spcom_restart_sp_subsystem(true /*is_uefi*/, is_fw_update);
      if (ret < 0) {
        LOGE("SPSS 2nd start request failed, ret [%d].\n", ret);
        goto rproc_error;
      }
    }

    // notify pil was called
    spcom_signal_pil_called();

    if (iar_is_iar_state_activated()) {
        #ifdef SPSS_IOC_SET_SSR_STATE
        ret = spcom_set_ssr_state(is_fw_update);
        if (ret < 0) {
            LOGE("setting ssr_disable_flag failed, ret [%d].\n", ret);
            goto load_app_error;
        }
        #endif
    }

    ret = handle_sp_out_of_reset();
    if (ret < 0) {
        LOGE("Initial SP initialization failed. ret [%d].\n", ret);
        goto load_app_error;
    }

    /* Create a thread for handling MPSS<->SPSS<->APSS communication */
    sp_uim_remote_create_thread();

    g_is_boot_sequence = false;

    while(1) {
        // go to sleep , nothing to do now , wake up upon SP reset event
        LOGD("Go to sleep , nothing to do now , wake up upon SP reset event.\n");
        pthread_cond_init(&g_pthread_cond, NULL);
        pthread_mutex_init(&g_pthread_mutex, NULL);

#if (SPSS_TARGET != sxr2330p)
	release_wake_lock(SPSS_WAKE_LOCK_NAME);
#endif
        suspend_me();
#if (SPSS_TARGET != sxr2330p)
	acquire_wake_lock(PARTIAL_WAKE_LOCK, SPSS_WAKE_LOCK_NAME);
#endif

        ret = handle_sp_out_of_reset();
        if (ret < 0) {
            LOGE("SSR SP initialization failed. ret [%d].\n", ret);
        }
    }

    return 0;

rproc_error:
load_app_error:
#if (SPSS_TARGET != sxr2330p)
    release_wake_lock(SPSS_WAKE_LOCK_NAME);
#endif
    // Avoid exit, since init process will start the daemon again
    while(1) {
        LOGD("going to sleep for a day.\n");
        sleep(60*60*24); // sleep 60 sec x 60 min x 24 hours = 1 day
    }

    return -ENODEV; // never happens
}
