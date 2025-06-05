#=============================================================================
# Copyright (c) 2022-2024 Qualcomm Technologies, Inc.
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

echo 4 > /proc/sys/kernel/printk

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

# Core control parameters for gold
echo 0 > /sys/devices/system/cpu/cpu0/core_ctl/enable

# Core control on gold+
echo 0 > /sys/devices/system/cpu/cpu2/core_ctl/enable

# Setting b.L scheduler parameters
echo 15 > /proc/sys/walt/sched_downmigrate
echo 20 > /proc/sys/walt/sched_upmigrate
echo 25 > /proc/sys/walt/sched_group_downmigrate
echo 30 > /proc/sys/walt/sched_group_upmigrate
echo 2 > /proc/sys/walt/sched_window_stats_policy
echo 1 > /proc/sys/walt/sched_walt_rotate_big_tasks
echo 0 > /proc/sys/walt/sched_coloc_busy_hysteresis_enable_cpus
echo 255 > /proc/sys/walt/sched_util_busy_hysteresis_enable_cpus
echo 2000000 2000000 5000000 5000000 5000000 > /proc/sys/walt/sched_util_busy_hyst_cpu_ns
echo 15 15 15 15 15 > /proc/sys/walt/sched_util_busy_hyst_cpu_util

# cpuset parameters
echo 0-3 > /dev/cpuset/background/cpus
echo 0-3 > /dev/cpuset/system-background/cpus

# Turn off scheduler boost at the end
echo 0 > /proc/sys/walt/sched_boost

# Reset the RT boost, which is 1024 (max) by default.
echo 0 > /proc/sys/kernel/sched_util_clamp_min_rt_default

# configure governor settings for gold cluster
echo "walt" > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
echo 0 > /sys/devices/system/cpu/cpufreq/policy0/walt/down_rate_limit_us
echo 0 > /sys/devices/system/cpu/cpufreq/policy0/walt/up_rate_limit_us
echo 1228800 > /sys/devices/system/cpu/cpufreq/policy0/walt/hispeed_freq
echo 85 > /sys/devices/system/cpu/cpufreq/policy0/walt/hispeed_load
echo 652800 > /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq
echo 0 > /sys/devices/system/cpu/cpufreq/policy0/walt/pl
echo -6 > /sys/devices/system/cpu/cpufreq/policy0/walt/boost
echo 0 > /sys/devices/system/cpu/cpufreq/policy0/walt/rtg_boost_freq
#echo 0 > /sys/devices/system/cpu/cpufreq/policy0/walt/pl

# configure governor settings for gold+ cluster
echo "walt" > /sys/devices/system/cpu/cpufreq/policy2/scaling_governor
echo 0 > /sys/devices/system/cpu/cpufreq/policy2/walt/down_rate_limit_us
echo 0 > /sys/devices/system/cpu/cpufreq/policy2/walt/up_rate_limit_us
echo 1382400 > /sys/devices/system/cpu/cpufreq/policy2/walt/hispeed_freq
echo 85 > /sys/devices/system/cpu/cpufreq/policy2/walt/hispeed_load
echo 652800 > /sys/devices/system/cpu/cpufreq/policy2/scaling_min_freq
echo 0 > /sys/devices/system/cpu/cpufreq/policy2/walt/pl
echo -6 > /sys/devices/system/cpu/cpufreq/policy2/walt/boost
echo 0 > /sys/devices/system/cpu/cpufreq/policy2/walt/rtg_boost_freq
#echo 0 > /sys/devices/system/cpu/cpufreq/policy2/walt/pl

# configure input boost settings
echo 0 0 1094400 0 0 > /proc/sys/walt/input_boost/input_boost_freq
echo 120 > /proc/sys/walt/input_boost/input_boost_ms

#set s2idle as default suspend mode
echo s2idle > /sys/power/mem_sleep

# Enable LPM
echo N > /sys/devices/system/cpu/qcom_lpm/parameters/sleep_disabled

# colocation V3 settings
# TODO

# Enable conservative pl
#echo 1 > /proc/sys/walt/sched_conservative_pl

# configure bus-dcvs
bus_dcvs="/sys/devices/system/cpu/bus_dcvs"

for device in $bus_dcvs/*
do
	cat $device/hw_min_freq > $device/boost_freq
done

for llccbw in $bus_dcvs/LLCC/*bwmon-llcc
do
	echo "4577 7110 9155 12298 14236 15258" > $llccbw/mbps_zones
	echo 4 > $llccbw/sample_ms
	echo 80 > $llccbw/io_percent
	echo 20 > $llccbw/hist_memory
	echo 80 > $llccbw/down_thres
	echo 0 > $llccbw/guard_band_mbps
	echo 250 > $llccbw/up_scale
	echo 1600 > $llccbw/idle_mbps
	echo 40 > $llccbw/window_ms
done

for ddrbw in $bus_dcvs/DDR/*bwmon-ddr
do
	echo "2086 2929 5931 6516 7980 10437 12157 14060 16113" > $ddrbw/mbps_zones
	echo 4 > $ddrbw/sample_ms
	echo 80 > $ddrbw/io_percent
	echo 20 > $ddrbw/hist_memory
	echo 80 > $ddrbw/down_thres
	echo 0 > $ddrbw/guard_band_mbps
	echo 250 > $ddrbw/up_scale
	echo 1600 > $ddrbw/idle_mbps
	echo 40 > $ddrbw/window_ms
done

for l3gold in $bus_dcvs/L3/*gold
do
	echo 6000 > $l3gold/ipm_ceil
done

for l3prime in $bus_dcvs/L3/*prime
do
	echo 6000 > $l3prime/ipm_ceil
done

for qosprime in $bus_dcvs/DDRQOS/*prime-latfloor
do
	echo 400 > $qosprime/ipm_ceil
done

# for qosgold in $bus_dcvs/DDRQOS/*gold
# do
# echo 50 > $qosgold/ipm_ceil
# done

setprop vendor.post_boot.parsed 1

