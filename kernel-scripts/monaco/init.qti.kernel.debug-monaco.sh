#=============================================================================
# Copyright (c) 2020-2022 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
# Copyright (c) 2014-2017, The Linux Foundation. All rights reserved.
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

enable_tracing_events()
{
    # timer
    echo 1 > /sys/kernel/tracing/events/timer/timer_expire_entry/enable
    echo 1 > /sys/kernel/tracing/events/timer/timer_expire_exit/enable
    echo 1 > /sys/kernel/tracing/events/timer/hrtimer_cancel/enable
    echo 1 > /sys/kernel/tracing/events/timer/hrtimer_expire_entry/enable
    echo 1 > /sys/kernel/tracing/events/timer/hrtimer_expire_exit/enable
    echo 1 > /sys/kernel/tracing/events/timer/hrtimer_init/enable
    echo 1 > /sys/kernel/tracing/events/timer/hrtimer_start/enable
    #enble FTRACE for softirq events
    echo 1 > /sys/kernel/tracing/events/irq/enable
    #enble FTRACE for Workqueue events
    echo 1 > /sys/kernel/tracing/events/workqueue/enable
    echo 1 > /sys/kernel/tracing/events/workqueue/workqueue_execute_start/enable
    # schedular
    echo 1 > /sys/kernel/tracing/events/sched/sched_cpu_hotplug/enable
    echo 1 > /sys/kernel/tracing/events/sched/sched_migrate_task/enable
    echo 1 > /sys/kernel/tracing/events/sched/sched_pi_setprio/enable
    echo 1 > /sys/kernel/tracing/events/sched/sched_switch/enable
    echo 1 > /sys/kernel/tracing/events/sched/sched_wakeup/enable
    echo 1 > /sys/kernel/tracing/events/sched/sched_wakeup_new/enable
    # sound
    echo 1 > /sys/kernel/tracing/events/asoc/snd_soc_reg_read/enable
    echo 1 > /sys/kernel/tracing/events/asoc/snd_soc_reg_write/enable
    # mdp
    echo 1 > /sys/kernel/tracing/events/mdss/mdp_video_underrun_done/enable
    # video
    echo 1 > /sys/kernel/tracing/events/msm_vidc/enable
    # clock
    echo 1 > /sys/kernel/tracing/events/power/clock_set_rate/enable
    echo 1 > /sys/kernel/tracing/events/power/clock_enable/enable
    echo 1 > /sys/kernel/tracing/events/power/clock_disable/enable
    echo 1 > /sys/kernel/tracing/events/power/cpu_frequency/enable
    # regulator
    echo 1 > /sys/kernel/tracing/events/regulator/enable
    # power
    echo 1 > /sys/kernel/tracing/events/msm_low_power/enable
    # fastrpc
    echo 1 > /sys/kernel/tracing/events/fastrpc/enable

    echo 1 > /sys/kernel/tracing/tracing_on
}

# function to disable SF tracing on perf config
sf_tracing_disablement()
{
    # disable SF tracing if its perf config
    if [ ! -d /sys/module/msm_rtb ]
    then
        setprop debug.sf.enable_transaction_tracing 0
    fi
}

# function to enable ftrace events
enable_ftrace_event_tracing()
{
    # bail out if its perf config
    if [ ! -d /sys/module/msm_rtb ]
    then
        return
    fi

    # bail out if ftrace events aren't present
    if [ ! -d /sys/kernel/tracing/events ]
    then
        return
    fi

    enable_tracing_events
}

# function to enable ftrace event transfer to CoreSight STM
enable_stm_events()
{
    # bail out if its perf config
    if [ ! -d /sys/module/msm_rtb ]
    then
        return
    fi
    # bail out if coresight isn't present
    if [ ! -d /sys/bus/coresight ]
    then
        return
    fi
    # bail out if ftrace events aren't present
    if [ ! -d /sys/kernel/tracing/events ]
    then
        return
    fi

    echo 0 3 > /sys/bus/coresight/devices/coresight-cti0/channels/trigout_attach
    echo 0 1 > /sys/bus/coresight/devices/coresight-cti0/channels/trigout_attach
    echo 0 5 > /sys/bus/coresight/devices/coresight-cti6/channels/trigin_attach
    echo 1 > /sys/bus/coresight/devices/coresight-cti0/enable
    echo 1 > /sys/bus/coresight/devices/coresight-cti6/enable

    echo $etr_size > /sys/bus/coresight/devices/coresight-tmc-etr/buffer_size
    echo 1 > /sys/bus/coresight/devices/coresight-tmc-etr/$sinkenable
    echo coresight-stm > /sys/class/stm_source/ftrace/stm_source_link
    echo 1 > /sys/bus/coresight/devices/coresight-stm/$srcenable
    echo 1 > /sys/kernel/tracing/tracing_on
    echo 0 > /sys/bus/coresight/devices/coresight-stm/hwevent_enable
}

enable_stm_hw_events()
{
   #TODO: Add HW events
}

enable_memory_debug()
{
    # bail out if its perf config
    if [ ! -d /sys/module/msm_rtb ]
    then
        return
    fi

    echo 2 > /proc/sys/vm/panic_on_oom
}

