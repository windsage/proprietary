#=============================================================================
# Copyright (c) 2020-2023 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# Copyright (c) 2009-2012, 2014-2019, The Linux Foundation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of The Linux Foundation nor
#       the names of its contributors may be used to endorse or promote
#       products derived from this software without specific prior written
#       permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================

rev=`cat /sys/devices/soc0/revision`

if [ -d /proc/sys/walt ]; then
	echo 4 > /sys/devices/system/cpu/cpu0/core_ctl/min_cpus
	echo 60 > /sys/devices/system/cpu/cpu0/core_ctl/busy_up_thres
	echo 40 > /sys/devices/system/cpu/cpu0/core_ctl/busy_down_thres
	echo 100 > /sys/devices/system/cpu/cpu0/core_ctl/offline_delay_ms
	echo 0 0 0 0 1 1 > /sys/devices/system/cpu/cpu0/core_ctl/not_preferred

	echo 1 > /sys/devices/system/cpu/cpu0/core_ctl/enable
	echo 0 > /sys/devices/system/cpu/cpu6/core_ctl/enable

	echo 0 > /proc/sys/walt/sched_sbt_delay_windows
	echo 0 > /proc/sys/walt/sched_sbt_pause_cpus

	echo 65 > /proc/sys/walt/sched_downmigrate
	echo 71 > /proc/sys/walt/sched_upmigrate
	echo 85 > /proc/sys/walt/sched_group_downmigrate
	echo 100 > /proc/sys/walt/sched_group_upmigrate
	echo 1 > /proc/sys/walt/sched_walt_rotate_big_tasks
	echo 400000000 > /proc/sys/walt/sched_coloc_downmigrate_ns
	echo 16000000 16000000 16000000 16000000 16000000 16000000 16000000 16000000 > /proc/sys/walt/sched_coloc_busy_hyst_cpu_ns
	echo 0 > /proc/sys/walt/sched_coloc_busy_hysteresis_enable_cpus
	echo 10 10 10 10 10 10 10 10 > /proc/sys/walt/sched_coloc_busy_hyst_cpu_busy_pct
	echo 8500000 8500000 8500000 8500000 5000000 5000000 5000000 5000000 > /proc/sys/walt/sched_util_busy_hyst_cpu_ns
	echo 255 > /proc/sys/walt/sched_util_busy_hysteresis_enable_cpus
	echo 1 1 1 1 15 15 15 15 > /proc/sys/walt/sched_util_busy_hyst_cpu_util
	echo 40 > /proc/sys/walt/sched_cluster_util_thres_pct
	echo 30 > /proc/sys/walt/sched_idle_enough
	echo 10 > /proc/sys/walt/sched_ed_boost

	# set the threshold for low latency task boost feature which prioritize
	# binder activity tasks
	echo 325 > /proc/sys/walt/walt_low_latency_task_threshold

	# Turn off scheduler boost at the end
	echo 0 > /proc/sys/walt/sched_boost

	# configure input boost settings
	echo 1113600 0 0 0 0 0 0 0 > /proc/sys/walt/input_boost/input_boost_freq
	echo 120 > /proc/sys/walt/input_boost/input_boost_ms

	echo "walt" > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
	echo "walt" > /sys/devices/system/cpu/cpufreq/policy6/scaling_governor

	echo 0 > /sys/devices/system/cpu/cpufreq/policy0/walt/down_rate_limit_us
	echo 0 > /sys/devices/system/cpu/cpufreq/policy0/walt/up_rate_limit_us
	echo 0 > /sys/devices/system/cpu/cpufreq/policy6/walt/down_rate_limit_us
	echo 0 > /sys/devices/system/cpu/cpufreq/policy6/walt/up_rate_limit_us

	echo 0 > /sys/devices/system/cpu/cpufreq/policy0/walt/pl
	echo 0 > /sys/devices/system/cpu/cpufreq/policy6/walt/pl

	echo 1113600 > /sys/devices/system/cpu/cpufreq/policy0/walt/hispeed_freq
	echo 1228800 > /sys/devices/system/cpu/cpufreq/policy6/walt/hispeed_freq
else
	echo "schedutil" > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
	echo "schedutil" > /sys/devices/system/cpu/cpufreq/policy6/scaling_governor
	echo 1 > /proc/sys/kernel/sched_pelt_multiplier
fi

echo 576000 > /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq
echo 691200 > /sys/devices/system/cpu/cpufreq/policy6/scaling_min_freq

# Reset the RT boost, which is 1024 (max) by default.
echo 0 > /proc/sys/kernel/sched_util_clamp_min_rt_default

# cpuset parameters
echo 0-5 > /dev/cpuset/background/cpus
echo 0-5 > /dev/cpuset/system-background/cpus

# configure bus-dcvs
bus_dcvs="/sys/devices/system/cpu/bus_dcvs"
for device in $bus_dcvs/*
do
	if [ -e $device/boost_freq ]; then
		cat $device/hw_min_freq > $device/boost_freq
	fi
done

for ddrbw in $bus_dcvs/DDR/*bwmon-ddr
do
	echo "1720 2086 2597 2929 3879 5161 5931 6881 7980" > $ddrbw/mbps_zones
	echo 4 > $ddrbw/sample_ms
	echo 68 > $ddrbw/io_percent
	echo 20 > $ddrbw/hist_memory
	echo 0 > $ddrbw/hyst_length
	echo 1 > $ddrbw/idle_length
	echo 80 > $ddrbw/down_thres
	echo 0 > $ddrbw/guard_band_mbps
	echo 250 > $ddrbw/up_scale
	echo 1600 > $ddrbw/idle_mbps
	echo 1804000 > $ddrbw/max_freq
	echo 40 > $ddrbw/window_ms
done

for l3gold in $bus_dcvs/L3/*gold
do
	echo 4000 > $l3gold/ipm_ceil
done

echo s2idle > /sys/power/mem_sleep

if [ -e /sys/devices/system/cpu/qcom_lpm/parameters/sleep_disabled ]; then
	echo N > /sys/devices/system/cpu/qcom_lpm/parameters/sleep_disabled
fi

# Change console log level as per console config property
console_config=`getprop persist.vendor.console.silent.config`
case "$console_config" in
	"1")
		echo "Enable console config to $console_config"
		echo 0 > /proc/sys/kernel/printk
	;;
	*)
		echo "Enable console config to $console_config"
	;;
esac

# set rq_affinity to 2 on ufs devices
for sd in /sys/block/sd*/queue/rq_affinity
do
	echo 2 > $sd
done

setprop vendor.post_boot.parsed 1
