#=============================================================================
# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#=============================================================================

enable_ftrace_event_tracing()
{
    # bail out if its perf config
    if [ "$debug_build" = false ]
    then
        return
    fi

    # bail out if ftrace events aren't present
    if [ ! -d /sys/kernel/tracing/events ]
    then
        return
    fi

    # sound
    echo 1 > /sys/kernel/tracing/events/asoc/snd_soc_reg_read/enable
    echo 1 > /sys/kernel/tracing/events/asoc/snd_soc_reg_write/enable
    # mdp
    echo 1 > /sys/kernel/tracing/events/mdss/mdp_video_underrun_done/enable
    # video
    echo 1 > /sys/kernel/tracing/events/msm_vidc/enable
    # power
    echo 1 > /sys/kernel/tracing/events/msm_low_power/enable
    # fastrpc
    echo 1 > /sys/kernel/tracing/events/fastrpc/enable

    echo 1 > /sys/kernel/tracing/tracing_on
}

# function to enable ftrace event transfer to CoreSight STM
enable_stm_events()
{
    # bail out if its perf config
    if [ "$debug_build" = false ]
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

    echo $etr_size > /sys/bus/coresight/devices/coresight-tmc-etr/buffer_size
    echo 1 > /sys/bus/coresight/devices/coresight-tmc-etr/$sinkenable
    echo coresight-stm > /sys/class/stm_source/ftrace/stm_source_link
    echo 1 > /sys/bus/coresight/devices/coresight-stm/$srcenable
    echo 0 > /sys/bus/coresight/devices/coresight-stm/hwevent_enable
}

enable_stm_hw_events()
{
   #TODO: Add HW events
}


adjust_permission()
{
    #add permission for block_size, mem_type, mem_size nodes to collect diag over QDSS by ODL
    #application by "oem_2902" group
    chown -h root.oem_2902 /sys/devices/platform/soc/10048000.tmc/coresight-tmc-etr/block_size
    chmod 660 /sys/devices/platform/soc/10048000.tmc/coresight-tmc-etr/block_size
    chown -h root.oem_2902 /sys/devices/platform/soc/10048000.tmc/coresight-tmc-etr/buffer_size
    chmod 660 /sys/devices/platform/soc/10048000.tmc/coresight-tmc-etr/buffer_size
}