config_dcc_power()
{
	#DCC_Power_Reg's
	echo 0xf112000 0x1 > $DCC_PATH/config
	echo 0xf11200c 0x1 > $DCC_PATH/config
	echo 0xf112c0c 0x1 > $DCC_PATH/config
	echo 0xf112c10 0x1 > $DCC_PATH/config
	echo 0xf112c20 0x1 > $DCC_PATH/config
	echo 0xf1b9000 0x1 > $DCC_PATH/config
	echo 0xf1b900c 0x1 > $DCC_PATH/config
	echo 0xf1b9c0c 0x1 > $DCC_PATH/config
	echo 0xf1b9c10 0x1 > $DCC_PATH/config
	echo 0xf1b9c18 0x1 > $DCC_PATH/config
	echo 0xf1a9000 0x1 > $DCC_PATH/config
	echo 0xf1a900c 0x1 > $DCC_PATH/config
	echo 0xf1a9c0c 0x1 > $DCC_PATH/config
	echo 0xf1a9c10 0x1 > $DCC_PATH/config
	echo 0xf1a9c20 0x1 > $DCC_PATH/config
	echo 0xf199000 0x1 > $DCC_PATH/config
	echo 0xf19900c 0x1 > $DCC_PATH/config
	echo 0xf199c0c 0x1 > $DCC_PATH/config
	echo 0xf199c10 0x1 > $DCC_PATH/config
	echo 0xf199c20 0x1 > $DCC_PATH/config
	echo 0xf189000 0x1 > $DCC_PATH/config
	echo 0xf18900c 0x1 > $DCC_PATH/config
	echo 0xf189c0c 0x1 > $DCC_PATH/config
	echo 0xf189c10 0x1 > $DCC_PATH/config
	echo 0xf189c20 0x1 > $DCC_PATH/config
	echo 0xf111014 0x1 > $DCC_PATH/config
	echo 0xf111018 0x1 > $DCC_PATH/config
	echo 0xf111218 0x1 > $DCC_PATH/config
	echo 0xf111234 0x1 > $DCC_PATH/config
	echo 0xf111264 0x1 > $DCC_PATH/config
	echo 0xf111290 0x1 > $DCC_PATH/config
	echo 0xF1B9C20 0x1 > $DCC_PATH/config
	echo 0xF112008 0x1 > $DCC_PATH/config
	echo 0xF111104 0x1 > $DCC_PATH/config
}

