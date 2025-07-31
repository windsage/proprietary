/*
 * Copyright (C) 2025 Transsion Holdings
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __LIBPERFCORE_CLOUD_H__
#define __LIBPERFCORE_CLOUD_H__

#define PERF_CONFIG_STORE_FILE "perfconfigstore.xml"
#define PERF_MAPPING_FILE "perfmapping.xml"
#define PEFF_BOOSTS_CONFIG_FILE "perfboostsconfig.xml"
#define PERF_HINT_FILE "powerhint.xml"
#define PERF_COMMON_RESOURCE_CONFIGS_FILE "commonresourceconfigs.xml"
#define PERF_TARGET_RESOURCE_CONFIGS_FILE "targetresourceconfigs.xml"

#define DATA_VENDOR_PERF_CONFIG_STORE_FILE "/data/vendor/perf/perfconfigstore.xml"
#define DATA_VENDOR_PERF_MAPPING_FILE "/data/vendor/perf/perfmapping.xml"
#define DATA_VENDOR_PEFF_BOOSTS_CONFIG_FILE "/data/vendor/perf/perfboostsconfig.xml"
#define DATA_VENDOR_PERF_HINT_FILE "/data/vendor/perf/powerhint.xml"
#define DATA_VENDOR_PERF_COMMON_RESOURCE_CONFIGS_FILE "/data/vendor/perf/commonresourceconfigs.xml"
#define DATA_VENDOR_PERF_TARGET_RESOURCE_CONFIGS_FILE "/data/vendor/perf/targetresourceconfigs.xml"

#define SIZE_1_K_BYTES 1024
#define TRAN_PERF_CLOUD_PROP "ro.vendor.perf.cloud"
#define TRAN_PERF_ENCODE_PROP "ro.vendor.perf.encode"

extern void RegistCloudctlListener();
extern void UnregistCloudctlListener();
#endif