enable_schedstats()
{
    # bail out if its perf config
    if [ "$debug_build" = false ]
    then
        return
    fi

    if [ -f /proc/sys/kernel/sched_schedstats ]
    then
        echo 1 > /proc/sys/kernel/sched_schedstats
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

#function to enable cti flush for etf
enable_cti_flush_for_etf()
{
    # bail out if its perf config
    if [ "$debug_build" = false ]
    then
        return
    fi

    echo 1 >/sys/bus/coresight/devices/coresight-cti-swao_cti/enable
    echo 0 24 >/sys/bus/coresight/devices/coresight-cti-swao_cti/channels/trigin_attach
    echo 0 1 >/sys/bus/coresight/devices/coresight-cti-swao_cti/channels/trigout_attach
}

enable_memory_debug()
{
    # bail out if its perf config
    if [ "$debug_build" = false ]
    then
        return
    fi

    echo 2 > /proc/sys/vm/panic_on_oom
}

config_dcc_thermal()
{
    echo 0x0C222004 1 > $DCC_PATH/config
    echo 0x0C263014 1 > $DCC_PATH/config
    echo 0x0C2630E0 1 > $DCC_PATH/config
    echo 0x0C2630EC 1 > $DCC_PATH/config
    echo 0x0C2630A0 16 > $DCC_PATH/config
    echo 0x0C2630E8 1 > $DCC_PATH/config
    echo 0x0C26313C 1 > $DCC_PATH/config
    echo 0x0C223004 1 > $DCC_PATH/config
    echo 0x0C265014 1 > $DCC_PATH/config
    echo 0x0C2650E0 1 > $DCC_PATH/config
    echo 0x0C2650EC 1 > $DCC_PATH/config
    echo 0x0C2650A0 16 > $DCC_PATH/config
    echo 0x0C2650E8 1 > $DCC_PATH/config
    echo 0x0C26513C 1 > $DCC_PATH/config

    echo 0x17B70220 2 > $DCC_PATH/config
    echo 0x17B702A0 2 > $DCC_PATH/config
    echo 0x17B704A0 12 > $DCC_PATH/config
    echo 0x17B70520 1 > $DCC_PATH/config
    echo 0x17B70588 1 > $DCC_PATH/config
    echo 0x17B70D90 12 > $DCC_PATH/config
    echo 0x17B71010 10 > $DCC_PATH/config
    echo 0x17B71090 10 > $DCC_PATH/config
    echo 0x17B71A90 8 > $DCC_PATH/config

    echo 0x17B70220 2 > $DCC_PATH/config
    echo 0x17B702A0 2 > $DCC_PATH/config
    echo 0x17B784A0 12 > $DCC_PATH/config
    echo 0x17B78520 1 > $DCC_PATH/config
    echo 0x17B78588 1 > $DCC_PATH/config
    echo 0x17B78D90 12 > $DCC_PATH/config
    echo 0x17B79010 10 > $DCC_PATH/config
    echo 0x17B79090 10 > $DCC_PATH/config
    echo 0x17B79A90 8 > $DCC_PATH/config

    echo 0xEC80010 1 > $DCC_PATH/config
    echo 0xEC81000 1 > $DCC_PATH/config
    echo 0xEC81010 16> $DCC_PATH/config
    echo 0xEC81050 16 > $DCC_PATH/config
    echo 0xEC81090 16 > $DCC_PATH/config
    echo 0xEC810D0 16 > $DCC_PATH/config

    # TODO: check if below turing regs create aurora like sdi issue
    #echo 0x32310220 3 > $DCC_PATH/config
    #echo 0x323102A0 3 > $DCC_PATH/config
    #echo 0x323104A0 6 > $DCC_PATH/config
    #echo 0x32310520 1 > $DCC_PATH/config
    #echo 0x32310588 1 > $DCC_PATH/config
    #echo 0x32310D90 8 > $DCC_PATH/config
    #echo 0x32311010 6 > $DCC_PATH/config
    #echo 0x32311090 6 > $DCC_PATH/config
    #echo 0x32311A90 3 > $DCC_PATH/config

}

config_dcc_gic()
{
    echo 0x17200104 29 > $DCC_PATH/config
    echo 0x17200204 29 > $DCC_PATH/config
    echo 0x17200384 29 > $DCC_PATH/config

    echo 0x17220000 1 > $DCC_PATH/config
    echo 0x17220008 1 > $DCC_PATH/config
    echo 0x17220010 1 > $DCC_PATH/config
    echo 0x17220018 1 > $DCC_PATH/config
    echo 0x17220020 1 > $DCC_PATH/config
    echo 0x17220028 1 > $DCC_PATH/config
    echo 0x17220040 1 > $DCC_PATH/config
    echo 0x17220048 1 > $DCC_PATH/config
    echo 0x17220050 1 > $DCC_PATH/config
    echo 0x17220060 1 > $DCC_PATH/config
    echo 0x17220068 1 > $DCC_PATH/config
    echo 0x17220080 1 > $DCC_PATH/config
    echo 0x17220088 1 > $DCC_PATH/config
    echo 0x17220090 1 > $DCC_PATH/config
    echo 0x172200A0 1 > $DCC_PATH/config
    echo 0x172200A8 1 > $DCC_PATH/config
    echo 0x172200C0 1 > $DCC_PATH/config
    echo 0x172200C8 1 > $DCC_PATH/config
    echo 0x172200D0 1 > $DCC_PATH/config
    echo 0x172200E0 1 > $DCC_PATH/config
    echo 0x172200E8 1 > $DCC_PATH/config
    echo 0x17220100 1 > $DCC_PATH/config
    echo 0x17220108 1 > $DCC_PATH/config
    echo 0x17220110 1 > $DCC_PATH/config
    echo 0x17220120 1 > $DCC_PATH/config
    echo 0x17220128 1 > $DCC_PATH/config
    echo 0x17220140 1 > $DCC_PATH/config
    echo 0x17220148 1 > $DCC_PATH/config
    echo 0x17220150 1 > $DCC_PATH/config
    echo 0x17220160 1 > $DCC_PATH/config
    echo 0x17220168 1 > $DCC_PATH/config
    echo 0x17220180 1 > $DCC_PATH/config
    echo 0x17220188 1 > $DCC_PATH/config
    echo 0x17220190 1 > $DCC_PATH/config
    echo 0x172201A0 1 > $DCC_PATH/config
    echo 0x172201A8 1 > $DCC_PATH/config
    echo 0x172201C0 1 > $DCC_PATH/config
    echo 0x172201C8 1 > $DCC_PATH/config
    echo 0x172201D0 1 > $DCC_PATH/config
    echo 0x172201E0 1 > $DCC_PATH/config
    echo 0x172201E8 1 > $DCC_PATH/config
    echo 0x17220200 1 > $DCC_PATH/config
    echo 0x17220208 1 > $DCC_PATH/config
    echo 0x17220210 1 > $DCC_PATH/config
    echo 0x17220220 1 > $DCC_PATH/config
    echo 0x17220228 1 > $DCC_PATH/config
    echo 0x17220240 1 > $DCC_PATH/config
    echo 0x17220248 1 > $DCC_PATH/config
    echo 0x17220250 1 > $DCC_PATH/config
    echo 0x17220260 1 > $DCC_PATH/config
    echo 0x17220268 1 > $DCC_PATH/config
    echo 0x17220280 1 > $DCC_PATH/config
    echo 0x17220288 1 > $DCC_PATH/config
    echo 0x17220290 1 > $DCC_PATH/config
    echo 0x172202A0 1 > $DCC_PATH/config
    echo 0x172202A8 1 > $DCC_PATH/config
    echo 0x172202C0 1 > $DCC_PATH/config
    echo 0x172202C8 1 > $DCC_PATH/config
    echo 0x172202D0 1 > $DCC_PATH/config
    echo 0x172202E0 1 > $DCC_PATH/config
    echo 0x172202E8 1 > $DCC_PATH/config
    echo 0x17220300 1 > $DCC_PATH/config
    echo 0x17220308 1 > $DCC_PATH/config
    echo 0x17220310 1 > $DCC_PATH/config
    echo 0x17220320 1 > $DCC_PATH/config
    echo 0x17220328 1 > $DCC_PATH/config
    echo 0x17220340 1 > $DCC_PATH/config
    echo 0x17220348 1 > $DCC_PATH/config
    echo 0x17220350 1 > $DCC_PATH/config
    echo 0x17220360 1 > $DCC_PATH/config
    echo 0x17220368 1 > $DCC_PATH/config
    echo 0x1722E000 1 > $DCC_PATH/config
}

config_dcc_gpu()
{
    echo 0x3d0201c > $DCC_PATH/config
    echo 0x3d00000 > $DCC_PATH/config
    echo 0x3d00008 > $DCC_PATH/config
    echo 0x3d00044 > $DCC_PATH/config
    echo 0x3d00058 > $DCC_PATH/config
    echo 0x3d0005c > $DCC_PATH/config
    echo 0x3d00060 > $DCC_PATH/config
    echo 0x3d00064 > $DCC_PATH/config
    echo 0x3d00068 > $DCC_PATH/config
    echo 0x3d0006c > $DCC_PATH/config
    echo 0x3d0007c > $DCC_PATH/config
    echo 0x3d00080 > $DCC_PATH/config
    echo 0x3d00084 > $DCC_PATH/config
    echo 0x3d00088 > $DCC_PATH/config
    echo 0x3d0008c > $DCC_PATH/config
    echo 0x3d00090 > $DCC_PATH/config
    echo 0x3d00094 > $DCC_PATH/config
    echo 0x3d00098 > $DCC_PATH/config
    echo 0x3d0009c > $DCC_PATH/config
    echo 0x3d000a0 > $DCC_PATH/config
    echo 0x3d000a4 > $DCC_PATH/config
    echo 0x3d000a8 > $DCC_PATH/config
    echo 0x3d000ac > $DCC_PATH/config
    echo 0x3d000b0 > $DCC_PATH/config
    echo 0x3d000b4 > $DCC_PATH/config
    echo 0x3d000b8 > $DCC_PATH/config
    echo 0x3d000bc > $DCC_PATH/config
    echo 0x3d000c0 > $DCC_PATH/config
    echo 0x3d000c4 > $DCC_PATH/config
    echo 0x3d000c8 > $DCC_PATH/config
    echo 0x3d000e0 > $DCC_PATH/config
    echo 0x3d000e4 > $DCC_PATH/config
    echo 0x3d000e8 > $DCC_PATH/config
    echo 0x3d000ec > $DCC_PATH/config
    echo 0x3d000f0 > $DCC_PATH/config
    echo 0x3d00108 > $DCC_PATH/config
    echo 0x3d00110 > $DCC_PATH/config
    echo 0x3d0011c > $DCC_PATH/config
    echo 0x3d00124 > $DCC_PATH/config
    echo 0x3d00128 > $DCC_PATH/config
    echo 0x3d00130 > $DCC_PATH/config
    echo 0x3d00140 > $DCC_PATH/config
    echo 0x3d00158 > $DCC_PATH/config
    echo 0x3d001cc > $DCC_PATH/config
    echo 0x3d001d0 > $DCC_PATH/config
    echo 0x3d001d4 > $DCC_PATH/config
    echo 0x3d002b4 > $DCC_PATH/config
    echo 0x3d002b8 > $DCC_PATH/config
    echo 0x3d002c0 > $DCC_PATH/config
    echo 0x3d002d0 > $DCC_PATH/config
    echo 0x3d002e0 > $DCC_PATH/config
    echo 0x3d002f0 > $DCC_PATH/config
    echo 0x3d00300 > $DCC_PATH/config
    echo 0x3d00310 > $DCC_PATH/config
    echo 0x3d00320 > $DCC_PATH/config
    echo 0x3d00330 > $DCC_PATH/config
    echo 0x3d00340 > $DCC_PATH/config
    echo 0x3d00350 > $DCC_PATH/config
    echo 0x3d00360 > $DCC_PATH/config
    echo 0x3d00370 > $DCC_PATH/config
    echo 0x3d00380 > $DCC_PATH/config
    echo 0x3d00390 > $DCC_PATH/config
    echo 0x3d003a0 > $DCC_PATH/config
    echo 0x3d003b0 > $DCC_PATH/config
    echo 0x3d003c0 > $DCC_PATH/config
    echo 0x3d003d0 > $DCC_PATH/config
    echo 0x3d003e0 > $DCC_PATH/config
    echo 0x3d00400 > $DCC_PATH/config
    echo 0x3d00410 > $DCC_PATH/config
    echo 0x3d00414 > $DCC_PATH/config
    echo 0x3d00418 > $DCC_PATH/config
    echo 0x3d0041c > $DCC_PATH/config
    echo 0x3d00420 > $DCC_PATH/config
    echo 0x3d00424 > $DCC_PATH/config
    echo 0x3d00428 > $DCC_PATH/config
    echo 0x3d0042c > $DCC_PATH/config
    echo 0x3d0043c > $DCC_PATH/config
    echo 0x3d00440 > $DCC_PATH/config
    echo 0x3d00444 > $DCC_PATH/config
    echo 0x3d00448 > $DCC_PATH/config
    echo 0x3d0044c > $DCC_PATH/config
    echo 0x3d00450 > $DCC_PATH/config
    echo 0x3d00454 > $DCC_PATH/config
    echo 0x3d00458 > $DCC_PATH/config
    echo 0x3d0045c > $DCC_PATH/config
    echo 0x3d00460 > $DCC_PATH/config
    echo 0x3d00464 > $DCC_PATH/config
    echo 0x3d00468 > $DCC_PATH/config
    echo 0x3d0046c > $DCC_PATH/config
    echo 0x3d00470 > $DCC_PATH/config
    echo 0x3d00474 > $DCC_PATH/config
    echo 0x3d004bc > $DCC_PATH/config
    echo 0x3d00800 > $DCC_PATH/config
    echo 0x3d00804 > $DCC_PATH/config
    echo 0x3d00808 > $DCC_PATH/config
    echo 0x3d0080c > $DCC_PATH/config
    echo 0x3d00810 > $DCC_PATH/config
    echo 0x3d00814 > $DCC_PATH/config
    echo 0x3d00818 > $DCC_PATH/config
    echo 0x3d0081c > $DCC_PATH/config
    echo 0x3d00820 > $DCC_PATH/config
    echo 0x3d00824 > $DCC_PATH/config
    echo 0x3d00828 > $DCC_PATH/config
    echo 0x3d0082c > $DCC_PATH/config
    echo 0x3d00830 > $DCC_PATH/config
    echo 0x3d00834 > $DCC_PATH/config
    echo 0x3d00840 > $DCC_PATH/config
    echo 0x3d00844 > $DCC_PATH/config
    echo 0x3d00848 > $DCC_PATH/config
    echo 0x3d0084c > $DCC_PATH/config
    echo 0x3d00854 > $DCC_PATH/config
    echo 0x3d00858 > $DCC_PATH/config
    echo 0x3d0085c > $DCC_PATH/config
    echo 0x3d00860 > $DCC_PATH/config
    echo 0x3d00864 > $DCC_PATH/config
    echo 0x3d00868 > $DCC_PATH/config
    echo 0x3d0086c > $DCC_PATH/config
    echo 0x3d00870 > $DCC_PATH/config
    echo 0x3d00874 > $DCC_PATH/config
    echo 0x3d00878 > $DCC_PATH/config
    echo 0x3d0087c > $DCC_PATH/config
    echo 0x3d00880 > $DCC_PATH/config
    echo 0x3d00884 > $DCC_PATH/config
    echo 0x3d00888 > $DCC_PATH/config
    echo 0x3d0088c > $DCC_PATH/config
    echo 0x3d00890 > $DCC_PATH/config
    echo 0x3d00894 > $DCC_PATH/config
    echo 0x3d00898 > $DCC_PATH/config
    echo 0x3d0089c > $DCC_PATH/config
    echo 0x3d008a0 > $DCC_PATH/config
    echo 0x3d008a4 > $DCC_PATH/config
    echo 0x3d008a8 > $DCC_PATH/config
    echo 0x3d008ac > $DCC_PATH/config
    echo 0x3d008b0 > $DCC_PATH/config
    echo 0x3d008b4 > $DCC_PATH/config
    echo 0x3d008b8 > $DCC_PATH/config
    echo 0x3d008bc > $DCC_PATH/config
    echo 0x3d008c0 > $DCC_PATH/config
    echo 0x3d008c4 > $DCC_PATH/config
    echo 0x3d008c8 > $DCC_PATH/config
    echo 0x3d008cc > $DCC_PATH/config
    echo 0x3d008d0 > $DCC_PATH/config
    echo 0x3d008d4 > $DCC_PATH/config
    echo 0x3d008d8 > $DCC_PATH/config
    echo 0x3d008dc > $DCC_PATH/config
    echo 0x3d008e0 > $DCC_PATH/config
    echo 0x3d008e4 > $DCC_PATH/config
    echo 0x3d008e8 > $DCC_PATH/config
    echo 0x3d008ec > $DCC_PATH/config
    echo 0x3d008f0 > $DCC_PATH/config
    echo 0x3d008f4 > $DCC_PATH/config
    echo 0x3d008f8 > $DCC_PATH/config
    echo 0x3d008fc > $DCC_PATH/config
    echo 0x3d00900 > $DCC_PATH/config
    echo 0x3d00904 > $DCC_PATH/config
    echo 0x3d00908 > $DCC_PATH/config
    echo 0x3d0090c > $DCC_PATH/config
    echo 0x3d00980 > $DCC_PATH/config
    echo 0x3d00984 > $DCC_PATH/config
    echo 0x3d00988 > $DCC_PATH/config
    echo 0x3d0098c > $DCC_PATH/config
    echo 0x3d00990 > $DCC_PATH/config
    echo 0x3d00994 > $DCC_PATH/config
    echo 0x3d00998 > $DCC_PATH/config
    echo 0x3d0099c > $DCC_PATH/config
    echo 0x3d009a0 > $DCC_PATH/config
    echo 0x3d009c8 > $DCC_PATH/config
    echo 0x3d009cc > $DCC_PATH/config
    echo 0x3d009d0 > $DCC_PATH/config
    echo 0x3d00a04 > $DCC_PATH/config
    echo 0x3d00a08 > $DCC_PATH/config
    echo 0x3d00a0c > $DCC_PATH/config
    echo 0x3d00a10 > $DCC_PATH/config
    echo 0x3d00a14 > $DCC_PATH/config
    echo 0x3d00a18 > $DCC_PATH/config
    echo 0x3d00a1c > $DCC_PATH/config
    echo 0x3d00a20 > $DCC_PATH/config
    echo 0x3d00a24 > $DCC_PATH/config
    echo 0x3d00a28 > $DCC_PATH/config
    echo 0x3d00a2c > $DCC_PATH/config
    echo 0x3d00a30 > $DCC_PATH/config
    echo 0x3d00a34 > $DCC_PATH/config
    echo 0x3d01444 > $DCC_PATH/config
    echo 0x3d014d4 > $DCC_PATH/config
    echo 0x3d014d8 > $DCC_PATH/config
    echo 0x3d017ec > $DCC_PATH/config
    echo 0x3d017f0 > $DCC_PATH/config
    echo 0x3d017f4 > $DCC_PATH/config
    echo 0x3d017f8 > $DCC_PATH/config
    echo 0x3d017fc > $DCC_PATH/config
    echo 0x129000  > $DCC_PATH/config
    echo 0x12903c  > $DCC_PATH/config
    echo 0x181004  > $DCC_PATH/config
    echo 0x181010  > $DCC_PATH/config
    echo 0x181018  > $DCC_PATH/config
    echo 0x181154  > $DCC_PATH/config
    echo 0x18b000  > $DCC_PATH/config
    echo 0x18b03c  > $DCC_PATH/config
    echo 0x18c000  > $DCC_PATH/config
    echo 0x18c03c  > $DCC_PATH/config
    echo 0x196000  > $DCC_PATH/config
    echo 0x19603c  > $DCC_PATH/config
    echo 0x18d000  > $DCC_PATH/config
    echo 0x18d03c  > $DCC_PATH/config
    echo 0x18e000  > $DCC_PATH/config
    echo 0x18e03c  > $DCC_PATH/config
    echo 0x18101c  > $DCC_PATH/config
    echo 0x181020  > $DCC_PATH/config
    echo 0x18100c  > $DCC_PATH/config
    echo 0x181014  > $DCC_PATH/config
    echo 0x3d990b0 > $DCC_PATH/config
    echo 0x3d990b4 > $DCC_PATH/config
    echo 0x3d99124 > $DCC_PATH/config
    echo 0x3d99128 > $DCC_PATH/config
    echo 0x3d9912c > $DCC_PATH/config
    echo 0x3d99138 > $DCC_PATH/config
    echo 0x3d99274 > $DCC_PATH/config
    echo 0x3d99278 > $DCC_PATH/config
    echo 0x3d99800 > $DCC_PATH/config
    echo 0x3d99804 > $DCC_PATH/config
    echo 0x3d99808 > $DCC_PATH/config
    echo 0x3d9980c > $DCC_PATH/config
    echo 0x3d99810 > $DCC_PATH/config
    echo 0x3d99814 > $DCC_PATH/config
    echo 0x3d99818 > $DCC_PATH/config
    echo 0x3d9981c > $DCC_PATH/config
    echo 0x3d99828 > $DCC_PATH/config
    echo 0x3d9983c > $DCC_PATH/config
    echo 0x3d998ac > $DCC_PATH/config
    echo 0x3d94000 > $DCC_PATH/config
    echo 0x3d94004 > $DCC_PATH/config
    echo 0x3d95000 > $DCC_PATH/config
    echo 0x3d95004 > $DCC_PATH/config
    echo 0x3d95008 > $DCC_PATH/config
    echo 0x3d9500c > $DCC_PATH/config
    echo 0x3d96000 > $DCC_PATH/config
    echo 0x3d96004 > $DCC_PATH/config
    echo 0x3d96008 > $DCC_PATH/config
    echo 0x3d9600c > $DCC_PATH/config
    echo 0x3d97000 > $DCC_PATH/config
    echo 0x3d97004 > $DCC_PATH/config
    echo 0x3d97008 > $DCC_PATH/config
    echo 0x3d9700c > $DCC_PATH/config
    echo 0x3d98000 > $DCC_PATH/config
    echo 0x3d98004 > $DCC_PATH/config
    echo 0x3d98008 > $DCC_PATH/config
    echo 0x3d9800c > $DCC_PATH/config
    echo 0x3d99000 > $DCC_PATH/config
    echo 0x3d99004 > $DCC_PATH/config
    echo 0x3d99008 > $DCC_PATH/config
    echo 0x3d9900c > $DCC_PATH/config
    echo 0x3d99010 > $DCC_PATH/config
    echo 0x3d99014 > $DCC_PATH/config
    echo 0x3d99050 > $DCC_PATH/config
    echo 0x3d99054 > $DCC_PATH/config
    echo 0x3d99058 > $DCC_PATH/config
    echo 0x3d9905c > $DCC_PATH/config
    echo 0x3d99060 > $DCC_PATH/config
    echo 0x3d99064 > $DCC_PATH/config
    echo 0x3d99068 > $DCC_PATH/config
    echo 0x3d9906c > $DCC_PATH/config
    echo 0x3d99070 > $DCC_PATH/config
    echo 0x3d99074 > $DCC_PATH/config
    echo 0x3d990a8 > $DCC_PATH/config
    echo 0x3d990ac > $DCC_PATH/config
    echo 0x3d990b8 > $DCC_PATH/config
    echo 0x3d990bc > $DCC_PATH/config
    echo 0x3d990c0 > $DCC_PATH/config
    echo 0x3d990c8 > $DCC_PATH/config
    echo 0x3d99104 > $DCC_PATH/config
    echo 0x3d99108 > $DCC_PATH/config
    echo 0x3d9910c > $DCC_PATH/config
    echo 0x3d99110 > $DCC_PATH/config
    echo 0x3d99114 > $DCC_PATH/config
    echo 0x3d99118 > $DCC_PATH/config
    echo 0x3d9911c > $DCC_PATH/config
    echo 0x3d99120 > $DCC_PATH/config
    echo 0x3d99134 > $DCC_PATH/config
    echo 0x3d9913c > $DCC_PATH/config
    echo 0x3d99140 > $DCC_PATH/config
    echo 0x3d99144 > $DCC_PATH/config
    echo 0x3d99148 > $DCC_PATH/config
    echo 0x3d9914c > $DCC_PATH/config
    echo 0x3d99150 > $DCC_PATH/config
    echo 0x3d99154 > $DCC_PATH/config
    echo 0x3d99198 > $DCC_PATH/config
    echo 0x3d9919c > $DCC_PATH/config
    echo 0x3d991a0 > $DCC_PATH/config
    echo 0x3d991e0 > $DCC_PATH/config
    echo 0x3d991e4 > $DCC_PATH/config
    echo 0x3d991e8 > $DCC_PATH/config
    echo 0x3d99224 > $DCC_PATH/config
    echo 0x3d99228 > $DCC_PATH/config
    echo 0x3d99280 > $DCC_PATH/config
    echo 0x3d99284 > $DCC_PATH/config
    echo 0x3d99288 > $DCC_PATH/config
    echo 0x3d9928c > $DCC_PATH/config
    echo 0x3d99314 > $DCC_PATH/config
    echo 0x3d99318 > $DCC_PATH/config
    echo 0x3d9931c > $DCC_PATH/config
    echo 0x3d99358 > $DCC_PATH/config
    echo 0x3d9935c > $DCC_PATH/config
    echo 0x3d99360 > $DCC_PATH/config
    echo 0x3d993a0 > $DCC_PATH/config
    echo 0x3d993a4 > $DCC_PATH/config
    echo 0x3d993e4 > $DCC_PATH/config
    echo 0x3d993e8 > $DCC_PATH/config
    echo 0x3d993ec > $DCC_PATH/config
    echo 0x3d993f0 > $DCC_PATH/config
    echo 0x3d99470 > $DCC_PATH/config
    echo 0x3d99474 > $DCC_PATH/config
    echo 0x3d99478 > $DCC_PATH/config
    echo 0x3d99500 > $DCC_PATH/config
    echo 0x3d99504 > $DCC_PATH/config
    echo 0x3d99508 > $DCC_PATH/config
    echo 0x3d9950c > $DCC_PATH/config
    echo 0x3d99528 > $DCC_PATH/config
    echo 0x3d9952c > $DCC_PATH/config
    echo 0x3d99530 > $DCC_PATH/config
    echo 0x3d99534 > $DCC_PATH/config
    echo 0x3d99538 > $DCC_PATH/config
    echo 0x3d9953c > $DCC_PATH/config
    echo 0x3d99540 > $DCC_PATH/config
    echo 0x3d99544 > $DCC_PATH/config
    echo 0x3d99548 > $DCC_PATH/config
    echo 0x3d9954c > $DCC_PATH/config
    echo 0x3d99550 > $DCC_PATH/config
    echo 0x3d99554 > $DCC_PATH/config
    echo 0x3d99558 > $DCC_PATH/config
    echo 0x3d9955c > $DCC_PATH/config
    echo 0x3d99560 > $DCC_PATH/config
    echo 0x3d99564 > $DCC_PATH/config
    echo 0x3d99568 > $DCC_PATH/config
    echo 0x3d9956c > $DCC_PATH/config
    echo 0x3d99570 > $DCC_PATH/config
    echo 0x3d99574 > $DCC_PATH/config
    echo 0x3d99578 > $DCC_PATH/config
    echo 0x3d9957c > $DCC_PATH/config
    echo 0x3d99580 > $DCC_PATH/config
    echo 0x3d99584 > $DCC_PATH/config
    echo 0x3d99588 > $DCC_PATH/config
    echo 0x3d9958c > $DCC_PATH/config
    echo 0x3d99590 > $DCC_PATH/config
    echo 0x3d99594 > $DCC_PATH/config
    echo 0x3d99598 > $DCC_PATH/config
    echo 0x3d9959c > $DCC_PATH/config
    echo 0x3d995a0 > $DCC_PATH/config
    echo 0x3d995a4 > $DCC_PATH/config
    echo 0x3d995a8 > $DCC_PATH/config
    echo 0x3d995ac > $DCC_PATH/config
    echo 0x3d995b0 > $DCC_PATH/config
    echo 0x3d995b4 > $DCC_PATH/config
    echo 0x3d995b8 > $DCC_PATH/config
    echo 0x3d995bc > $DCC_PATH/config
    echo 0x3d995c0 > $DCC_PATH/config
    echo 0x3d90000 > $DCC_PATH/config
    echo 0x3d90004 > $DCC_PATH/config
    echo 0x3d90008 > $DCC_PATH/config
    echo 0x3d9000c > $DCC_PATH/config
    echo 0x3d90010 > $DCC_PATH/config
    echo 0x3d90014 > $DCC_PATH/config
    echo 0x3d90018 > $DCC_PATH/config
    echo 0x3d9001c > $DCC_PATH/config
    echo 0x3d90020 > $DCC_PATH/config
    echo 0x3d90024 > $DCC_PATH/config
    echo 0x3d90028 > $DCC_PATH/config
    echo 0x3d9002c > $DCC_PATH/config
    echo 0x3d90030 > $DCC_PATH/config
    echo 0x3d90034 > $DCC_PATH/config
    echo 0x3d90038 > $DCC_PATH/config
    echo 0x3d91000 > $DCC_PATH/config
    echo 0x3d91004 > $DCC_PATH/config
    echo 0x3d91008 > $DCC_PATH/config
    echo 0x3d9100c > $DCC_PATH/config
    echo 0x3d91010 > $DCC_PATH/config
    echo 0x3d91014 > $DCC_PATH/config
    echo 0x3d91018 > $DCC_PATH/config
    echo 0x3d9101c > $DCC_PATH/config
    echo 0x3d91020 > $DCC_PATH/config
    echo 0x3d91024 > $DCC_PATH/config
    echo 0x3d91028 > $DCC_PATH/config
    echo 0x3d9102c > $DCC_PATH/config
    echo 0x3d91030 > $DCC_PATH/config
    echo 0x3d91034 > $DCC_PATH/config
    echo 0x3d91038 > $DCC_PATH/config
    echo 0x3d3b000 > $DCC_PATH/config
    echo 0x3d3b004 > $DCC_PATH/config
    echo 0x3d3b014 > $DCC_PATH/config
    echo 0x3d3b01c > $DCC_PATH/config
    echo 0x3d3b028 > $DCC_PATH/config
    echo 0x3d3b0ac > $DCC_PATH/config
    echo 0x3d3b100 > $DCC_PATH/config
    echo 0x3d3b104 > $DCC_PATH/config
    echo 0x3d3b114 > $DCC_PATH/config
    echo 0x3d3b11c > $DCC_PATH/config
    echo 0x3d3b128 > $DCC_PATH/config
    echo 0x3d3b1ac > $DCC_PATH/config
    echo 0x3d50000 > $DCC_PATH/config
    echo 0x3d50004 > $DCC_PATH/config
    echo 0x3d50008 > $DCC_PATH/config
    echo 0x3d5000c > $DCC_PATH/config
    echo 0x3d50010 > $DCC_PATH/config
    echo 0x3d50014 > $DCC_PATH/config
    echo 0x3d50018 > $DCC_PATH/config
    echo 0x3d5001c > $DCC_PATH/config
    echo 0x3d50020 > $DCC_PATH/config
    echo 0x3d50024 > $DCC_PATH/config
    echo 0x3d50028 > $DCC_PATH/config
    echo 0x3d5002c > $DCC_PATH/config
    echo 0x3d50030 > $DCC_PATH/config
    echo 0x3d50034 > $DCC_PATH/config
    echo 0x3d50038 > $DCC_PATH/config
    echo 0x3d5003c > $DCC_PATH/config
    echo 0x3d50040 > $DCC_PATH/config
    echo 0x3d50044 > $DCC_PATH/config
    echo 0x3d50048 > $DCC_PATH/config
    echo 0x3d5004c > $DCC_PATH/config
    echo 0x3d50050 > $DCC_PATH/config
    echo 0x3d500d0 > $DCC_PATH/config
    echo 0x3d500d8 > $DCC_PATH/config
    echo 0x3d50100 > $DCC_PATH/config
    echo 0x3d50104 > $DCC_PATH/config
    echo 0x3d50108 > $DCC_PATH/config
    echo 0x3d50200 > $DCC_PATH/config
    echo 0x3d50204 > $DCC_PATH/config
    echo 0x3d50208 > $DCC_PATH/config
    echo 0x3d5020c > $DCC_PATH/config
    echo 0x3d50210 > $DCC_PATH/config
    echo 0x3d50400 > $DCC_PATH/config
    echo 0x3d50404 > $DCC_PATH/config
    echo 0x3d50408 > $DCC_PATH/config
    echo 0x3d50450 > $DCC_PATH/config
    echo 0x3d50460 > $DCC_PATH/config
    echo 0x3d50464 > $DCC_PATH/config
    echo 0x3d50490 > $DCC_PATH/config
    echo 0x3d50494 > $DCC_PATH/config
    echo 0x3d50498 > $DCC_PATH/config
    echo 0x3d5049c > $DCC_PATH/config
    echo 0x3d504a0 > $DCC_PATH/config
    echo 0x3d504a4 > $DCC_PATH/config
    echo 0x3d504a8 > $DCC_PATH/config
    echo 0x3d504ac > $DCC_PATH/config
    echo 0x3d504b0 > $DCC_PATH/config
    echo 0x3d504b4 > $DCC_PATH/config
    echo 0x3d504b8 > $DCC_PATH/config
    echo 0x3d504bc > $DCC_PATH/config
    echo 0x3d50550 > $DCC_PATH/config
    echo 0x3d50d00 > $DCC_PATH/config
    echo 0x3d50d04 > $DCC_PATH/config
    echo 0x3d50d20 > $DCC_PATH/config
    echo 0x3d50d24 > $DCC_PATH/config
    echo 0x3d50d28 > $DCC_PATH/config
    echo 0x3d50d2c > $DCC_PATH/config
    echo 0x3d50d34 > $DCC_PATH/config
    echo 0x3d50d38 > $DCC_PATH/config
    echo 0x3d50d3c > $DCC_PATH/config
    echo 0x3d50d40 > $DCC_PATH/config
    echo 0x3d50d44 > $DCC_PATH/config
    echo 0x3d50d10 > $DCC_PATH/config
    echo 0x3d50d18 > $DCC_PATH/config
    echo 0x3d50d1c > $DCC_PATH/config
    echo 0x3d8e100 > $DCC_PATH/config
    echo 0x3d8e104 > $DCC_PATH/config
    echo 0x3d8ec00 > $DCC_PATH/config
    echo 0x3d8ec04 > $DCC_PATH/config
    echo 0x3d8ec0c > $DCC_PATH/config
    echo 0x3d8ec14 > $DCC_PATH/config
    echo 0x3d8ec18 > $DCC_PATH/config
    echo 0x3d8ec1c > $DCC_PATH/config
    echo 0x3d8ec20 > $DCC_PATH/config
    echo 0x3d8ec24 > $DCC_PATH/config
    echo 0x3d8ec28 > $DCC_PATH/config
    echo 0x3d8ec2c > $DCC_PATH/config
    echo 0x3d8ec30 > $DCC_PATH/config
    echo 0x3d8ec34 > $DCC_PATH/config
    echo 0x3d8ec38 > $DCC_PATH/config
    echo 0x3d8ec40 > $DCC_PATH/config
    echo 0x3d8ec44 > $DCC_PATH/config
    echo 0x3d8ec48 > $DCC_PATH/config
    echo 0x3d8ec4c > $DCC_PATH/config
    echo 0x3d8ec54 > $DCC_PATH/config
    echo 0x3d8ec58 > $DCC_PATH/config
    echo 0x3d8ec80 > $DCC_PATH/config
    echo 0x3d8eca0 > $DCC_PATH/config
    echo 0x3d8ecc0 > $DCC_PATH/config
    echo 0x3d7d000 > $DCC_PATH/config
    echo 0x3d7d004 > $DCC_PATH/config
    echo 0x3d7d008 > $DCC_PATH/config
    echo 0x3d7d00c > $DCC_PATH/config
    echo 0x3d7d010 > $DCC_PATH/config
    echo 0x3d7d014 > $DCC_PATH/config
    echo 0x3d7d018 > $DCC_PATH/config
    echo 0x3d7d01c > $DCC_PATH/config
    echo 0x3d7d020 > $DCC_PATH/config
    echo 0x3d7d024 > $DCC_PATH/config
    echo 0x3d7d028 > $DCC_PATH/config
    echo 0x3d7d02c > $DCC_PATH/config
    echo 0x3d7d03c > $DCC_PATH/config
    echo 0x3d7d040 > $DCC_PATH/config
    echo 0x3d7d044 > $DCC_PATH/config
    echo 0x3d7d400 > $DCC_PATH/config
    echo 0x3d7d41c > $DCC_PATH/config
    echo 0x3d7d424 > $DCC_PATH/config
    echo 0x3d7d428 > $DCC_PATH/config
    echo 0x3d7d42c > $DCC_PATH/config
    echo 0x3d7e000 > $DCC_PATH/config
    echo 0x3d7e004 > $DCC_PATH/config
    echo 0x3d7e008 > $DCC_PATH/config
    echo 0x3d7e00c > $DCC_PATH/config
    echo 0x3d7e010 > $DCC_PATH/config
    echo 0x3d7e01c > $DCC_PATH/config
    echo 0x3d7e020 > $DCC_PATH/config
    echo 0x3d7e02c > $DCC_PATH/config
    echo 0x3d7e030 > $DCC_PATH/config
    echo 0x3d7e03c > $DCC_PATH/config
    echo 0x3d7e044 > $DCC_PATH/config
    echo 0x3d7e04c > $DCC_PATH/config
    echo 0x3d7e050 > $DCC_PATH/config
    echo 0x3d7e054 > $DCC_PATH/config
    echo 0x3d7e058 > $DCC_PATH/config
    echo 0x3d7e05c > $DCC_PATH/config
    echo 0x3d7e064 > $DCC_PATH/config
    echo 0x3d7e068 > $DCC_PATH/config
    echo 0x3d7e06c > $DCC_PATH/config
    echo 0x3d7e070 > $DCC_PATH/config
    echo 0x3d7e090 > $DCC_PATH/config
    echo 0x3d7e094 > $DCC_PATH/config
    echo 0x3d7e098 > $DCC_PATH/config
    echo 0x3d7e09c > $DCC_PATH/config
    echo 0x3d7e0a0 > $DCC_PATH/config
    echo 0x3d7e0a4 > $DCC_PATH/config
    echo 0x3d7e0a8 > $DCC_PATH/config
    echo 0x3d7e0b4 > $DCC_PATH/config
    echo 0x3d7e0b8 > $DCC_PATH/config
    echo 0x3d7e0bc > $DCC_PATH/config
    echo 0x3d7e0c0 > $DCC_PATH/config
    echo 0x3d7e100 > $DCC_PATH/config
    echo 0x3d7e104 > $DCC_PATH/config
    echo 0x3d7e108 > $DCC_PATH/config
    echo 0x3d7e10c > $DCC_PATH/config
    echo 0x3d7e110 > $DCC_PATH/config
    echo 0x3d7e114 > $DCC_PATH/config
    echo 0x3d7e118 > $DCC_PATH/config
    echo 0x3d7e11c > $DCC_PATH/config
    echo 0x3d7e120 > $DCC_PATH/config
    echo 0x3d7e124 > $DCC_PATH/config
    echo 0x3d7e128 > $DCC_PATH/config
    echo 0x3d7e12c > $DCC_PATH/config
    echo 0x3d7e130 > $DCC_PATH/config
    echo 0x3d7e134 > $DCC_PATH/config
    echo 0x3d7e138 > $DCC_PATH/config
    echo 0x3d7e13c > $DCC_PATH/config
    echo 0x3d7e140 > $DCC_PATH/config
    echo 0x3d7e144 > $DCC_PATH/config
    echo 0x3d7e148 > $DCC_PATH/config
    echo 0x3d7e14c > $DCC_PATH/config
    echo 0x3d7e180 > $DCC_PATH/config
    echo 0x3d7e1c0 > $DCC_PATH/config
    echo 0x3d7e1c4 > $DCC_PATH/config
    echo 0x3d7e1c8 > $DCC_PATH/config
    echo 0x3d7e1cc > $DCC_PATH/config
    echo 0x3d7e1d0 > $DCC_PATH/config
    echo 0x3d7e1d4 > $DCC_PATH/config
    echo 0x3d7e1d8 > $DCC_PATH/config
    echo 0x3d7e1dc > $DCC_PATH/config
    echo 0x3d7e1e0 > $DCC_PATH/config
    echo 0x3d7e1e4 > $DCC_PATH/config
    echo 0x3d7e1fc > $DCC_PATH/config
    echo 0x3d7e220 > $DCC_PATH/config
    echo 0x3d7e224 > $DCC_PATH/config
    echo 0x3d7e300 > $DCC_PATH/config
    echo 0x3d7e304 > $DCC_PATH/config
    echo 0x3d7e30c > $DCC_PATH/config
    echo 0x3d7e310 > $DCC_PATH/config
    echo 0x3d7e340 > $DCC_PATH/config
    echo 0x3d7e3b0 > $DCC_PATH/config
    echo 0x3d7e3c0 > $DCC_PATH/config
    echo 0x3d7e3c4 > $DCC_PATH/config
    echo 0x3d7e440 > $DCC_PATH/config
    echo 0x3d7e444 > $DCC_PATH/config
    echo 0x3d7e448 > $DCC_PATH/config
    echo 0x3d7e44c > $DCC_PATH/config
    echo 0x3d7e450 > $DCC_PATH/config
    echo 0x3d7e480 > $DCC_PATH/config
    echo 0x3d7e484 > $DCC_PATH/config
    echo 0x3d7e490 > $DCC_PATH/config
    echo 0x3d7e494 > $DCC_PATH/config
    echo 0x3d7e4a0 > $DCC_PATH/config
    echo 0x3d7e4a4 > $DCC_PATH/config
    echo 0x3d7e4b0 > $DCC_PATH/config
    echo 0x3d7e4b4 > $DCC_PATH/config
    echo 0x3d7e500 > $DCC_PATH/config
    echo 0x3d7e508 > $DCC_PATH/config
    echo 0x3d7e50c > $DCC_PATH/config
    echo 0x3d7e510 > $DCC_PATH/config
    echo 0x3d7e520 > $DCC_PATH/config
    echo 0x3d7e524 > $DCC_PATH/config
    echo 0x3d7e528 > $DCC_PATH/config
    echo 0x3d7e53c > $DCC_PATH/config
    echo 0x3d7e540 > $DCC_PATH/config
    echo 0x3d7e544 > $DCC_PATH/config
    echo 0x3d7e560 > $DCC_PATH/config
    echo 0x3d7e564 > $DCC_PATH/config
    echo 0x3d7e568 > $DCC_PATH/config
    echo 0x3d7e574 > $DCC_PATH/config
    echo 0x3d7e588 > $DCC_PATH/config
    echo 0x3d7e590 > $DCC_PATH/config
    echo 0x3d7e594 > $DCC_PATH/config
    echo 0x3d7e598 > $DCC_PATH/config
    echo 0x3d7e59c > $DCC_PATH/config
    echo 0x3d7e5a0 > $DCC_PATH/config
    echo 0x3d7e5a4 > $DCC_PATH/config
    echo 0x3d7e5a8 > $DCC_PATH/config
    echo 0x3d7e5ac > $DCC_PATH/config
    echo 0x3d7e5c0 > $DCC_PATH/config
    echo 0x3d7e5c4 > $DCC_PATH/config
    echo 0x3d7e5c8 > $DCC_PATH/config
    echo 0x3d7e5cc > $DCC_PATH/config
    echo 0x3d7e5d0 > $DCC_PATH/config
    echo 0x3d7e5d4 > $DCC_PATH/config
    echo 0x3d7e5d8 > $DCC_PATH/config
    echo 0x3d7e5dc > $DCC_PATH/config
    echo 0x3d7e5e0 > $DCC_PATH/config
    echo 0x3d7e5e4 > $DCC_PATH/config
    echo 0x3d7e600 > $DCC_PATH/config
    echo 0x3d7e604 > $DCC_PATH/config
    echo 0x3d7e610 > $DCC_PATH/config
    echo 0x3d7e614 > $DCC_PATH/config
    echo 0x3d7e618 > $DCC_PATH/config
    echo 0x3d7e648 > $DCC_PATH/config
    echo 0x3d7e64c > $DCC_PATH/config
    echo 0x3d7e658 > $DCC_PATH/config
    echo 0x3d7e65c > $DCC_PATH/config
    echo 0x3d7e660 > $DCC_PATH/config
    echo 0x3d7e664 > $DCC_PATH/config
    echo 0x3d7e668 > $DCC_PATH/config
    echo 0x3d7e66c > $DCC_PATH/config
    echo 0x3d7e670 > $DCC_PATH/config
    echo 0x3d7e674 > $DCC_PATH/config
    echo 0x3d7e678 > $DCC_PATH/config
    echo 0x3d7e700 > $DCC_PATH/config
    echo 0x3d7e714 > $DCC_PATH/config
    echo 0x3d7e718 > $DCC_PATH/config
    echo 0x3d7e71c > $DCC_PATH/config
    echo 0x3d7e720 > $DCC_PATH/config
    echo 0x3d7e724 > $DCC_PATH/config
    echo 0x3d7e728 > $DCC_PATH/config
    echo 0x3d7e72c > $DCC_PATH/config
    echo 0x3d7e730 > $DCC_PATH/config
    echo 0x3d7e734 > $DCC_PATH/config
    echo 0x3d7e738 > $DCC_PATH/config
    echo 0x3d7e73c > $DCC_PATH/config
    echo 0x3d7e740 > $DCC_PATH/config
    echo 0x3d7e744 > $DCC_PATH/config
    echo 0x3d7e748 > $DCC_PATH/config
    echo 0x3d7e74c > $DCC_PATH/config
    echo 0x3d7e750 > $DCC_PATH/config
    echo 0x3d7e7c0 > $DCC_PATH/config
    echo 0x3d7e7c4 > $DCC_PATH/config
    echo 0x3d7e7e0 > $DCC_PATH/config
    echo 0x3d7e7e4 > $DCC_PATH/config
    echo 0x3d7e7e8 > $DCC_PATH/config
    echo 0x3d9e000 > $DCC_PATH/config
    echo 0x3d9e040 > $DCC_PATH/config
    echo 0x3d9e044 > $DCC_PATH/config
    echo 0x3d9e048 > $DCC_PATH/config
    echo 0x3d9e04c > $DCC_PATH/config
    echo 0x3d9e050 > $DCC_PATH/config
    echo 0x3d9e080 > $DCC_PATH/config
    echo 0x3d9e084 > $DCC_PATH/config
    echo 0x3d9e088 > $DCC_PATH/config
    echo 0x3d9e08c > $DCC_PATH/config
    echo 0x3d9e090 > $DCC_PATH/config
    echo 0x3d9e0cc > $DCC_PATH/config
    echo 0x3d9e0d0 > $DCC_PATH/config
    echo 0x3d9e0d4 > $DCC_PATH/config
    echo 0x3d9e0d8 > $DCC_PATH/config
    echo 0x3d9e0dc > $DCC_PATH/config
    echo 0x3d9e0e0 > $DCC_PATH/config
    echo 0x3d9e0e4 > $DCC_PATH/config
    echo 0x3d9e0e8 > $DCC_PATH/config
    echo 0x3d9e0ec > $DCC_PATH/config
    echo 0x3d9e0fc > $DCC_PATH/config
    echo 0x3d9e100 > $DCC_PATH/config
    echo 0x3d9e118 > $DCC_PATH/config
    echo 0x3d9e11c > $DCC_PATH/config
    echo 0x3d9e134 > $DCC_PATH/config
    echo 0x3d0f114 > $DCC_PATH/config
    echo 0x3d0f118 > $DCC_PATH/config
    echo 0x3d0f11c > $DCC_PATH/config
    echo 0x3d0f000 > $DCC_PATH/config

}

config_dcc_gcc()
{
    echo 0x100000 > $DCC_PATH/config
    echo 0x100004 > $DCC_PATH/config
    echo 0x101000 > $DCC_PATH/config
    echo 0x101004 > $DCC_PATH/config
    echo 0x102000 > $DCC_PATH/config
    echo 0x102004 > $DCC_PATH/config
    echo 0x103000 > $DCC_PATH/config
    echo 0x103004 > $DCC_PATH/config
    echo 0x104000 > $DCC_PATH/config
    echo 0x104004 > $DCC_PATH/config
    echo 0x105000 > $DCC_PATH/config
    echo 0x105004 > $DCC_PATH/config
    echo 0x106000 > $DCC_PATH/config
    echo 0x106004 > $DCC_PATH/config
    echo 0x107000 > $DCC_PATH/config
    echo 0x107004 > $DCC_PATH/config
    echo 0x108000 > $DCC_PATH/config
    echo 0x108004 > $DCC_PATH/config
    echo 0x109000 > $DCC_PATH/config
    echo 0x109004 > $DCC_PATH/config
    echo 0xC2A0000 > $DCC_PATH/config
    echo 0xC2A0004 > $DCC_PATH/config
    echo 0xC2A1000 > $DCC_PATH/config
    echo 0xC2A1004 > $DCC_PATH/config
    echo 0x127028 > $DCC_PATH/config
    echo 0x12715C > $DCC_PATH/config
    echo 0x127290 > $DCC_PATH/config
    echo 0x1273C4 > $DCC_PATH/config
    echo 0x1274F8 > $DCC_PATH/config
    echo 0x127634 > $DCC_PATH/config
    echo 0x127768 > $DCC_PATH/config
    echo 0x128028 > $DCC_PATH/config
    echo 0x12815C > $DCC_PATH/config
    echo 0x128290 > $DCC_PATH/config
    echo 0x1283C4 > $DCC_PATH/config
    echo 0x1284F8 > $DCC_PATH/config
    echo 0x128634 > $DCC_PATH/config
    echo 0x128768 > $DCC_PATH/config
    echo 0x12C084 > $DCC_PATH/config
    echo 0x133028 > $DCC_PATH/config
    echo 0x146020 > $DCC_PATH/config
    echo 0x151028 > $DCC_PATH/config
    echo 0x154164 > $DCC_PATH/config
    echo 0x15502C > $DCC_PATH/config
    echo 0x184074 > $DCC_PATH/config
    echo 0x186034 > $DCC_PATH/config
    echo 0x193024 > $DCC_PATH/config
    echo 0x19904C > $DCC_PATH/config
    echo 0x116100 > $DCC_PATH/config
    echo 0x120004 > $DCC_PATH/config
    echo 0x120008 > $DCC_PATH/config
    echo 0x12C004 > $DCC_PATH/config
    echo 0x12C008 > $DCC_PATH/config
    echo 0x149004 > $DCC_PATH/config
    echo 0x149008 > $DCC_PATH/config
    echo 0x17B004 > $DCC_PATH/config
    echo 0x17B008 > $DCC_PATH/config
    echo 0x187004 > $DCC_PATH/config
    echo 0x187008 > $DCC_PATH/config
    echo 0x199004 > $DCC_PATH/config
    echo 0x199008 > $DCC_PATH/config
    echo 0x19D004 > $DCC_PATH/config
    echo 0x19D008 > $DCC_PATH/config
    echo 0x1A8004 > $DCC_PATH/config
    echo 0x1A9000 > $DCC_PATH/config
    echo 0x1A9004 > $DCC_PATH/config
    echo 0x1AD004 > $DCC_PATH/config
    echo 0x1AD008 > $DCC_PATH/config
    echo 0x161000 > $DCC_PATH/config
    echo 0x161008 > $DCC_PATH/config
    echo 0x161010 > $DCC_PATH/config
    echo 0x161018 > $DCC_PATH/config
    echo 0x162000 > $DCC_PATH/config
    echo 0x162008 > $DCC_PATH/config
    echo 0x162010 > $DCC_PATH/config
    echo 0x162018 > $DCC_PATH/config
    echo 0x163024 > $DCC_PATH/config
    echo 0x16302C > $DCC_PATH/config
    echo 0x163034 > $DCC_PATH/config
    echo 0x16303C > $DCC_PATH/config
    echo 0x165000 > $DCC_PATH/config
    echo 0x165008 > $DCC_PATH/config
    echo 0x165010 > $DCC_PATH/config
    echo 0x165018 > $DCC_PATH/config
    echo 0x166000 > $DCC_PATH/config
    echo 0x166008 > $DCC_PATH/config
    echo 0x166010 > $DCC_PATH/config
    echo 0x166018 > $DCC_PATH/config
    echo 0x167000 > $DCC_PATH/config
    echo 0x167008 > $DCC_PATH/config
    echo 0x167010 > $DCC_PATH/config
    echo 0x167018 > $DCC_PATH/config
    echo 0x16A000 > $DCC_PATH/config
    echo 0x16A008 > $DCC_PATH/config
    echo 0x16A010 > $DCC_PATH/config
    echo 0x16A018 > $DCC_PATH/config
    echo 0x16B000 > $DCC_PATH/config
    echo 0x16B008 > $DCC_PATH/config
    echo 0x16B010 > $DCC_PATH/config
    echo 0x16B018 > $DCC_PATH/config
    echo 0x179024 > $DCC_PATH/config
    echo 0x17902C > $DCC_PATH/config
    echo 0x179034 > $DCC_PATH/config
    echo 0x17903C > $DCC_PATH/config
    echo 0x120024 > $DCC_PATH/config
    echo 0x12C018 > $DCC_PATH/config
    echo 0x12C020 > $DCC_PATH/config
    echo 0x12C028 > $DCC_PATH/config
    echo 0x12C030 > $DCC_PATH/config
    echo 0x12C038 > $DCC_PATH/config
    echo 0x12C040 > $DCC_PATH/config
    echo 0x12C048 > $DCC_PATH/config
    echo 0x12C050 > $DCC_PATH/config
    echo 0x157008 > $DCC_PATH/config
    echo 0x19D0A0 > $DCC_PATH/config
    echo 0x1A0004 > $DCC_PATH/config
    echo 0x1A000C > $DCC_PATH/config
    echo 0x1A0014 > $DCC_PATH/config
    echo 0x1A9014 > $DCC_PATH/config
    echo 0x1A901C > $DCC_PATH/config
    echo 0x193004 > $DCC_PATH/config
    echo 0x193008 > $DCC_PATH/config
    echo 0x193140 > $DCC_PATH/config

    # NOC gds
    echo 0x12015C 1 > $DCC_PATH/config
    echo 0x120160 1 > $DCC_PATH/config
    echo 0x120464 1 > $DCC_PATH/config
    echo 0x120468 1 > $DCC_PATH/config
    echo 0x12C15C 1 > $DCC_PATH/config
    echo 0x12C160 1 > $DCC_PATH/config
    echo 0x12C464 1 > $DCC_PATH/config
    echo 0x12C468 1 > $DCC_PATH/config
    echo 0x188040 1 > $DCC_PATH/config
    echo 0x1A0884 1 > $DCC_PATH/config
    echo 0x1A915C 1 > $DCC_PATH/config
    echo 0x1A9160 1 > $DCC_PATH/config
    echo 0x1A9464 1 > $DCC_PATH/config
    echo 0x1A9468 1 > $DCC_PATH/config
    echo 0x15709000 1 > $DCC_PATH/config
    echo 0x322D1010 1 > $DCC_PATH/config
    echo 0x322D1020 1 > $DCC_PATH/config
    echo 0x322D1024 1 > $DCC_PATH/config
    echo 0x322D1028 1 > $DCC_PATH/config
    echo 0x322D102C 1 > $DCC_PATH/config
    echo 0x322D1030 1 > $DCC_PATH/config
    echo 0x322D1034 1 > $DCC_PATH/config
    echo 0x322D1038 1 > $DCC_PATH/config
    echo 0x322D103C 1 > $DCC_PATH/config
    echo 0x3DA55DC 1 > $DCC_PATH/config
    echo 0x3DA25DC 1 > $DCC_PATH/config
    echo 0x3DA75DC 1 > $DCC_PATH/config
    echo 0x3DA2204 1 > $DCC_PATH/config
    echo 0x3DA2670 1 > $DCC_PATH/config
    echo 0x19181040 1 > $DCC_PATH/config

    echo 0x10C1000 1 > $DCC_PATH/config
    echo 0x10C1004 1 > $DCC_PATH/config
    echo 0x10C1010 1 > $DCC_PATH/config
    echo 0x10C1014 1 > $DCC_PATH/config
    echo 0x10C1018 1 > $DCC_PATH/config
    echo 0x10C101C 1 > $DCC_PATH/config
    echo 0x10C1020 1 > $DCC_PATH/config
    echo 0x10C1024 1 > $DCC_PATH/config
    echo 0x10C1028 1 > $DCC_PATH/config
    echo 0x10C1100 1 > $DCC_PATH/config
    echo 0x10C1104 1 > $DCC_PATH/config
    echo 0x10C1108 1 > $DCC_PATH/config
    echo 0x10C1110 1 > $DCC_PATH/config
    echo 0x10C1114 1 > $DCC_PATH/config
    echo 0x10C1118 1 > $DCC_PATH/config
    echo 0x10C111C 1 > $DCC_PATH/config
    echo 0x10C1120 1 > $DCC_PATH/config
    echo 0x10C1124 1 > $DCC_PATH/config
    echo 0x10C1128 1 > $DCC_PATH/config
    echo 0x10C1130 1 > $DCC_PATH/config
    echo 0x10C1134 1 > $DCC_PATH/config
    echo 0x10C113C 1 > $DCC_PATH/config
    echo 0x10C1140 1 > $DCC_PATH/config
    echo 0x10C1148 1 > $DCC_PATH/config
    echo 0x10C114C 1 > $DCC_PATH/config
    echo 0x10C1150 1 > $DCC_PATH/config
}

config_lcp_dare(){
    echo 0x19EAA004 1 > $DCC_PATH/config
    echo 0x1A0AA004 1 > $DCC_PATH/config
    echo 0x1A2AA004 1 > $DCC_PATH/config
    echo 0x1A4AA004 1 > $DCC_PATH/config
    echo 0x19EAA010 1 > $DCC_PATH/config
    echo 0x1A0AA010 1 > $DCC_PATH/config
    echo 0x1A2AA010 1 > $DCC_PATH/config
    echo 0x1A4AA010 1 > $DCC_PATH/config
    echo 0x19EA9004 1 > $DCC_PATH/config
    echo 0x1A0A9004 1 > $DCC_PATH/config
    echo 0x1A2A9004 1 > $DCC_PATH/config
    echo 0x1A4A9004 1 > $DCC_PATH/config
    echo 0x19EA9010 1 > $DCC_PATH/config
    echo 0x1A0A9010 1 > $DCC_PATH/config
    echo 0x1A2A9010 1 > $DCC_PATH/config
    echo 0x1A4A9010 1 > $DCC_PATH/config
}

config_dcc_smmu()
{
    echo 0x151c4008 > $DCC_PATH/config
    echo 0x150025dc > $DCC_PATH/config
    echo 0x150055dc > $DCC_PATH/config
    echo 0x150075dc > $DCC_PATH/config
    echo 0x150075dc > $DCC_PATH/config
    echo 0x15002204 > $DCC_PATH/config
    echo 0x15002670 > $DCC_PATH/config
    echo 0x15002648 > $DCC_PATH/config
    echo 0x150022fc > $DCC_PATH/config
    echo 0x15002300 > $DCC_PATH/config
    echo 0x15002304 > $DCC_PATH/config
    echo 0x150022fc > $DCC_PATH/config
    echo 0x15002304 > $DCC_PATH/config

    echo 0x3DA2634 > $DCC_PATH/config
    echo 0x3DE0010 > $DCC_PATH/config
    echo 0x3DE0038 > $DCC_PATH/config
}

config_dcc_gemnoc()
{
    # gem_noc_qns_*_poc_err
    echo 0x19100010 1 > $DCC_PATH/config
    echo 0x19100020 1 > $DCC_PATH/config
    echo 0x19100024 1 > $DCC_PATH/config
    echo 0x19100028 1 > $DCC_PATH/config
    echo 0x1910002C 1 > $DCC_PATH/config
    echo 0x19100030 1 > $DCC_PATH/config
    echo 0x19100034 1 > $DCC_PATH/config
    echo 0x19100410 1 > $DCC_PATH/config
    echo 0x19100420 1 > $DCC_PATH/config
    echo 0x19100424 1 > $DCC_PATH/config
    echo 0x19100428 1 > $DCC_PATH/config
    echo 0x1910042C 1 > $DCC_PATH/config
    echo 0x19100430 1 > $DCC_PATH/config
    echo 0x19100434 1 > $DCC_PATH/config
    echo 0x19140010 1 > $DCC_PATH/config
    echo 0x19140020 1 > $DCC_PATH/config
    echo 0x19140024 1 > $DCC_PATH/config
    echo 0x19140028 1 > $DCC_PATH/config
    echo 0x1914002C 1 > $DCC_PATH/config
    echo 0x19140030 1 > $DCC_PATH/config
    echo 0x19140034 1 > $DCC_PATH/config
    echo 0x19140410 1 > $DCC_PATH/config
    echo 0x19140420 1 > $DCC_PATH/config
    echo 0x19140424 1 > $DCC_PATH/config
    echo 0x19140428 1 > $DCC_PATH/config
    echo 0x1914042C 1 > $DCC_PATH/config
    echo 0x19140430 1 > $DCC_PATH/config
    echo 0x19140434 1 > $DCC_PATH/config
    echo 0x19180010 1 > $DCC_PATH/config
    echo 0x19180020 1 > $DCC_PATH/config
    echo 0x19180024 1 > $DCC_PATH/config
    echo 0x19180028 1 > $DCC_PATH/config
    echo 0x1918002C 1 > $DCC_PATH/config
    echo 0x19180030 1 > $DCC_PATH/config
    echo 0x19180034 1 > $DCC_PATH/config
    echo 0x19180410 1 > $DCC_PATH/config
    echo 0x19180420 1 > $DCC_PATH/config
    echo 0x19180424 1 > $DCC_PATH/config
    echo 0x19180428 1 > $DCC_PATH/config
    echo 0x1918042C 1 > $DCC_PATH/config
    echo 0x19180430 1 > $DCC_PATH/config
    echo 0x19180434 1 > $DCC_PATH/config

    # gem_noc_qns_*_poc_dbg_Cfg
    echo 0x19117008 1 > $DCC_PATH/config
    echo 0x1911700C 1 > $DCC_PATH/config
    echo 0x19117408 1 > $DCC_PATH/config
    echo 0x1911740C 1 > $DCC_PATH/config
    echo 0x19157008 1 > $DCC_PATH/config
    echo 0x1915700C 1 > $DCC_PATH/config
    echo 0x19157408 1 > $DCC_PATH/config
    echo 0x1915740C 1 > $DCC_PATH/config
    echo 0x191A3008 1 > $DCC_PATH/config
    echo 0x191A300C 1 > $DCC_PATH/config
    echo 0x191A3408 1 > $DCC_PATH/config
    echo 0x191A340C 1 > $DCC_PATH/config

    # gem_noc_fault_sbm_FaultInStatus0_Low
    echo 0x19181048 1 > $DCC_PATH/config

    # gem_noc qosgen
    echo 0x19119010 1 > $DCC_PATH/config
    echo 0x1911A010 1 > $DCC_PATH/config
    echo 0x1911B010 1 > $DCC_PATH/config
    echo 0x1911C010 1 > $DCC_PATH/config
    echo 0x19159010 1 > $DCC_PATH/config
    echo 0x1915A010 1 > $DCC_PATH/config
    echo 0x1915B010 1 > $DCC_PATH/config
    echo 0x1915C010 1 > $DCC_PATH/config
    echo 0x191A5010 1 > $DCC_PATH/config
    echo 0x191A6010 1 > $DCC_PATH/config
    echo 0x191A7010 1 > $DCC_PATH/config
    echo 0x191A8010 1 > $DCC_PATH/config
    echo 0x191A9010 1 > $DCC_PATH/config

    #coherent even chain
    echo 0x19118000 1 > $DCC_PATH/config
    echo 0x19118004 1 > $DCC_PATH/config
    echo 0x19118008 1 > $DCC_PATH/config
    echo 0x3  > $DCC_PATH/loop
    echo 0x19118010 1 > $DCC_PATH/config
    echo 0x19118014 1 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop
    echo 0x19118018 1 > $DCC_PATH/config

    #noncoherent even chain
    echo 0x19118080 1 > $DCC_PATH/config
    echo 0x19118084 1 > $DCC_PATH/config
    echo 0x19118088 1 > $DCC_PATH/config
    echo 0x4  > $DCC_PATH/loop
    echo 0x19118090 1 > $DCC_PATH/config
    echo 0x19118094 1 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop
    echo 0x19118098 1 > $DCC_PATH/config

    #coherent odd chain
    echo 0x19158000 1 > $DCC_PATH/config
    echo 0x19158004 1 > $DCC_PATH/config
    echo 0x19158008 1 > $DCC_PATH/config
    echo 0x3  > $DCC_PATH/loop
    echo 0x19158010 1 > $DCC_PATH/config
    echo 0x19158014 1 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop
    echo 0x19158018 1 > $DCC_PATH/config

    #noncoherent odd chain
    echo 0x19158080 1 > $DCC_PATH/config
    echo 0x19158084 1 > $DCC_PATH/config
    echo 0x19158088 1 > $DCC_PATH/config
    echo 0x4  > $DCC_PATH/loop
    echo 0x19158090 1 > $DCC_PATH/config
    echo 0x19158094 1 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop
    echo 0x19158098 1 > $DCC_PATH/config

    #coherent sys chain
    echo 0x191A4000 1 > $DCC_PATH/config
    echo 0x191A4004 1 > $DCC_PATH/config
    echo 0x191A4008 1 > $DCC_PATH/config
    echo 0x3  > $DCC_PATH/loop
    echo 0x191A4010 1 > $DCC_PATH/config
    echo 0x191A4014 1 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop
    echo 0x191A4018 1 > $DCC_PATH/config

    #noncoherent sys chain
    echo 0x191A4080 1 > $DCC_PATH/config
    echo 0x191A4084 1 > $DCC_PATH/config
    echo 0x191A4088 1 > $DCC_PATH/config
    echo 0x8  > $DCC_PATH/loop
    echo 0x191A4090 1 > $DCC_PATH/config
    echo 0x191A4094 1 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop
    echo 0x191A4098 1 > $DCC_PATH/config

    #llcc0 tmo
    echo 0x19117010 1 > $DCC_PATH/config
    echo 0x40  > $DCC_PATH/loop
    echo 0x19117038 1 > $DCC_PATH/config
    echo 0x19117030 2 > $DCC_PATH/config
    echo 0x19117030 2 > $DCC_PATH/config
    echo 0x19117030 2 > $DCC_PATH/config
    echo 0x19117030 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #llcc2 tmo
    echo 0x19117410 1 > $DCC_PATH/config
    echo 0x40  > $DCC_PATH/loop
    echo 0x19117438 1 > $DCC_PATH/config
    echo 0x19117430 2 > $DCC_PATH/config
    echo 0x19117430 2 > $DCC_PATH/config
    echo 0x19117430 2 > $DCC_PATH/config
    echo 0x19117430 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #llcc1 tmo
    echo 0x19157010 1 > $DCC_PATH/config
    echo 0x40  > $DCC_PATH/loop
    echo 0x19157038 1 > $DCC_PATH/config
    echo 0x19157030 2 > $DCC_PATH/config
    echo 0x19157030 2 > $DCC_PATH/config
    echo 0x19157030 2 > $DCC_PATH/config
    echo 0x19157030 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #llcc3 tmo
    echo 0x19157410 1 > $DCC_PATH/config
    echo 0x40  > $DCC_PATH/loop
    echo 0x19157438 1 > $DCC_PATH/config
    echo 0x19157430 2 > $DCC_PATH/config
    echo 0x19157430 2 > $DCC_PATH/config
    echo 0x19157430 2 > $DCC_PATH/config
    echo 0x19157430 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #cnoc tmo
    echo 0x191A3010 1 > $DCC_PATH/config
    echo 0x10  > $DCC_PATH/loop
    echo 0x191A3038 1 > $DCC_PATH/config
    echo 0x191A3030 2 > $DCC_PATH/config
    echo 0x191A3030 2 > $DCC_PATH/config
    echo 0x191A3030 2 > $DCC_PATH/config
    echo 0x191A3030 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #pcie tmo
    echo 0x191A3410 1 > $DCC_PATH/config
    echo 0x10  > $DCC_PATH/loop
    echo 0x191A3438 1 > $DCC_PATH/config
    echo 0x191A3430 2 > $DCC_PATH/config
    echo 0x191A3430 2 > $DCC_PATH/config
    echo 0x191A3430 2 > $DCC_PATH/config
    echo 0x191A3430 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop
}

config_dcc_a1_aggre_noc()
{
    echo 0x16E0000 1 > $DCC_PATH/config
    echo 0x16E0004 1 > $DCC_PATH/config
    echo 0x16E0008 1 > $DCC_PATH/config
    echo 0x16E0010 1 > $DCC_PATH/config
    echo 0x16E0018 1 > $DCC_PATH/config
    echo 0x16E0020 1 > $DCC_PATH/config
    echo 0x16E0024 1 > $DCC_PATH/config
    echo 0x16E0028 1 > $DCC_PATH/config
    echo 0x16E002C 1 > $DCC_PATH/config
    echo 0x16E0030 1 > $DCC_PATH/config
    echo 0x16E0034 1 > $DCC_PATH/config
    echo 0x16E0038 1 > $DCC_PATH/config
    echo 0x16E003C 1 > $DCC_PATH/config
    echo 0x16E0240 1 > $DCC_PATH/config
    echo 0x16E0248 1 > $DCC_PATH/config

    #qosgen
    echo 0x16E7010 1 > $DCC_PATH/config
    echo 0x16E8010 1 > $DCC_PATH/config
    echo 0x16E9010 1 > $DCC_PATH/config

    #debugchain
    echo 0x16E1008 1 > $DCC_PATH/config
    echo 0x3 > $DCC_PATH/loop
    echo 0x16E1010 1 > $DCC_PATH/config
    echo 0x16E1014 1 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x16E1018 1 > $DCC_PATH/config
}

config_dcc_a2_aggre_noc()
{
    echo 0x1700008 1 > $DCC_PATH/config
    echo 0x1700010 1 > $DCC_PATH/config
    echo 0x1700018 1 > $DCC_PATH/config
    echo 0x1700020 1 > $DCC_PATH/config
    echo 0x1700024 1 > $DCC_PATH/config
    echo 0x1700028 1 > $DCC_PATH/config
    echo 0x170002C 1 > $DCC_PATH/config
    echo 0x1700030 1 > $DCC_PATH/config
    echo 0x1700034 1 > $DCC_PATH/config
    echo 0x1700038 1 > $DCC_PATH/config
    echo 0x170003C 1 > $DCC_PATH/config
    echo 0x1700240 1 > $DCC_PATH/config
    echo 0x1700248 1 > $DCC_PATH/config

    #qosgen
    echo 0x170D010 1 > $DCC_PATH/config
    echo 0x170E010 1 > $DCC_PATH/config
    #echo 0x170F010 1 > $DCC_PATH/config
    echo 0x1710010 1 > $DCC_PATH/config
    echo 0x1711010 1 > $DCC_PATH/config
    echo 0x1712010 1 > $DCC_PATH/config
    echo 0x1713010 1 > $DCC_PATH/config
    echo 0x1714010 1 > $DCC_PATH/config
    echo 0x1715010 1 > $DCC_PATH/config

    #debug chain
    echo 0x1701008 1 > $DCC_PATH/config
    echo 0x5 > $DCC_PATH/loop
    echo 0x1701010 1 > $DCC_PATH/config
    echo 0x1701014 1 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1701018 1 > $DCC_PATH/config
    echo 0x1701088 1 > $DCC_PATH/config
    echo 0x3 > $DCC_PATH/loop
    echo 0x1701090 1 > $DCC_PATH/config
    echo 0x1701094 1 > $DCC_PATH/config
    echo 0x4 > $DCC_PATH/loop
    echo 0x1701098 1 > $DCC_PATH/config
}

config_dcc_pcie_aggre_noc()
{
    echo 0x16C0008 1 > $DCC_PATH/config
    echo 0x16C0010 1 > $DCC_PATH/config
    echo 0x16C0018 1 > $DCC_PATH/config
    echo 0x16C0020 1 > $DCC_PATH/config
    echo 0x16C0024 1 > $DCC_PATH/config
    echo 0x16C0028 1 > $DCC_PATH/config
    echo 0x16C002C 1 > $DCC_PATH/config
    echo 0x16C0030 1 > $DCC_PATH/config
    echo 0x16C0034 1 > $DCC_PATH/config
    echo 0x16C0038 1 > $DCC_PATH/config
    echo 0x16C003C 1 > $DCC_PATH/config
    echo 0x16C0240 1 > $DCC_PATH/config
    echo 0x16C0248 1 > $DCC_PATH/config
    echo 0x16CA010 1 > $DCC_PATH/config
    echo 0x16CB010 1 > $DCC_PATH/config
    echo 0x16CC010 1 > $DCC_PATH/config

    #debugchain
    echo 0x16C1008 1 > $DCC_PATH/config
    echo 0x4 > $DCC_PATH/loop
    echo 0x16C1010 1 > $DCC_PATH/config
    echo 0x16C1014 1 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x16C1018 1 > $DCC_PATH/config
}

config_dcc_cnoc()
{
    echo 0x1500008 1 > $DCC_PATH/config
    echo 0x1500010 1 > $DCC_PATH/config
    echo 0x1500018 1 > $DCC_PATH/config
    echo 0x1500020 1 > $DCC_PATH/config
    echo 0x1500024 1 > $DCC_PATH/config
    echo 0x1500028 1 > $DCC_PATH/config
    echo 0x150002C 1 > $DCC_PATH/config
    echo 0x1500030 1 > $DCC_PATH/config
    echo 0x1500034 1 > $DCC_PATH/config
    echo 0x1500038 1 > $DCC_PATH/config
    echo 0x150003C 1 > $DCC_PATH/config
    echo 0x1500240 1 > $DCC_PATH/config
    echo 0x1500244 1 > $DCC_PATH/config
    echo 0x1500248 1 > $DCC_PATH/config
    echo 0x150024C 1 > $DCC_PATH/config
    echo 0x1500250 1 > $DCC_PATH/config
    echo 0x1500258 1 > $DCC_PATH/config
    echo 0x1500440 1 > $DCC_PATH/config
    echo 0x1500448 1 > $DCC_PATH/config

    echo 0x1502008 1 > $DCC_PATH/config
    echo 0xe > $DCC_PATH/loop
    echo 0x1502010 1 > $DCC_PATH/config
    echo 0x1502014 1 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1502018 1 > $DCC_PATH/config
    echo 0x1502088 1 > $DCC_PATH/config
    echo 0x3 > $DCC_PATH/loop
    echo 0x1502090 1 > $DCC_PATH/config
    echo 0x1502094 1 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1502098 1 > $DCC_PATH/config
    echo 0x1502108 1 > $DCC_PATH/config
    echo 0x3 > $DCC_PATH/loop
    echo 0x1502110 1 > $DCC_PATH/config
    echo 0x1502114 1 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1502118 1 > $DCC_PATH/config
    echo 0x1502188 1 > $DCC_PATH/config
    echo 0x4 > $DCC_PATH/loop
    echo 0x1502190 1 > $DCC_PATH/config
    echo 0x1502194 1 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1502198 1 > $DCC_PATH/config
    echo 0x1502208 1 > $DCC_PATH/config
    echo 0x4 > $DCC_PATH/loop
    echo 0x1502210 1 > $DCC_PATH/config
    echo 0x1502214 1 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1502218 1 > $DCC_PATH/config
}

config_dcc_dc_dch_noc()
{
    echo 0x190E0008 1 > $DCC_PATH/config
    echo 0x190E0010 1 > $DCC_PATH/config
    echo 0x190E0018 1 > $DCC_PATH/config
    echo 0x190E0020 1 > $DCC_PATH/config
    echo 0x190E0024 1 > $DCC_PATH/config
    echo 0x190E0028 1 > $DCC_PATH/config
    echo 0x190E002C 1 > $DCC_PATH/config
    echo 0x190E0030 1 > $DCC_PATH/config
    echo 0x190E0034 1 > $DCC_PATH/config
    echo 0x190E0038 1 > $DCC_PATH/config
    echo 0x190E003C 1 > $DCC_PATH/config
    echo 0x190E0240 1 > $DCC_PATH/config
    echo 0x190E0248 1 > $DCC_PATH/config
    echo 0x190E5008 1 > $DCC_PATH/config
    echo 0x7 > $DCC_PATH/loop
    echo 0x190E5010 1 > $DCC_PATH/config
    echo 0x190E5014 1 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x190E5018 1 > $DCC_PATH/config
}

config_dcc_dc_ch02_noc()
{
    echo 0x195F0008 1 > $DCC_PATH/config
    echo 0x195F0010 1 > $DCC_PATH/config
    echo 0x195F0018 1 > $DCC_PATH/config
    echo 0x195F0020 1 > $DCC_PATH/config
    echo 0x195F0024 1 > $DCC_PATH/config
    echo 0x195F0028 1 > $DCC_PATH/config
    echo 0x195F002C 1 > $DCC_PATH/config
    echo 0x195F0030 1 > $DCC_PATH/config
    echo 0x195F0034 1 > $DCC_PATH/config
    echo 0x195F0038 1 > $DCC_PATH/config
    echo 0x195F003C 1 > $DCC_PATH/config
    echo 0x195F0240 1 > $DCC_PATH/config
    echo 0x195F0248 1 > $DCC_PATH/config
    echo 0x195F2008 1 > $DCC_PATH/config
    echo 0x3 > $DCC_PATH/loop
    echo 0x195F2010 1 > $DCC_PATH/config
    echo 0x195F2014 1 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x195F2018 1 > $DCC_PATH/config
}

config_dcc_dc_ch13_noc()
{
    echo 0x199F0008 1 > $DCC_PATH/config
    echo 0x199F0010 1 > $DCC_PATH/config
    echo 0x199F0018 1 > $DCC_PATH/config
    echo 0x199F0020 1 > $DCC_PATH/config
    echo 0x199F0024 1 > $DCC_PATH/config
    echo 0x199F0028 1 > $DCC_PATH/config
    echo 0x199F002C 1 > $DCC_PATH/config
    echo 0x199F0030 1 > $DCC_PATH/config
    echo 0x199F0034 1 > $DCC_PATH/config
    echo 0x199F0038 1 > $DCC_PATH/config
    echo 0x199F003C 1 > $DCC_PATH/config
    echo 0x199F0240 1 > $DCC_PATH/config
    echo 0x199F0248 1 > $DCC_PATH/config
    echo 0x199F2008 1 > $DCC_PATH/config
    echo 0x3 > $DCC_PATH/loop
    echo 0x199F2010 1 > $DCC_PATH/config
    echo 0x199F2014 1 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x199F2018 1 > $DCC_PATH/config
}

config_dcc_snoc()
{
    echo 0x1680008 1 > $DCC_PATH/config
    echo 0x1680010 1 > $DCC_PATH/config
    echo 0x1680018 1 > $DCC_PATH/config
    echo 0x1680020 1 > $DCC_PATH/config
    echo 0x1680024 1 > $DCC_PATH/config
    echo 0x1680028 1 > $DCC_PATH/config
    echo 0x168002C 1 > $DCC_PATH/config
    echo 0x1680030 1 > $DCC_PATH/config
    echo 0x1680034 1 > $DCC_PATH/config
    echo 0x1680038 1 > $DCC_PATH/config
    echo 0x168003C 1 > $DCC_PATH/config
    echo 0x1680240 1 > $DCC_PATH/config
    echo 0x1680248 1 > $DCC_PATH/config
    echo 0x1690010 1 > $DCC_PATH/config
    echo 0x1691010 1 > $DCC_PATH/config
    echo 0x1692010 1 > $DCC_PATH/config
    echo 0x1693010 1 > $DCC_PATH/config
    echo 0x1681008 1 > $DCC_PATH/config
    echo 0x6 > $DCC_PATH/loop
    echo 0x1681010 1 > $DCC_PATH/config
    echo 0x1681014 1 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1681018 1 > $DCC_PATH/config
}

config_dcc_tlb()
{
    echo 0x3DA0074 1 > $DCC_PATH/config
    echo 0x3DA0474 1 > $DCC_PATH/config
    echo 0x3DA0070 1 > $DCC_PATH/config

    echo 0x15000474 1 > $DCC_PATH/config
    echo 0x15000074 1 > $DCC_PATH/config
    echo 0x15000070 1 > $DCC_PATH/config
}

config_dcc_mmnoc()
{
    echo 0x1740008 1 > $DCC_PATH/config
    echo 0x1740010 1 > $DCC_PATH/config
    echo 0x1740018 1 > $DCC_PATH/config
    echo 0x1740020 1 > $DCC_PATH/config
    echo 0x1740024 1 > $DCC_PATH/config
    echo 0x1740028 1 > $DCC_PATH/config
    echo 0x174002C 1 > $DCC_PATH/config
    echo 0x1740030 1 > $DCC_PATH/config
    echo 0x1740034 1 > $DCC_PATH/config
    echo 0x1740038 1 > $DCC_PATH/config
    echo 0x174003C 1 > $DCC_PATH/config
    echo 0x1740240 1 > $DCC_PATH/config
    echo 0x1740248 1 > $DCC_PATH/config
    echo 0x1755010 1 > $DCC_PATH/config
    echo 0x1755090 1 > $DCC_PATH/config
    echo 0x1756010 1 > $DCC_PATH/config
    echo 0x1757010 1 > $DCC_PATH/config
    echo 0x1757090 1 > $DCC_PATH/config
    echo 0x1758010 1 > $DCC_PATH/config
    echo 0x1759010 1 > $DCC_PATH/config
    echo 0x1759090 1 > $DCC_PATH/config
    echo 0x175A010 1 > $DCC_PATH/config
    echo 0x175A090 1 > $DCC_PATH/config
    echo 0x175A110 1 > $DCC_PATH/config
    echo 0x175B010 1 > $DCC_PATH/config
    echo 0x175B090 1 > $DCC_PATH/config
    echo 0x175C010 1 > $DCC_PATH/config
    echo 0x175C090 1 > $DCC_PATH/config
    echo 0x1741008 1 > $DCC_PATH/config
    echo 0xc > $DCC_PATH/loop
    echo 0x1741010 1 > $DCC_PATH/config
    echo 0x1741014 1 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1741018 1 > $DCC_PATH/config
}

config_dcc_lpass_ag_noc()
{
    echo 0x3C40008 1 > $DCC_PATH/config
    echo 0x3C40010 1 > $DCC_PATH/config
    echo 0x3C40018 1 > $DCC_PATH/config
    echo 0x3C40020 1 > $DCC_PATH/config
    echo 0x3C40024 1 > $DCC_PATH/config
    echo 0x3C40028 1 > $DCC_PATH/config
    echo 0x3C4002C 1 > $DCC_PATH/config
    echo 0x3C40030 1 > $DCC_PATH/config
    echo 0x3C40034 1 > $DCC_PATH/config
    echo 0x3C40038 1 > $DCC_PATH/config
    echo 0x3C4003C 1 > $DCC_PATH/config
    echo 0x3C4B040 1 > $DCC_PATH/config
    echo 0x3C4B048 1 > $DCC_PATH/config
    echo 0x3C44010 1 > $DCC_PATH/config
    echo 0x3C45010 1 > $DCC_PATH/config
    echo 0x3C46010 1 > $DCC_PATH/config
    echo 0x3C54010 1 > $DCC_PATH/config
    echo 0x3C41008 1 > $DCC_PATH/config
    echo 0x3C41010 1 > $DCC_PATH/config
    echo 0x3C41014 1 > $DCC_PATH/config
    echo 0x3C41018 1 > $DCC_PATH/config

    #debug chain
    echo 0x3C41008 1 > $DCC_PATH/config
    echo 0x5 > $DCC_PATH/loop
    echo 0x3C41010 1 > $DCC_PATH/config
    echo 0x3C41014 1 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x3C41018 1 > $DCC_PATH/config
}

config_dcc_turing_noc()
{
    echo 0x320C0008 1 > $DCC_PATH/config
    echo 0x320C0010 1 > $DCC_PATH/config
    echo 0x320C0018 1 > $DCC_PATH/config
    echo 0x320C0020 1 > $DCC_PATH/config
    echo 0x320C0024 1 > $DCC_PATH/config
    echo 0x320C0028 1 > $DCC_PATH/config
    echo 0x320C002C 1 > $DCC_PATH/config
    echo 0x320C0030 1 > $DCC_PATH/config
    echo 0x320C0034 1 > $DCC_PATH/config
    echo 0x320C0038 1 > $DCC_PATH/config
    echo 0x320C003C 1 > $DCC_PATH/config
    echo 0x320C1040 1 > $DCC_PATH/config
    echo 0x320C1048 1 > $DCC_PATH/config
    echo 0x320C9010 1 > $DCC_PATH/config
    echo 0x320CA010 1 > $DCC_PATH/config
    echo 0x320CB010 1 > $DCC_PATH/config
    echo 0x320C4008 1 > $DCC_PATH/config
    echo 0x5 > $DCC_PATH/loop
    echo 0x320C4010 1 > $DCC_PATH/config
    echo 0x320C4014 1 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x320C4018 1 > $DCC_PATH/config
}

config_dcc_core()
{
    # CORE_HANG_THRESHOLD
    echo 0x17800058 > $DCC_PATH/config
    echo 0x17810058 > $DCC_PATH/config
    echo 0x17820058 > $DCC_PATH/config
    echo 0x17830058 > $DCC_PATH/config
    echo 0x17840058 > $DCC_PATH/config
    echo 0x17850058 > $DCC_PATH/config
    # CORE_HANG_VALUE
    echo 0x1780005C > $DCC_PATH/config
    echo 0x1781005C > $DCC_PATH/config
    echo 0x1782005C > $DCC_PATH/config
    echo 0x1783005C > $DCC_PATH/config
    echo 0x1784005C > $DCC_PATH/config
    echo 0x1785005C > $DCC_PATH/config
    # CORE_HANG_CONFIG
    echo 0x17800060 > $DCC_PATH/config
    echo 0x17810060 > $DCC_PATH/config
    echo 0x17820060 > $DCC_PATH/config
    echo 0x17830060 > $DCC_PATH/config
    echo 0x17840060 > $DCC_PATH/config
    echo 0x17850060 > $DCC_PATH/config
    #CORE_HANG_DBG_STS
    echo 0x17800064 > $DCC_PATH/config
    echo 0x17810064 > $DCC_PATH/config
    echo 0x17820064 > $DCC_PATH/config
    echo 0x17830064 > $DCC_PATH/config
    echo 0x17840064 > $DCC_PATH/config
    echo 0x17850064 > $DCC_PATH/config
    #first core hang
    echo 0x1740003c > $DCC_PATH/config
    #PLL
    echo 0x17A80000 > $DCC_PATH/config
    echo 0x17A8002C > $DCC_PATH/config
    echo 0x17A84000 > $DCC_PATH/config
    echo 0x17A8402C > $DCC_PATH/config
    echo 0x190A0000 > $DCC_PATH/config
    echo 0x190A0004 > $DCC_PATH/config
    echo 0x190A1000 > $DCC_PATH/config
    echo 0x190A1004 > $DCC_PATH/config
    #rpmh
    echo 0xc201244 > $DCC_PATH/config
    echo 0xc202244 > $DCC_PATH/config
    #cprh
    echo 0x17BA3A84 2 > $DCC_PATH/config
    echo 0x17B93A84 1 > $DCC_PATH/config
    #APM
    echo 0x17b00000 70 > $DCC_PATH/config
    #CPRh
    echo 0x17900908 1 > $DCC_PATH/config
    echo 0x17900c18 1 > $DCC_PATH/config

    echo 0x17901908 1 > $DCC_PATH/config
    echo 0x17901c18 1 > $DCC_PATH/config

    echo 0x17b90810 1 > $DCC_PATH/config
    echo 0x17b90814 1 > $DCC_PATH/config
    echo 0x17b90818 1 > $DCC_PATH/config

    echo 0x17ba0810 1 > $DCC_PATH/config
    echo 0x17ba0814 1 > $DCC_PATH/config
    echo 0x17ba0818 1 > $DCC_PATH/config

    echo 0x17b93500 40 > $DCC_PATH/config
    echo 0x17ba3500 40 > $DCC_PATH/config

    #gold,Gold+,L3 PLL
    echo 0x17a80000 16 > $DCC_PATH/config
    echo 0x17a82000 6 > $DCC_PATH/config
    echo 0x17a84000 16 > $DCC_PATH/config
    echo 0x17a86000 6 > $DCC_PATH/config
    echo 0x17a88000 16 > $DCC_PATH/config
    echo 0x17a8a000 6 > $DCC_PATH/config
}
config_dcc_clk()
{
    echo 0x128008 > $DCC_PATH/config
    echo 0x127008 > $DCC_PATH/config
}
config_dcc_lpm_pcu()
{
    echo 0x17800010 > $DCC_PATH/config
    echo 0x17800024 > $DCC_PATH/config
    echo 0x17800038 > $DCC_PATH/config
    echo 0x1780003C > $DCC_PATH/config
    echo 0x17800040 > $DCC_PATH/config
    echo 0x17800044 > $DCC_PATH/config
    echo 0x17800048 > $DCC_PATH/config
    echo 0x1780004C > $DCC_PATH/config
    echo 0x1780006C > $DCC_PATH/config
    echo 0x178000F0 > $DCC_PATH/config
    echo 0x178000F4 > $DCC_PATH/config
    echo 0x17810010 > $DCC_PATH/config
    echo 0x17810024 > $DCC_PATH/config
    echo 0x17810038 > $DCC_PATH/config
    echo 0x1781003C > $DCC_PATH/config
    echo 0x17810040 > $DCC_PATH/config
    echo 0x17810044 > $DCC_PATH/config
    echo 0x17810048 > $DCC_PATH/config
    echo 0x1781004C > $DCC_PATH/config
    echo 0x1781006C > $DCC_PATH/config
    echo 0x178100F0 > $DCC_PATH/config
    echo 0x178100F4 > $DCC_PATH/config
    echo 0x17820010 > $DCC_PATH/config
    echo 0x17820024 > $DCC_PATH/config
    echo 0x17820038 > $DCC_PATH/config
    echo 0x1782003C > $DCC_PATH/config
    echo 0x17820040 > $DCC_PATH/config
    echo 0x17820044 > $DCC_PATH/config
    echo 0x17820048 > $DCC_PATH/config
    echo 0x1782004C > $DCC_PATH/config
    echo 0x1782006C > $DCC_PATH/config
    echo 0x178200F0 > $DCC_PATH/config
    echo 0x178200F4 > $DCC_PATH/config
    echo 0x17830010 > $DCC_PATH/config
    echo 0x17830024 > $DCC_PATH/config
    echo 0x17830038 > $DCC_PATH/config
    echo 0x1783003C > $DCC_PATH/config
    echo 0x17830040 > $DCC_PATH/config
    echo 0x17830044 > $DCC_PATH/config
    echo 0x17830048 > $DCC_PATH/config
    echo 0x1783004C > $DCC_PATH/config
    echo 0x1783006C > $DCC_PATH/config
    echo 0x178300F0 > $DCC_PATH/config
    echo 0x178300F4 > $DCC_PATH/config
    echo 0x17840010 > $DCC_PATH/config
    echo 0x17840024 > $DCC_PATH/config
    echo 0x17840038 > $DCC_PATH/config
    echo 0x1784003C > $DCC_PATH/config
    echo 0x17840040 > $DCC_PATH/config
    echo 0x17840044 > $DCC_PATH/config
    echo 0x17840048 > $DCC_PATH/config
    echo 0x1784004C > $DCC_PATH/config
    echo 0x1784006C > $DCC_PATH/config
    echo 0x178400F0 > $DCC_PATH/config
    echo 0x178400F4 > $DCC_PATH/config
    echo 0x17840010 > $DCC_PATH/config
    echo 0x17850024 > $DCC_PATH/config
    echo 0x17850038 > $DCC_PATH/config
    echo 0x1785003C > $DCC_PATH/config
    echo 0x17850040 > $DCC_PATH/config
    echo 0x17850044 > $DCC_PATH/config
    echo 0x17850048 > $DCC_PATH/config
    echo 0x1785004C > $DCC_PATH/config
    echo 0x1785006C > $DCC_PATH/config
    echo 0x178500F0 > $DCC_PATH/config
    echo 0x178500F4 > $DCC_PATH/config

    echo 0x17880010 > $DCC_PATH/config
    echo 0x17880024 > $DCC_PATH/config
    echo 0x1788003C > $DCC_PATH/config
    echo 0x17880040 > $DCC_PATH/config
    echo 0x17880044 > $DCC_PATH/config
    echo 0x17880048 > $DCC_PATH/config
    echo 0x1788004C > $DCC_PATH/config
    echo 0x1788006C > $DCC_PATH/config
    echo 0x17880070 > $DCC_PATH/config
    echo 0x17880074 > $DCC_PATH/config
    echo 0x17880078 > $DCC_PATH/config
    echo 0x1788007C > $DCC_PATH/config
    echo 0x17880080 > $DCC_PATH/config
    echo 0x17880084 > $DCC_PATH/config
    echo 0x17880090 > $DCC_PATH/config
    echo 0x17880094 > $DCC_PATH/config
    echo 0x17880098 > $DCC_PATH/config
    echo 0x1788009C > $DCC_PATH/config
    echo 0x178800A0 > $DCC_PATH/config
    echo 0x178800E0 > $DCC_PATH/config
    echo 0x178800E4 > $DCC_PATH/config
    echo 0x178800E8 > $DCC_PATH/config
    echo 0x178800EC > $DCC_PATH/config
    echo 0x178800F0 > $DCC_PATH/config
    echo 0x178800F4 > $DCC_PATH/config
    echo 0x178800F8 > $DCC_PATH/config
    echo 0x178800FC > $DCC_PATH/config
    echo 0x17880100 > $DCC_PATH/config
    echo 0x17880104 > $DCC_PATH/config
    echo 0x17880108 > $DCC_PATH/config
    echo 0x1788010C > $DCC_PATH/config
    echo 0x17880110 > $DCC_PATH/config
    echo 0x17880114 > $DCC_PATH/config
    echo 0x17880118 > $DCC_PATH/config
    echo 0x1788011C > $DCC_PATH/config
    echo 0x17880120 > $DCC_PATH/config
    echo 0x17880124 > $DCC_PATH/config
    echo 0x17880128 > $DCC_PATH/config
    echo 0x1788012C > $DCC_PATH/config
    echo 0x17880130 > $DCC_PATH/config
    echo 0x17880134 > $DCC_PATH/config
    echo 0x17880138 > $DCC_PATH/config
    echo 0x17880140 > $DCC_PATH/config
    echo 0x17880144 > $DCC_PATH/config
    echo 0x1788019C > $DCC_PATH/config
    echo 0x178801A0 > $DCC_PATH/config
    echo 0x178801B4 > $DCC_PATH/config
    echo 0x178801B8 > $DCC_PATH/config
    echo 0x178801BC > $DCC_PATH/config
    echo 0x178801C0 > $DCC_PATH/config
    echo 0x178801C4 > $DCC_PATH/config
    echo 0x178801C8 > $DCC_PATH/config
    echo 0x178801F0 > $DCC_PATH/config
    echo 0x178801F4 > $DCC_PATH/config
    echo 0x178801F8 > $DCC_PATH/config
    echo 0x178801FC > $DCC_PATH/config
    echo 0x17880200 > $DCC_PATH/config
    echo 0x17880250 > $DCC_PATH/config
    echo 0x17880254 > $DCC_PATH/config
    echo 0x17880258 > $DCC_PATH/config
    echo 0x1788025C > $DCC_PATH/config
}

config_dcc_apss_rscc()
{
    echo 0x17A00010 > $DCC_PATH/config
    echo 0x17A00030 > $DCC_PATH/config
    echo 0x17A00038 > $DCC_PATH/config
    echo 0x17A00040 > $DCC_PATH/config
    echo 0x17A00048 > $DCC_PATH/config
    echo 0x17A00400 > $DCC_PATH/config
    echo 0x17A00404 > $DCC_PATH/config
    echo 0x17A00408 > $DCC_PATH/config
    echo 0x17A00408 > $DCC_PATH/config
    echo 0x17AA0070 > $DCC_PATH/config
    echo 0x17AA0080 > $DCC_PATH/config
    echo 0x1741000C > $DCC_PATH/config
    echo 0x1741100C > $DCC_PATH/config
    echo 0x17410008 > $DCC_PATH/config
    echo 0x17410010 > $DCC_PATH/config
    echo 0x17410014 > $DCC_PATH/config
    echo 0xC2F1000 > $DCC_PATH/config
    echo 0xC230008 > $DCC_PATH/config
    echo 0xC23000C > $DCC_PATH/config
    echo 0xC230010 > $DCC_PATH/config
    echo 0xC230004 > $DCC_PATH/config
    echo 0xC230014 > $DCC_PATH/config
    echo 0x17411008 > $DCC_PATH/config
    echo 0x17411010 > $DCC_PATH/config
    echo 0x17411014 > $DCC_PATH/config
    echo 0x20810010 > $DCC_PATH/config

    echo 0xB2C1020 > $DCC_PATH/config
    echo 0xB2D1020 > $DCC_PATH/config
    echo 0xB2C1024 > $DCC_PATH/config
    echo 0xB2D1024 > $DCC_PATH/config
    echo 0xB2C1030 > $DCC_PATH/config
    echo 0xB2D1030 > $DCC_PATH/config
    echo 0x22220404 > $DCC_PATH/config
    echo 0x22220408 > $DCC_PATH/config
    echo 0x22220204 > $DCC_PATH/config
    echo 0x22220224 > $DCC_PATH/config
    echo 0x22220244 > $DCC_PATH/config
    echo 0x22220264 > $DCC_PATH/config
    echo 0x22220284 > $DCC_PATH/config
    echo 0x22220208 > $DCC_PATH/config
    echo 0x22220228 > $DCC_PATH/config
    echo 0x22220248 > $DCC_PATH/config
    echo 0x22220268 > $DCC_PATH/config
    echo 0x22220288 > $DCC_PATH/config
    echo 0x2222020C > $DCC_PATH/config
    echo 0x2222022C > $DCC_PATH/config
    echo 0x2222024C > $DCC_PATH/config
    echo 0x2222026C > $DCC_PATH/config
    echo 0x2222028C > $DCC_PATH/config
    echo 0x22200404 > $DCC_PATH/config
    echo 0x22200408 > $DCC_PATH/config
    echo 0x22200204 > $DCC_PATH/config
    echo 0x22200224 > $DCC_PATH/config
    echo 0x22200244 > $DCC_PATH/config
    echo 0x22200264 > $DCC_PATH/config
    echo 0x22200284 > $DCC_PATH/config
    echo 0x22210204 > $DCC_PATH/config
    echo 0x22210224 > $DCC_PATH/config
    echo 0x22210244 > $DCC_PATH/config
    echo 0x22210264 > $DCC_PATH/config
    echo 0x22210284 > $DCC_PATH/config

    echo 0x22200208 > $DCC_PATH/config
    echo 0x22200228 > $DCC_PATH/config
    echo 0x22200248 > $DCC_PATH/config
    echo 0x22200268 > $DCC_PATH/config
    echo 0x22200288 > $DCC_PATH/config
    echo 0x22210208 > $DCC_PATH/config
    echo 0x22210228 > $DCC_PATH/config
    echo 0x22210248 > $DCC_PATH/config
    echo 0x22210268 > $DCC_PATH/config
    echo 0x22210288 > $DCC_PATH/config
    echo 0x2220020C > $DCC_PATH/config
    echo 0x2220022C > $DCC_PATH/config
    echo 0x2220024C > $DCC_PATH/config
    echo 0x2220026C > $DCC_PATH/config
    echo 0x2220028C > $DCC_PATH/config
    echo 0x2221020C > $DCC_PATH/config
    echo 0x2221022C > $DCC_PATH/config
    echo 0x2221024C > $DCC_PATH/config
    echo 0x2221026C > $DCC_PATH/config
    echo 0x2221028C > $DCC_PATH/config
    echo 0x22210D18 > $DCC_PATH/config

    echo 0x20C200F0 > $DCC_PATH/config
    echo 0x20C200F4 > $DCC_PATH/config
    echo 0x20C200F8 > $DCC_PATH/config
    echo 0x20C200FC > $DCC_PATH/config
    echo 0x20C20100 > $DCC_PATH/config
    echo 0x20C20104 > $DCC_PATH/config
    echo 0x20C20108 > $DCC_PATH/config
    echo 0x20C2010C > $DCC_PATH/config

    echo 0xB2C1204 > $DCC_PATH/config
    echo 0xB2C1218 > $DCC_PATH/config
    echo 0xB2C122C > $DCC_PATH/config
    echo 0xB2C1240 > $DCC_PATH/config
    echo 0xB2C1254 > $DCC_PATH/config

    echo 0xB2D1204 > $DCC_PATH/config
    echo 0xB2D1218 > $DCC_PATH/config
    echo 0xB2D122C > $DCC_PATH/config
    echo 0xB2D1240 > $DCC_PATH/config
    echo 0xB2D1254 > $DCC_PATH/config

    echo 0xB2C1208 > $DCC_PATH/config
    echo 0xB2C121C > $DCC_PATH/config
    echo 0xB2C1230 > $DCC_PATH/config
    echo 0xB2C1244 > $DCC_PATH/config
    echo 0xB2C1258 > $DCC_PATH/config

    echo 0xB2D1208 > $DCC_PATH/config
    echo 0xB2D121C > $DCC_PATH/config
    echo 0xB2D1230 > $DCC_PATH/config
    echo 0xB2D1244 > $DCC_PATH/config
    echo 0xB2D1258 > $DCC_PATH/config
}

config_dcc_rpmh()
{
    echo 0xB200010 > $DCC_PATH/config
    echo 0xB200110 > $DCC_PATH/config
    echo 0xB200900 > $DCC_PATH/config
    echo 0xB201020 > $DCC_PATH/config
    echo 0xB201030 > $DCC_PATH/config
    echo 0xB20103C > $DCC_PATH/config
    echo 0xB201200 > $DCC_PATH/config
    echo 0xB201204 > $DCC_PATH/config
    echo 0xB201208 > $DCC_PATH/config
    echo 0xB204510 > $DCC_PATH/config
    echo 0xB204514 > $DCC_PATH/config
    echo 0xB204520 > $DCC_PATH/config
}

config_dcc_ddr()
{
    #SHRM CSR
    echo 0x1908E008 1 > $DCC_PATH/config
    #MCCC
    echo 0x190BA280 1 > $DCC_PATH/config
    echo 0x192E0610 1 > $DCC_PATH/config
    echo 0x190BA288 8 > $DCC_PATH/config
    echo 0x192E0614 3 > $DCC_PATH/config
    #LLCC/MC5
    echo 0x1A600004 2 > $DCC_PATH/config
    echo 0x19E38100 1 > $DCC_PATH/config
    echo 0x19E420B0 1 > $DCC_PATH/config
    echo 0x19E42044 4 > $DCC_PATH/config
    echo 0x19E4C030 2 > $DCC_PATH/config
    echo 0x19E4C04C 1 > $DCC_PATH/config
    echo 0x19E4C10C 2 > $DCC_PATH/config
    echo 0x19E50020 1 > $DCC_PATH/config
    echo 0x19E6005C 5 > $DCC_PATH/config
    echo 0x192C0410 3 > $DCC_PATH/config
    echo 0x192C0420 2 > $DCC_PATH/config
    echo 0x192C0430 1 > $DCC_PATH/config
    echo 0x192C0440 1 > $DCC_PATH/config
    echo 0x192C0448 1 > $DCC_PATH/config
    echo 0x192C04A0 1 > $DCC_PATH/config
    echo 0x192C04B0 1 > $DCC_PATH/config
    echo 0x1A038100 1 > $DCC_PATH/config
    echo 0x1A0420B0 1 > $DCC_PATH/config
    echo 0x1A042044 4 > $DCC_PATH/config
    echo 0x1A04C030 2 > $DCC_PATH/config
    echo 0x1A04C04C 1 > $DCC_PATH/config
    echo 0x1A04C10C 2 > $DCC_PATH/config
    echo 0x1A050020 1 > $DCC_PATH/config
    echo 0x1A06005C 5 > $DCC_PATH/config
    echo 0x196C0410 3 > $DCC_PATH/config
    echo 0x196C0420 2 > $DCC_PATH/config
    echo 0x196C0430 1 > $DCC_PATH/config
    echo 0x196C0440 1 > $DCC_PATH/config
    echo 0x196C0448 1 > $DCC_PATH/config
    echo 0x196C04A0 1 > $DCC_PATH/config
    echo 0x196C04B0 1 > $DCC_PATH/config
    echo 0x1A238100 1 > $DCC_PATH/config
    echo 0x1A2420B0 1 > $DCC_PATH/config
    echo 0x1A242044 4 > $DCC_PATH/config
    echo 0x1A24C030 2 > $DCC_PATH/config
    echo 0x1A24C04C 1 > $DCC_PATH/config
    echo 0x1A24C10C 2 > $DCC_PATH/config
    echo 0x1A250020 1 > $DCC_PATH/config
    echo 0x1A26005C 5 > $DCC_PATH/config
    echo 0x193C0410 3 > $DCC_PATH/config
    echo 0x193C0420 2 > $DCC_PATH/config
    echo 0x193C0430 1 > $DCC_PATH/config
    echo 0x193C0440 1 > $DCC_PATH/config
    echo 0x193C0448 1 > $DCC_PATH/config
    echo 0x193C04A0 1 > $DCC_PATH/config
    echo 0x193C04B0 1 > $DCC_PATH/config
    echo 0x1A438100 1 > $DCC_PATH/config
    echo 0x1A4420B0 1 > $DCC_PATH/config
    echo 0x1A442044 4 > $DCC_PATH/config
    echo 0x1A44C030 2 > $DCC_PATH/config
    echo 0x1A44C04C 1 > $DCC_PATH/config
    echo 0x1A44C10C 2 > $DCC_PATH/config
    echo 0x1A450020 1 > $DCC_PATH/config
    echo 0x1A46005C 5 > $DCC_PATH/config
    echo 0x197C0410 3 > $DCC_PATH/config
    echo 0x197C0420 2 > $DCC_PATH/config
    echo 0x197C0430 1 > $DCC_PATH/config
    echo 0x197C0440 1 > $DCC_PATH/config
    echo 0x197C0448 1 > $DCC_PATH/config
    echo 0x197C04A0 1 > $DCC_PATH/config
    echo 0x197C04B0 1 > $DCC_PATH/config
    echo 0x1A600000 1 > $DCC_PATH/config
    echo 0x1A600004 1 > $DCC_PATH/config
    echo 0x1A601000 1 > $DCC_PATH/config
    echo 0x1A601004 1 > $DCC_PATH/config
    echo 0x1A602000 1 > $DCC_PATH/config
    echo 0x1A602004 1 > $DCC_PATH/config
    echo 0x1A603000 1 > $DCC_PATH/config
    echo 0x1A603004 1 > $DCC_PATH/config
    echo 0x1A604000 1 > $DCC_PATH/config
    echo 0x1A604004 1 > $DCC_PATH/config
    echo 0x1A605000 1 > $DCC_PATH/config
    echo 0x1A605004 1 > $DCC_PATH/config
    echo 0x1A606000 1 > $DCC_PATH/config
    echo 0x1A606004 1 > $DCC_PATH/config
    echo 0x1A607000 1 > $DCC_PATH/config
    echo 0x1A607004 1 > $DCC_PATH/config
    echo 0x1A608000 1 > $DCC_PATH/config
    echo 0x1A608004 1 > $DCC_PATH/config
    echo 0x1A609000 1 > $DCC_PATH/config
    echo 0x1A609004 1 > $DCC_PATH/config
    echo 0x1A60A000 1 > $DCC_PATH/config
    echo 0x1A60A004 1 > $DCC_PATH/config
    echo 0x1A60B000 1 > $DCC_PATH/config
    echo 0x1A60B004 1 > $DCC_PATH/config
    echo 0x1A60C000 1 > $DCC_PATH/config
    echo 0x1A60C004 1 > $DCC_PATH/config
    echo 0x1A60D000 1 > $DCC_PATH/config
    echo 0x1A60D004 1 > $DCC_PATH/config
    echo 0x1A60E000 1 > $DCC_PATH/config
    echo 0x1A60E004 1 > $DCC_PATH/config
    echo 0x1A60F000 1 > $DCC_PATH/config
    echo 0x1A60F004 1 > $DCC_PATH/config
    echo 0x1A610000 1 > $DCC_PATH/config
    echo 0x1A610004 1 > $DCC_PATH/config
    echo 0x1A611000 1 > $DCC_PATH/config
    echo 0x1A611004 1 > $DCC_PATH/config
    echo 0x1A612000 1 > $DCC_PATH/config
    echo 0x1A612004 1 > $DCC_PATH/config
    echo 0x1A613000 1 > $DCC_PATH/config
    echo 0x1A613004 1 > $DCC_PATH/config
    echo 0x1A614000 1 > $DCC_PATH/config
    echo 0x1A614004 1 > $DCC_PATH/config
    echo 0x1A615000 1 > $DCC_PATH/config
    echo 0x1A615004 1 > $DCC_PATH/config
    echo 0x1A616000 1 > $DCC_PATH/config
    echo 0x1A616004 1 > $DCC_PATH/config
    echo 0x1A617000 1 > $DCC_PATH/config
    echo 0x1A617004 1 > $DCC_PATH/config
    echo 0x1A618000 1 > $DCC_PATH/config
    echo 0x1A618004 1 > $DCC_PATH/config
    echo 0x1A619000 1 > $DCC_PATH/config
    echo 0x1A619004 1 > $DCC_PATH/config
    echo 0x1A61A000 1 > $DCC_PATH/config
    echo 0x1A61A004 1 > $DCC_PATH/config
    echo 0x1A61B000 1 > $DCC_PATH/config
    echo 0x1A61B004 1 > $DCC_PATH/config
    echo 0x1A61C000 1 > $DCC_PATH/config
    echo 0x1A61C004 1 > $DCC_PATH/config
    echo 0x1A61D000 1 > $DCC_PATH/config
    echo 0x1A61D004 1 > $DCC_PATH/config
    echo 0x1A61E000 1 > $DCC_PATH/config
    echo 0x1A61E004 1 > $DCC_PATH/config
    echo 0x1A61F000 1 > $DCC_PATH/config
    echo 0x1A61F004 1 > $DCC_PATH/config
    echo 0x192C6418 2 > $DCC_PATH/config
    echo 0x192C5804 1 > $DCC_PATH/config
    echo 0x192C590C 1 > $DCC_PATH/config
    echo 0x192C5A14 1 > $DCC_PATH/config
    echo 0x192C04B8 1 > $DCC_PATH/config
    #PHY
    echo 0x192856D4 4 > $DCC_PATH/config
    echo 0x19281B1C 3 > $DCC_PATH/config
    echo 0x19283B1C 3 > $DCC_PATH/config
    echo 0x19285188 3 > $DCC_PATH/config
    echo 0x19281DB4 1 > $DCC_PATH/config
    echo 0x19281DBC 1 > $DCC_PATH/config
    echo 0x19281DC4 1 > $DCC_PATH/config
    echo 0x19283DB4 1 > $DCC_PATH/config
    echo 0x19283DBC 1 > $DCC_PATH/config
    echo 0x19283DC4 1 > $DCC_PATH/config
    echo 0x1928583C 1 > $DCC_PATH/config
    echo 0x1928581C 1 > $DCC_PATH/config
    echo 0x192857FC 1 > $DCC_PATH/config
    echo 0x19285830 1 > $DCC_PATH/config
    echo 0x19285840 1 > $DCC_PATH/config
    echo 0x19285848 1 > $DCC_PATH/config
    echo 0x19285810 1 > $DCC_PATH/config
    echo 0x19285258 1 > $DCC_PATH/config
    echo 0x19281F20 2 > $DCC_PATH/config
    echo 0x19281E6C 1 > $DCC_PATH/config
    echo 0x19283E6C 1 > $DCC_PATH/config
    #SHKE
    echo 0x192C5240 2 > $DCC_PATH/config
    echo 0x192C5C1C 1 > $DCC_PATH/config
    echo 0x192C5C20 1 > $DCC_PATH/config
    echo 0x192C5C24 1 > $DCC_PATH/config
    #SHRM
    echo 0x19032020 1 > $DCC_PATH/config
    echo 0x19032024 1 > $DCC_PATH/config
    echo 0x1908E01C 1 > $DCC_PATH/config
    echo 0x1908E030 1 > $DCC_PATH/config
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1908E008 1 > $DCC_PATH/config
    echo 0x19032020 1 > $DCC_PATH/config
    echo 0x1908E948 1 > $DCC_PATH/config
    echo 0x19032024 1 > $DCC_PATH/config
    echo 0x19030040 0x1 > $DCC_PATH/config_write
    echo 0x1903005C 0x22C000 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C001 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C002 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C003 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C004 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C005 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C006 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C007 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C008 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C009 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C00A > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C00B > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C00C > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C00D > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C00E > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C00F > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C010 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C011 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C012 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C013 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C014 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C015 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C016 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C017 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C018 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C019 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C01A > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C01B > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C01C > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C01D > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C01E > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C01F > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C300 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C341 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config
    echo 0x1903005C 0x22C7B1 > $DCC_PATH/config_write
    echo 0x19030010 1 > $DCC_PATH/config

    echo 0x192C0400 > $DCC_PATH/config
    echo 0x196C0400 > $DCC_PATH/config
    echo 0x193C0400 > $DCC_PATH/config
    echo 0x197C0400 > $DCC_PATH/config

    echo 0x192C04B8 > $DCC_PATH/config
    echo 0x196C04B8 > $DCC_PATH/config
    echo 0x193C04B8 > $DCC_PATH/config
    echo 0x197C04B8 > $DCC_PATH/config

    echo 0x19E6004C 4 > $DCC_PATH/config
    echo 0x1A06004C 4 > $DCC_PATH/config
    echo 0x1A26004C 4 > $DCC_PATH/config
    echo 0x1A46004C 4 > $DCC_PATH/config

    echo 0x192C1420 > $DCC_PATH/config
    echo 0x196C1420 > $DCC_PATH/config
    echo 0x193C1420 > $DCC_PATH/config
    echo 0x197C1420 > $DCC_PATH/config

    echo 0x192C1430 > $DCC_PATH/config
    echo 0x196C1430 > $DCC_PATH/config
    echo 0x193C1430 > $DCC_PATH/config
    echo 0x197C1430 > $DCC_PATH/config
}

config_dcc_misc()
{
    # WDOG_BITE_INT0_CONFIG
    echo 0x17400038 > $DCC_PATH/config
    # EPSSTOP_MUC_HANG_DET_CTRL
    echo 0x17d98010 > $DCC_PATH/config
    # SOC_HW_VERSION
    echo 0x1fc8000 > $DCC_PATH/config

    # QFPROM_CORR_PTE_ROW1_MSB register
    echo 0x221c20a4 > $DCC_PATH/config

    echo 0xC430000 0x40 > $DCC_PATH/config_write
    echo 0xC430028 > $DCC_PATH/config
    echo 0xC430000 0x140 > $DCC_PATH/config_write
    echo 0xC430028 > $DCC_PATH/config
    echo 0xC430000 0x80 > $DCC_PATH/config_write
    echo 0xC430028 > $DCC_PATH/config
    echo 0xC430000 0xC0 > $DCC_PATH/config_write
    echo 0xC430028 > $DCC_PATH/config
    echo 0xC430000 0x100 > $DCC_PATH/config_write
    echo 0xC430028 > $DCC_PATH/config
    echo 0xC430000 0x41 > $DCC_PATH/config_write
    echo 0xC430028 > $DCC_PATH/config
    echo 0xC430000 0x141 > $DCC_PATH/config_write
    echo 0xC430028 > $DCC_PATH/config
    echo 0xC430000 0x81 > $DCC_PATH/config_write
    echo 0xC430028 > $DCC_PATH/config
    echo 0xC430000 0xC1 > $DCC_PATH/config_write
    echo 0xC430028 > $DCC_PATH/config
    echo 0xC430000 0x101 > $DCC_PATH/config_write
    echo 0xC430028 > $DCC_PATH/config
    echo 0xC430000 0x42 > $DCC_PATH/config_write
    echo 0xC430028 > $DCC_PATH/config
    echo 0xC430000 0x142 > $DCC_PATH/config_write
    echo 0xC430028 > $DCC_PATH/config
    echo 0xC430000 0x82 > $DCC_PATH/config_write
    echo 0xC430028 > $DCC_PATH/config
    echo 0xC430000 0xC2 > $DCC_PATH/config_write
    echo 0xC430028 > $DCC_PATH/config
    echo 0xC430000 0x102 > $DCC_PATH/config_write
    echo 0xC430028 > $DCC_PATH/config
}

config_dcc_adsp()
{
    echo 0x30b0208 1 > $DCC_PATH/config
    echo 0x30b0228 1 > $DCC_PATH/config
    echo 0x30b0248 1 > $DCC_PATH/config
    echo 0x30b0268 1 > $DCC_PATH/config
    echo 0x30b0288 1 > $DCC_PATH/config
    echo 0x30b02a8 1 > $DCC_PATH/config
    echo 0x30b020c 1 > $DCC_PATH/config
    echo 0x30b022c 1 > $DCC_PATH/config
    echo 0x30b024c 1 > $DCC_PATH/config
    echo 0x30b026c 1 > $DCC_PATH/config
    echo 0x30b028c 1 > $DCC_PATH/config
    echo 0x30b02ac 1 > $DCC_PATH/config
    echo 0x30b0210 1 > $DCC_PATH/config
    echo 0x30b0230 1 > $DCC_PATH/config
    echo 0x30b0250 1 > $DCC_PATH/config
    echo 0x30b0270 1 > $DCC_PATH/config
    echo 0x30b0290 1 > $DCC_PATH/config
    echo 0x30b02b0 1 > $DCC_PATH/config
    echo 0x30b0400 1 > $DCC_PATH/config
    echo 0x30b0404 1 > $DCC_PATH/config
    echo 0x30b0408 1 > $DCC_PATH/config
    echo 0x3480400 1 > $DCC_PATH/config
    echo 0x3480404 1 > $DCC_PATH/config
    echo 0x3480408 1 > $DCC_PATH/config
    echo 0x3002028 1 > $DCC_PATH/config
    echo 0x3000304 1 > $DCC_PATH/config
}
config_dcc_wpss()
{
    echo 0x323b0208 1 > $DCC_PATH/config
    echo 0x323b0228 1 > $DCC_PATH/config
    echo 0x323b0248 1 > $DCC_PATH/config
    echo 0x323b0268 1 > $DCC_PATH/config
    echo 0x323b0288 1 > $DCC_PATH/config
    echo 0x323b02a8 1 > $DCC_PATH/config
    echo 0x323b020c 1 > $DCC_PATH/config
    echo 0x323b022c 1 > $DCC_PATH/config
    echo 0x323b024c 1 > $DCC_PATH/config
    echo 0x323b026c 1 > $DCC_PATH/config
    echo 0x323b028c 1 > $DCC_PATH/config
    echo 0x323b02ac 1 > $DCC_PATH/config
    echo 0x323b0210 1 > $DCC_PATH/config
    echo 0x323b0230 1 > $DCC_PATH/config
    echo 0x323b0250 1 > $DCC_PATH/config
    echo 0x323b0270 1 > $DCC_PATH/config
    echo 0x323b0290 1 > $DCC_PATH/config
    echo 0x323b02b0 1 > $DCC_PATH/config
    echo 0x323b0400 1 > $DCC_PATH/config
    echo 0x323b0404 1 > $DCC_PATH/config
    echo 0x323b0408 1 > $DCC_PATH/config
    echo 0x320a4400 1 > $DCC_PATH/config
    echo 0x320a4404 1 > $DCC_PATH/config
    echo 0x320a4408 1 > $DCC_PATH/config
    echo 0x32302028 1 > $DCC_PATH/config
    echo 0x32300304 1 > $DCC_PATH/config
}
config_dcc_epss()
{
    echo 0x17d80100 200 > $DCC_PATH/config

    echo 0x17D9001C 1 > $DCC_PATH/config
    echo 0x17D900DC 1 > $DCC_PATH/config
    echo 0x17D900E8 1 > $DCC_PATH/config
    echo 0x17D90320 1 > $DCC_PATH/config
    echo 0x17D9101C 1 > $DCC_PATH/config
    echo 0x17D910DC 1 > $DCC_PATH/config
    echo 0x17D910E8 1 > $DCC_PATH/config
    echo 0x17D91320 1 > $DCC_PATH/config
    echo 0x17D9201C 1 > $DCC_PATH/config
    echo 0x17D920DC 1 > $DCC_PATH/config
    echo 0x17D920E8 1 > $DCC_PATH/config
    echo 0x17D92320 1 > $DCC_PATH/config

    echo 0x17D9034C 1 > $DCC_PATH/config
    echo 0x17D9134C 1 > $DCC_PATH/config
    echo 0x17D9234C 1 > $DCC_PATH/config
    echo 0x17880054 1 > $DCC_PATH/config
    echo 0x17880000 1 > $DCC_PATH/config

    echo 0x17A80000 1 > $DCC_PATH/config

    echo 0x17d80000 3 > $DCC_PATH/config
    echo 0x17d80010 2 > $DCC_PATH/config
    echo 0x17d90000 4 > $DCC_PATH/config
    echo 0x17d90014 2 > $DCC_PATH/config
    echo 0x17d90024 22 > $DCC_PATH/config
    echo 0x17d90080 5 > $DCC_PATH/config
    echo 0x17d900b0 1 > $DCC_PATH/config
    echo 0x17d900b8 2 > $DCC_PATH/config
    echo 0x17d900d0 3 > $DCC_PATH/config
    echo 0x17d900e0 2 > $DCC_PATH/config
    echo 0x17d900ec 2 > $DCC_PATH/config
    echo 0x17d90100 40 > $DCC_PATH/config
    echo 0x17d90200 40 > $DCC_PATH/config
    echo 0x17d90304 4 > $DCC_PATH/config
    echo 0x17d90350 30 > $DCC_PATH/config
    echo 0x17d903e0 2 > $DCC_PATH/config
    echo 0x17d90404 1 > $DCC_PATH/config
    echo 0x17d91000 2 > $DCC_PATH/config
    echo 0x17d91014 2 > $DCC_PATH/config
    echo 0x17d91024 22 > $DCC_PATH/config
    echo 0x17d91080 6 > $DCC_PATH/config
    echo 0x17d910b0 1 > $DCC_PATH/config
    echo 0x17d910b8 2 > $DCC_PATH/config
    echo 0x17d910d0 3 > $DCC_PATH/config
    echo 0x17d910e0 2 > $DCC_PATH/config
    echo 0x17d910ec 2 > $DCC_PATH/config
    echo 0x17d91100 40 > $DCC_PATH/config
    echo 0x17d91200 40 > $DCC_PATH/config
    echo 0x17d91304 4 > $DCC_PATH/config
    echo 0x17d91324 1 > $DCC_PATH/config
    echo 0x17d91350 32 > $DCC_PATH/config
    echo 0x17d913e0 3 > $DCC_PATH/config
    echo 0x17d91404 1 > $DCC_PATH/config
    echo 0x17d92000 4 > $DCC_PATH/config
    echo 0x17d92014 2 > $DCC_PATH/config
    echo 0x17d92024 22 > $DCC_PATH/config
    echo 0x17d92080 7 > $DCC_PATH/config
    echo 0x17d920b0 1 > $DCC_PATH/config
    echo 0x17d920b8 2 > $DCC_PATH/config
    echo 0x17d920d0 3 > $DCC_PATH/config
    echo 0x17d920e0 2 > $DCC_PATH/config
    echo 0x17d920ec 2 > $DCC_PATH/config
    echo 0x17d92100 40 > $DCC_PATH/config
    echo 0x17d92200 40 > $DCC_PATH/config
    echo 0x17d92304 4 > $DCC_PATH/config
    echo 0x17d92324 2 > $DCC_PATH/config
    echo 0x17d92350 33 > $DCC_PATH/config
    echo 0x17d923e0 4 > $DCC_PATH/config
    echo 0x17d92404 1 > $DCC_PATH/config
    echo 0x17d98000 8 > $DCC_PATH/config
    echo 0x17d98020 2 > $DCC_PATH/config
}

config_dcc_qdss()
{
    echo 0x12822000 > $DCC_PATH/config
    echo 0x12822004 > $DCC_PATH/config
    echo 0x12824C00 > $DCC_PATH/config
    echo 0x12824D04 > $DCC_PATH/config
    echo 0x12824D08 > $DCC_PATH/config
    echo 0x1282000C > $DCC_PATH/config
    echo 0x1282802C > $DCC_PATH/config
    echo 0x1282D004 > $DCC_PATH/config
    echo 0x1282D008 > $DCC_PATH/config
    echo 0x1282D00C > $DCC_PATH/config
    echo 0x1282D010 > $DCC_PATH/config
    echo 0x1282D018 > $DCC_PATH/config
    echo 0x1282D01C > $DCC_PATH/config
    echo 0x1282D020 > $DCC_PATH/config
    echo 0x1282D024 > $DCC_PATH/config
    echo 0x1282D028 > $DCC_PATH/config
}

config_dcc_lpass_rscc()
{
    echo 0x348020C > $DCC_PATH/config
    echo 0x348022C > $DCC_PATH/config
    echo 0x348024C > $DCC_PATH/config
    echo 0x348026C > $DCC_PATH/config
    echo 0x3480210 > $DCC_PATH/config
    echo 0x3480230 > $DCC_PATH/config
    echo 0x3480250 > $DCC_PATH/config
    echo 0x3480270 > $DCC_PATH/config
    echo 0x3480208 > $DCC_PATH/config
    echo 0x3480228 > $DCC_PATH/config
    echo 0x3480248 > $DCC_PATH/config
    echo 0x3480268 > $DCC_PATH/config
}

config_dcc_aoss_pmic()
{
    echo 0xC2A8064 > $DCC_PATH/config
    echo 0xC2A8068 > $DCC_PATH/config
    echo 0xC2A806C > $DCC_PATH/config
}
enable_dcc()
{
    DCC_PATH="/sys/bus/platform/devices/100ff000.dcc_v2"
    soc_version=`cat /sys/devices/soc0/revision`
    soc_version=${soc_version/./}

    if [ ! -d $DCC_PATH ]; then
        echo "DCC does not exist on this build."
        return
    fi

    echo 0 > $DCC_PATH/enable
    echo 1 > $DCC_PATH/config_reset
    echo 6 > $DCC_PATH/curr_list
    echo cap > $DCC_PATH/func_type
    echo sram > $DCC_PATH/data_sink

    config_dcc_tlb
    config_dcc_thermal
    config_dcc_core
    config_dcc_lpm_pcu
    config_dcc_rpmh
    config_dcc_apss_rscc
    config_dcc_aoss_pmic
    #config_dcc_epss
    config_dcc_misc
    config_dcc_ddr
    config_dcc_gpu
    config_dcc_epss
    config_dcc_clk

    echo 4 > $DCC_PATH/curr_list
    echo cap > $DCC_PATH/func_type
    echo sram > $DCC_PATH/data_sink

    config_dcc_tlb
    config_lcp_dare
    config_dcc_ddr
    config_dcc_gic
    config_dcc_gcc
    config_dcc_smmu
    config_dcc_gemnoc
    config_dcc_cnoc
    config_dcc_snoc
    config_dcc_a1_aggre_noc
    config_dcc_a2_aggre_noc
    config_dcc_pcie_aggre_noc
    config_dcc_mmnoc
    config_dcc_lpass_ag_noc
    config_dcc_dc_dch_noc
    config_dcc_dc_ch02_noc
    config_dcc_dc_ch13_noc
    config_dcc_turing_noc
    config_dcc_adsp
    config_dcc_wpss
    config_dcc_qdss
    config_dcc_lpass_rscc
    config_dcc_tlb

    echo  1 > $DCC_PATH/enable
}

enable_cpuss_register()
{
}

#function to enable cti flush for etr
enable_cti_flush_for_etr()
{
    # bail out if its perf config
    if [ "$debug_build" = false ]
    then
        return
    fi

    echo 1 > /sys/bus/coresight/devices/coresight-cti-qc_cti/enable
    echo 0 3 > /sys/bus/coresight/devices/coresight-cti-qc_cti/channels/trigout_attach
    echo 0 53 > /sys/bus/coresight/devices/coresight-cti-qc_cti/channels/trigin_attach
}

find_build_type()
{
    linux_banner=`cat /proc/version`
    if [[ "$linux_banner" == *"-consolidate"* ]]
    then
       debug_build=true
    fi
}

ftrace_disable=`getprop persist.debug.ftrace_events_disable`
tracefs=/sys/kernel/tracing
debug_build=false
enable_debug()
{
    echo "anorak61 debug"
    find_build_type
    etr_size="0x2000000"
    srcenable="enable_source"
    sinkenable="enable_sink"
    create_stp_policy
    echo "Enabling STM events on anorak61."
    adjust_permission
    enable_stm_events
    enable_cti_flush_for_etf
    enable_cti_flush_for_etr
    if [ "$ftrace_disable" != "Yes" ]; then
        enable_ftrace_event_tracing
    fi

    enable_dcc
    enable_cpuss_register
    enable_memory_debug
    enable_schedstats
    setprop ro.dbg.coresight.stm_cfg_done 1
    if [ -d $tracefs ] && [ "$(getprop persist.vendor.tracing.enabled)" -eq "1" ]; then
        mkdir $tracefs/instances/hsuart
        #UART
        echo 800 > $tracefs/instances/hsuart/buffer_size_kb
        echo 1 > $tracefs/instances/hsuart/events/serial/enable
        echo 1 > $tracefs/instances/hsuart/tracing_on

        #SPI
        mkdir $tracefs/instances/spi_qup
        echo 20 > $tracefs/instances/spi_qup/buffer_size_kb
        echo 1 > $tracefs/instances/spi_qup/events/qup_spi_trace/enable
        echo 1 > $tracefs/instances/spi_qup/tracing_on

        #I2C
        mkdir $tracefs/instances/i2c_qup
        echo 20 > $tracefs/instances/i2c_qup/buffer_size_kb
        echo 1 > $tracefs/instances/i2c_qup/events/qup_i2c_trace/enable
        echo 1 > $tracefs/instances/i2c_qup/tracing_on

        #GENI_COMMON
        mkdir $tracefs/instances/qupv3_common
        echo 20 > $tracefs/instances/qupv3_common/buffer_size_kb
        echo 1 > $tracefs/instances/qupv3_common/events/qup_common_trace/enable
        echo 1 > $tracefs/instances/qupv3_common/tracing_on

        #SLIMBUS
        mkdir $tracefs/instances/slimbus
        echo 20 > $tracefs/instances/slimbus/buffer_size_kb
        echo 1 > $tracefs/instances/slimbus/events/slimbus/slimbus_dbg/enable
        echo 1 > $tracefs/instances/slimbus/tracing_on
    fi

}


enable_debug