config_dcc_clock()
{
	#DCC_CLOCK_Reg's
    echo 0x1400000 0x1 > $DCC_PATH/config
    echo 0x1400004 0x1 > $DCC_PATH/config
    echo 0x1401000 0x1 > $DCC_PATH/config
    echo 0x1401004 0x1 > $DCC_PATH/config
    echo 0x1402000 0x1 > $DCC_PATH/config
    echo 0x1402004 0x1 > $DCC_PATH/config
    echo 0x1403000 0x1 > $DCC_PATH/config
    echo 0x1403004 0x1 > $DCC_PATH/config
    echo 0x1404000 0x1 > $DCC_PATH/config
    echo 0x1404004 0x1 > $DCC_PATH/config
    echo 0x1405000 0x1 > $DCC_PATH/config
    echo 0x1405004 0x1 > $DCC_PATH/config
    echo 0x1406000 0x1 > $DCC_PATH/config
    echo 0x1406004 0x1 > $DCC_PATH/config
    echo 0x1407000 0x1 > $DCC_PATH/config
    echo 0x1407004 0x1 > $DCC_PATH/config
    echo 0x1408000 0x1 > $DCC_PATH/config
    echo 0x1408004 0x1 > $DCC_PATH/config
    echo 0x1409000 0x1 > $DCC_PATH/config
    echo 0x1409004 0x1 > $DCC_PATH/config
    echo 0x140A000 0x1 > $DCC_PATH/config
    echo 0x140A004 0x1 > $DCC_PATH/config
    echo 0x440C000 0x1 > $DCC_PATH/config
    echo 0x440C004 0x1 > $DCC_PATH/config
    echo 0x141001C 0x1 > $DCC_PATH/config
    echo 0x14103EC 0x1 > $DCC_PATH/config
    echo 0x1414024 0x1 > $DCC_PATH/config
    echo 0x1415034 0x1 > $DCC_PATH/config
    echo 0x1416038 0x1 > $DCC_PATH/config
    echo 0x141F034 0x1 > $DCC_PATH/config
    echo 0x141F168 0x1 > $DCC_PATH/config
    echo 0x141F29C 0x1 > $DCC_PATH/config
    echo 0x141F3D0 0x1 > $DCC_PATH/config
    echo 0x141F504 0x1 > $DCC_PATH/config
    echo 0x141F638 0x1 > $DCC_PATH/config
    echo 0x141F76C 0x1 > $DCC_PATH/config
    echo 0x141F8A0 0x1 > $DCC_PATH/config
    echo 0x141F9D4 0x1 > $DCC_PATH/config
    echo 0x1427028 0x1 > $DCC_PATH/config
    echo 0x143203C 0x1 > $DCC_PATH/config
    echo 0x143C018 0x1 > $DCC_PATH/config
    echo 0x144603C 0x1 > $DCC_PATH/config
    echo 0x1446168 0x1 > $DCC_PATH/config
    echo 0x1449018 0x1 > $DCC_PATH/config
    echo 0x146B000 0x1 > $DCC_PATH/config
    echo 0x146B004 0x1 > $DCC_PATH/config
    echo 0x146B008 0x1 > $DCC_PATH/config
    echo 0x146B00C 0x1 > $DCC_PATH/config
    echo 0x146B010 0x1 > $DCC_PATH/config
    echo 0x146B014 0x1 > $DCC_PATH/config
    echo 0x146B018 0x1 > $DCC_PATH/config
    echo 0x146B01C 0x1 > $DCC_PATH/config
    echo 0x146B020 0x1 > $DCC_PATH/config
    echo 0x146B024 0x1 > $DCC_PATH/config
    echo 0x146B028 0x1 > $DCC_PATH/config
    echo 0x141C004 0x1 > $DCC_PATH/config
    echo 0x1429004 0x1 > $DCC_PATH/config
    echo 0x1414004 0x1 > $DCC_PATH/config
    echo 0x1414008 0x1 > $DCC_PATH/config
    echo 0x1483140 0x1 > $DCC_PATH/config
    echo 0x1415010 0x1 > $DCC_PATH/config
    echo 0x1416010 0x1 > $DCC_PATH/config
    echo 0x1447004 0x1 > $DCC_PATH/config
    echo 0x149E0C4 0x1 > $DCC_PATH/config
    echo 0x1457000 0x1 > $DCC_PATH/config
    echo 0x145A000 0x1 > $DCC_PATH/config
    echo 0x1469000 0x1 > $DCC_PATH/config
    echo 0x146C000 0x1 > $DCC_PATH/config
    echo 0x1475000 0x1 > $DCC_PATH/config
    echo 0x1477000 0x1 > $DCC_PATH/config
    echo 0x1479000 0x1 > $DCC_PATH/config
    echo 0x147A000 0x1 > $DCC_PATH/config
    echo 0x1482000 0x1 > $DCC_PATH/config
    echo 0x1495000 0x1 > $DCC_PATH/config
    echo 0x149E0C8 0x1 > $DCC_PATH/config
    echo 0x1457004 0x1 > $DCC_PATH/config
    echo 0x145700C 0x1 > $DCC_PATH/config
    echo 0x145A004 0x1 > $DCC_PATH/config
    echo 0x145A00C 0x1 > $DCC_PATH/config
    echo 0x1469004 0x1 > $DCC_PATH/config
    echo 0x146900C 0x1 > $DCC_PATH/config
    echo 0x146C004 0x1 > $DCC_PATH/config
    echo 0x146C00C 0x1 > $DCC_PATH/config
    echo 0x1475004 0x1 > $DCC_PATH/config
    echo 0x147500C 0x1 > $DCC_PATH/config
    echo 0x1477004 0x1 > $DCC_PATH/config
    echo 0x147700C 0x1 > $DCC_PATH/config
    echo 0x1479004 0x1 > $DCC_PATH/config
    echo 0x147900C 0x1 > $DCC_PATH/config
    echo 0x147A004 0x1 > $DCC_PATH/config
    echo 0x147A00C 0x1 > $DCC_PATH/config
    echo 0x1482004 0x1 > $DCC_PATH/config
    echo 0x148200C 0x1 > $DCC_PATH/config
    echo 0x1495004 0x1 > $DCC_PATH/config
    echo 0x149500C 0x1 > $DCC_PATH/config
    echo 0x1453000 0x1 > $DCC_PATH/config
    echo 0x1453004 0x1 > $DCC_PATH/config
    echo 0xF521700 0x1 > $DCC_PATH/config
    echo 0xF521920 0x1 > $DCC_PATH/config
    echo 0xF52102C 0x1 > $DCC_PATH/config
    echo 0xF521044 0x1 > $DCC_PATH/config
    echo 0xF52170C 0x1 > $DCC_PATH/config
    echo 0xF521710 0x1 > $DCC_PATH/config
    echo 0xF521714 0x1 > $DCC_PATH/config
    echo 0xF521784 0x1 > $DCC_PATH/config
    echo 0xF52176C 0x1 > $DCC_PATH/config
    echo 0xF522C18 0x1 > $DCC_PATH/config
    echo 0xF118000 0x1 > $DCC_PATH/config
    echo 0xF118004 0x1 > $DCC_PATH/config
    echo 0xF118008 0x1 > $DCC_PATH/config
    echo 0xF11802C 0x1 > $DCC_PATH/config
    echo 0xF118030 0x1 > $DCC_PATH/config
    echo 0xF118034 0x1 > $DCC_PATH/config
    echo 0xF118038 0x1 > $DCC_PATH/config
    echo 0xF11803C 0x1 > $DCC_PATH/config
    echo 0xF119004 0x1 > $DCC_PATH/config
    echo 0xF119008 0x1 > $DCC_PATH/config
    echo 0xF119040 0x1 > $DCC_PATH/config
    echo 0xF119044 0x1 > $DCC_PATH/config
    echo 0xF119300 0x1 > $DCC_PATH/config
    echo 0xF119304 0x1 > $DCC_PATH/config
    echo 0xF119308 0x1 > $DCC_PATH/config
    echo 0xF11930C 0x1 > $DCC_PATH/config
    echo 0xF119310 0x1 > $DCC_PATH/config
}

