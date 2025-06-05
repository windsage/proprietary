#=============================================================================
# Copyright (c) 2020-2024 Qualcomm Technologies, Inc.
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
    # clock
    echo 1 > /sys/kernel/tracing/events/power/clock_set_rate/enable
    echo 1 > /sys/kernel/tracing/events/power/clock_enable/enable
    echo 1 > /sys/kernel/tracing/events/power/clock_disable/enable
    echo 1 > /sys/kernel/tracing/events/power/cpu_frequency/enable
    # regulator
    echo 1 > /sys/kernel/tracing/events/regulator/enable
    # fastrpc
    echo 1 > /sys/kernel/tracing/events/fastrpc/enable

    #EMMC
    echo 1 > /sys/kernel/tracing/events/mmc/mmc_request_start/enable
    echo 1 > /sys/kernel/tracing/events/mmc/mmc_request_done/enable

    echo 1 > /sys/kernel/tracing/tracing_on
}

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

   echo "configure trace sink settings"
   echo 1 > /sys/bus/coresight/reset_source_sink
   echo mem > /sys/bus/coresight/devices/coresight-tmc-etr/out_mode
   echo 1 > /sys/bus/coresight/devices/coresight-tmc-etr/enable_sink
   echo 0x2000000 > /sys/bus/coresight/devices/coresight-tmc-etr/buffer_size
   echo coresight-stm > /sys/class/stm_source/ftrace/stm_source_link
   echo 1 > /sys/bus/coresight/devices/coresight-stm/enable_source
   echo 1 > /sys/kernel/tracing/tracing_on
   echo 0 > /sys/bus/coresight/devices/coresight-stm/hwevent_enable
   enable_tracing_events
}

adjust_permission()
{
    #add permission for block_size, mem_type, mem_size nodes to collect diag over QDSS by ODL
    #application by "oem_2902" group
    chown -h root.oem_2902 /sys/devices/platform/soc/10048000.tmc/coresight-tmc-etr/block_size
    chmod 660 /sys/devices/platform/soc/10048000.tmc/coresight-tmc-etr/block_size
    chown -h root.oem_2902 /sys/devices/platform/soc/10048000.tmc/coresight-tmc-etr/buffer_size
    chmod 660 /sys/devices/platform/soc/10048000.tmc/coresight-tmc-etr/buffer_size
    chmod 660 /sys/devices/platform/soc/10048000.tmc/coresight-tmc-etr/out_mode
    chown -h root.oem_2902 /sys/devices/platform/soc/1004f000.tmc/coresight-tmc-etr1/block_size
    chmod 660 /sys/devices/platform/soc/1004f000.tmc/coresight-tmc-etr1/block_size
    chown -h root.oem_2902 /sys/devices/platform/soc/1004f000.tmc/coresight-tmc-etr1/buffer_size
    chmod 660 /sys/devices/platform/soc/1004f000.tmc/coresight-tmc-etr1/buffer_size
    chmod 660 /sys/devices/platform/soc/1004f000.tmc/coresight-tmc-etr1/out_mode
    chmod 660 /sys/devices/platform/soc/1004f000.tmc/coresight-tmc-etr1/enable_sink
    chown -h root.oem_2902 /sys/bus/coresight/reset_source_sink
    chmod 660 /sys/bus/coresight/reset_source_sink
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

    echo 1 > /sys/bus/coresight/devices/coresight-tmc-etf/stop_on_flush
    echo 1 >/sys/bus/coresight/devices/coresight-cti-swao_cti/enable
    echo 0 24 >/sys/bus/coresight/devices/coresight-cti-swao_cti/channels/trigin_attach
    echo 0 1 >/sys/bus/coresight/devices/coresight-cti-swao_cti/channels/trigout_attach
}

# function to enable ftrace events
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

    enable_tracing_events
}