config_dcc_ddr()
{
	#DCC_DDR_Reg's
    echo 0x447D02C 0x1 > $DCC_PATH/config
    echo 0x447D030 0x1 > $DCC_PATH/config
    echo 0x447D034 0x1 > $DCC_PATH/config
    echo 0x447D038 0x1 > $DCC_PATH/config
    echo 0x447D040 0x1 > $DCC_PATH/config
    echo 0x1B60110 0x1 > $DCC_PATH/config
    echo 0x04480000 0x1 > $DCC_PATH/confi
    echo 0x4480040 0x1 > $DCC_PATH/config
    echo 0x4480810 0x1 > $DCC_PATH/config
    echo 0x04488000 0x1 > $DCC_PATH/confi
    echo 0x4488100 0x1 > $DCC_PATH/config
    echo 0x4488100 0x1 > $DCC_PATH/config
    echo 0x4488400 0x1 > $DCC_PATH/config
    echo 0x4488404 0x1 > $DCC_PATH/config
    echo 0x4488410 0x1 > $DCC_PATH/config
    echo 0x4488420 0x1 > $DCC_PATH/config
    echo 0x4488424 0x1 > $DCC_PATH/config
    echo 0x4488430 0x1 > $DCC_PATH/config
    echo 0x4488434 0x1 > $DCC_PATH/config
    echo 0x448C100 0x1 > $DCC_PATH/config
    echo 0x448C400 0x1 > $DCC_PATH/config
    echo 0x448C404 0x1 > $DCC_PATH/config
    echo 0x448C410 0x1 > $DCC_PATH/config
    echo 0x448C420 0x1 > $DCC_PATH/config
    echo 0x448C424 0x1 > $DCC_PATH/config
    echo 0x448C430 0x1 > $DCC_PATH/config
    echo 0x448C434 0x1 > $DCC_PATH/config
    echo 0x4490100 0x1 > $DCC_PATH/config
    echo 0x4490400 0x1 > $DCC_PATH/config
    echo 0x4490404 0x1 > $DCC_PATH/config
    echo 0x4490410 0x1 > $DCC_PATH/config
    echo 0x4490420 0x1 > $DCC_PATH/config
    echo 0x4490424 0x1 > $DCC_PATH/config
    echo 0x4490430 0x1 > $DCC_PATH/config
    echo 0x4490434 0x1 > $DCC_PATH/config
    echo 0x4494100 0x1 > $DCC_PATH/config
    echo 0x4494400 0x1 > $DCC_PATH/config
    echo 0x4494404 0x1 > $DCC_PATH/config
    echo 0x4494410 0x1 > $DCC_PATH/config
    echo 0x4494420 0x1 > $DCC_PATH/config
    echo 0x4494424 0x1 > $DCC_PATH/config
    echo 0x4494430 0x1 > $DCC_PATH/config
    echo 0x4494434 0x1 > $DCC_PATH/config
    echo 0x4498100 0x1 > $DCC_PATH/config
    echo 0x4498400 0x1 > $DCC_PATH/config
    echo 0x4498404 0x1 > $DCC_PATH/config
    echo 0x4498410 0x1 > $DCC_PATH/config
    echo 0x4498420 0x1 > $DCC_PATH/config
    echo 0x4498424 0x1 > $DCC_PATH/config
    echo 0x4498430 0x1 > $DCC_PATH/config
    echo 0x4498434 0x1 > $DCC_PATH/config
    echo 0x44A0100 0x1 > $DCC_PATH/config
    echo 0x44A0400 0x1 > $DCC_PATH/config
    echo 0x44A0404 0x1 > $DCC_PATH/config
    echo 0x44A0410 0x1 > $DCC_PATH/config
    echo 0x44A0420 0x1 > $DCC_PATH/config
    echo 0x44A0424 0x1 > $DCC_PATH/config
    echo 0x44A0430 0x1 > $DCC_PATH/config
    echo 0x44B0020 0x1 > $DCC_PATH/config
    echo 0x44B0100 0x1 > $DCC_PATH/config
    echo 0x44B0120 0x1 > $DCC_PATH/config
    echo 0x44B0124 0x1 > $DCC_PATH/config
    echo 0x44B0128 0x1 > $DCC_PATH/config
    echo 0x44B012C 0x1 > $DCC_PATH/config
    echo 0x44B0130 0x1 > $DCC_PATH/config
    echo 0x44B0520 0x1 > $DCC_PATH/config
    echo 0x44B0560 0x1 > $DCC_PATH/config
    echo 0x44B05A0 0x1 > $DCC_PATH/config
    echo 0x44B0A40 0x1 > $DCC_PATH/config
    echo 0x44B1800 0x1 > $DCC_PATH/config
    echo 0x44B408C 0x1 > $DCC_PATH/config
    echo 0x44B409C 0x1 > $DCC_PATH/config
    echo 0x44B5070 0x1 > $DCC_PATH/config
    echo 0x44B5074 0x1 > $DCC_PATH/config
    echo 0x44C8220 0x1 > $DCC_PATH/config
    echo 0x44C8400 0x1 > $DCC_PATH/config
    echo 0x44C8404 0x1 > $DCC_PATH/config
    echo 0x44C8408 0x1 > $DCC_PATH/config
    echo 0x44C840C 0x1 > $DCC_PATH/config
    echo 0x44C8410 0x1 > $DCC_PATH/config
    echo 0x44C8414 0x1 > $DCC_PATH/config
    echo 0x44C8418 0x1 > $DCC_PATH/config
    echo 0x44C8420 0x1 > $DCC_PATH/config
    echo 0x44C8424 0x1 > $DCC_PATH/config
    echo 0x44C8428 0x1 > $DCC_PATH/config
    echo 0x44C842C 0x1 > $DCC_PATH/config
    echo 0x44C8430 0x1 > $DCC_PATH/config
    echo 0x44C8434 0x1 > $DCC_PATH/config
    echo 0x44C8438 0x1 > $DCC_PATH/config
    echo 0x44C843C 0x1 > $DCC_PATH/config
    echo 0x44C8440 0x1 > $DCC_PATH/config
    echo 0x44C9800 0x1 > $DCC_PATH/config
    echo 0x44D0000 0x1 > $DCC_PATH/config
    echo 0x44D0020 0x1 > $DCC_PATH/config
    echo 0x44D0030 0x1 > $DCC_PATH/config
    echo 0x44D0100 0x1 > $DCC_PATH/config
    echo 0x44D010C 0x1 > $DCC_PATH/config
    echo 0x44D0400 0x1 > $DCC_PATH/config
    echo 0x44D0410 0x1 > $DCC_PATH/config
    echo 0x44D0420 0x1 > $DCC_PATH/config
    echo 0x44D1800 0x1 > $DCC_PATH/config
    echo 0x450002C 0x1 > $DCC_PATH/config
    echo 0x4500030 0x1 > $DCC_PATH/config
    echo 0x4500094 0x1 > $DCC_PATH/config
    echo 0x450009C 0x1 > $DCC_PATH/config
    echo 0x45000C4 0x1 > $DCC_PATH/config
    echo 0x45000C8 0x1 > $DCC_PATH/config
    echo 0x45003DC 0x1 > $DCC_PATH/config
    echo 0x45005D8 0x1 > $DCC_PATH/config
    echo 0x450202C 0x1 > $DCC_PATH/config
    echo 0x4502030 0x1 > $DCC_PATH/config
    echo 0x4502094 0x1 > $DCC_PATH/config
    echo 0x450209C 0x1 > $DCC_PATH/config
    echo 0x45020C4 0x1 > $DCC_PATH/config
    echo 0x45020C8 0x1 > $DCC_PATH/config
    echo 0x45023DC 0x1 > $DCC_PATH/config
    echo 0x45025D8 0x1 > $DCC_PATH/config
    echo 0x450302C 0x1 > $DCC_PATH/config
    echo 0x4503030 0x1 > $DCC_PATH/config
    echo 0x4503094 0x1 > $DCC_PATH/config
    echo 0x450309C 0x1 > $DCC_PATH/config
    echo 0x45030C4 0x1 > $DCC_PATH/config
    echo 0x45030C8 0x1 > $DCC_PATH/config
    echo 0x45033DC 0x1 > $DCC_PATH/config
    echo 0x45035D8 0x1 > $DCC_PATH/config
    echo 0x4506028 0x1 > $DCC_PATH/config
    echo 0x450602C 0x1 > $DCC_PATH/config
    echo 0x4506044 0x1 > $DCC_PATH/config
    echo 0x4506094 0x1 > $DCC_PATH/config
    echo 0x45061DC 0x1 > $DCC_PATH/config
    echo 0x45061EC 0x1 > $DCC_PATH/config
    echo 0x4506608 0x1 > $DCC_PATH/config
}

config_dcc_thermal()
{
	# DCC_Thermal_Tense_Reg's

    echo 0x4410004 0x1 > $DCC_PATH/config
    echo 0x4411014 0x1 > $DCC_PATH/config
    echo 0x44110E0 0x1 > $DCC_PATH/config
    echo 0x44110EC 0x1 > $DCC_PATH/config
    echo 0x44110A0 16 > $DCC_PATH/config
    echo 0x44110E8 0x1 > $DCC_PATH/config
    echo 0x441113C 0x1 > $DCC_PATH/config
    echo 0x4410010 0x1 > $DCC_PATH/config

    #DCC_Thermal_LMH_Reg's

    echo 0xF55903C 0x1 > $DCC_PATH/config
    echo 0xF559814 0x1 > $DCC_PATH/config
    echo 0xF521700 0x1 > $DCC_PATH/config
    echo 0xF521704 0x1 > $DCC_PATH/config
    echo 0xF52172C 0x1 > $DCC_PATH/config
}

config_tcsr_registers()
{
    echo 0x3C2010 0x1 > $DCC_PATH/config
    echo 0x3C4010 0x1 > $DCC_PATH/config
    echo 0x3C3000 0x1 > $DCC_PATH/config
    echo 0x3C5000 0x1 > $DCC_PATH/config
    echo 0x3C2000 0x1 > $DCC_PATH/config
    echo 0x3C2004 0x1 > $DCC_PATH/config
    echo 0x3C4000 0x1 > $DCC_PATH/config
    echo 0x3C4004 0x1 > $DCC_PATH/config
    echo 0x3D0004 0x1 > $DCC_PATH/config
    echo 0x3D4018 0x1 > $DCC_PATH/config
}

config_gpu_registers()
{
    echo 0x0599404C 0x1 > $DCC_PATH/config
    echo 0x0599418C 0x1 > $DCC_PATH/config
    echo 0x05994078 0x1 > $DCC_PATH/config
    echo 0x05994098 0x1 > $DCC_PATH/config
    echo 0x05995000 0x1 > $DCC_PATH/config
    echo 0x05996000 0x1 > $DCC_PATH/config
    echo 0x05998000 0x1 > $DCC_PATH/config
    echo 0x05999000 0x1 > $DCC_PATH/config
    echo 0x01471154 0x1 > $DCC_PATH/config
    echo 0x0143600C 0x1 > $DCC_PATH/config
}