enable_buses_and_interconnect_tracefs_debug()
{
    tracefs=/sys/kernel/tracing

    # enable tracing for consolidate/debug builds, where debug_build is set true
    if [ "$debug_build" = true ]
    then
        setprop persist.vendor.tracing.enabled 1
    fi

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

enable_core_hang_config()
{
    CORE_PATH="/sys/devices/system/cpu/hang_detect_core"
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

config_dcc_thermal()
{
    #TSENS
    echo 0x0C222004 1 > $DCC_PATH/config
    echo 0x0C228014 1 > $DCC_PATH/config
    echo 0x0C2280E0 1 > $DCC_PATH/config
    echo 0x0C2280EC 1 > $DCC_PATH/config
    echo 0x0C2280A0 16 > $DCC_PATH/config
    echo 0x0C2280E8 1 > $DCC_PATH/config
    echo 0x0C22813C 1 > $DCC_PATH/config
    echo 0x0C223004 1 > $DCC_PATH/config
    echo 0x0C229014 1 > $DCC_PATH/config
    echo 0x0C2290E0 1 > $DCC_PATH/config
    echo 0x0C2290EC 1 > $DCC_PATH/config
    echo 0x0C2290A0 16 > $DCC_PATH/config
    echo 0x0C2290E8 1 > $DCC_PATH/config
    echo 0x0C22913C 1 > $DCC_PATH/config
    echo 0x0C224004 1 > $DCC_PATH/config
    echo 0x0C22A014 1 > $DCC_PATH/config
    echo 0x0C22A0E0 1 > $DCC_PATH/config
    echo 0x0C22A0EC 1 > $DCC_PATH/config
    echo 0x0C22A0A0 16 > $DCC_PATH/config
    echo 0x0C22A0E8 1 > $DCC_PATH/config
    echo 0x0C22A13C 1 > $DCC_PATH/config

    #GoldP LLM
    echo 0x17B70220 6 > $DCC_PATH/config
    echo 0x17B702A0 6 > $DCC_PATH/config
    echo 0x17B70320 1 > $DCC_PATH/config
    echo 0x17B704A0 16 > $DCC_PATH/config
    echo 0x17B70520 1 > $DCC_PATH/config
    echo 0x17B70524 1 > $DCC_PATH/config
    echo 0x17B70588 1 > $DCC_PATH/config
    echo 0x17B70630 2 > $DCC_PATH/config
    echo 0x17B70638 2 > $DCC_PATH/config
    echo 0x17B70640 2 > $DCC_PATH/config
    echo 0x17B71010 24 > $DCC_PATH/config
    echo 0x17B71310 16 > $DCC_PATH/config
    echo 0x17B71390 16 > $DCC_PATH/config
    echo 0x17B72090 8 > $DCC_PATH/config

    #Gold LLM
    echo 0x17B744A0 16 > $DCC_PATH/config
    echo 0x17B74520 1 > $DCC_PATH/config
    echo 0x17B74588 1 > $DCC_PATH/config
    echo 0x17B74630 2 > $DCC_PATH/config
    echo 0x17B74638  2 > $DCC_PATH/config
    echo 0x17B74640 2 > $DCC_PATH/config
    echo 0x17B75010 24 > $DCC_PATH/config
    echo 0x17B75310 16 > $DCC_PATH/config
    echo 0x17B75390 16 > $DCC_PATH/config
    echo 0x17B76090 8 > $DCC_PATH/config

    #Turing LLM
    #echo 0x32310220 4 > $DCC_PATH/config
    #echo 0x323102A0 4 > $DCC_PATH/config
    #echo 0x323104A0 6 > $DCC_PATH/config
    #echo 0x32310520 1 > $DCC_PATH/config
    #echo 0x32310588 1 > $DCC_PATH/config
    #echo 0x32310630 2 > $DCC_PATH/config
    #echo 0x32310638 2 > $DCC_PATH/config
    #echo 0x32310640 2 > $DCC_PATH/config
    #echo 0x32311010 8 > $DCC_PATH/config
    #echo 0x32311310 6 > $DCC_PATH/config
    #echo 0x32311390 6 > $DCC_PATH/config
    #echo 0x32312090 3 > $DCC_PATH/config

    #Turing HMX LLM
    #echo 0x32318220 4 > $DCC_PATH/config
    #echo 0x323182A0 4 > $DCC_PATH/config
    #echo 0x323184A0 6 > $DCC_PATH/config
    #echo 0x32318520 1 > $DCC_PATH/config
    #echo 0x32318588 1 > $DCC_PATH/config
    #echo 0x32318630 2 > $DCC_PATH/config
    #echo 0x32318638 2 > $DCC_PATH/config
    #echo 0x32318640 2 > $DCC_PATH/config
    #echo 0x32319010 8 > $DCC_PATH/config
    #echo 0x32319310 6 > $DCC_PATH/config
    #echo 0x32319390  6 > $DCC_PATH/config
    #echo 0x3231A090  3 > $DCC_PATH/config

    #central broadcast
    echo 0xEC80010 1 > $DCC_PATH/config
    echo 0xEC81000 1 > $DCC_PATH/config
    echo 0xEC81010 16> $DCC_PATH/config
    #echo 0xEC81050 16 > $DCC_PATH/config
    echo 0xEC81050 4 > $DCC_PATH/config
    echo 0xEC81090 16 > $DCC_PATH/config
    echo 0xEC810D0 16 > $DCC_PATH/config
    echo 0xEC81110 16 > $DCC_PATH/config
    echo 0xEC81150 16 > $DCC_PATH/config
    echo 0xEC81550 1 > $DCC_PATH/config

    #HVX DPM
    #echo 0x3231C004 1 > $DCC_PATH/config
    #echo 0x3231C008 1 > $DCC_PATH/config
    #echo 0x3231C214 1 > $DCC_PATH/config
    #echo 0x3231C218 1 > $DCC_PATH/config

    #HMX DPM
    #echo 0x3231D004 1 > $DCC_PATH/config
    #echo 0x3231D008 1 > $DCC_PATH/config
    #echo 0x3231D214 1 > $DCC_PATH/config
    #echo 0x3231D218 1 > $DCC_PATH/config
}

config_dcc_timer()
{
    echo 0x17421000 2 > $DCC_PATH/config
}

config_dcc_core()
{
    #CORE_HANG_THRESHOLD
    echo 0x17800058 > $DCC_PATH/config
    echo 0x17810058 > $DCC_PATH/config
    echo 0x17820058 > $DCC_PATH/config
    echo 0x17830058 > $DCC_PATH/config
    echo 0x17840058 > $DCC_PATH/config
    echo 0x17850058 > $DCC_PATH/config
    #CORE_HANG_VALUE
    echo 0x1780005C > $DCC_PATH/config
    echo 0x1781005C > $DCC_PATH/config
    echo 0x1782005C > $DCC_PATH/config
    echo 0x1783005C > $DCC_PATH/config
    echo 0x1784005C > $DCC_PATH/config
    echo 0x1785005C > $DCC_PATH/config
    #CORE_HANG_CONFIG
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
    echo 0x240A0000 > $DCC_PATH/config
    echo 0x240A0004 > $DCC_PATH/config
    echo 0x240A1000 > $DCC_PATH/config
    echo 0x240A1004 > $DCC_PATH/config
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

config_dcc_lpm_pcu()
{
    echo 0x17800010  > $DCC_PATH/config
    echo 0x17800024  > $DCC_PATH/config
    echo 0x17800038 6 > $DCC_PATH/config
    echo 0x17800058 4 > $DCC_PATH/config
    echo 0x1780006C  > $DCC_PATH/config
    echo 0x178000F0 2 > $DCC_PATH/config
    echo 0x17810010  > $DCC_PATH/config
    echo 0x17810024  > $DCC_PATH/config
    echo 0x17810038 6 > $DCC_PATH/config
    echo 0x17810058 4 > $DCC_PATH/config
    echo 0x1781006C  > $DCC_PATH/config
    echo 0x178100F0 2 > $DCC_PATH/config
    echo 0x17820010  > $DCC_PATH/config
    echo 0x17820024  > $DCC_PATH/config
    echo 0x17820038 6 > $DCC_PATH/config
    echo 0x17820058 4 > $DCC_PATH/config
    echo 0x1782006C  > $DCC_PATH/config
    echo 0x178200F0 2 > $DCC_PATH/config
    echo 0x17830010  > $DCC_PATH/config
    echo 0x17830024  > $DCC_PATH/config
    echo 0x17830038 6 > $DCC_PATH/config
    echo 0x17830058 4 > $DCC_PATH/config
    echo 0x1783006C  > $DCC_PATH/config
    echo 0x178300F0 2 > $DCC_PATH/config
    echo 0x17840010  > $DCC_PATH/config
    echo 0x17840010  > $DCC_PATH/config
    echo 0x17840024  > $DCC_PATH/config
    echo 0x17840038 6 > $DCC_PATH/config
    echo 0x17840058 4 > $DCC_PATH/config
    echo 0x1784006C  > $DCC_PATH/config
    echo 0x178400F0 2 > $DCC_PATH/config
    echo 0x17850024  > $DCC_PATH/config
    echo 0x17850038 6 > $DCC_PATH/config
    echo 0x17850058 4 > $DCC_PATH/config
    echo 0x1785006C  > $DCC_PATH/config
    echo 0x178500F0 2 > $DCC_PATH/config
    echo 0x17880010  > $DCC_PATH/config
    echo 0x17880024  > $DCC_PATH/config
    echo 0x1788003C 5 > $DCC_PATH/config
    echo 0x1788006C 7 > $DCC_PATH/config
    echo 0x17880090 5 > $DCC_PATH/config
    echo 0x178800E0 23 > $DCC_PATH/config
    echo 0x17880140 2 > $DCC_PATH/config
    echo 0x1788019C 2 > $DCC_PATH/config
    echo 0x178801B4 6 > $DCC_PATH/config
    echo 0x178801F0 5 > $DCC_PATH/config
    echo 0x17880250 4 > $DCC_PATH/config
}

config_dcc_rpmh()
{
    echo 0xB200110  > $DCC_PATH/config
    echo 0xB200900  > $DCC_PATH/config
    echo 0xB201020 2 > $DCC_PATH/config
    echo 0xB201030  > $DCC_PATH/config
    echo 0xB20103C  > $DCC_PATH/config
    echo 0xB201200 3 > $DCC_PATH/config
    echo 0xB204510 2 > $DCC_PATH/config
    echo 0xB204520  > $DCC_PATH/config

    echo 0xB211024  > $DCC_PATH/config
    echo 0xB221024  > $DCC_PATH/config
    echo 0xB231024  > $DCC_PATH/config
}

config_dcc_apss_rscc()
{
    echo 0x17A00010  > $DCC_PATH/config
    echo 0x17A00030  > $DCC_PATH/config
    echo 0x17A00038  > $DCC_PATH/config
    echo 0x17A00040  > $DCC_PATH/config
    echo 0x17A00048  > $DCC_PATH/config
    echo 0x17A00400 3 > $DCC_PATH/config
    echo 0x17A00408  > $DCC_PATH/config
}

config_dcc_aoss_pmic()
{
    echo 0xC2A8070 > $DCC_PATH/config
    echo 0xC2A8074 > $DCC_PATH/config
    echo 0xC2A8078 > $DCC_PATH/config
}

config_dcc_gemnoc()
{
    #gem_noc_qns_*_poc_err
    echo 0x24102008  > $DCC_PATH/config
    echo 0x24102010  > $DCC_PATH/config
    echo 0x24102018  > $DCC_PATH/config
    echo 0x24102020 6 > $DCC_PATH/config
    echo 0x24102408  > $DCC_PATH/config
    echo 0x24102410  > $DCC_PATH/config
    echo 0x24102418  > $DCC_PATH/config
    echo 0x24102420 6 > $DCC_PATH/config
    echo 0x24142008  > $DCC_PATH/config
    echo 0x24142010  > $DCC_PATH/config
    echo 0x24142018  > $DCC_PATH/config
    echo 0x24142020 6 > $DCC_PATH/config
    echo 0x24142408  > $DCC_PATH/config
    echo 0x24142410  > $DCC_PATH/config
    echo 0x24142418  > $DCC_PATH/config
    echo 0x24142420 6 > $DCC_PATH/config
    echo 0x24182008  > $DCC_PATH/config
    echo 0x24182010  > $DCC_PATH/config
    echo 0x24182018  > $DCC_PATH/config
    echo 0x24182020 6 > $DCC_PATH/config
    echo 0x24182408  > $DCC_PATH/config
    echo 0x24182410  > $DCC_PATH/config
    echo 0x24182418  > $DCC_PATH/config
    echo 0x24182420 6 > $DCC_PATH/config

    #gem_noc_qns_*_poc_dbg_Cfg
    echo 0x24100808 2 > $DCC_PATH/config
    echo 0x24100C08 2 > $DCC_PATH/config
    echo 0x24140808 2 > $DCC_PATH/config
    echo 0x24140C08 2 > $DCC_PATH/config
    echo 0x24180008 2 > $DCC_PATH/config
    echo 0x24180408 2 > $DCC_PATH/config

    #gem_noc_fault_sbm_FaultInStatus0_Low
    echo 0x24183048 1 > $DCC_PATH/config

    #coherent even chain
    echo 0x24101008 1 > $DCC_PATH/config
    echo 0x4  > $DCC_PATH/loop
    echo 0x24101010 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop
    echo 0x24101018 1 > $DCC_PATH/config

    #coherent odd chain
    echo 0x24141008 1 > $DCC_PATH/config
    echo 0x4  > $DCC_PATH/loop
    echo 0x24141010 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop
    echo 0x24141018 1 > $DCC_PATH/config

    #coherent sys chain
    echo 0x24181008 1 > $DCC_PATH/config
    echo 0x3  > $DCC_PATH/loop
    echo 0x24181010 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop
    echo 0x24181018 1 > $DCC_PATH/config

    #llcc0 tmo
    echo 0x24100810 1 > $DCC_PATH/config
    echo 0x40  > $DCC_PATH/loop
    echo 0x24100838 1 > $DCC_PATH/config
    echo 0x24100830 2 > $DCC_PATH/config
    echo 0x24100830 2 > $DCC_PATH/config
    echo 0x24100830 2 > $DCC_PATH/config
    echo 0x24100830 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #llcc2 tmo
    echo 0x24100C10 1 > $DCC_PATH/config
    echo 0x40  > $DCC_PATH/loop
    echo 0x24100C38 1 > $DCC_PATH/config
    echo 0x24100C30 2 > $DCC_PATH/config
    echo 0x24100C30 2 > $DCC_PATH/config
    echo 0x24100C30 2 > $DCC_PATH/config
    echo 0x24100C30 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #llcc1 tmo
    echo 0x24140810 1 > $DCC_PATH/config
    echo 0x40  > $DCC_PATH/loop
    echo 0x24140838 1 > $DCC_PATH/config
    echo 0x24140830 2 > $DCC_PATH/config
    echo 0x24140830 2 > $DCC_PATH/config
    echo 0x24140830 2 > $DCC_PATH/config
    echo 0x24140830 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #llcc3 tmo
    echo 0x24140C10 1 > $DCC_PATH/config
    echo 0x40  > $DCC_PATH/loop
    echo 0x24140C38 1 > $DCC_PATH/config
    echo 0x24140C30 2 > $DCC_PATH/config
    echo 0x24140C30 2 > $DCC_PATH/config
    echo 0x24140C30 2 > $DCC_PATH/config
    echo 0x24140C30 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #cnoc tmo
    echo 0x24180010 1 > $DCC_PATH/config
    echo 0x10  > $DCC_PATH/loop
    echo 0x24180038 1 > $DCC_PATH/config
    echo 0x24180030 2 > $DCC_PATH/config
    echo 0x24180030 2 > $DCC_PATH/config
    echo 0x24180030 2 > $DCC_PATH/config
    echo 0x24180030 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #pcie tmo
    echo 0x24180410 1 > $DCC_PATH/config
    echo 0x10  > $DCC_PATH/loop
    echo 0x24180438 1 > $DCC_PATH/config
    echo 0x24180430 2 > $DCC_PATH/config
    echo 0x24180430 2 > $DCC_PATH/config
    echo 0x24180430 2 > $DCC_PATH/config
    echo 0x24180430 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    # gem_noc qosgen
    echo 0x24131010  > $DCC_PATH/config
    echo 0x24133010  > $DCC_PATH/config
    echo 0x24135010  > $DCC_PATH/config
    echo 0x24137010  > $DCC_PATH/config
    echo 0x24138010  > $DCC_PATH/config
    echo 0x24139010  > $DCC_PATH/config
    echo 0x24171010  > $DCC_PATH/config
    echo 0x24173010  > $DCC_PATH/config
    echo 0x24175010  > $DCC_PATH/config
    echo 0x24177010  > $DCC_PATH/config
    echo 0x24178010  > $DCC_PATH/config
    echo 0x24179010  > $DCC_PATH/config
    echo 0x241B5010  > $DCC_PATH/config
    echo 0x241B7010  > $DCC_PATH/config
    echo 0x241B9010  > $DCC_PATH/config
    echo 0x241BB010  > $DCC_PATH/config
    echo 0x241BC010  > $DCC_PATH/config
    echo 0x241BD010  > $DCC_PATH/config
    echo 0x241BF010  > $DCC_PATH/config
    echo 0x241C1010  > $DCC_PATH/config
}

config_dcc_a1_aggre_noc()
{
    #A1_NOC_aggre_noc_a1noc_ErrorLogger_erl_*
    echo 0x16E0008  > $DCC_PATH/config
    echo 0x16E0010  > $DCC_PATH/config
    echo 0x16E0018  > $DCC_PATH/config
    echo 0x16E0020 8 > $DCC_PATH/config

    #A1_NOC_aggre_noc_a1noc_sbm_sbm_FaultInStatus0_Low
    echo 0x16E0248  > $DCC_PATH/config

    #south debugchain
    echo 0x16E1008 1 > $DCC_PATH/config
    echo 0x3 > $DCC_PATH/loop
    echo 0x16E1010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x16E1018 1 > $DCC_PATH/config

    #west debugchain
    echo 0x16E1088 1 > $DCC_PATH/config
    echo 0x3 > $DCC_PATH/loop
    echo 0x16E1090 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x16E1098 1 > $DCC_PATH/config

    #debugchain ANOC QTB
    echo 0x16E1108 1 > $DCC_PATH/config
    echo 0x7 > $DCC_PATH/loop
    echo 0x16E1110 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x16E1118 1 > $DCC_PATH/config

    #debug chain ANOC NIU
    echo 0x16E3088 1 > $DCC_PATH/config
    echo 0x2 > $DCC_PATH/loop
    echo 0x16E3090 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x16E3098 1 > $DCC_PATH/config

    #qosgen
    echo 0x16EE010  > $DCC_PATH/config
    echo 0x16EF010  > $DCC_PATH/config
    echo 0x16F0010  > $DCC_PATH/config
    echo 0x16F2010  > $DCC_PATH/config
    echo 0x16F3010  > $DCC_PATH/config
}

config_dcc_a2_aggre_noc()
{
    #A2_NOC_aggre_noc_a2noc_ErrorLogger_erl_*
    echo 0x1700008  > $DCC_PATH/config
    echo 0x1700010  > $DCC_PATH/config
    echo 0x1700018  > $DCC_PATH/config
    echo 0x1700020 8 > $DCC_PATH/config

    #A2_NOC_aggre_noc_a2noc_sbm_sbm*
    echo 0x1700240  > $DCC_PATH/config
    echo 0x1700248  > $DCC_PATH/config

    #qosgen
    echo 0x1713010  > $DCC_PATH/config
    echo 0x1714010  > $DCC_PATH/config
    echo 0x1715010  > $DCC_PATH/config
    echo 0x1716010  > $DCC_PATH/config
    echo 0x1717010  > $DCC_PATH/config
    echo 0x1718010  > $DCC_PATH/config
    echo 0x171A010  > $DCC_PATH/config

    #debug chain
    echo 0x1701008 1 > $DCC_PATH/config
    echo 0x6 > $DCC_PATH/loop
    echo 0x1701010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1701018 1 > $DCC_PATH/config
}

config_dcc_pcie_aggre_noc()
{
    #PCIE_ANOC_aggre_noc_pcie_anoc_ErrorLogger_erl_*
    echo 0x16C0008  > $DCC_PATH/config
    echo 0x16C0010  > $DCC_PATH/config
    echo 0x16C0018  > $DCC_PATH/config
    echo 0x16C0020 8 > $DCC_PATH/config

    #PCIE_ANOC_aggre_noc_pcie_anoc_sbm_Fault*
    echo 0x16C0240  > $DCC_PATH/config
    echo 0x16C0248  > $DCC_PATH/config

    #qosgen
    echo 0x16CC010  > $DCC_PATH/config
    echo 0x16CD010  > $DCC_PATH/config
    echo 0x16CE010  > $DCC_PATH/config

    #debugchain PCIE
    echo 0x16C2008  > $DCC_PATH/config
    echo 0x5 > $DCC_PATH/loop
    echo 0x16C2010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x16C2018  > $DCC_PATH/config

    #debugchain QTB
    echo 0x16C3008  > $DCC_PATH/config
    echo 0x4 > $DCC_PATH/loop
    echo 0x16C3010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x16C3018  > $DCC_PATH/config
}

config_dcc_cnoc()
{
    #CNOC_cnoc_cfg_erl_*
    echo 0x1600008  > $DCC_PATH/config
    echo 0x1600010  > $DCC_PATH/config
    echo 0x1600018  > $DCC_PATH/config
    echo 0x1600020 8 > $DCC_PATH/config

    #CNOC_cnoc_cfg_FaultManager_FaultIn*
    echo 0x1600240 5 > $DCC_PATH/config
    echo 0x1600258  > $DCC_PATH/config

    #Cfg center debug chain
    echo 0x1602008  > $DCC_PATH/config
    echo 0x8 > $DCC_PATH/loop
    echo 0x1602010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1602018  > $DCC_PATH/config

    #cfg west debug chain
    echo 0x1602088  > $DCC_PATH/config
    echo 0x3 > $DCC_PATH/loop
    echo 0x1602090 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1602098  > $DCC_PATH/config

    #cfg mmnoc debugchain
    echo 0x1602108  > $DCC_PATH/config
    echo 0x3 > $DCC_PATH/loop
    echo 0x1602110 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1602118  > $DCC_PATH/config

    #cfg west_2 debug chain
    echo 0x1602188  > $DCC_PATH/config
    echo 0x2 > $DCC_PATH/loop
    echo 0x1602190 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1602198  > $DCC_PATH/config

    #cfg south debug chain
    echo 0x1602208  > $DCC_PATH/config
    echo 0x3 > $DCC_PATH/loop
    echo 0x1602210 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1602218  > $DCC_PATH/config

    #CNOC_cnoc_main_erl_*
    echo 0x1500008  > $DCC_PATH/config
    echo 0x1500010  > $DCC_PATH/config
    echo 0x1500018  > $DCC_PATH/config
    echo 0x1500020 8 > $DCC_PATH/config

    #CNOC_cnoc_main_main_fault_sbm_FaultIn*
    echo 0x1500240  > $DCC_PATH/config
    echo 0x1500248  > $DCC_PATH/config

    #CNOC_cnoc_main_FaultManager_FaultIn*
    echo 0x1500440 4 > $DCC_PATH/config

    #main center debug chain
    echo 0x1502008  > $DCC_PATH/config
    echo 0xB > $DCC_PATH/loop
    echo 0x1502010 2  > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1502018  > $DCC_PATH/config

    #main south debug chain
    echo 0x1502088  > $DCC_PATH/config
    echo 0x4 > $DCC_PATH/loop
    echo 0x1502090 2  > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1502098  > $DCC_PATH/config

    #main mmnoc debug chain
    echo 0x1502108  > $DCC_PATH/config
    echo 0x3 > $DCC_PATH/loop
    echo 0x1502110 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1502118  > $DCC_PATH/config
}

config_dcc_snoc()
{
    #SNOC_system_noc_erl_*
    echo 0x1680008  > $DCC_PATH/config
    echo 0x1680010  > $DCC_PATH/config
    echo 0x1680018  > $DCC_PATH/config
    echo 0x1680020 8 > $DCC_PATH/config

    #SNOC_system_noc_error_status_sbm_FaultIn*
    echo 0x1681040  > $DCC_PATH/config
    echo 0x1681048  > $DCC_PATH/config

    #qosgen
    echo 0x1690010  > $DCC_PATH/config
    echo 0x169C010  > $DCC_PATH/config
    echo 0x169D010  > $DCC_PATH/config

    #debug
    echo 0x1682008  > $DCC_PATH/config
    echo 0x5 > $DCC_PATH/loop
    echo 0x1682010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1682018  > $DCC_PATH/config
}

config_dcc_dc_dch_noc()
{
    #dc_noc_dch_erl_*
    echo 0x240E0008  > $DCC_PATH/config
    echo 0x240E0010  > $DCC_PATH/config
    echo 0x240E0018  > $DCC_PATH/config
    echo 0x240E0020 8 > $DCC_PATH/config

    #dc_noc_dch_sbm_FaultIn*
    echo 0x240E0240  > $DCC_PATH/config
    echo 0x240E0248  > $DCC_PATH/config

    #debug
    echo 0x240E1000 3 > $DCC_PATH/config
    echo 0x8 > $DCC_PATH/loop
    echo 0x240E1010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x240E1018 1 > $DCC_PATH/config
}

config_dcc_dc_ch02_noc()
{
    #DC_NOC_CH02_SERVICE_NETWORKdc_noc_ch_hm_erl_
    echo 0x245F0000 3 > $DCC_PATH/config
    echo 0x245F0010  > $DCC_PATH/config
    echo 0x245F0018  > $DCC_PATH/config
    echo 0x245F0020 8 > $DCC_PATH/config

    #DC_NOC_CH02_SERVICE_NETWORKdc_noc_ch_hm_sbm_FaultIn*
    echo 0x245F0240  > $DCC_PATH/config
    echo 0x245F0248  > $DCC_PATH/config

    #debug
    echo 0x245F2008  > $DCC_PATH/config
    echo 0x3 > $DCC_PATH/loop
    echo 0x245F2010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x245F2018  > $DCC_PATH/config
}

config_dcc_dc_ch13_noc()
{
    #DC_NOC_CH13_SERVICE_NETWORKdc_noc_ch_hm_erl_*
    echo 0x247F0000 3 > $DCC_PATH/config
    echo 0x247F0010  > $DCC_PATH/config
    echo 0x247F0018  > $DCC_PATH/config
    echo 0x247F0020 8 > $DCC_PATH/config

    #DC_NOC_CH13_SERVICE_NETWORKdc_noc_ch_hm_sbm_FaultIn*
    echo 0x247F0240  > $DCC_PATH/config
    echo 0x247F0248  > $DCC_PATH/config

    #debug
    echo 0x247F2008  > $DCC_PATH/config
    echo 0x3 > $DCC_PATH/loop
    echo 0x247F2010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x247F2018  > $DCC_PATH/config
}

config_dcc_mmnoc()
{
    #MNOC_HF_mmss_noc_mnoc_hf_ErrorLogger_erl_*
    echo 0x1780008  > $DCC_PATH/config
    echo 0x1780010  > $DCC_PATH/config
    echo 0x1780018  > $DCC_PATH/config
    echo 0x1780020 8 > $DCC_PATH/config

    #MNOC_HF_mmss_noc_SidebandManager_hf_sbm_FaultIn*
    echo 0x1780240  > $DCC_PATH/config
    echo 0x1780248  > $DCC_PATH/config

    #qnm
    echo 0x17A5010  > $DCC_PATH/config
    echo 0x17A6010  > $DCC_PATH/config
    echo 0x17A7010  > $DCC_PATH/config
    echo 0x17A8010  > $DCC_PATH/config
    echo 0x17A9010  > $DCC_PATH/config
    echo 0x17AA010  > $DCC_PATH/config
    echo 0x17AB010  > $DCC_PATH/config
    echo 0x17AC010  > $DCC_PATH/config
    echo 0x17AD010  > $DCC_PATH/config

    #QTB500_sf_debugchain
    echo 0x1782208  > $DCC_PATH/config
    echo 0x3 > $DCC_PATH/loop
    echo 0x1782210 2  > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1782218  > $DCC_PATH/config

    #noc_sf_DebugChain
    echo 0x1782308  > $DCC_PATH/config
    echo 0x8 > $DCC_PATH/loop
    echo 0x1782310 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x1782318  > $DCC_PATH/config

    #MNOC_HF_mmss_noc_mnoc_hf_ErrorLogger_erl_*
    echo 0x17C0008  > $DCC_PATH/config
    echo 0x17C0010  > $DCC_PATH/config
    echo 0x17C0018  > $DCC_PATH/config
    echo 0x17C0020 8 > $DCC_PATH/config

    #MNOC_HF_mmss_noc_SidebandManager_hf_sbm_FaultIn*
    echo 0x17C0240  > $DCC_PATH/config
    echo 0x17C0248  > $DCC_PATH/config

    #qosgen
    echo 0x17E4010  > $DCC_PATH/config
    echo 0x17E5010  > $DCC_PATH/config
    echo 0x17E6010  > $DCC_PATH/config
    echo 0x17E7010  > $DCC_PATH/config
    echo 0x17E8010  > $DCC_PATH/config
    echo 0x17E9010  > $DCC_PATH/config

    #DebugChain_hf_debug
    echo 0x17C2008  > $DCC_PATH/config
    echo 0x7 > $DCC_PATH/loop
    echo 0x17C2010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x17C2018  > $DCC_PATH/config

    #QTB500_hf_DebugChain
    echo 0x17C2108  > $DCC_PATH/config
    echo 0x15 > $DCC_PATH/loop
    echo 0x17C2110 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x17C2118  > $DCC_PATH/config
}

config_dcc_lpass_ag_noc()
{
    #lpass_ag_noc_lpass_ag_noc_Errorlogger_erl_*
    echo 0x3C40008  > $DCC_PATH/config
    echo 0x3C40010  > $DCC_PATH/config
    echo 0x3C40018  > $DCC_PATH/config
    echo 0x3C40020 8 > $DCC_PATH/config

    #lpass_ag_noc_SidebandManagerError_sbm_FaultIn*
    echo 0x3C4B040  > $DCC_PATH/config
    echo 0x3C4B048  > $DCC_PATH/config

    #agnoc_core_DebugChain
    echo 0x3C41008  > $DCC_PATH/config
    echo 0x5 > $DCC_PATH/loop
    echo 0x3C41010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x3C41018  > $DCC_PATH/config

    #qosgen
    echo 0x3C44008  > $DCC_PATH/config
    echo 0x3C45008  > $DCC_PATH/config
    echo 0x3C46008  > $DCC_PATH/config

    #lpass_ag_noc_lpass_aml_noc_Errorlogger_erl_*
    echo 0x3C50008  > $DCC_PATH/config
    echo 0x3C50010  > $DCC_PATH/config
    echo 0x3C50018  > $DCC_PATH/config
    echo 0x3C50020 8 > $DCC_PATH/config

    #aml_noc_SidebandManagerError_sbm_FaultIn*
    echo 0x3C56040  > $DCC_PATH/config
    echo 0x3C56048  > $DCC_PATH/config

    #aml_noc_DebugChain
    echo 0x3C51008  > $DCC_PATH/config
    echo 0x2 > $DCC_PATH/loop
    echo 0x3C51010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x3C51018  > $DCC_PATH/config

    #LPASS_hw_af_noc_*
    echo 0x39C4008  > $DCC_PATH/config
    echo 0x39C4010  > $DCC_PATH/config
    echo 0x39C4018  > $DCC_PATH/config
    echo 0x39C4020 8 > $DCC_PATH/config
    echo 0x39C5010  > $DCC_PATH/config

    echo 0x3C54008  > $DCC_PATH/config
}

config_dcc_turing_noc()
{
    #TURING_VAPSS_NOC_vapss_errlog_*
    echo 0x322F0000 3 > $DCC_PATH/config
    echo 0x322F0010  > $DCC_PATH/config
    echo 0x322F0018  > $DCC_PATH/config
    echo 0x322F0020 8 > $DCC_PATH/config

    #TURING_VAPSS_NOC_vapss_sb_FaultIn*
    echo 0x322F0240  > $DCC_PATH/config
    echo 0x322F0248  > $DCC_PATH/config

    #debug
    echo 0x322F1008  > $DCC_PATH/config
    echo 0x7 > $DCC_PATH/loop
    echo 0x322F1010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x322F1018  > $DCC_PATH/config

    #TURING_nsp_noc_erl*
    echo 0x320C0008  > $DCC_PATH/config
    echo 0x320C0010  > $DCC_PATH/config
    echo 0x320C0018  > $DCC_PATH/config
    echo 0x320C0020 8 > $DCC_PATH/config

    #TURING_nsp_noc_sbm_FaultIn*
    echo 0x320C0240  > $DCC_PATH/config
    echo 0x320C0248  > $DCC_PATH/config

    #debug
    echo 0x320C1008  > $DCC_PATH/config
    echo 0x7 > $DCC_PATH/loop
    echo 0x320C1010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x320C1018  > $DCC_PATH/config

    #qosgen
    echo 0x322FE010  > $DCC_PATH/config
    echo 0x322FE090  > $DCC_PATH/config
    echo 0x322FE110  > $DCC_PATH/config
    echo 0x322FE190  > $DCC_PATH/config

    #qosgen
    #echo 0x320D0010  > $DCC_PATH/config
    echo 0x320D1010  > $DCC_PATH/config
    echo 0x320D2010  > $DCC_PATH/config
    #echo 0x320D3010  > $DCC_PATH/config
}

config_dcc_lpass_rscc()
{
    echo 0x3002028  > $DCC_PATH/config
    echo 0x30C0208 3 > $DCC_PATH/config
    echo 0x30C0228 3 > $DCC_PATH/config
    echo 0x30C0248 3 > $DCC_PATH/config
    echo 0x30C0268 3 > $DCC_PATH/config
    echo 0x30C0288 3 > $DCC_PATH/config
    echo 0x30C02a8 3 > $DCC_PATH/config
    echo 0x30C0404 2 > $DCC_PATH/config
    echo 0x3480208 3 > $DCC_PATH/config
    echo 0x3480228 3 > $DCC_PATH/config
    echo 0x3480248 3 > $DCC_PATH/config
    echo 0x3480268 3 > $DCC_PATH/config
    echo 0x3480288 3 > $DCC_PATH/config
    echo 0x34802a8 3 > $DCC_PATH/config
    echo 0x3480404 2 > $DCC_PATH/config
}

config_dcc_turing_rscc()
{
    echo 0x320A4208 3 > $DCC_PATH/config
    echo 0x320A4228 3 > $DCC_PATH/config
    echo 0x320A4248 3 > $DCC_PATH/config
    echo 0x320A4268 3 > $DCC_PATH/config
    echo 0x320A4288 3 > $DCC_PATH/config
    echo 0x320A42a8 3 > $DCC_PATH/config
    echo 0x320A4404 2 > $DCC_PATH/config
    echo 0x32302028  > $DCC_PATH/config
    echo 0x323C0208 3 > $DCC_PATH/config
    echo 0x323C0228 3 > $DCC_PATH/config
    echo 0x323C0248 3 > $DCC_PATH/config
    echo 0x323C0268 3 > $DCC_PATH/config
    echo 0x323C0288 3 > $DCC_PATH/config
    echo 0x323C02a8 3 > $DCC_PATH/config
    echo 0x323C0404 2 > $DCC_PATH/config

    echo 0x32300010 1 > $DCC_PATH/config
    echo 0x32348020 1 > $DCC_PATH/config
    echo 0x32348040 1 > $DCC_PATH/config
    echo 0x32348060 1 > $DCC_PATH/config
    echo 0x32348080 1 > $DCC_PATH/config
    echo 0x323480A0 1 > $DCC_PATH/config
    echo 0x323480C0 1 > $DCC_PATH/config
    echo 0x323481A0 1 > $DCC_PATH/config
    echo 0x32348180 1 > $DCC_PATH/config
    echo 0x32008368 1 > $DCC_PATH/config
    echo 0x32008188 1 > $DCC_PATH/config
    echo 0x320080CC 1 > $DCC_PATH/config
    echo 0x320081F0 1 > $DCC_PATH/config
    echo 0x32008348 1 > $DCC_PATH/config
    echo 0x3200816C 1 > $DCC_PATH/config
    echo 0x32008158 1 > $DCC_PATH/config
    echo 0x32008104 1 > $DCC_PATH/config
    echo 0x3230202C 1 > $DCC_PATH/config
    echo 0x32302468 1 > $DCC_PATH/config
    echo 0x32302448 1 > $DCC_PATH/config
    echo 0x32302000 1 > $DCC_PATH/config
    echo 0x32302004 1 > $DCC_PATH/config
    echo 0x32302498 1 > $DCC_PATH/config
    echo 0x32300568 1 > $DCC_PATH/config
    echo 0x323005B8 1 > $DCC_PATH/config
    echo 0x32302098 1 > $DCC_PATH/config
    echo 0x323C04B4 1 > $DCC_PATH/config
    echo 0x32300438 1 > $DCC_PATH/config
    echo 0x323C0010 1 > $DCC_PATH/config
    echo 0x32302458 1 > $DCC_PATH/config
    echo 0x32348024 1 > $DCC_PATH/config
    echo 0x323C04B0 1 > $DCC_PATH/config

    echo 0x32340000 16 > $DCC_PATH/config
    #PLL settings start
    echo 0x3234002C 0x100 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x500 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x900 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0xD00 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x1100 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x1500 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x1900 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x1D00 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x2100 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x2500 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x2900 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x2D00 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x3100 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x3500 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x3900 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x3D00 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x4100 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x4500 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x4900 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x4D00 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x5100 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x5500 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x5900 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x5D00 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x6100 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x6500 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x6900 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x6D00 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x7100 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x7500 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x7900 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    echo 0x3234002C 0x7D00 > $DCC_PATH/config_write
    echo 0x3234000C 1 > $DCC_PATH/config
    #PLL settings end
    echo 0x32341000 16 > $DCC_PATH/config
    echo 0x3238C008 1 > $DCC_PATH/config
    echo 0x3238C010 1 > $DCC_PATH/config
    echo 0x3238C014 1 > $DCC_PATH/config
    echo 0x3238C01C 1 > $DCC_PATH/config
    echo 0x3238C020 1 > $DCC_PATH/config
    echo 0x3238C024 5 > $DCC_PATH/config
    echo 0x3238C054 4 > $DCC_PATH/config
    echo 0x3238C068 4 > $DCC_PATH/config
    echo 0x3238C100 6 > $DCC_PATH/config
    echo 0x3238C120 18 > $DCC_PATH/config
}

config_dcc_gcc()
{
    echo 0x11C004 2 > $DCC_PATH/config
    echo 0x110018 5 > $DCC_PATH/config
    echo 0x110030  > $DCC_PATH/config
    echo 0x11C018 5 > $DCC_PATH/config
    echo 0x11C030  > $DCC_PATH/config
    echo 0x178040  > $DCC_PATH/config
    echo 0x199014 5 > $DCC_PATH/config
    echo 0x19902C  > $DCC_PATH/config
    echo 0x19D014 5 > $DCC_PATH/config
    echo 0x19D02C  > $DCC_PATH/config

    #PRNG regs
    echo 0x10C0000 4 > $DCC_PATH/config
    echo 0x10C1000 2 > $DCC_PATH/config
    echo 0x10C1010 7 > $DCC_PATH/config
    echo 0x10C1100 3 > $DCC_PATH/config
    echo 0x10C1110 5 > $DCC_PATH/config
    echo 0x10C1130 2 > $DCC_PATH/config
    echo 0x10C113C 2 > $DCC_PATH/config
    echo 0x10C1148 3 > $DCC_PATH/config
}

config_dcc_smmu()
{
    echo 0x15002204  > $DCC_PATH/config
    echo 0x150025DC  > $DCC_PATH/config
    echo 0x15002670  > $DCC_PATH/config
    echo 0x3DA26B4  > $DCC_PATH/config
}

config_dcc_camera()
{
    echo 0x126010 2 > $DCC_PATH/config
    echo 0xADF0004 2 > $DCC_PATH/config
    echo 0xADF0018 2 > $DCC_PATH/config
    echo 0xADF0030 3 > $DCC_PATH/config
    echo 0xADF004C 3 > $DCC_PATH/config
    echo 0xADF0068  > $DCC_PATH/config
    echo 0xADF0074  > $DCC_PATH/config
    echo 0xADF0080 2 > $DCC_PATH/config
    echo 0xADF0094 2 > $DCC_PATH/config
    echo 0xADF00AC  > $DCC_PATH/config
    echo 0xADF00B8 2 > $DCC_PATH/config
    echo 0xADF00D0 3 > $DCC_PATH/config
    echo 0xADF1004 2 > $DCC_PATH/config
    echo 0xADF1018 2 > $DCC_PATH/config
    echo 0xADF1030  > $DCC_PATH/config
    echo 0xADF103C  > $DCC_PATH/config
    echo 0xADF1048 3 > $DCC_PATH/config
    echo 0xADF2004 2 > $DCC_PATH/config
    echo 0xADF2018 2 > $DCC_PATH/config
    echo 0xADF2030  > $DCC_PATH/config
    echo 0xADF203C  > $DCC_PATH/config
    echo 0xADF2048  > $DCC_PATH/config
    echo 0xADF3000 2 > $DCC_PATH/config
    echo 0xADF3018  > $DCC_PATH/config
    echo 0xADF3024 3 > $DCC_PATH/config
    echo 0xADF3040  > $DCC_PATH/config
    echo 0xADF304C 4 > $DCC_PATH/config
    echo 0xADF306C  > $DCC_PATH/config
    echo 0xADF309C 2 > $DCC_PATH/config
    echo 0xADF30B4  > $DCC_PATH/config
    echo 0xADF30C0  > $DCC_PATH/config
    echo 0xADF316C  > $DCC_PATH/config
    echo 0xADF319C  > $DCC_PATH/config
    echo 0xADF31A8 2 > $DCC_PATH/config
    echo 0xADF31C0  > $DCC_PATH/config
    echo 0xADF4020 3 > $DCC_PATH/config
    echo 0xADF4038  > $DCC_PATH/config
    echo 0xADF5170  > $DCC_PATH/config
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

    echo 0x17d90020 1 > $DCC_PATH/config
    echo 0x17d91020 1 > $DCC_PATH/config
    echo 0x17d92020 1 > $DCC_PATH/config
    
    echo 0x17D90300 1 > $DCC_PATH/config
    echo 0x17D90340 1 > $DCC_PATH/config
    echo 0x17D90410 1 > $DCC_PATH/config
    echo 0x17D91008 1 > $DCC_PATH/config
    echo 0x17D9100C 1 > $DCC_PATH/config
    echo 0x17D91300 1 > $DCC_PATH/config
    echo 0x17D91340 1 > $DCC_PATH/config
    echo 0x17D91410 1 > $DCC_PATH/config
    echo 0x17D92300 1 > $DCC_PATH/config
    echo 0x17D92340 1 > $DCC_PATH/config
    echo 0x17D92410 1 > $DCC_PATH/config
    echo 0x17D8000C 1 > $DCC_PATH/config
    echo 0x17D80018 1 > $DCC_PATH/config
}

config_dcc_clk()
{
    echo 0x123140 > $DCC_PATH/config
}

config_dcc_misc()
{
    #MCN identifier
    echo 0x221C00A0 > $DCC_PATH/config
    echo 0x221C00A4 > $DCC_PATH/config

    # WDOG_BITE_INT0_CONFIG
    echo 0x17400038 > $DCC_PATH/config
    # EPSSTOP_MUC_HANG_DET_CTRL
    echo 0x17d98010 > $DCC_PATH/config
    # SOC_HW_VERSION
    echo 0x1fc8000 > $DCC_PATH/config
    # QFPROM_CORR_PTE_ROW1_MSB register
    echo 0x221c20a4 > $DCC_PATH/config
}

config_dcc_ddr()
{
    echo 0x2407701c  > $DCC_PATH/config
    echo 0x24077030  > $DCC_PATH/config
    echo 0x2408005c  > $DCC_PATH/config
    echo 0x240800c8  > $DCC_PATH/config
    echo 0x240800d4  > $DCC_PATH/config
    echo 0x240800e0  > $DCC_PATH/config
    echo 0x240800ec  > $DCC_PATH/config
    echo 0x240800f8  > $DCC_PATH/config
    echo 0x240801b4  > $DCC_PATH/config
    echo 0x240a8120 5 > $DCC_PATH/config
    echo 0x240a8144 2 > $DCC_PATH/config
    echo 0x240a81c4 4 > $DCC_PATH/config
    echo 0x240a81e4  > $DCC_PATH/config
    echo 0x240a81f0  > $DCC_PATH/config
    echo 0x240a81fc  > $DCC_PATH/config
    echo 0x240a8208  > $DCC_PATH/config
    echo 0x240a8258  > $DCC_PATH/config
    echo 0x240a8278  > $DCC_PATH/config
    echo 0x240a82e8  > $DCC_PATH/config
    echo 0x240a8860 3 > $DCC_PATH/config
    echo 0x240aa034 3 > $DCC_PATH/config
    echo 0x240ba28c  > $DCC_PATH/config
    echo 0x240ba294  > $DCC_PATH/config
    echo 0x240ba29c  > $DCC_PATH/config
    echo 0x24185100 4 > $DCC_PATH/config
    echo 0x24187100  > $DCC_PATH/config
    echo 0x2418c100  > $DCC_PATH/config
    echo 0x24401e64  > $DCC_PATH/config
    echo 0x24401ea0  > $DCC_PATH/config
    echo 0x24403e64  > $DCC_PATH/config
    echo 0x24403ea0  > $DCC_PATH/config
    echo 0x2440527c  > $DCC_PATH/config
    echo 0x24405290  > $DCC_PATH/config
    echo 0x244054ec  > $DCC_PATH/config
    echo 0x244054f4  > $DCC_PATH/config
    echo 0x24405514  > $DCC_PATH/config
    echo 0x2440551c  > $DCC_PATH/config
    echo 0x24405524  > $DCC_PATH/config
    echo 0x24405548  > $DCC_PATH/config
    echo 0x24405550  > $DCC_PATH/config
    echo 0x24405558  > $DCC_PATH/config
    echo 0x244055b8  > $DCC_PATH/config
    echo 0x244055c0  > $DCC_PATH/config
    echo 0x244055ec  > $DCC_PATH/config
    echo 0x24405688  > $DCC_PATH/config
    echo 0x24405870  > $DCC_PATH/config
    echo 0x244058a0  > $DCC_PATH/config
    echo 0x244058a8  > $DCC_PATH/config
    echo 0x244058b0  > $DCC_PATH/config
    echo 0x244058b8  > $DCC_PATH/config
    echo 0x244058d8 2 > $DCC_PATH/config
    echo 0x244058f4  > $DCC_PATH/config
    echo 0x244058fc  > $DCC_PATH/config
    echo 0x24405920  > $DCC_PATH/config
    echo 0x24405928  > $DCC_PATH/config
    echo 0x24405944  > $DCC_PATH/config
    echo 0x24406604  > $DCC_PATH/config
    echo 0x2440660c  > $DCC_PATH/config
    echo 0x24407010  > $DCC_PATH/config
    echo 0x24440310  > $DCC_PATH/config
    echo 0x24440400 2 > $DCC_PATH/config
    echo 0x24440410 3 > $DCC_PATH/config
    echo 0x24440428  > $DCC_PATH/config
    echo 0x24440430  > $DCC_PATH/config
    echo 0x24440440  > $DCC_PATH/config
    echo 0x24440448  > $DCC_PATH/config
    echo 0x244404a0  > $DCC_PATH/config
    echo 0x244404b0 3 > $DCC_PATH/config
    echo 0x244404d0 2 > $DCC_PATH/config
    echo 0x2444341c  > $DCC_PATH/config
    echo 0x24445804  > $DCC_PATH/config
    echo 0x2444590c  > $DCC_PATH/config
    echo 0x24445a14  > $DCC_PATH/config
    echo 0x24445c1c  > $DCC_PATH/config
    echo 0x24445c38  > $DCC_PATH/config
    echo 0x24449100  > $DCC_PATH/config
    echo 0x24449110  > $DCC_PATH/config
    echo 0x24449120  > $DCC_PATH/config
    echo 0x24449180 2 > $DCC_PATH/config
    echo 0x24460618  > $DCC_PATH/config
    echo 0x24460684  > $DCC_PATH/config
    echo 0x2446068c  > $DCC_PATH/config
    echo 0x24481e64  > $DCC_PATH/config
    echo 0x24481ea0  > $DCC_PATH/config
    echo 0x24483e64  > $DCC_PATH/config
    echo 0x24483ea0  > $DCC_PATH/config
    echo 0x2448527c  > $DCC_PATH/config
    echo 0x24485290  > $DCC_PATH/config
    echo 0x244854ec  > $DCC_PATH/config
    echo 0x244854f4  > $DCC_PATH/config
    echo 0x24485514  > $DCC_PATH/config
    echo 0x2448551c  > $DCC_PATH/config
    echo 0x24485524  > $DCC_PATH/config
    echo 0x24485548  > $DCC_PATH/config
    echo 0x24485550  > $DCC_PATH/config
    echo 0x24485558  > $DCC_PATH/config
    echo 0x244855b8  > $DCC_PATH/config
    echo 0x244855c0  > $DCC_PATH/config
    echo 0x244855ec  > $DCC_PATH/config
    echo 0x24485688  > $DCC_PATH/config
    echo 0x24485870  > $DCC_PATH/config
    echo 0x244858a0  > $DCC_PATH/config
    echo 0x244858a8  > $DCC_PATH/config
    echo 0x244858b0  > $DCC_PATH/config
    echo 0x244858b8  > $DCC_PATH/config
    echo 0x244858d8 2 > $DCC_PATH/config
    echo 0x244858f4  > $DCC_PATH/config
    echo 0x244858fc  > $DCC_PATH/config
    echo 0x24485920  > $DCC_PATH/config
    echo 0x24485928  > $DCC_PATH/config
    echo 0x24485944  > $DCC_PATH/config
    echo 0x24486604  > $DCC_PATH/config
    echo 0x2448660c  > $DCC_PATH/config
    echo 0x24487010  > $DCC_PATH/config
    echo 0x244c0310  > $DCC_PATH/config
    echo 0x244c0400 2 > $DCC_PATH/config
    echo 0x244c0410 3 > $DCC_PATH/config
    echo 0x244c0428  > $DCC_PATH/config
    echo 0x244c0430  > $DCC_PATH/config
    echo 0x244c0440  > $DCC_PATH/config
    echo 0x244c0448  > $DCC_PATH/config
    echo 0x244c04a0  > $DCC_PATH/config
    echo 0x244c04b0 3 > $DCC_PATH/config
    echo 0x244c04d0 2 > $DCC_PATH/config
    echo 0x244c341c  > $DCC_PATH/config
    echo 0x244c5804  > $DCC_PATH/config
    echo 0x244c590c  > $DCC_PATH/config
    echo 0x244c5a14  > $DCC_PATH/config
    echo 0x244c5c1c  > $DCC_PATH/config
    echo 0x244c5c38  > $DCC_PATH/config
    echo 0x244c9100  > $DCC_PATH/config
    echo 0x244c9110  > $DCC_PATH/config
    echo 0x244c9120  > $DCC_PATH/config
    echo 0x244c9180 2 > $DCC_PATH/config
    echo 0x244e0618  > $DCC_PATH/config
    echo 0x244e0684  > $DCC_PATH/config
    echo 0x244e068c  > $DCC_PATH/config
    echo 0x24601e64  > $DCC_PATH/config
    echo 0x24601ea0  > $DCC_PATH/config
    echo 0x24603e64  > $DCC_PATH/config
    echo 0x24603ea0  > $DCC_PATH/config
    echo 0x2460527c  > $DCC_PATH/config
    echo 0x24605290  > $DCC_PATH/config
    echo 0x246054ec  > $DCC_PATH/config
    echo 0x246054f4  > $DCC_PATH/config
    echo 0x24605514  > $DCC_PATH/config
    echo 0x2460551c  > $DCC_PATH/config
    echo 0x24605524  > $DCC_PATH/config
    echo 0x24605548  > $DCC_PATH/config
    echo 0x24605550  > $DCC_PATH/config
    echo 0x24605558  > $DCC_PATH/config
    echo 0x246055b8  > $DCC_PATH/config
    echo 0x246055c0  > $DCC_PATH/config
    echo 0x246055ec  > $DCC_PATH/config
    echo 0x24605688  > $DCC_PATH/config
    echo 0x24605870  > $DCC_PATH/config
    echo 0x246058a0  > $DCC_PATH/config
    echo 0x246058a8  > $DCC_PATH/config
    echo 0x246058b0  > $DCC_PATH/config
    echo 0x246058b8  > $DCC_PATH/config
    echo 0x246058d8 2 > $DCC_PATH/config
    echo 0x246058f4  > $DCC_PATH/config
    echo 0x246058fc  > $DCC_PATH/config
    echo 0x24605920  > $DCC_PATH/config
    echo 0x24605928  > $DCC_PATH/config
    echo 0x24605944  > $DCC_PATH/config
    echo 0x24606604  > $DCC_PATH/config
    echo 0x2460660c  > $DCC_PATH/config
    echo 0x24607010  > $DCC_PATH/config
    echo 0x24640310  > $DCC_PATH/config
    echo 0x24640400 2 > $DCC_PATH/config
    echo 0x24640410 3 > $DCC_PATH/config
    echo 0x24640428  > $DCC_PATH/config
    echo 0x24640430  > $DCC_PATH/config
    echo 0x24640440  > $DCC_PATH/config
    echo 0x24640448  > $DCC_PATH/config
    echo 0x246404a0  > $DCC_PATH/config
    echo 0x246404b0 3 > $DCC_PATH/config
    echo 0x246404d0 2 > $DCC_PATH/config
    echo 0x2464341c  > $DCC_PATH/config
    echo 0x24645804  > $DCC_PATH/config
    echo 0x2464590c  > $DCC_PATH/config
    echo 0x24645a14  > $DCC_PATH/config
    echo 0x24645c1c  > $DCC_PATH/config
    echo 0x24645c38  > $DCC_PATH/config
    echo 0x24649100  > $DCC_PATH/config
    echo 0x24649110  > $DCC_PATH/config
    echo 0x24649120  > $DCC_PATH/config
    echo 0x24649180 2 > $DCC_PATH/config
    echo 0x24660618  > $DCC_PATH/config
    echo 0x24660684  > $DCC_PATH/config
    echo 0x2466068c  > $DCC_PATH/config
    echo 0x24681e64  > $DCC_PATH/config
    echo 0x24681ea0  > $DCC_PATH/config
    echo 0x24683e64  > $DCC_PATH/config
    echo 0x24683ea0  > $DCC_PATH/config
    echo 0x2468527c  > $DCC_PATH/config
    echo 0x24685290  > $DCC_PATH/config
    echo 0x246854ec  > $DCC_PATH/config
    echo 0x246854f4  > $DCC_PATH/config
    echo 0x24685514  > $DCC_PATH/config
    echo 0x2468551c  > $DCC_PATH/config
    echo 0x24685524  > $DCC_PATH/config
    echo 0x24685548  > $DCC_PATH/config
    echo 0x24685550  > $DCC_PATH/config
    echo 0x24685558  > $DCC_PATH/config
    echo 0x246855b8  > $DCC_PATH/config
    echo 0x246855c0  > $DCC_PATH/config
    echo 0x246855ec  > $DCC_PATH/config
    echo 0x24685688  > $DCC_PATH/config
    echo 0x24685870  > $DCC_PATH/config
    echo 0x246858a0  > $DCC_PATH/config
    echo 0x246858a8  > $DCC_PATH/config
    echo 0x246858b0  > $DCC_PATH/config
    echo 0x246858b8  > $DCC_PATH/config
    echo 0x246858d8 2 > $DCC_PATH/config
    echo 0x246858f4  > $DCC_PATH/config
    echo 0x246858fc  > $DCC_PATH/config
    echo 0x24685920  > $DCC_PATH/config
    echo 0x24685928  > $DCC_PATH/config
    echo 0x24685944  > $DCC_PATH/config
    echo 0x24686604  > $DCC_PATH/config
    echo 0x2468660c  > $DCC_PATH/config
    echo 0x24687010  > $DCC_PATH/config
    echo 0x246C5804  > $DCC_PATH/config
    echo 0x246c0310  > $DCC_PATH/config
    echo 0x246c0400 2 > $DCC_PATH/config
    echo 0x246c0410 3 > $DCC_PATH/config
    echo 0x246c0428  > $DCC_PATH/config
    echo 0x246c0430  > $DCC_PATH/config
    echo 0x246c0440  > $DCC_PATH/config
    echo 0x246c0448  > $DCC_PATH/config
    echo 0x246c04a0  > $DCC_PATH/config
    echo 0x246c04b0 3 > $DCC_PATH/config
    echo 0x246c04d0 2 > $DCC_PATH/config
    echo 0x246c341c  > $DCC_PATH/config
    echo 0x246c590c  > $DCC_PATH/config
    echo 0x246c5a14  > $DCC_PATH/config
    echo 0x246c5c1c  > $DCC_PATH/config
    echo 0x246c5c38  > $DCC_PATH/config
    echo 0x246c9100  > $DCC_PATH/config
    echo 0x246c9110  > $DCC_PATH/config
    echo 0x246c9120  > $DCC_PATH/config
    echo 0x246c9180 2 > $DCC_PATH/config
    echo 0x246e0618  > $DCC_PATH/config
    echo 0x246e0684  > $DCC_PATH/config
    echo 0x246e068c  > $DCC_PATH/config
    echo 0x24840310  > $DCC_PATH/config
    echo 0x24840400 2 > $DCC_PATH/config
    echo 0x24840410 3 > $DCC_PATH/config
    echo 0x24840428  > $DCC_PATH/config
    echo 0x24840430  > $DCC_PATH/config
    echo 0x24840440  > $DCC_PATH/config
    echo 0x24840448  > $DCC_PATH/config
    echo 0x248404a0  > $DCC_PATH/config
    echo 0x248404b0 3 > $DCC_PATH/config
    echo 0x248404d0 2 > $DCC_PATH/config
    echo 0x2484341c  > $DCC_PATH/config
    echo 0x24845804  > $DCC_PATH/config
    echo 0x2484590c  > $DCC_PATH/config
    echo 0x24845a14  > $DCC_PATH/config
    echo 0x24845c1c  > $DCC_PATH/config
    echo 0x24845c38  > $DCC_PATH/config
    echo 0x24849100  > $DCC_PATH/config
    echo 0x24849110  > $DCC_PATH/config
    echo 0x24849120  > $DCC_PATH/config
    echo 0x24849180 2 > $DCC_PATH/config
    echo 0x24860618  > $DCC_PATH/config
    echo 0x24860684  > $DCC_PATH/config
    echo 0x2486068c  > $DCC_PATH/config
    echo 0x248c0310  > $DCC_PATH/config
    echo 0x248c0400 2 > $DCC_PATH/config
    echo 0x248c0410 3 > $DCC_PATH/config
    echo 0x248c0428  > $DCC_PATH/config
    echo 0x248c0430  > $DCC_PATH/config
    echo 0x248c0440  > $DCC_PATH/config
    echo 0x248c0448  > $DCC_PATH/config
    echo 0x248c04a0  > $DCC_PATH/config
    echo 0x248c04b0 3 > $DCC_PATH/config
    echo 0x248c04d0 2 > $DCC_PATH/config
    echo 0x248c341c  > $DCC_PATH/config
    echo 0x248c5804  > $DCC_PATH/config
    echo 0x248c590c  > $DCC_PATH/config
    echo 0x248c5a14  > $DCC_PATH/config
    echo 0x248c5c1c  > $DCC_PATH/config
    echo 0x248c5c38  > $DCC_PATH/config
    echo 0x248c9100  > $DCC_PATH/config
    echo 0x248c9110  > $DCC_PATH/config
    echo 0x248c9120  > $DCC_PATH/config
    echo 0x248c9180 2 > $DCC_PATH/config
    echo 0x248e0618  > $DCC_PATH/config
    echo 0x248e0684  > $DCC_PATH/config
    echo 0x248e068c  > $DCC_PATH/config
    echo 0x25000004  > $DCC_PATH/config
    echo 0x25001004  > $DCC_PATH/config
    echo 0x25002004  > $DCC_PATH/config
    echo 0x25003004  > $DCC_PATH/config
    echo 0x25004004  > $DCC_PATH/config
    echo 0x25005004  > $DCC_PATH/config
    echo 0x25006004  > $DCC_PATH/config
    echo 0x25007004  > $DCC_PATH/config
    echo 0x25008004  > $DCC_PATH/config
    echo 0x25009004  > $DCC_PATH/config
    echo 0x2500a004  > $DCC_PATH/config
    echo 0x2500b004  > $DCC_PATH/config
    echo 0x2500c004  > $DCC_PATH/config
    echo 0x2500d004  > $DCC_PATH/config
    echo 0x2500e004  > $DCC_PATH/config
    echo 0x2500f004  > $DCC_PATH/config
    echo 0x25010004  > $DCC_PATH/config
    echo 0x25011004  > $DCC_PATH/config
    echo 0x25012004  > $DCC_PATH/config
    echo 0x25013004  > $DCC_PATH/config
    echo 0x25014004  > $DCC_PATH/config
    echo 0x25015004  > $DCC_PATH/config
    echo 0x25016004  > $DCC_PATH/config
    echo 0x25017004  > $DCC_PATH/config
    echo 0x25018004  > $DCC_PATH/config
    echo 0x25019004  > $DCC_PATH/config
    echo 0x2501a004  > $DCC_PATH/config
    echo 0x2501b004  > $DCC_PATH/config
    echo 0x2501c004  > $DCC_PATH/config
    echo 0x2501d004  > $DCC_PATH/config
    echo 0x2501e004  > $DCC_PATH/config
    echo 0x2501f004  > $DCC_PATH/config
    echo 0x25020348  > $DCC_PATH/config
    echo 0x25020480  > $DCC_PATH/config
    echo 0x25022400  > $DCC_PATH/config
    echo 0x25023210  > $DCC_PATH/config
    echo 0x25023220 4 > $DCC_PATH/config
    echo 0x25023258 2 > $DCC_PATH/config
    echo 0x25023308  > $DCC_PATH/config
    echo 0x25023318  > $DCC_PATH/config
    echo 0x25025000  > $DCC_PATH/config
    echo 0x25025010  > $DCC_PATH/config
    echo 0x25038100  > $DCC_PATH/config
    echo 0x2503c030  > $DCC_PATH/config
    echo 0x2504002c 4 > $DCC_PATH/config
    echo 0x25040048 5 > $DCC_PATH/config
    echo 0x25040060 2 > $DCC_PATH/config
    echo 0x25040070 5 > $DCC_PATH/config
    echo 0x25042044 3 > $DCC_PATH/config
    echo 0x250420b0  > $DCC_PATH/config
    echo 0x25042104  > $DCC_PATH/config
    echo 0x25042114  > $DCC_PATH/config
    echo 0x25048004 5 > $DCC_PATH/config
    echo 0x2504c034  > $DCC_PATH/config
    echo 0x25050020  > $DCC_PATH/config
    echo 0x2506004c 8 > $DCC_PATH/config
    echo 0x2507B03C  > $DCC_PATH/config
    echo 0x2507B05C  > $DCC_PATH/config
    echo 0x2507a01c 4 > $DCC_PATH/config
    echo 0x2507a034 4 > $DCC_PATH/config
    echo 0x2507a058  > $DCC_PATH/config
    echo 0x2507a060  > $DCC_PATH/config
    echo 0x2507a068  > $DCC_PATH/config
    echo 0x2507a200  > $DCC_PATH/config
    echo 0x2507b020  > $DCC_PATH/config
    echo 0x2507b030 3 > $DCC_PATH/config
    echo 0x2507b040 7 > $DCC_PATH/config
    echo 0x2507b060 8 > $DCC_PATH/config
    echo 0x2507b084  > $DCC_PATH/config
    echo 0x2507b090 5 > $DCC_PATH/config
    echo 0x2507b18c  > $DCC_PATH/config
    echo 0x2507b1b0  > $DCC_PATH/config
    echo 0x2507b204 13 > $DCC_PATH/config
    echo 0x250A6010  > $DCC_PATH/config
    echo 0x250a002c  > $DCC_PATH/config
    echo 0x250a009c 2 > $DCC_PATH/config
    echo 0x250a00a8 3 > $DCC_PATH/config
    echo 0x250a00b8  > $DCC_PATH/config
    echo 0x250a00c0 2 > $DCC_PATH/config
    echo 0x250a00cc 4 > $DCC_PATH/config
    echo 0x250a00e0  > $DCC_PATH/config
    echo 0x250a00e8  > $DCC_PATH/config
    echo 0x250a00f0  > $DCC_PATH/config
    echo 0x250a00f0  > $DCC_PATH/config
    echo 0x250a0100  > $DCC_PATH/config
    echo 0x250a0108  > $DCC_PATH/config
    echo 0x250a0110  > $DCC_PATH/config
    echo 0x250a0118  > $DCC_PATH/config
    echo 0x250a0120  > $DCC_PATH/config
    echo 0x250a0128  > $DCC_PATH/config
    echo 0x250a1010  > $DCC_PATH/config
    echo 0x250a1070  > $DCC_PATH/config
    echo 0x250a3004  > $DCC_PATH/config
    echo 0x250a6008 2 > $DCC_PATH/config
    echo 0x250a7008 3 > $DCC_PATH/config
    echo 0x250a9004  > $DCC_PATH/config
    echo 0x250a9010 3 > $DCC_PATH/config
    echo 0x250a9020 3 > $DCC_PATH/config
    echo 0x250a9030 3 > $DCC_PATH/config
    echo 0x250a9040 3 > $DCC_PATH/config
    echo 0x250a9050 3 > $DCC_PATH/config
    echo 0x250aa004  > $DCC_PATH/config
    echo 0x250aa010 3 > $DCC_PATH/config
    echo 0x250aa020 3 > $DCC_PATH/config
    echo 0x250aa030 3 > $DCC_PATH/config
    echo 0x250aa040 3 > $DCC_PATH/config
    echo 0x250aa050 3 > $DCC_PATH/config
    echo 0x250b001c  > $DCC_PATH/config
    echo 0x250b101c  > $DCC_PATH/config
    echo 0x250b201c  > $DCC_PATH/config
    echo 0x250b301c  > $DCC_PATH/config
    echo 0x250b401c  > $DCC_PATH/config
    echo 0x250b501c  > $DCC_PATH/config
    echo 0x250b601c  > $DCC_PATH/config
    echo 0x250b701c  > $DCC_PATH/config
    echo 0x250b801c  > $DCC_PATH/config
    echo 0x250b901c  > $DCC_PATH/config
    echo 0x250ba01c  > $DCC_PATH/config
    echo 0x250bb01c  > $DCC_PATH/config
    echo 0x250bc01c  > $DCC_PATH/config
    echo 0x250bd01c  > $DCC_PATH/config
    echo 0x250be01c  > $DCC_PATH/config
    echo 0x250bf01c  > $DCC_PATH/config
    echo 0x25200004  > $DCC_PATH/config
    echo 0x25201004  > $DCC_PATH/config
    echo 0x25202004  > $DCC_PATH/config
    echo 0x25203004  > $DCC_PATH/config
    echo 0x25204004  > $DCC_PATH/config
    echo 0x25205004  > $DCC_PATH/config
    echo 0x25206004  > $DCC_PATH/config
    echo 0x25207004  > $DCC_PATH/config
    echo 0x25208004  > $DCC_PATH/config
    echo 0x25209004  > $DCC_PATH/config
    echo 0x2520a004  > $DCC_PATH/config
    echo 0x2520b004  > $DCC_PATH/config
    echo 0x2520c004  > $DCC_PATH/config
    echo 0x2520d004  > $DCC_PATH/config
    echo 0x2520e004  > $DCC_PATH/config
    echo 0x2520f004  > $DCC_PATH/config
    echo 0x25210004  > $DCC_PATH/config
    echo 0x25211004  > $DCC_PATH/config
    echo 0x25212004  > $DCC_PATH/config
    echo 0x25213004  > $DCC_PATH/config
    echo 0x25214004  > $DCC_PATH/config
    echo 0x25215004  > $DCC_PATH/config
    echo 0x25216004  > $DCC_PATH/config
    echo 0x25217004  > $DCC_PATH/config
    echo 0x25218004  > $DCC_PATH/config
    echo 0x25219004  > $DCC_PATH/config
    echo 0x2521a004  > $DCC_PATH/config
    echo 0x2521b004  > $DCC_PATH/config
    echo 0x2521c004  > $DCC_PATH/config
    echo 0x2521d004  > $DCC_PATH/config
    echo 0x2521e004  > $DCC_PATH/config
    echo 0x2521f004  > $DCC_PATH/config
    echo 0x25220348  > $DCC_PATH/config
    echo 0x25220480  > $DCC_PATH/config
    echo 0x25222400  > $DCC_PATH/config
    echo 0x25223210  > $DCC_PATH/config
    echo 0x25223220 4 > $DCC_PATH/config
    echo 0x25223258 2 > $DCC_PATH/config
    echo 0x25223308  > $DCC_PATH/config
    echo 0x25223318  > $DCC_PATH/config
    echo 0x25225000  > $DCC_PATH/config
    echo 0x25225010  > $DCC_PATH/config
    echo 0x25238100  > $DCC_PATH/config
    echo 0x2523c030  > $DCC_PATH/config
    echo 0x2524002c 4 > $DCC_PATH/config
    echo 0x25240048 5 > $DCC_PATH/config
    echo 0x25240060 2 > $DCC_PATH/config
    echo 0x25240070 5 > $DCC_PATH/config
    echo 0x25242044 3 > $DCC_PATH/config
    echo 0x252420b0  > $DCC_PATH/config
    echo 0x25242104  > $DCC_PATH/config
    echo 0x25242114  > $DCC_PATH/config
    echo 0x25248004 5 > $DCC_PATH/config
    echo 0x2524c034  > $DCC_PATH/config
    echo 0x25250020  > $DCC_PATH/config
    echo 0x2526004c 8 > $DCC_PATH/config
    echo 0x2527B03C  > $DCC_PATH/config
    echo 0x2527B05C  > $DCC_PATH/config
    echo 0x2527a01c 4 > $DCC_PATH/config
    echo 0x2527a034 4 > $DCC_PATH/config
    echo 0x2527a058  > $DCC_PATH/config
    echo 0x2527a060  > $DCC_PATH/config
    echo 0x2527a068  > $DCC_PATH/config
    echo 0x2527a200  > $DCC_PATH/config
    echo 0x2527b020  > $DCC_PATH/config
    echo 0x2527b030 3 > $DCC_PATH/config
    echo 0x2527b040 7 > $DCC_PATH/config
    echo 0x2527b060 8 > $DCC_PATH/config
    echo 0x2527b084  > $DCC_PATH/config
    echo 0x2527b090 5 > $DCC_PATH/config
    echo 0x2527b18c  > $DCC_PATH/config
    echo 0x2527b1b0  > $DCC_PATH/config
    echo 0x2527b204 13 > $DCC_PATH/config
    echo 0x252a002c  > $DCC_PATH/config
    echo 0x252a009c 2 > $DCC_PATH/config
    echo 0x252a00a8 3 > $DCC_PATH/config
    echo 0x252a00b8  > $DCC_PATH/config
    echo 0x252a00c0 2 > $DCC_PATH/config
    echo 0x252a00cc 4 > $DCC_PATH/config
    echo 0x252a00e0  > $DCC_PATH/config
    echo 0x252a00e8  > $DCC_PATH/config
    echo 0x252a00f0  > $DCC_PATH/config
    echo 0x252a00f0  > $DCC_PATH/config
    echo 0x252a0100  > $DCC_PATH/config
    echo 0x252a0108  > $DCC_PATH/config
    echo 0x252a0110  > $DCC_PATH/config
    echo 0x252a0118  > $DCC_PATH/config
    echo 0x252a0120  > $DCC_PATH/config
    echo 0x252a0128  > $DCC_PATH/config
    echo 0x252a1010  > $DCC_PATH/config
    echo 0x252a1070  > $DCC_PATH/config
    echo 0x252a3004  > $DCC_PATH/config
    echo 0x252a6008 3 > $DCC_PATH/config
    echo 0x252a7008 3 > $DCC_PATH/config
    echo 0x252a9004  > $DCC_PATH/config
    echo 0x252a9010 3 > $DCC_PATH/config
    echo 0x252a9020 3 > $DCC_PATH/config
    echo 0x252a9030 3 > $DCC_PATH/config
    echo 0x252a9040 3 > $DCC_PATH/config
    echo 0x252a9050 3 > $DCC_PATH/config
    echo 0x252aa004  > $DCC_PATH/config
    echo 0x252aa010 3 > $DCC_PATH/config
    echo 0x252aa020 3 > $DCC_PATH/config
    echo 0x252aa030 3 > $DCC_PATH/config
    echo 0x252aa040 3 > $DCC_PATH/config
    echo 0x252aa050 3 > $DCC_PATH/config
    echo 0x252b001c  > $DCC_PATH/config
    echo 0x252b101c  > $DCC_PATH/config
    echo 0x252b201c  > $DCC_PATH/config
    echo 0x252b301c  > $DCC_PATH/config
    echo 0x252b401c  > $DCC_PATH/config
    echo 0x252b501c  > $DCC_PATH/config
    echo 0x252b601c  > $DCC_PATH/config
    echo 0x252b701c  > $DCC_PATH/config
    echo 0x252b801c  > $DCC_PATH/config
    echo 0x252b901c  > $DCC_PATH/config
    echo 0x252ba01c  > $DCC_PATH/config
    echo 0x252bb01c  > $DCC_PATH/config
    echo 0x252bc01c  > $DCC_PATH/config
    echo 0x252bd01c  > $DCC_PATH/config
    echo 0x252be01c  > $DCC_PATH/config
    echo 0x252bf01c  > $DCC_PATH/config
    echo 0x25400004  > $DCC_PATH/config
    echo 0x25401004  > $DCC_PATH/config
    echo 0x25402004  > $DCC_PATH/config
    echo 0x25403004  > $DCC_PATH/config
    echo 0x25404004  > $DCC_PATH/config
    echo 0x25405004  > $DCC_PATH/config
    echo 0x25406004  > $DCC_PATH/config
    echo 0x25407004  > $DCC_PATH/config
    echo 0x25408004  > $DCC_PATH/config
    echo 0x25409004  > $DCC_PATH/config
    echo 0x2540a004  > $DCC_PATH/config
    echo 0x2540b004  > $DCC_PATH/config
    echo 0x2540c004  > $DCC_PATH/config
    echo 0x2540d004  > $DCC_PATH/config
    echo 0x2540e004  > $DCC_PATH/config
    echo 0x2540f004  > $DCC_PATH/config
    echo 0x25410004  > $DCC_PATH/config
    echo 0x25411004  > $DCC_PATH/config
    echo 0x25412004  > $DCC_PATH/config
    echo 0x25413004  > $DCC_PATH/config
    echo 0x25414004  > $DCC_PATH/config
    echo 0x25415004  > $DCC_PATH/config
    echo 0x25416004  > $DCC_PATH/config
    echo 0x25417004  > $DCC_PATH/config
    echo 0x25418004  > $DCC_PATH/config
    echo 0x25419004  > $DCC_PATH/config
    echo 0x2541a004  > $DCC_PATH/config
    echo 0x2541b004  > $DCC_PATH/config
    echo 0x2541c004  > $DCC_PATH/config
    echo 0x2541d004  > $DCC_PATH/config
    echo 0x2541e004  > $DCC_PATH/config
    echo 0x2541f004  > $DCC_PATH/config
    echo 0x25420348  > $DCC_PATH/config
    echo 0x25420480  > $DCC_PATH/config
    echo 0x25422400  > $DCC_PATH/config
    echo 0x25423210  > $DCC_PATH/config
    echo 0x25423220 4 > $DCC_PATH/config
    echo 0x25423258 2 > $DCC_PATH/config
    echo 0x25423308  > $DCC_PATH/config
    echo 0x25423318  > $DCC_PATH/config
    echo 0x25425000  > $DCC_PATH/config
    echo 0x25425010  > $DCC_PATH/config
    echo 0x25438100  > $DCC_PATH/config
    echo 0x2543c030  > $DCC_PATH/config
    echo 0x2544002c 4 > $DCC_PATH/config
    echo 0x25440048 5 > $DCC_PATH/config
    echo 0x25440060 2 > $DCC_PATH/config
    echo 0x25440070 5 > $DCC_PATH/config
    echo 0x25442044 3 > $DCC_PATH/config
    echo 0x254420b0  > $DCC_PATH/config
    echo 0x25442104  > $DCC_PATH/config
    echo 0x25442114  > $DCC_PATH/config
    echo 0x25448004 5 > $DCC_PATH/config
    echo 0x2544c034  > $DCC_PATH/config
    echo 0x25450020  > $DCC_PATH/config
    echo 0x2546004c 8 > $DCC_PATH/config
    echo 0x2547B03C  > $DCC_PATH/config
    echo 0x2547B05C  > $DCC_PATH/config
    echo 0x2547a01c 4 > $DCC_PATH/config
    echo 0x2547a034 4 > $DCC_PATH/config
    echo 0x2547a058  > $DCC_PATH/config
    echo 0x2547a060  > $DCC_PATH/config
    echo 0x2547a068  > $DCC_PATH/config
    echo 0x2547a200  > $DCC_PATH/config
    echo 0x2547b020  > $DCC_PATH/config
    echo 0x2547b030 3 > $DCC_PATH/config
    echo 0x2547b040 7 > $DCC_PATH/config
    echo 0x2547b060 8 > $DCC_PATH/config
    echo 0x2547b084  > $DCC_PATH/config
    echo 0x2547b090 5 > $DCC_PATH/config
    echo 0x2547b18c  > $DCC_PATH/config
    echo 0x2547b1b0  > $DCC_PATH/config
    echo 0x2547b204 13 > $DCC_PATH/config
    echo 0x254a002c  > $DCC_PATH/config
    echo 0x254a009c 2 > $DCC_PATH/config
    echo 0x254a00a8 3 > $DCC_PATH/config
    echo 0x254a00b8  > $DCC_PATH/config
    echo 0x254a00c0 2 > $DCC_PATH/config
    echo 0x254a00cc 4 > $DCC_PATH/config
    echo 0x254a00e0  > $DCC_PATH/config
    echo 0x254a00e8  > $DCC_PATH/config
    echo 0x254a00f0  > $DCC_PATH/config
    echo 0x254a00f0  > $DCC_PATH/config
    echo 0x254a0100  > $DCC_PATH/config
    echo 0x254a0108  > $DCC_PATH/config
    echo 0x254a0110  > $DCC_PATH/config
    echo 0x254a0118  > $DCC_PATH/config
    echo 0x254a0120  > $DCC_PATH/config
    echo 0x254a0128  > $DCC_PATH/config
    echo 0x254a1010  > $DCC_PATH/config
    echo 0x254a1070  > $DCC_PATH/config
    echo 0x254a3004  > $DCC_PATH/config
    echo 0x254a6008 3 > $DCC_PATH/config
    echo 0x254a7008 3 > $DCC_PATH/config
    echo 0x254a9004  > $DCC_PATH/config
    echo 0x254a9010 3 > $DCC_PATH/config
    echo 0x254a9020 3 > $DCC_PATH/config
    echo 0x254a9030 3 > $DCC_PATH/config
    echo 0x254a9040 3 > $DCC_PATH/config
    echo 0x254a9050 3 > $DCC_PATH/config
    echo 0x254aa004  > $DCC_PATH/config
    echo 0x254aa010 3 > $DCC_PATH/config
    echo 0x254aa020 3 > $DCC_PATH/config
    echo 0x254aa030 3 > $DCC_PATH/config
    echo 0x254aa040 3 > $DCC_PATH/config
    echo 0x254aa050 3 > $DCC_PATH/config
    echo 0x254b001c  > $DCC_PATH/config
    echo 0x254b101c  > $DCC_PATH/config
    echo 0x254b201c  > $DCC_PATH/config
    echo 0x254b301c  > $DCC_PATH/config
    echo 0x254b401c  > $DCC_PATH/config
    echo 0x254b501c  > $DCC_PATH/config
    echo 0x254b601c  > $DCC_PATH/config
    echo 0x254b701c  > $DCC_PATH/config
    echo 0x254b801c  > $DCC_PATH/config
    echo 0x254b901c  > $DCC_PATH/config
    echo 0x254ba01c  > $DCC_PATH/config
    echo 0x254bb01c  > $DCC_PATH/config
    echo 0x254bc01c  > $DCC_PATH/config
    echo 0x254bd01c  > $DCC_PATH/config
    echo 0x254be01c  > $DCC_PATH/config
    echo 0x254bf01c  > $DCC_PATH/config
    echo 0x25600004  > $DCC_PATH/config
    echo 0x25601004  > $DCC_PATH/config
    echo 0x25602004  > $DCC_PATH/config
    echo 0x25603004  > $DCC_PATH/config
    echo 0x25604004  > $DCC_PATH/config
    echo 0x25605004  > $DCC_PATH/config
    echo 0x25606004  > $DCC_PATH/config
    echo 0x25607004  > $DCC_PATH/config
    echo 0x25608004  > $DCC_PATH/config
    echo 0x25609004  > $DCC_PATH/config
    echo 0x2560a004  > $DCC_PATH/config
    echo 0x2560b004  > $DCC_PATH/config
    echo 0x2560c004  > $DCC_PATH/config
    echo 0x2560d004  > $DCC_PATH/config
    echo 0x2560e004  > $DCC_PATH/config
    echo 0x2560f004  > $DCC_PATH/config
    echo 0x25610004  > $DCC_PATH/config
    echo 0x25611004  > $DCC_PATH/config
    echo 0x25612004  > $DCC_PATH/config
    echo 0x25613004  > $DCC_PATH/config
    echo 0x25614004  > $DCC_PATH/config
    echo 0x25615004  > $DCC_PATH/config
    echo 0x25616004  > $DCC_PATH/config
    echo 0x25617004  > $DCC_PATH/config
    echo 0x25618004  > $DCC_PATH/config
    echo 0x25619004  > $DCC_PATH/config
    echo 0x2561a004  > $DCC_PATH/config
    echo 0x2561b004  > $DCC_PATH/config
    echo 0x2561c004  > $DCC_PATH/config
    echo 0x2561d004  > $DCC_PATH/config
    echo 0x2561e004  > $DCC_PATH/config
    echo 0x2561f004  > $DCC_PATH/config
    echo 0x25620348  > $DCC_PATH/config
    echo 0x25620480  > $DCC_PATH/config
    echo 0x25622400  > $DCC_PATH/config
    echo 0x25623210  > $DCC_PATH/config
    echo 0x25623220 4 > $DCC_PATH/config
    echo 0x25623258 2 > $DCC_PATH/config
    echo 0x25623308  > $DCC_PATH/config
    echo 0x25623318  > $DCC_PATH/config
    echo 0x25625000  > $DCC_PATH/config
    echo 0x25625010  > $DCC_PATH/config
    echo 0x25638100  > $DCC_PATH/config
    echo 0x2563c030  > $DCC_PATH/config
    echo 0x2564002c 4 > $DCC_PATH/config
    echo 0x25640048 5 > $DCC_PATH/config
    echo 0x25640060 2 > $DCC_PATH/config
    echo 0x25640070 5 > $DCC_PATH/config
    echo 0x25642044 3 > $DCC_PATH/config
    echo 0x256420b0  > $DCC_PATH/config
    echo 0x25642104  > $DCC_PATH/config
    echo 0x25642114  > $DCC_PATH/config
    echo 0x25648004 5 > $DCC_PATH/config
    echo 0x2564c034  > $DCC_PATH/config
    echo 0x25650020  > $DCC_PATH/config
    echo 0x2566004c 8 > $DCC_PATH/config
    echo 0x2567B03C  > $DCC_PATH/config
    echo 0x2567B05C  > $DCC_PATH/config
    echo 0x2567a01c 4 > $DCC_PATH/config
    echo 0x2567a034 4 > $DCC_PATH/config
    echo 0x2567a058  > $DCC_PATH/config
    echo 0x2567a060  > $DCC_PATH/config
    echo 0x2567a068  > $DCC_PATH/config
    echo 0x2567a200  > $DCC_PATH/config
    echo 0x2567b020  > $DCC_PATH/config
    echo 0x2567b030 3 > $DCC_PATH/config
    echo 0x2567b040 7 > $DCC_PATH/config
    echo 0x2567b060 8 > $DCC_PATH/config
    echo 0x2567b084  > $DCC_PATH/config
    echo 0x2567b090 5 > $DCC_PATH/config
    echo 0x2567b18c  > $DCC_PATH/config
    echo 0x2567b1b0  > $DCC_PATH/config
    echo 0x2567b204 13 > $DCC_PATH/config
    echo 0x256a002c  > $DCC_PATH/config
    echo 0x256a009c 2 > $DCC_PATH/config
    echo 0x256a00a8 3 > $DCC_PATH/config
    echo 0x256a00b8  > $DCC_PATH/config
    echo 0x256a00c0 2 > $DCC_PATH/config
    echo 0x256a00cc 4 > $DCC_PATH/config
    echo 0x256a00e0  > $DCC_PATH/config
    echo 0x256a00e8  > $DCC_PATH/config
    echo 0x256a00f0  > $DCC_PATH/config
    echo 0x256a00f0  > $DCC_PATH/config
    echo 0x256a0100  > $DCC_PATH/config
    echo 0x256a0108  > $DCC_PATH/config
    echo 0x256a0110  > $DCC_PATH/config
    echo 0x256a0118  > $DCC_PATH/config
    echo 0x256a0120  > $DCC_PATH/config
    echo 0x256a0128  > $DCC_PATH/config
    echo 0x256a1010  > $DCC_PATH/config
    echo 0x256a1070  > $DCC_PATH/config
    echo 0x256a3004  > $DCC_PATH/config
    echo 0x256a6008 3 > $DCC_PATH/config
    echo 0x256a7008 3 > $DCC_PATH/config
    echo 0x256a9004  > $DCC_PATH/config
    echo 0x256a9010 3 > $DCC_PATH/config
    echo 0x256a9020 3 > $DCC_PATH/config
    echo 0x256a9030 3 > $DCC_PATH/config
    echo 0x256a9040 3 > $DCC_PATH/config
    echo 0x256a9050 3 > $DCC_PATH/config
    echo 0x256aa004  > $DCC_PATH/config
    echo 0x256aa010 3 > $DCC_PATH/config
    echo 0x256aa020 3 > $DCC_PATH/config
    echo 0x256aa030 3 > $DCC_PATH/config
    echo 0x256aa040 3 > $DCC_PATH/config
    echo 0x256aa050 3 > $DCC_PATH/config
    echo 0x256b001c  > $DCC_PATH/config
    echo 0x256b101c  > $DCC_PATH/config
    echo 0x256b201c  > $DCC_PATH/config
    echo 0x256b301c  > $DCC_PATH/config
    echo 0x256b401c  > $DCC_PATH/config
    echo 0x256b501c  > $DCC_PATH/config
    echo 0x256b601c  > $DCC_PATH/config
    echo 0x256b701c  > $DCC_PATH/config
    echo 0x256b801c  > $DCC_PATH/config
    echo 0x256b901c  > $DCC_PATH/config
    echo 0x256ba01c  > $DCC_PATH/config
    echo 0x256bb01c  > $DCC_PATH/config
    echo 0x256bc01c  > $DCC_PATH/config
    echo 0x256bd01c  > $DCC_PATH/config
    echo 0x256be01c  > $DCC_PATH/config
    echo 0x256bf01c  > $DCC_PATH/config
    echo 0x25820348  > $DCC_PATH/config
    echo 0x25820480  > $DCC_PATH/config
    echo 0x25822400  > $DCC_PATH/config
    echo 0x25823220 4 > $DCC_PATH/config
    echo 0x25823258 2 > $DCC_PATH/config
    echo 0x25823308  > $DCC_PATH/config
    echo 0x25823318  > $DCC_PATH/config
    echo 0x25838100  > $DCC_PATH/config
    echo 0x2583c030  > $DCC_PATH/config
    echo 0x25842044 3 > $DCC_PATH/config
    echo 0x258420b0  > $DCC_PATH/config
    echo 0x25842104  > $DCC_PATH/config
    echo 0x25842114  > $DCC_PATH/config
    echo 0x25848004 5 > $DCC_PATH/config
    echo 0x2584c034  > $DCC_PATH/config
    echo 0x25850020  > $DCC_PATH/config
    echo 0x2586004c 8 > $DCC_PATH/config
    echo 0x258a4040 48 > $DCC_PATH/config
    echo 0x258b0000  > $DCC_PATH/config
    echo 0x258b005c  > $DCC_PATH/config
    echo 0x258b1000  > $DCC_PATH/config
    echo 0x258b105c  > $DCC_PATH/config
    echo 0x258b2000  > $DCC_PATH/config
    echo 0x258b205c  > $DCC_PATH/config
    echo 0x258b3000  > $DCC_PATH/config
    echo 0x258b305c  > $DCC_PATH/config
    echo 0x258b4000  > $DCC_PATH/config
    echo 0x258b405c  > $DCC_PATH/config
    echo 0x258b5000  > $DCC_PATH/config
    echo 0x258b505c  > $DCC_PATH/config
    echo 0x258b6000  > $DCC_PATH/config
    echo 0x258b605c  > $DCC_PATH/config
    echo 0x258b7000  > $DCC_PATH/config
    echo 0x258b705c  > $DCC_PATH/config
    echo 0x258b8000  > $DCC_PATH/config
    echo 0x258b805c  > $DCC_PATH/config
    echo 0x258b9000  > $DCC_PATH/config
    echo 0x258b905c  > $DCC_PATH/config
    echo 0x258ba000  > $DCC_PATH/config
    echo 0x258ba05c  > $DCC_PATH/config
    echo 0x258bb000  > $DCC_PATH/config
    echo 0x258bb05c  > $DCC_PATH/config
    echo 0x258bc000  > $DCC_PATH/config
    echo 0x258bc05c  > $DCC_PATH/config
    echo 0x258bd000  > $DCC_PATH/config
    echo 0x258bd05c  > $DCC_PATH/config
    echo 0x258be000  > $DCC_PATH/config
    echo 0x258be05c  > $DCC_PATH/config
    echo 0x258bf000  > $DCC_PATH/config
    echo 0x258bf05c  > $DCC_PATH/config
    echo 0x25a00004  > $DCC_PATH/config
    echo 0x25a01004  > $DCC_PATH/config
    echo 0x25a02004  > $DCC_PATH/config
    echo 0x25a03004  > $DCC_PATH/config
    echo 0x25a04004  > $DCC_PATH/config
    echo 0x25a05004  > $DCC_PATH/config
    echo 0x25a06004  > $DCC_PATH/config
    echo 0x25a07004  > $DCC_PATH/config
    echo 0x25a08004  > $DCC_PATH/config
    echo 0x25a09004  > $DCC_PATH/config
    echo 0x25a0a004  > $DCC_PATH/config
    echo 0x25a0b004  > $DCC_PATH/config
    echo 0x25a0c004  > $DCC_PATH/config
    echo 0x25a0d004  > $DCC_PATH/config
    echo 0x25a0e004  > $DCC_PATH/config
    echo 0x25a0f004  > $DCC_PATH/config
    echo 0x25a10004  > $DCC_PATH/config
    echo 0x25a11004  > $DCC_PATH/config
    echo 0x25a12004  > $DCC_PATH/config
    echo 0x25a13004  > $DCC_PATH/config
    echo 0x25a14004  > $DCC_PATH/config
    echo 0x25a15004  > $DCC_PATH/config
    echo 0x25a16004  > $DCC_PATH/config
    echo 0x25a17004  > $DCC_PATH/config
    echo 0x25a18004  > $DCC_PATH/config
    echo 0x25a19004  > $DCC_PATH/config
    echo 0x25a1a004  > $DCC_PATH/config
    echo 0x25a1b004  > $DCC_PATH/config
    echo 0x25a1c004  > $DCC_PATH/config
    echo 0x25a1d004  > $DCC_PATH/config
    echo 0x25a1e004  > $DCC_PATH/config
    echo 0x25a1f004  > $DCC_PATH/config
    echo 0x25a20348  > $DCC_PATH/config
    echo 0x25a20480  > $DCC_PATH/config
    echo 0x25a22400  > $DCC_PATH/config
    echo 0x25a23220 4 > $DCC_PATH/config
    echo 0x25a23258 2 > $DCC_PATH/config
    echo 0x25a23308  > $DCC_PATH/config
    echo 0x25a23318  > $DCC_PATH/config
    echo 0x25a38100  > $DCC_PATH/config
    echo 0x25a3c030  > $DCC_PATH/config
    echo 0x25a42044 3 > $DCC_PATH/config
    echo 0x25a420b0  > $DCC_PATH/config
    echo 0x25a42104  > $DCC_PATH/config
    echo 0x25a42114  > $DCC_PATH/config
    echo 0x25a48004 5 > $DCC_PATH/config
    echo 0x25a4c034  > $DCC_PATH/config
    echo 0x25a50020  > $DCC_PATH/config
    echo 0x25a6004c 8 > $DCC_PATH/config
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

config_dcc_gic()
{
    echo 0x17AA006C 1 > $DCC_PATH/config
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

config_dcc_llcc()
{
    echo 0x2503400C  > $DCC_PATH/config
    echo 0x25821004  > $DCC_PATH/config
    echo 0x2582100C  > $DCC_PATH/config
    echo 0x25821014  > $DCC_PATH/config
    echo 0x2582101C  > $DCC_PATH/config
    echo 0x25821024  > $DCC_PATH/config
    echo 0x2582102C  > $DCC_PATH/config
    echo 0x25821034  > $DCC_PATH/config
    echo 0x2582103C  > $DCC_PATH/config
    echo 0x25821044  > $DCC_PATH/config
    echo 0x2582104C  > $DCC_PATH/config
    echo 0x25821054  > $DCC_PATH/config
    echo 0x2582105C  > $DCC_PATH/config
    echo 0x25821064  > $DCC_PATH/config
    echo 0x2582106C  > $DCC_PATH/config
    echo 0x25821074  > $DCC_PATH/config
    echo 0x2582107C  > $DCC_PATH/config
    echo 0x25821084  > $DCC_PATH/config
    echo 0x2582108C  > $DCC_PATH/config
    echo 0x25821094  > $DCC_PATH/config
    echo 0x2582109C  > $DCC_PATH/config
    echo 0x258210A4  > $DCC_PATH/config
    echo 0x258210AC  > $DCC_PATH/config
    echo 0x258210B4  > $DCC_PATH/config
    echo 0x258210BC  > $DCC_PATH/config
    echo 0x258210C4  > $DCC_PATH/config
    echo 0x258210CC  > $DCC_PATH/config
    echo 0x258210D4  > $DCC_PATH/config
    echo 0x258210DC  > $DCC_PATH/config
    echo 0x258210E4  > $DCC_PATH/config
    echo 0x258210EC  > $DCC_PATH/config
    echo 0x258210F4  > $DCC_PATH/config
    echo 0x258210FC  > $DCC_PATH/config
}

config_dcc_qdss()
{
    echo 0x12822000  > $DCC_PATH/config
    echo 0x12824D04 2 > $DCC_PATH/config
}

config_dcc_video()
{
    echo 0x132018  > $DCC_PATH/config
    echo 0x132030  > $DCC_PATH/config
    echo 0xAA00070 4 > $DCC_PATH/config
    echo 0xAA1F004  > $DCC_PATH/config
    echo 0xAAA0018  > $DCC_PATH/config
    echo 0xAAA004C  > $DCC_PATH/config
    echo 0xAAA0150  > $DCC_PATH/config
    echo 0xAAB000C 2 > $DCC_PATH/config
    echo 0xAAB0050  > $DCC_PATH/config
    echo 0xAAB0088  > $DCC_PATH/config
    echo 0xAAC0010  > $DCC_PATH/config
    echo 0xAAE0000  > $DCC_PATH/config
    echo 0xAAF804C  > $DCC_PATH/config
    echo 0xAAF8064  > $DCC_PATH/config
    echo 0xAAF80A4  > $DCC_PATH/config
    echo 0xAAF80B8  > $DCC_PATH/config
    echo 0xAAF8150  > $DCC_PATH/config
}

config_dcc_eva()
{
    echo 0x132024   > $DCC_PATH/config
    echo 0xAB00050  > $DCC_PATH/config
    echo 0xAB1F004  > $DCC_PATH/config
    echo 0xABA0018  > $DCC_PATH/config
    echo 0xABA004C  > $DCC_PATH/config
    echo 0xABA0150  > $DCC_PATH/config
    echo 0xABB000C  > $DCC_PATH/config
    echo 0xABB0050  > $DCC_PATH/config
    echo 0xABB0088  > $DCC_PATH/config
    echo 0xABC0010  > $DCC_PATH/config
    echo 0xABE0008  > $DCC_PATH/config
    echo 0xABE001C  > $DCC_PATH/config
    echo 0xABE0024  > $DCC_PATH/config
    echo 0xABF8078  > $DCC_PATH/config
    echo 0xABF8090  > $DCC_PATH/config
    echo 0xABF80CC  > $DCC_PATH/config
    echo 0xABF80E0  > $DCC_PATH/config
}

config_dcc_audio()
{
    echo 0x031B0080  > $DCC_PATH/config
    echo 0x031B0100  > $DCC_PATH/config
    echo 0x031B0108  > $DCC_PATH/config
    echo 0x031B0110  > $DCC_PATH/config
    echo 0x031B0244 2 > $DCC_PATH/config
    echo 0x031B0264 2 > $DCC_PATH/config
    echo 0x031B0284 2 > $DCC_PATH/config
    echo 0x031B02a4 2 > $DCC_PATH/config
    echo 0x031B0400 2 > $DCC_PATH/config
    echo 0x031B0418  > $DCC_PATH/config
    echo 0x031B0480 2 > $DCC_PATH/config
    echo 0x031B0498  > $DCC_PATH/config
    echo 0x3200180  > $DCC_PATH/config
    echo 0x3200188  > $DCC_PATH/config
    echo 0x3200190  > $DCC_PATH/config
    echo 0x3200400 2 > $DCC_PATH/config
    echo 0x3200418  > $DCC_PATH/config
    echo 0x3200480 2 > $DCC_PATH/config
    echo 0x32004D8  > $DCC_PATH/config
    echo 0x32004c0 2 > $DCC_PATH/config
    echo 0x3200500 2 > $DCC_PATH/config
    echo 0x3200580 2 > $DCC_PATH/config
    echo 0x3200598  > $DCC_PATH/config
    echo 0x3220080  > $DCC_PATH/config
    echo 0x3220100  > $DCC_PATH/config
    echo 0x3220108  > $DCC_PATH/config
    echo 0x3220110  > $DCC_PATH/config
    echo 0x3220118  > $DCC_PATH/config
    echo 0x3220120  > $DCC_PATH/config
    echo 0x3220128  > $DCC_PATH/config
    echo 0x3220130  > $DCC_PATH/config
    echo 0x3220138  > $DCC_PATH/config
    echo 0x3220400 2 > $DCC_PATH/config
    echo 0x3220480 2 > $DCC_PATH/config
    echo 0x3220500 2 > $DCC_PATH/config
    echo 0x3220580 2 > $DCC_PATH/config
    echo 0x3220600 2 > $DCC_PATH/config
    echo 0x3220680 2 > $DCC_PATH/config
    echo 0x3220700 2 > $DCC_PATH/config
    echo 0x3220780 2 > $DCC_PATH/config
    echo 0x3240080  > $DCC_PATH/config
    echo 0x3240100  > $DCC_PATH/config
    echo 0x3240110  > $DCC_PATH/config
    echo 0x3240244 2 > $DCC_PATH/config
    echo 0x3240264 2 > $DCC_PATH/config
    echo 0x3240284 2 > $DCC_PATH/config
    echo 0x32402A4 2 > $DCC_PATH/config
    echo 0x3240400 2 > $DCC_PATH/config
    echo 0x3240418  > $DCC_PATH/config
    echo 0x3240480 2 > $DCC_PATH/config
    echo 0x3240498  > $DCC_PATH/config
    echo 0x33F0100  > $DCC_PATH/config
    echo 0x33F0108  > $DCC_PATH/config
    echo 0x33F0110  > $DCC_PATH/config
    echo 0x33F0118  > $DCC_PATH/config
    echo 0x33F0400 2 > $DCC_PATH/config
    echo 0x33F0480 2 > $DCC_PATH/config
    echo 0x33F0500 2 > $DCC_PATH/config
    echo 0x33F0580 2 > $DCC_PATH/config
}

config_dcc_gnoc_reg()
{
    echo 0x17600404  > $DCC_PATH/config
    echo 0x17600434  > $DCC_PATH/config
    echo 0x1760043C 2 > $DCC_PATH/config
}

config_dcc_gpu_aon()
{
    echo 0x3d50000 21 > $DCC_PATH/config
    echo 0x3d500d0  > $DCC_PATH/config
    echo 0x3d500d8  > $DCC_PATH/config
    echo 0x3d50100 3 > $DCC_PATH/config
    echo 0x3d50110 2 > $DCC_PATH/config
    echo 0x3d5011c  > $DCC_PATH/config
    echo 0x3d50200 5 > $DCC_PATH/config
    echo 0x3d50400 5 > $DCC_PATH/config
    echo 0x3d50450  > $DCC_PATH/config
    echo 0x3d50460 2 > $DCC_PATH/config
    echo 0x3d50490 12 > $DCC_PATH/config
    echo 0x3d50550  > $DCC_PATH/config
    echo 0x3d50d00 2 > $DCC_PATH/config
    echo 0x3d50d10  > $DCC_PATH/config
    echo 0x3d50d18 13 > $DCC_PATH/config
    echo 0x3d50fe0  > $DCC_PATH/config
    echo 0x3d50ff8  > $DCC_PATH/config
    echo 0x3d51010  > $DCC_PATH/config
    echo 0x3d51028  > $DCC_PATH/config
    echo 0x3d51280  > $DCC_PATH/config
    echo 0x3d51520  > $DCC_PATH/config
    echo 0x3d51538  > $DCC_PATH/config
    echo 0x3d51550  > $DCC_PATH/config
    echo 0x3d517c0  > $DCC_PATH/config
    echo 0x3d51a60  > $DCC_PATH/config
    echo 0x3d51a78  > $DCC_PATH/config
    echo 0x3d51a90  > $DCC_PATH/config
    echo 0x3d51aa8  > $DCC_PATH/config
    echo 0x3d51d00  > $DCC_PATH/config
    echo 0x3d51fa0  > $DCC_PATH/config
    echo 0x3d51fb8  > $DCC_PATH/config
    echo 0x3d52240  > $DCC_PATH/config
    echo 0x3d52258  > $DCC_PATH/config
    echo 0x3d524e0  > $DCC_PATH/config
    echo 0x3d8e100 8 > $DCC_PATH/config
    echo 0x3d8ec00 2 > $DCC_PATH/config
    echo 0x3d8ec0c  > $DCC_PATH/config
    echo 0x3d8ec14 10 > $DCC_PATH/config
    echo 0x3d8ec40 4 > $DCC_PATH/config
    echo 0x3d8ec54 2 > $DCC_PATH/config
    echo 0x3d8eca0  > $DCC_PATH/config
    echo 0x3d8ecc0  > $DCC_PATH/config
    echo 0x3d9200c 3 > $DCC_PATH/config
    echo 0x3d93000  > $DCC_PATH/config
    echo 0x3d94000 3 > $DCC_PATH/config
    echo 0x3d95000 5 > $DCC_PATH/config
    echo 0x3d96000 5 > $DCC_PATH/config
    echo 0x3d97000 5 > $DCC_PATH/config
    echo 0x3d98000 5 > $DCC_PATH/config
    echo 0x3d99000 7 > $DCC_PATH/config
    echo 0x3d99054 9 > $DCC_PATH/config
    echo 0x3d9907c 25 > $DCC_PATH/config
    echo 0x3d990e4 3 > $DCC_PATH/config
    echo 0x3d990f4 3 > $DCC_PATH/config
    echo 0x3d9910c  > $DCC_PATH/config
    echo 0x3d991e0 3 > $DCC_PATH/config
    echo 0x3d99224 2 > $DCC_PATH/config
    echo 0x3d99270 3 > $DCC_PATH/config
    echo 0x3d99280 2 > $DCC_PATH/config
    echo 0x3d99314 3 > $DCC_PATH/config
    echo 0x3d993a0 3 > $DCC_PATH/config
    echo 0x3d993e4 4 > $DCC_PATH/config
    echo 0x3d9942c  > $DCC_PATH/config
    echo 0x3d99470 3 > $DCC_PATH/config
    echo 0x3d99500 12 > $DCC_PATH/config
    echo 0x3d99550 3 > $DCC_PATH/config
    echo 0x3d99560 5 > $DCC_PATH/config
    echo 0x3d99578 2 > $DCC_PATH/config
    echo 0x3d9958c  > $DCC_PATH/config
    echo 0x3d995b4 7 > $DCC_PATH/config
    echo 0x3d995d8  > $DCC_PATH/config
    echo 0x3d995e0 3 > $DCC_PATH/config
    echo 0x3d9e000  > $DCC_PATH/config
    echo 0x3d9e040 5 > $DCC_PATH/config
    echo 0x3d9e080 5 > $DCC_PATH/config
    echo 0x3d9e0a0 3 > $DCC_PATH/config
    echo 0x3d9e0c8 11 > $DCC_PATH/config
    echo 0x3d9e0f8 26 > $DCC_PATH/config
    echo 0x3d9e200 4 > $DCC_PATH/config
    echo 0x3d9f000 2 > $DCC_PATH/config
}

config_dcc_gpu_gcc()
{
    echo 0x129000  > $DCC_PATH/config
    echo 0x12903c  > $DCC_PATH/config
    echo 0x171004  > $DCC_PATH/config
    echo 0x17100c 5 > $DCC_PATH/config
    echo 0x171150  > $DCC_PATH/config
    echo 0x17b000  > $DCC_PATH/config
    echo 0x17b03c  > $DCC_PATH/config
    echo 0x17c000  > $DCC_PATH/config
    echo 0x17c03c  > $DCC_PATH/config
    echo 0x17d000  > $DCC_PATH/config
    echo 0x17d03c  > $DCC_PATH/config
    echo 0x17e000  > $DCC_PATH/config
    echo 0x17e03c  > $DCC_PATH/config
    echo 0x186000  > $DCC_PATH/config
    echo 0x18603c  > $DCC_PATH/config
}

config_dcc_gpu_gmu()
{
    echo 0x3d7d000 12 > $DCC_PATH/config
    echo 0x3d7d03c 3 > $DCC_PATH/config
    echo 0x3d7d400  > $DCC_PATH/config
    echo 0x3d7d41c  > $DCC_PATH/config
    echo 0x3d7d424 3 > $DCC_PATH/config
    echo 0x3d7dc58  > $DCC_PATH/config
    echo 0x3d7dc94  > $DCC_PATH/config
    echo 0x3d7dca4  > $DCC_PATH/config
    echo 0x3d7dd58 2 > $DCC_PATH/config
    echo 0x3d7df80 2 > $DCC_PATH/config
    echo 0x3d7df90 2 > $DCC_PATH/config
    echo 0x3d7dfa0 2 > $DCC_PATH/config
    echo 0x3d7dfb0 2 > $DCC_PATH/config
    echo 0x3d7e000 5 > $DCC_PATH/config
    echo 0x3d7e01c 2 > $DCC_PATH/config
    echo 0x3d7e02c 2 > $DCC_PATH/config
    echo 0x3d7e03c  > $DCC_PATH/config
    echo 0x3d7e044  > $DCC_PATH/config
    echo 0x3d7e04c 5 > $DCC_PATH/config
    echo 0x3d7e064 4 > $DCC_PATH/config
    echo 0x3d7e090 13 > $DCC_PATH/config
    echo 0x3d7e100 2 > $DCC_PATH/config
    echo 0x3d7e130  > $DCC_PATH/config
    echo 0x3d7e140  > $DCC_PATH/config
    echo 0x3d7e500 2 > $DCC_PATH/config
    echo 0x3d7e50c  > $DCC_PATH/config
    echo 0x3d7e520  > $DCC_PATH/config
    echo 0x3d7e53c  > $DCC_PATH/config
    echo 0x3d7e550 2 > $DCC_PATH/config
    echo 0x3d7e574  > $DCC_PATH/config
    echo 0x3d7e5c0  > $DCC_PATH/config
    echo 0x3d7e5f0 3 > $DCC_PATH/config
    echo 0x3d7e600 2 > $DCC_PATH/config
    echo 0x3d7e610 3 > $DCC_PATH/config
    echo 0x3d7e648 2 > $DCC_PATH/config
    echo 0x3d7e658 9 > $DCC_PATH/config
    echo 0x3d7e700 16 > $DCC_PATH/config
    echo 0x3d7e7c4  > $DCC_PATH/config
    echo 0x3d7e7e0 3 > $DCC_PATH/config
    echo 0x3d7e7f0  > $DCC_PATH/config
    echo 0x3d7e800 4 > $DCC_PATH/config
    echo 0x3d7f050  > $DCC_PATH/config
    echo 0x3d7f060 2 > $DCC_PATH/config
    echo 0x3d7f080  > $DCC_PATH/config
    echo 0x3d7f090 3 > $DCC_PATH/config
    echo 0x3d80080  > $DCC_PATH/config
    echo 0x3d800d0 3 > $DCC_PATH/config
    echo 0x3d80c80 3 > $DCC_PATH/config
    echo 0x3d80c90 3 > $DCC_PATH/config
    echo 0x3d80ca0 3 > $DCC_PATH/config
    echo 0x3d80d40  > $DCC_PATH/config
}

config_dcc_gpu_gbif()
{
    echo 0x3d0f000 12 > $DCC_PATH/config
    echo 0x3d0f100 3 > $DCC_PATH/config
    echo 0x3d0f114 3 > $DCC_PATH/config
    echo 0x3d0f124 6 > $DCC_PATH/config
}

config_dcc_gpu_cp()
{
    echo 0x3d00000  > $DCC_PATH/config
    echo 0x3d00008  > $DCC_PATH/config
    echo 0x3d00020 6 > $DCC_PATH/config
    echo 0x3d00040 4 > $DCC_PATH/config
    echo 0x3d00054 2 > $DCC_PATH/config
    echo 0x3d00060  > $DCC_PATH/config
    echo 0x3d00068  > $DCC_PATH/config
    echo 0x3d00070  > $DCC_PATH/config
    echo 0x3d000a0 4 > $DCC_PATH/config
    echo 0x3d000b4 13 > $DCC_PATH/config
    echo 0x3d00100 20 > $DCC_PATH/config
    echo 0x3d00188 5 > $DCC_PATH/config
    echo 0x3d001a4 6 > $DCC_PATH/config
    echo 0x3d001c4 2 > $DCC_PATH/config
    echo 0x3d001d0  > $DCC_PATH/config
    echo 0x3d001d8 2 > $DCC_PATH/config
    echo 0x3d001fc 28 > $DCC_PATH/config
    echo 0x3d00274 19 > $DCC_PATH/config
    echo 0x3d002c8 35 > $DCC_PATH/config
    echo 0x3d0035c 12 > $DCC_PATH/config
    echo 0x3d00394 2 > $DCC_PATH/config
    echo 0x3d003a4 9 > $DCC_PATH/config
    echo 0x3d003d0 3 > $DCC_PATH/config
    echo 0x3d003e4 16 > $DCC_PATH/config
    echo 0x3d0042c 4 > $DCC_PATH/config
    echo 0x3d00444  > $DCC_PATH/config
    echo 0x3d00450 9 > $DCC_PATH/config
    echo 0x3d0047c 3 > $DCC_PATH/config
    echo 0x3d00494  > $DCC_PATH/config
    echo 0x3d0049c  > $DCC_PATH/config
    echo 0x3d004a4  > $DCC_PATH/config
    echo 0x3d004ac 7 > $DCC_PATH/config
    echo 0x3d004d0 5 > $DCC_PATH/config
    echo 0x3d004e8  > $DCC_PATH/config
    echo 0x3d004f0 4 > $DCC_PATH/config
    echo 0x3d00508 15 > $DCC_PATH/config
    echo 0x3d0054c 3 > $DCC_PATH/config
    echo 0x3d00560 2 > $DCC_PATH/config
    echo 0x3d00570  > $DCC_PATH/config
    echo 0x3d00598 20 > $DCC_PATH/config
    echo 0x3d00680  > $DCC_PATH/config
    echo 0x3d00f80  > $DCC_PATH/config
    echo 0x3d00fc0  > $DCC_PATH/config
    echo 0x3d01100  > $DCC_PATH/config
    echo 0x3d02000 31 > $DCC_PATH/config
    echo 0x3d02080 4 > $DCC_PATH/config
    echo 0x3d020d8 9 > $DCC_PATH/config
    echo 0x3d02100 8 > $DCC_PATH/config
    echo 0x3d0212c 5 > $DCC_PATH/config
    echo 0x3d022c0 12 > $DCC_PATH/config
    echo 0x3d02300 12 > $DCC_PATH/config
    echo 0x3d0239c 5 > $DCC_PATH/config
    echo 0x3d3c000 3 > $DCC_PATH/config
    echo 0x3d3d000  > $DCC_PATH/config
    echo 0x3d3e000 4 > $DCC_PATH/config
    echo 0x3d3f000 2 > $DCC_PATH/config
    echo 0x3d64000 13 > $DCC_PATH/config
    echo 0x3d6403c  > $DCC_PATH/config
    echo 0x3d66000 6 > $DCC_PATH/config
    echo 0x3d67000  > $DCC_PATH/config
    echo 0x3d68000 5 > $DCC_PATH/config
    echo 0x3d68020 11 > $DCC_PATH/config
    echo 0x3d68050  > $DCC_PATH/config
    echo 0x3d6805c  > $DCC_PATH/config
    echo 0x3d68064  > $DCC_PATH/config
    echo 0x3d68088  > $DCC_PATH/config
    echo 0x3d68090 6 > $DCC_PATH/config
    echo 0x3d680fc 31 > $DCC_PATH/config
    echo 0x3d68180 4 > $DCC_PATH/config
    echo 0x3d68194 2 > $DCC_PATH/config
    echo 0x3d681a0 15 > $DCC_PATH/config
    echo 0x3d681e0 4 > $DCC_PATH/config
    echo 0x3d69000 23 > $DCC_PATH/config
    echo 0x3d69080 14 > $DCC_PATH/config
    echo 0x3d690c0 2 > $DCC_PATH/config
    echo 0x3d690d4  > $DCC_PATH/config
    echo 0x3d690dc  > $DCC_PATH/config
    echo 0x3d690e8  > $DCC_PATH/config
    echo 0x3d69108  > $DCC_PATH/config
    echo 0x3d69158 3 > $DCC_PATH/config
    echo 0x3d6916c 3 > $DCC_PATH/config
    echo 0x3d6917c 4 > $DCC_PATH/config
}

config_turing_exp()
{
    echo 0x3238C108 0x3BFFF > $DCC_PATH/config_write
    echo 0x323005B4 0x1 > $DCC_PATH/config_write
    echo 0x3238C108 1 > $DCC_PATH/config
    echo 0x3238C108 1 > $DCC_PATH/config
    echo 0x3238C108 1 > $DCC_PATH/config
    echo 0x3238C108 1 > $DCC_PATH/config
    echo 0x3238C108 1 > $DCC_PATH/config
    echo 0x3238C108 1 > $DCC_PATH/config
    echo 0x3238C108 1 > $DCC_PATH/config
    echo 0x3238C108 1 > $DCC_PATH/config
    echo 0x3238C108 1 > $DCC_PATH/config
    echo 0x3238C108 1 > $DCC_PATH/config
}

enable_hw_events()
{
    # bail out if its perf config
    if [ "$debug_build" = false ]
    then
        return
    fi
    #configure tpdm hw events
    echo 1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/reset
    echo 0x0 0x0 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x0 0x0 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x1 0x1 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x1 0x1 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x2 0x2 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x2 0x2 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x3 0x3 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x3 0x3 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x6 0x6 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x6 0x6 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x7 0x7 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x7 0x7 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x8 0x8 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x8 0x8 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x9 0x9 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x9 0x9 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0xc 0xc 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0xc 0xc 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0xd 0xd 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0xd 0xd 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0xe 0xe 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0xe 0xe 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0xf 0xf 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0xf 0xf 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x12 0x12 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x12 0x12 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x13 0x13 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x13 0x13 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x14 0x14 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x14 0x14 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x15 0x15 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x15 0x15 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x1d 0x1d 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x1d 0x1d 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x1e 0x1e 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x1e 0x1e 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x1f 0x1f 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x1f 0x1f 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x20 0x20 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x20 0x20 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x40 0x40 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x40 0x40 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x41 0x41 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x41 0x41 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x42 0x42 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x42 0x42 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x43 0x43 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x43 0x43 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x5d 0x5d 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x5d 0x5d 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x5e 0x5e 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x5e 0x5e 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x5f 0x5f 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x5f 0x5f 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x60 0x60 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x60 0x60 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x80 0x80 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x80 0x80 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x81 0x81 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x81 0x81 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x82 0x82 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x82 0x82 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x83 0x83 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x83 0x83 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x8b 0x8b 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x8b 0x8b 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x8c 0x8c 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x8c 0x8c 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x8d 0x8d 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x8d 0x8d 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x8e 0x8e 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x8e 0x8e 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x8f 0x8f 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x8f 0x8f 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x90 0x90 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x90 0x90 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x91 0x91 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x91 0x91 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x92 0x92 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x92 0x92 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x93 0x93 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x93 0x93 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x94 0x94 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x94 0x94 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x95 0x95 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x95 0x95 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x96 0x96 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x96 0x96 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x99 0x99 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x99 0x99 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x9a 0x9a 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x9a 0x9a 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x9d 0x9d 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x9d 0x9d 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x9e 0x9e 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x9e 0x9e 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0x9f 0x9f 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0x9f 0x9f 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0xa0 0xa0 0x1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl_mask
    echo 0xa0 0xa0 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_edge_ctrl
    echo 0 0x66000000  > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_msr
    echo 1 0x66660066  > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_msr
    echo 2 0x00666600  > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_msr
    echo 3 0x00000000  > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_msr
    echo 4 0x00000000  > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_msr
    echo 8 0x00000000  > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_msr
    echo 11 0x00000000  > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_msr
    echo 12 0x00000000  > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_msr
    echo 16 0x00000000  > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_msr
    echo 17 0x11111000  > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_msr
    echo 18 0x01111111  > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_msr
    echo 19 0x00000110  > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_msr
    echo 20 0x00000000  > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_msr
    echo 1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_patt_ts
    echo 1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_patt_type
    echo 0 > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_trig_ts
    echo 0 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_patt_mask
    echo 1 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_patt_mask
    echo 2 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_patt_mask
    echo 3 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_patt_mask
    echo 4 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_patt_mask
    echo 5 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_patt_mask
    echo 6 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_patt_mask
    echo 7 0xFFFFFFFF > /sys/bus/coresight/devices/coresight-tpdm-apss1/dsb_patt_mask
    #start trace collection
    echo 0x2 > /sys/bus/coresight/devices/coresight-tpdm-apss1/enable_datasets
    echo 1 > /sys/bus/coresight/devices/coresight-tpdm-apss1/enable_source
}

#function to enable cti flush for etr
enable_cti_flush_for_etr()
{
    # bail out if its perf config
    if [ ! -d /sys/module/msm_rtb ]
    then
        return
    fi

    echo 1 > /sys/bus/coresight/devices/coresight-cti-qc_cti/enable
    echo 0 3 > /sys/bus/coresight/devices/coresight-cti-qc_cti/channels/trigout_attach
    echo 0 53 > /sys/bus/coresight/devices/coresight-cti-qc_cti/channels/trigin_attach
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

find_build_type()
{
    linux_banner=`cat /proc/version`
    if [[ "$linux_banner" == *"-consolidate"* ]]
    then
        debug_build=true
    fi
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
    config_dcc_timer
    #config_dcc_tlb
    config_dcc_thermal
    #config_dcc_core
    #config_dcc_lpm_pcu
    config_dcc_gic
    #config_dcc_epss
    config_dcc_rpmh
    config_dcc_apss_rscc
    config_dcc_aoss_pmic
    #config_dcc_clk
    config_dcc_gcc
    config_dcc_smmu
    config_dcc_camera
    config_dcc_misc
    config_dcc_gpu_aon
    config_dcc_gpu_gcc
    config_dcc_gpu_gmu
    config_dcc_gpu_gbif
    config_dcc_gpu_cp
    config_dcc_qdss
    config_dcc_video
    config_dcc_eva
    config_dcc_audio
    config_dcc_gnoc_reg
    config_dcc_timer

    echo 4 > $DCC_PATH/curr_list
    echo cap > $DCC_PATH/func_type
    echo sram > $DCC_PATH/data_sink
    config_dcc_timer
    #config_dcc_tlb
    config_dcc_ddr
    config_dcc_llcc
    config_dcc_gemnoc
    config_dcc_a1_aggre_noc
    config_dcc_a2_aggre_noc
    config_dcc_pcie_aggre_noc
    config_dcc_cnoc
    config_dcc_snoc
    config_dcc_dc_dch_noc
    config_dcc_dc_ch02_noc
    config_dcc_dc_ch13_noc
    config_dcc_mmnoc
    config_dcc_lpass_ag_noc
    config_dcc_turing_noc
    config_turing_exp
    #config_dcc_lpass_rscc
    config_dcc_turing_rscc
    #config_dcc_tlb
    config_dcc_timer

    echo 2 > $DCC_PATH/curr_list
    echo cap > $DCC_PATH/func_type
    echo sram > $DCC_PATH/data_sink
    echo 0 > $DCC_PATH/ap_ns_qad_override_en
    source dcc_extension.sh
    extension

    echo  1 > $DCC_PATH/enable
}

ftrace_disable=`getprop persist.debug.ftrace_events_disable`
debug_build=false
enable_debug()
{
    echo "niobe debug"
    echo "Enabling STM events on niobe."
    find_build_type
    create_stp_policy
    adjust_permission
    enable_cti_flush_for_etf
    enable_cti_flush_for_etr
    enable_stm_events
    if [ "$ftrace_disable" != "Yes" ]; then
        enable_ftrace_event_tracing
        enable_buses_and_interconnect_tracefs_debug
    fi
    enable_core_hang_config
    enable_dcc
    enable_hw_events
    enable_memory_debug
    enable_schedstats
    setprop ro.dbg.coresight.stm_cfg_done 1
    #enable_cpuss_register
}

enable_debug