config_noc_sensin_registers()
{
    echo 0x1880300 0x1 > $DCC_PATH/config
    echo 0x1880304 0x1 > $DCC_PATH/config
    echo 0x1880308 0x1 > $DCC_PATH/config
    echo 0x188030C 0x1 > $DCC_PATH/config
    echo 0x1880310 0x1 > $DCC_PATH/config
    echo 0x1880314 0x1 > $DCC_PATH/config
    echo 0x1880318 0x1 > $DCC_PATH/config
    echo 0x188031C 0x1 > $DCC_PATH/config
    echo 0x1880500 0x1 > $DCC_PATH/config
    echo 0x1880504 0x1 > $DCC_PATH/config
    echo 0x1880508 0x1 > $DCC_PATH/config
    echo 0x188050C 0x1 > $DCC_PATH/config
    echo 0x1880510 0x1 > $DCC_PATH/config
    echo 0x1880514 0x1 > $DCC_PATH/config
    echo 0x1880518 0x1 > $DCC_PATH/config
    echo 0x188051C 0x1 > $DCC_PATH/config
    echo 0x1880700 0x1 > $DCC_PATH/config
    echo 0x1880704 0x1 > $DCC_PATH/config
    echo 0x1880708 0x1 > $DCC_PATH/config
    echo 0x188070C 0x1 > $DCC_PATH/config
    echo 0x1880110 0x1 > $DCC_PATH/config
    echo 0x1880118 0x1 > $DCC_PATH/config
    echo 0x1880120 0x1 > $DCC_PATH/config
    echo 0x1880124 0x1 > $DCC_PATH/config
    echo 0x1880128 0x1 > $DCC_PATH/config
    echo 0x188012C 0x1 > $DCC_PATH/config
    echo 0x1880130 0x1 > $DCC_PATH/config
    echo 0x1880134 0x1 > $DCC_PATH/config
    echo 0x1880138 0x1 > $DCC_PATH/config
    echo 0x188013C 0x1 > $DCC_PATH/config
    echo 0x1880248 0x1 > $DCC_PATH/config
    echo 0x1900300 0x1 > $DCC_PATH/config
    echo 0x1900304 0x1 > $DCC_PATH/config
    echo 0x1900308 0x1 > $DCC_PATH/config
    echo 0x190030C 0x1 > $DCC_PATH/config
    echo 0x1900310 0x1 > $DCC_PATH/config
    echo 0x1900314 0x1 > $DCC_PATH/config
    echo 0x1900318 0x1 > $DCC_PATH/config
    echo 0x190031C 0x1 > $DCC_PATH/config
    echo 0x1900900 0x1 > $DCC_PATH/config
    echo 0x1900904 0x1 > $DCC_PATH/config
    echo 0x1909100 0x1 > $DCC_PATH/config
    echo 0x1909104 0x1 > $DCC_PATH/config
    echo 0x1900010 0x1 > $DCC_PATH/config
    echo 0x1900018 0x1 > $DCC_PATH/config
    echo 0x1900020 0x1 > $DCC_PATH/config
    echo 0x1900024 0x1 > $DCC_PATH/config
    echo 0x1900028 0x1 > $DCC_PATH/config
    echo 0x190002C 0x1 > $DCC_PATH/config
    echo 0x1900030 0x1 > $DCC_PATH/config
    echo 0x1900034 0x1 > $DCC_PATH/config
    echo 0x1900038 0x1 > $DCC_PATH/config
    echo 0x190003C 0x1 > $DCC_PATH/config
    echo 0x1900248 0x1 > $DCC_PATH/config
    echo 0x190024C 0x1 > $DCC_PATH/config
    echo 0x1900258 0x1 > $DCC_PATH/config
}

enable_dcc()
{
    #TODO: Add DCC configuration
    DCC_PATH="/sys/bus/platform/devices/16ff000.dcc_v2"
    soc_version=`cat /sys/devices/soc0/revision`
    soc_version=${soc_version/./}

    if [ ! -d $DCC_PATH ]; then
        echo "DCC does not exist on this build."
        return
    fi

    echo 0 > $DCC_PATH/enable
    echo 1 > $DCC_PATH/config_reset
    echo 2 > $DCC_PATH/curr_list
    echo cap > $DCC_PATH/func_type
    echo sram > $DCC_PATH/data_sink
    config_dcc_power
    config_dcc_clock
    config_dcc_ddr
    config_dcc_thermal
    config_dcc_core
    config_mss_qdsp
    config_lpass_qdsp
    config_gpu_registers
    config_noc_sensin_registers
    config_tcsr_registers
    echo  1 > $DCC_PATH/enable
}

enable_core_hang_config()
{
#TODO: Add core hang configuration
    CORE_PATH ="/sys/devices/system/cpu/hang_detect_core"
    if [ ! -d $CORE_PATH ]; then
        echo "CORE hang does not exist on this build."
        return
    fi
    #set the threshold to max
    echo 0xffffffff > $CORE_PATH/threshold
    #To enable core hang detection
    #It's a boolean variable. Do not use Hex value to enable/disable
    echo 1 > $CORE_PATH/enable
}

config_dcc_core()
{
    echo 0xF1880B4 0x1 > $DCC_PATH/config	#Core Hung Values core  to core 3
    echo 0xF1980B4 0x1 > $DCC_PATH/config
    echo 0xF1A80B4 0x1 > $DCC_PATH/config
    echo 0xF1B80B4 0x1 > $DCC_PATH/config
    echo 0xF1D1228 0x1 > $DCC_PATH/config	#Core Hung common Value in case thresold is not set
    echo 0xF510000 0x1 > $DCC_PATH/config	#SILVER_CPRH_WRAPPER (for Current Voltage)
}

config_mss_qdsp()
{
    echo 0x6130404 0x1 > $DCC_PATH/config	#Core Hung Values core  to core 3
    echo 0x6130408 0x1 > $DCC_PATH/config
    echo 0x6082028 0x1 > $DCC_PATH/config
    echo 0x6130208 0x1 > $DCC_PATH/config
}

config_lpass_qdsp()
{
    echo 0xa400044 0x1 > $DCC_PATH/config
    echo 0xa402028 0x1 > $DCC_PATH/config
    echo 0xa4b0208 0x1 > $DCC_PATH/config
    echo 0xa4b0228 0x1 > $DCC_PATH/config
    echo 0xa4b0248 0x1 > $DCC_PATH/config
    echo 0xa4b0268 0x1 > $DCC_PATH/config
    echo 0xa4b0288 0x1 > $DCC_PATH/config
    echo 0xa4b02a8 0x1 > $DCC_PATH/config
    echo 0xa4b020c 0x1 > $DCC_PATH/config
    echo 0xa4b022c 0x1 > $DCC_PATH/config
    echo 0xa4b024c 0x1 > $DCC_PATH/config
    echo 0xa4b026c 0x1 > $DCC_PATH/config
    echo 0xa4b028c 0x1 > $DCC_PATH/config
    echo 0xa4b02ac 0x1 > $DCC_PATH/config
    echo 0xa4b0210 0x1 > $DCC_PATH/config
    echo 0xa4b0230 0x1 > $DCC_PATH/config
    echo 0xa4b0250 0x1 > $DCC_PATH/config
    echo 0xa4b0270 0x1 > $DCC_PATH/config
    echo 0xa4b0290 0x1 > $DCC_PATH/config
    echo 0xa4b02b0 0x1 > $DCC_PATH/config
    echo 0xa4b0400 0x1 > $DCC_PATH/config
    echo 0xa4b0404 0x1 > $DCC_PATH/config
    echo 0xa4b0408 0x1 > $DCC_PATH/config
    echo 0x6082028 0x1 > $DCC_PATH/config
    echo 0x440B014 0x1 > $DCC_PATH/config
    echo 0x440B00C 0x1 > $DCC_PATH/config
    echo 0xA900408 0x1 > $DCC_PATH/config
    echo 0xA900404 0x1 > $DCC_PATH/config
    echo 0xA751024 0x1 > $DCC_PATH/config
    echo 0xA751020 0x1 > $DCC_PATH/config
    echo 0xA788004 0x1 > $DCC_PATH/config
    echo 0xA788008 0x1 > $DCC_PATH/config
    echo 0xA78A004 0x1 > $DCC_PATH/config
    echo 0xA78A008 0x1 > $DCC_PATH/config
    echo 0xA791000 0x1 > $DCC_PATH/config
    echo 0xA791004 0x1 > $DCC_PATH/config
    echo 0xA792004 0x1 > $DCC_PATH/config
    echo 0xA792008 0x1 > $DCC_PATH/config
    echo 0xA793004 0x1 > $DCC_PATH/config
    echo 0xA793008 0x1 > $DCC_PATH/config
    echo 0xA795004 0x1 > $DCC_PATH/config
    echo 0xA795008 0x1 > $DCC_PATH/config
    echo 0xA795010 0x1 > $DCC_PATH/config
    echo 0xA795014 0x1 > $DCC_PATH/config
    echo 0xA795028 0x1 > $DCC_PATH/config
    echo 0xA79502C 0x1 > $DCC_PATH/config
    echo 0xA780000 0x1 > $DCC_PATH/config
    echo 0xA780004 0x1 > $DCC_PATH/config
    echo 0xA780008 0x1 > $DCC_PATH/config
    echo 0xA78000C 0x1 > $DCC_PATH/config
    echo 0xA780010 0x1 > $DCC_PATH/config
    echo 0xA780014 0x1 > $DCC_PATH/config
    echo 0xA780018 0x1 > $DCC_PATH/config
    echo 0xA78001C 0x1 > $DCC_PATH/config
    echo 0xA780020 0x1 > $DCC_PATH/config
    echo 0xA780024 0x1 > $DCC_PATH/config
    echo 0xA780028 0x1 > $DCC_PATH/config
    echo 0xA78002C 0x1 > $DCC_PATH/config
    echo 0xA780030 0x1 > $DCC_PATH/config
    echo 0xA780034 0x1 > $DCC_PATH/config
    echo 0xA780038 0x1 > $DCC_PATH/config
    echo 0xA78003C 0x1 > $DCC_PATH/config
    echo 0xA780040 0x1 > $DCC_PATH/config
    echo 0xA780044 0x1 > $DCC_PATH/config
    echo 0xA781000 0x1 > $DCC_PATH/config
    echo 0xA781004 0x1 > $DCC_PATH/config
    echo 0xA781008 0x1 > $DCC_PATH/config
    echo 0xA78100C 0x1 > $DCC_PATH/config
    echo 0xA781010 0x1 > $DCC_PATH/config
    echo 0xA781014 0x1 > $DCC_PATH/config
    echo 0xA781018 0x1 > $DCC_PATH/config
    echo 0xA78101C 0x1 > $DCC_PATH/config
    echo 0xA781020 0x1 > $DCC_PATH/config
    echo 0xA781024 0x1 > $DCC_PATH/config
    echo 0xA781028 0x1 > $DCC_PATH/config
    echo 0xA78102C 0x1 > $DCC_PATH/config
    echo 0xA781030 0x1 > $DCC_PATH/config
    echo 0xA781034 0x1 > $DCC_PATH/config
    echo 0xA78D010 0x1 > $DCC_PATH/config
    echo 0xA78D000 0x1 > $DCC_PATH/config
    echo 0xA440000 0x1 > $DCC_PATH/config
    echo 0xA440004 0x1 > $DCC_PATH/config
    echo 0xA440008 0x1 > $DCC_PATH/config
    echo 0xA44000C 0x1 > $DCC_PATH/config
    echo 0xA440010 0x1 > $DCC_PATH/config
    echo 0xA440014 0x1 > $DCC_PATH/config
    echo 0xA440018 0x1 > $DCC_PATH/config
    echo 0xA44001C 0x1 > $DCC_PATH/config
    echo 0xA440020 0x1 > $DCC_PATH/config
    echo 0xA440024 0x1 > $DCC_PATH/config
    echo 0xA440028 0x1 > $DCC_PATH/config
    echo 0xA44002C 0x1 > $DCC_PATH/config
    echo 0xA440030 0x1 > $DCC_PATH/config
    echo 0xA440034 0x1 > $DCC_PATH/config
    echo 0xA448000 0x1 > $DCC_PATH/config
    echo 0xA448004 0x1 > $DCC_PATH/config
}

adjust_permission()
{
    #add permission for block_size, mem_type, mem_size nodes to collect diag over QDSS by ODL
    #application by "oem_2902" group
    chown -h root.oem_2902 /sys/devices/platform/soc/8048000.tmc/coresight-tmc-etr/block_size
    chmod 660 /sys/devices/platform/soc/8048000.tmc/coresight-tmc-etr/block_size
    chown -h root.oem_2902 /sys/devices/platform/soc/8048000.tmc/coresight-tmc-etr/buffer_size
    chmod 660 /sys/devices/platform/soc/8048000.tmc/coresight-tmc-etr/buffer_size
}

enable_schedstats()
{
    # bail out if its perf config
    if [ ! -d /sys/module/msm_rtb ]
    then
        return
    fi

    if [ -f /proc/sys/kernel/sched_schedstats ]
    then
        echo 1 > /proc/sys/kernel/sched_schedstats
    fi
}

enable_buses_and_interconnect_tracefs_debug()
{
    if [ -d $tracefs ] && [ "$(getprop persist.vendor.tracing.enabled)" -eq "1" ]; then
        mkdir $tracefs/instances/hsuart
        #UART
        echo 800 > $tracefs/instances/hsuart/buffer_size_kb
        echo 1 > $tracefs/instances/hsuart/events/serial/enable
        echo 1 > $tracefs/instances/hsuart/tracing_on

        #SPI
        mkdir $tracefs/instances/spi_qup
        echo 1 > $tracefs/instances/spi_qup/events/qup_spi_trace/enable
        echo 1 > $tracefs/instances/spi_qup/tracing_on

        #I2C
        mkdir $tracefs/instances/i2c_qup
        echo 1 > $tracefs/instances/i2c_qup/events/qup_i2c_trace/enable
        echo 1 > $tracefs/instances/i2c_qup/tracing_on

        #GENI_COMMON
        mkdir $tracefs/instances/qupv3_common
        echo 1 > $tracefs/instances/qupv3_common/events/qup_common_trace/enable
        echo 1 > $tracefs/instances/qupv3_common/tracing_on

        #SLIMBUS
        mkdir $tracefs/instances/slimbus
        echo 1 > $tracefs/instances/slimbus/events/slimbus/slimbus_dbg/enable
        echo 1 > $tracefs/instances/slimbus/tracing_on

        #CLOCK, REGULATOR, INTERCONNECT, RPMH
        mkdir $tracefs/instances/clock_reg
        echo 1 > $tracefs/instances/clock_reg/events/clk/enable
        echo 1 > $tracefs/instances/clock_reg/events/regulator/enable
        echo 1 > $tracefs/instances/clock_reg/events/interconnect/enable
        echo 1 > $tracefs/instances/clock_reg/events/rpmh/enable
        echo 1 > $tracefs/instances/clock_reg/tracing_on
    fi
}

create_stp_policy()
{
    mkdir /config/stp-policy/coresight-stm:p_ost.policy
    chmod 660 /config/stp-policy/coresight-stm:p_ost.policy
    mkdir /config/stp-policy/coresight-stm:p_ost.policy/default
    chmod 660 /config/stp-policy/coresight-stm:p_ost.policy/default
    echo 0x10 > /sys/bus/coresight/devices/coresight-stm/traceid
}

ftrace_disable=`getprop persist.debug.ftrace_events_disable`
coresight_config=`getprop persist.debug.coresight.config`
tracefs=/sys/kernel/tracing
srcenable="enable"
enable_debug()
{
    echo "monaco debug"
    etr_size="0x2000000"
    srcenable="enable_source"
    sinkenable="enable_sink"
    create_stp_policy
    echo "Enabling STM events on monaco."
    adjust_permission
    enable_stm_events
    if [ "$ftrace_disable" != "Yes" ]; then
        enable_ftrace_event_tracing
	enable_buses_and_interconnect_tracefs_debug
    fi
    enable_core_hang_config
    enable_dcc
    enable_memory_debug
    enable_schedstats
    setprop ro.dbg.coresight.stm_cfg_done 1
    sf_tracing_disablement
}

enable_debug
