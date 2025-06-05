#=============================================================================
# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
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
    # EMMC Ftrace
    echo 1 > /sys/kernel/tracing/events/mmc/mmc_request_start/enable
    echo 1 > /sys/kernel/tracing/events/mmc/mmc_request_done/enable

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

config_dcc_thermal()
{
    #Tsense
    echo 0xc222004 > $DCC_PATH/config
    echo 0xc263014 > $DCC_PATH/config
    echo 0xc2630e0 > $DCC_PATH/config
    echo 0xc2630ec > $DCC_PATH/config
    echo 0xc2630a0 16 > $DCC_PATH/config
    echo 0xc2630e8 > $DCC_PATH/config
    echo 0xc26313c > $DCC_PATH/config

    #Silver LLVM
    echo 0x17b784a0 12 > $DCC_PATH/config
    echo 0x17b78520 > $DCC_PATH/config
    echo 0x17b78588 > $DCC_PATH/config
    echo 0x17b78d90 8 > $DCC_PATH/config
    echo 0x17b79010 6 > $DCC_PATH/config
    echo 0x17b79090 6 > $DCC_PATH/config
    echo 0x17b79a90 4 > $DCC_PATH/config

    #Turing LLM
    #echo 0x32310220 3 > $DCC_PATH/config
    #echo 0x323102a0 3 > $DCC_PATH/config
    #echo 0x323104a0 6 > $DCC_PATH/config
    #echo 0x32310520 > $DCC_PATH/config
    #echo 0x32310588 > $DCC_PATH/config
    #echo 0x32310d90 8 > $DCC_PATH/config
    #echo 0x32311010 6 > $DCC_PATH/config
    #echo 0x32311090 6 > $DCC_PATH/config
    #echo 0x32311a90 3 > $DCC_PATH/config

    # Central Broadcast
    echo 0xec80010 > $DCC_PATH/config
    echo 0xec81000 > $DCC_PATH/config
    echo 0xec81010 64 > $DCC_PATH/config
}

config_dcc_core()
{
    # CORE_HANG_THRESHOLD
    echo 0x17800058 > $DCC_PATH/config
    echo 0x17810058 > $DCC_PATH/config
    echo 0x17820058 > $DCC_PATH/config
    echo 0x17830058 > $DCC_PATH/config
    # CORE_HANG_VALUE
    echo 0x1780005c > $DCC_PATH/config
    echo 0x1781005c > $DCC_PATH/config
    echo 0x1782005c > $DCC_PATH/config
    echo 0x1783005c > $DCC_PATH/config
    #first core hang
    echo 0x1740003c > $DCC_PATH/config
    # CORE_HANG_CONFIG
    echo 0x17800060 > $DCC_PATH/config
    echo 0x17810060 > $DCC_PATH/config
    echo 0x17820060 > $DCC_PATH/config
    echo 0x17830060 > $DCC_PATH/config
    #CORE_HANG_DBG_STS
    echo 0x17800064 > $DCC_PATH/config
    echo 0x17810064 > $DCC_PATH/config
    echo 0x17820064 > $DCC_PATH/config
    echo 0x17830064 > $DCC_PATH/config

    #MIBU Debug registers
    echo 0x17600238 > $DCC_PATH/config

    #GNOC Hang counters
    echo 0x17600404 > $DCC_PATH/config
    echo 0x1760041c 2 > $DCC_PATH/config
    echo 0x17600434 > $DCC_PATH/config
    echo 0x1760043c 2 > $DCC_PATH/config

    #CPRh
    echo 0x17900908 > $DCC_PATH/config
    echo 0x17900c18 > $DCC_PATH/config

    # pll status for all banks and all domains
    # silver PLL
    echo 0x17a80000 0x8007 > $DCC_PATH/config_write
    echo 0x17a80000 > $DCC_PATH/config
    echo 0x17a80028 0x0 > $DCC_PATH/config_write
    echo 0x17a80028 > $DCC_PATH/config
    echo 0x17a80024 0x0 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config
    echo 0x17a80024 0x40 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config
    echo 0x17a80024 0x80 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config
    echo 0x17a80024 0xc0 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config
    echo 0x17a80024 0x100 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config
    echo 0x17a80024 0x140 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config
    echo 0x17a80024 0x180 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config
    echo 0x17a80024 0x1c0 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config
    echo 0x17a80024 0x200 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config
    echo 0x17a80024 0x240 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config
    echo 0x17a80024 0x280 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config
    echo 0x17a80024 0x2c0 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config
    echo 0x17a80024 0x300 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config
    echo 0x17a80024 0x340 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config
    echo 0x17a80024 0x380 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config
    echo 0x17a80024 0x3c0 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config
    echo 0x17a80024 0x4000 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a80024 0x0 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a80024 0x0 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a80024 0x0 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config
    echo 0x17a80024 0x40 > $DCC_PATH/config_write
    echo 0x17a80024 > $DCC_PATH/config
    echo 0x17a8003c > $DCC_PATH/config

    # L3 pll
    echo 0x17a84000 0x8007 > $DCC_PATH/config_write
    echo 0x17a84000 > $DCC_PATH/config
    echo 0x17a84024 0x0 > $DCC_PATH/config_write
    echo 0x17a84024 > $DCC_PATH/config
    echo 0x17a84020 0x0 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config
    echo 0x17a84020 0x40 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config
    echo 0x17a84020 0x80 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config
    echo 0x17a84020 0xc0 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config
    echo 0x17a84020 0x100 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config
    echo 0x17a84020 0x140 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config
    echo 0x17a84020 0x180 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config
    echo 0x17a84020 0x1c0 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config
    echo 0x17a84020 0x200 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config
    echo 0x17a84020 0x240 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config
    echo 0x17a84020 0x280 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config
    echo 0x17a84020 0x2c0 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config
    echo 0x17a84020 0x300 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config
    echo 0x17a84020 0x340 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config
    echo 0x17a84020 0x380 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config
    echo 0x17a84020 0x3c0 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config
    echo 0x17a84024 0x4000 > $DCC_PATH/config_write
    echo 0x17a84024 > $DCC_PATH/config
    echo 0x17a84020 0x0 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a84020 0x0 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a84020 0x0 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config
    echo 0x17a84020 0x40 > $DCC_PATH/config_write
    echo 0x17a84020 > $DCC_PATH/config
    echo 0x17a8400c > $DCC_PATH/config

    #rpmh
    echo 0xc201244 > $DCC_PATH/config
    echo 0xc202244 > $DCC_PATH/config

    #L3-ACD
    echo 0x17a94030 > $DCC_PATH/config
    echo 0x17a9408c > $DCC_PATH/config
    echo 0x17a9409c 0x78 > $DCC_PATH/config_write
    echo 0x17a9409c 0x0  > $DCC_PATH/config_write
    echo 0x17a94048 0x1  > $DCC_PATH/config_write
    echo 0x17a94090 0x0  > $DCC_PATH/config_write
    echo 0x17a94090 0x25 > $DCC_PATH/config_write
    echo 0x17a94098 > $DCC_PATH/config
    echo 0x17a94048 0x1D > $DCC_PATH/config_write
    echo 0x17a94090 0x0  > $DCC_PATH/config_write
    echo 0x17a94090 0x25 > $DCC_PATH/config_write
    echo 0x17a94098 > $DCC_PATH/config

    #SILVER-ACD
    echo 0x17a90030 > $DCC_PATH/config
    echo 0x17a9008c > $DCC_PATH/config
    echo 0x17a9009c 0x78 > $DCC_PATH/config_write
    echo 0x17a9009c 0x0  > $DCC_PATH/config_write
    echo 0x17a90048 0x1  > $DCC_PATH/config_write
    echo 0x17a90090 0x0  > $DCC_PATH/config_write
    echo 0x17a90090 0x25 > $DCC_PATH/config_write
    echo 0x17a90098 > $DCC_PATH/config
    echo 0x17a90048 0x1D > $DCC_PATH/config_write
    echo 0x17a90090 0x0  > $DCC_PATH/config_write
    echo 0x17a90090 0x25 > $DCC_PATH/config_write
    echo 0x17a90098 > $DCC_PATH/config

    echo 0x17ba0000 6 > $DCC_PATH/config
    echo 0x17ba0020 5 > $DCC_PATH/config
    echo 0x17ba0050 > $DCC_PATH/config
    echo 0x17ba0070 > $DCC_PATH/config
    echo 0x17ba0080 25 > $DCC_PATH/config
    echo 0x17ba0100 > $DCC_PATH/config
    echo 0x17ba0120 > $DCC_PATH/config
    echo 0x17ba0140 > $DCC_PATH/config
    echo 0x17ba0200 6 > $DCC_PATH/config
    echo 0x17ba0700 > $DCC_PATH/config
    echo 0x17ba070c 3 > $DCC_PATH/config
    echo 0x17ba0780 32 > $DCC_PATH/config
    echo 0x17ba0808 > $DCC_PATH/config
    echo 0x17ba0c48 > $DCC_PATH/config
    echo 0x17ba080c > $DCC_PATH/config
    echo 0x17ba0c4c > $DCC_PATH/config
    echo 0x17ba0810 > $DCC_PATH/config
    echo 0x17ba0c50 > $DCC_PATH/config
    echo 0x17ba0814 > $DCC_PATH/config
    echo 0x17ba0c54 > $DCC_PATH/config
    echo 0x17ba0818 > $DCC_PATH/config
    echo 0x17ba0c58 > $DCC_PATH/config
    echo 0x17ba081c > $DCC_PATH/config
    echo 0x17ba0c5c > $DCC_PATH/config
    echo 0x17ba0824 > $DCC_PATH/config
    echo 0x17ba0c64 > $DCC_PATH/config
    echo 0x17ba0828 > $DCC_PATH/config
    echo 0x17ba0c68 > $DCC_PATH/config
    echo 0x17ba082c > $DCC_PATH/config
    echo 0x17ba0c6c > $DCC_PATH/config
    echo 0x17ba0840 > $DCC_PATH/config
    echo 0x17ba0c80 > $DCC_PATH/config
    echo 0x17ba0844 > $DCC_PATH/config
    echo 0x17ba0c84 > $DCC_PATH/config
    echo 0x17ba0848 > $DCC_PATH/config
    echo 0x17ba0c88 > $DCC_PATH/config
    echo 0x17ba084c > $DCC_PATH/config
    echo 0x17ba0c8c > $DCC_PATH/config
    echo 0x17ba0850 > $DCC_PATH/config
    echo 0x17ba0c90 > $DCC_PATH/config
    echo 0x17ba0854 > $DCC_PATH/config
    echo 0x17ba0c94 > $DCC_PATH/config
    echo 0x17ba0858 > $DCC_PATH/config
    echo 0x17ba0c98 > $DCC_PATH/config
    echo 0x17ba085c > $DCC_PATH/config
    echo 0x17ba0c9c > $DCC_PATH/config
    echo 0x17ba0860 > $DCC_PATH/config
    echo 0x17ba0ca0 > $DCC_PATH/config
    echo 0x17ba0864 > $DCC_PATH/config
    echo 0x17ba0ca4 > $DCC_PATH/config
    echo 0x17ba0868 > $DCC_PATH/config
    echo 0x17ba0ca8 > $DCC_PATH/config
    echo 0x17ba086c > $DCC_PATH/config
    echo 0x17ba0cac > $DCC_PATH/config
    echo 0x17ba0870 > $DCC_PATH/config
    echo 0x17ba0cb0 > $DCC_PATH/config
    echo 0x17ba0874 > $DCC_PATH/config
    echo 0x17ba0cb4 > $DCC_PATH/config
    echo 0x17ba0878 > $DCC_PATH/config
    echo 0x17ba0cb8 > $DCC_PATH/config
    echo 0x17ba087c > $DCC_PATH/config
    echo 0x17ba0cbc > $DCC_PATH/config
    echo 0x17ba3500 80 > $DCC_PATH/config
    echo 0x17ba3a00 3 > $DCC_PATH/config
    echo 0x17ba3aa8 18 > $DCC_PATH/config
    echo 0x17ba3b00 2 > $DCC_PATH/config
    echo 0x17ba3b20 3 > $DCC_PATH/config
    echo 0x17ba3b30 11 > $DCC_PATH/config
    echo 0x17ba3b64 > $DCC_PATH/config
    echo 0x17ba3b00 2 > $DCC_PATH/config
    echo 0x17ba3b20 3 > $DCC_PATH/config
    echo 0x17ba3b30 11 > $DCC_PATH/config
    echo 0x17ba3b70 2 > $DCC_PATH/config

    #APM
    echo 0x17b00000 70 > $DCC_PATH/config
}

config_dcc_lpm_pcu()
{
    #PCU -DCC for LPM path
    #  Read only registers
    # core#0
    echo 0x17800010 > $DCC_PATH/config
    echo 0x17800024 > $DCC_PATH/config
    echo 0x17800038 6 > $DCC_PATH/config
    echo 0x1780006c > $DCC_PATH/config
    echo 0x178000f0 2 > $DCC_PATH/config
    # core#1
    echo 0x17810010 > $DCC_PATH/config
    echo 0x17810024 > $DCC_PATH/config
    echo 0x17810038 6 > $DCC_PATH/config
    echo 0x1781006c > $DCC_PATH/config
    echo 0x178100f0 2 > $DCC_PATH/config
    # core#2
    echo 0x17820010 > $DCC_PATH/config
    echo 0x17820024 > $DCC_PATH/config
    echo 0x17820038 6 > $DCC_PATH/config
    echo 0x1782006c > $DCC_PATH/config
    echo 0x178200f0 2 > $DCC_PATH/config
    # core#3
    echo 0x17830010 > $DCC_PATH/config
    echo 0x17830024 > $DCC_PATH/config
    echo 0x17830038 6 > $DCC_PATH/config
    echo 0x1783006c > $DCC_PATH/config
    echo 0x178300f0 2 > $DCC_PATH/config
    # L3
    echo 0x17880010 > $DCC_PATH/config
    echo 0x17880024 > $DCC_PATH/config
    echo 0x17880038 3 > $DCC_PATH/config
    #APPS CL
    echo 0x17880044 3 > $DCC_PATH/config
    echo 0x1788006c 5 > $DCC_PATH/config
    # APSS_BOOTFSM_STS
    echo 0x17880084 > $DCC_PATH/config
    # APSS_SILVER_PLL
    echo 0x178800f4 5 > $DCC_PATH/config
    # APSS_ITM_SILVER
    echo 0x17880134 2 > $DCC_PATH/config
    # APSS_SILVER_PWR_CTL_STS
    echo 0x178801b4 > $DCC_PATH/config
    # APSS_CL_PCU_PWR_CTL_STS
    echo 0x178801bc 2 > $DCC_PATH/config
    echo 0x178801c8 > $DCC_PATH/config
}

config_dcc_rpmh()
{
    echo 0xb281024 > $DCC_PATH/config
    echo 0xbde1034 > $DCC_PATH/config

    #RPMH_PDC_APSS
    echo 0xb201020 2 > $DCC_PATH/config
    echo 0xb211020 2 > $DCC_PATH/config
    echo 0xb221020 2 > $DCC_PATH/config
    echo 0xb231020 2 > $DCC_PATH/config
    echo 0xb204520 > $DCC_PATH/config

    echo 0xb200010 4 > $DCC_PATH/config
    echo 0xb200900 4 > $DCC_PATH/config
    echo 0xb201030 > $DCC_PATH/config
    echo 0xb201204 2 > $DCC_PATH/config
    echo 0xb201218 2 > $DCC_PATH/config
    echo 0xb20122c 2 > $DCC_PATH/config
    echo 0xb201240 2 > $DCC_PATH/config
    echo 0xb201254 2 > $DCC_PATH/config
    echo 0xb204510 2 > $DCC_PATH/config
    echo 0xb220010 4 > $DCC_PATH/config
    echo 0xb220900 4 > $DCC_PATH/config
}

config_dcc_apss_rscc()
{
    #APSS_RSCC_RSC register
    echo 0x17a00010 > $DCC_PATH/config
    echo 0x17a10010 > $DCC_PATH/config
    echo 0x17a20010 > $DCC_PATH/config
    echo 0x17a30010 > $DCC_PATH/config
    echo 0x17a00030 > $DCC_PATH/config
    echo 0x17a10030 > $DCC_PATH/config
    echo 0x17a20030 > $DCC_PATH/config
    echo 0x17a30030 > $DCC_PATH/config
    echo 0x17a00038 > $DCC_PATH/config
    echo 0x17a10038 > $DCC_PATH/config
    echo 0x17a20038 > $DCC_PATH/config
    echo 0x17a30038 > $DCC_PATH/config
    echo 0x17a00040 > $DCC_PATH/config
    echo 0x17a10040 > $DCC_PATH/config
    echo 0x17a20040 > $DCC_PATH/config
    echo 0x17a30040 > $DCC_PATH/config
    echo 0x17a00048 > $DCC_PATH/config
    echo 0x17a00400 3 > $DCC_PATH/config
    echo 0x17a10400 3 > $DCC_PATH/config
    echo 0x17a20400 3 > $DCC_PATH/config
    echo 0x17a30400 3 > $DCC_PATH/config
}

config_dcc_epss()
{
    echo 0x17d80100 144 > $DCC_PATH/config
    # EPSSSLOW_CLKDOM0
    echo 0x17d9001c > $DCC_PATH/config
    echo 0x17d900dc > $DCC_PATH/config
    echo 0x17d900e8 > $DCC_PATH/config
    echo 0x17d90320 > $DCC_PATH/config
    echo 0x17d90020 > $DCC_PATH/config
    echo 0x17d9034c > $DCC_PATH/config
    echo 0x17d90300 > $DCC_PATH/config
    # EPSSSLOW_CLKDOM1
    echo 0x17d9101c > $DCC_PATH/config
    echo 0x17d910dc > $DCC_PATH/config
    echo 0x17d910e8 > $DCC_PATH/config
    echo 0x17d91320 > $DCC_PATH/config
    echo 0x17d91020 > $DCC_PATH/config
    echo 0x17d9134c > $DCC_PATH/config
    echo 0x17d91300 > $DCC_PATH/config

    echo 0x12822000 2 > $DCC_PATH/config
    echo 0x12824c00 > $DCC_PATH/config
    echo 0x12824d04 2 > $DCC_PATH/config
    echo 0x17d98014 4 > $DCC_PATH/config
    echo 0x17d900e0 > $DCC_PATH/config
    echo 0x17d90410 > $DCC_PATH/config
    echo 0x17d90074 > $DCC_PATH/config
    echo 0x17d90064 > $DCC_PATH/config
    echo 0x17d91074 > $DCC_PATH/config
    echo 0x17d910e0 > $DCC_PATH/config
    echo 0x17d91410 > $DCC_PATH/config

}

config_dcc_misc()
{
    # WDOG_BITE_INT0_CONFIG
    echo 0x17400038 > $DCC_PATH/config
    # EPSSTOP_MUC_HANG_DET_CTRL
    echo 0x17d98010 > $DCC_PATH/config
    # SOC_HW_VERSION
    echo 0x1fc8000 > $DCC_PATH/config
}

config_dcc_ddr()
{
    #Pimem
    echo 0x610110 5 > $DCC_PATH/config

    # SHRM CSR
    echo 0x1908e008 > $DCC_PATH/config
    # MCCC
    echo 0x190ba280 > $DCC_PATH/config
    echo 0x192e0610 > $DCC_PATH/config
    echo 0x190ba288 8 > $DCC_PATH/config
    echo 0x192e0614 3 > $DCC_PATH/config
    # LLCC/CABO
    echo 0x19a00000 2 > $DCC_PATH/config
    echo 0x19238100 > $DCC_PATH/config
    echo 0x192420b0 > $DCC_PATH/config
    echo 0x19242044 4 > $DCC_PATH/config
    echo 0x1924c180 19 > $DCC_PATH/config
    echo 0x19250020 > $DCC_PATH/config
    echo 0x192521b0 16 > $DCC_PATH/config
    echo 0x1926005c 5 > $DCC_PATH/config
    echo 0x192c0410 3 > $DCC_PATH/config
    echo 0x192c0420 2 > $DCC_PATH/config
    echo 0x192c0430 > $DCC_PATH/config
    echo 0x192c0440 > $DCC_PATH/config
    echo 0x192c0448 > $DCC_PATH/config
    echo 0x192c04a0 > $DCC_PATH/config
    echo 0x192c04b0 > $DCC_PATH/config
    echo 0x19338100 > $DCC_PATH/config
    echo 0x193420b0 > $DCC_PATH/config
    echo 0x19342044 4 > $DCC_PATH/config
    echo 0x1934c180 > $DCC_PATH/config
    echo 0x1934c1c8 > $DCC_PATH/config
    echo 0x19350020 > $DCC_PATH/config
    echo 0x193521b0 > $DCC_PATH/config
    echo 0x1936005c 5 > $DCC_PATH/config
    echo 0x19a00000 2 > $DCC_PATH/config
    echo 0x19a01000 2 > $DCC_PATH/config
    echo 0x19a02000 2 > $DCC_PATH/config
    echo 0x19a03000 2 > $DCC_PATH/config
    echo 0x19a04000 2 > $DCC_PATH/config
    echo 0x19a05000 2 > $DCC_PATH/config
    echo 0x19a06000 2 > $DCC_PATH/config
    echo 0x19a07000 2 > $DCC_PATH/config
    echo 0x19a08000 2 > $DCC_PATH/config
    echo 0x19a09000 2 > $DCC_PATH/config
    echo 0x19a0a000 2 > $DCC_PATH/config
    echo 0x19a0b000 2 > $DCC_PATH/config
    echo 0x19a0c000 2 > $DCC_PATH/config
    echo 0x19a0d000 2 > $DCC_PATH/config
    echo 0x19a0e000 2 > $DCC_PATH/config
    echo 0x19a0f000 2 > $DCC_PATH/config
    echo 0x19a10000 2 > $DCC_PATH/config
    echo 0x19a11000 2 > $DCC_PATH/config
    echo 0x19a12000 2 > $DCC_PATH/config
    echo 0x19a13000 2 > $DCC_PATH/config
    echo 0x19a14000 2 > $DCC_PATH/config
    echo 0x19a15000 2 > $DCC_PATH/config
    echo 0x19a16000 2 > $DCC_PATH/config
    echo 0x19a17000 2 > $DCC_PATH/config
    echo 0x19a18000 2 > $DCC_PATH/config
    echo 0x19a19000 2 > $DCC_PATH/config
    echo 0x19a1a000 2 > $DCC_PATH/config
    echo 0x19a1b000 2 > $DCC_PATH/config
    echo 0x19a1c000 2 > $DCC_PATH/config
    echo 0x19a1d000 2 > $DCC_PATH/config
    echo 0x19a1e000 2 > $DCC_PATH/config
    echo 0x19a1f000 2 > $DCC_PATH/config
    echo 0x192c6418 > $DCC_PATH/config
    echo 0x192c5804 > $DCC_PATH/config
    echo 0x192c04b8 > $DCC_PATH/config
    # PHY
    echo 0x192856d4 4 > $DCC_PATH/config
    echo 0x19281b1c 3 > $DCC_PATH/config
    echo 0x19283b1c 3 > $DCC_PATH/config
    echo 0x19285188 3 > $DCC_PATH/config
    echo 0x19281db4 > $DCC_PATH/config
    echo 0x19281dbc > $DCC_PATH/config
    echo 0x19281dc4 > $DCC_PATH/config
    echo 0x19283db4 > $DCC_PATH/config
    echo 0x19283dbc > $DCC_PATH/config
    echo 0x19283dc4 > $DCC_PATH/config
    echo 0x1928583c > $DCC_PATH/config
    echo 0x1928581c > $DCC_PATH/config
    echo 0x192857fc > $DCC_PATH/config
    echo 0x19285830 > $DCC_PATH/config
    echo 0x19285840 > $DCC_PATH/config
    echo 0x19285848 > $DCC_PATH/config
    echo 0x19285810 > $DCC_PATH/config
    echo 0x19285258 > $DCC_PATH/config
    echo 0x19281f20 2 > $DCC_PATH/config
    echo 0x19281e6c > $DCC_PATH/config
    echo 0x19283e6c > $DCC_PATH/config
    # SHKE
    echo 0x192c5240 2 > $DCC_PATH/config
    echo 0x192c5b1c 3 > $DCC_PATH/config

    #SHRM
    echo 0x19032020 2 > $DCC_PATH/config
    echo 0x1908e01c > $DCC_PATH/config
    echo 0x1908e030 > $DCC_PATH/config
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1908e008 > $DCC_PATH/config
    echo 0x19032020 > $DCC_PATH/config
    echo 0x1908e948 > $DCC_PATH/config
    echo 0x19032024 > $DCC_PATH/config
    echo 0x19030040 0x1 > $DCC_PATH/config_write
    echo 0x1903005c 0x22C000 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C001 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C002 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C003 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C004 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C005 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C006 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C007 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C008 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C009 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C00A > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C00B > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C00C > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C00D > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C00E > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C00F > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C010 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C011 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C012 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C013 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C014 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C015 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C016 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C017 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C018 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C019 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C01A > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C01B > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C01C > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C01D > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C01E > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C01F > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C300 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C341 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C7B1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config

    #LLCC
    echo 0x19220344 9 > $DCC_PATH/config
    echo 0x19220370 7 > $DCC_PATH/config
    echo 0x19220480 > $DCC_PATH/config
    echo 0x19222400 26 > $DCC_PATH/config
    echo 0x19222470 5 > $DCC_PATH/config
    echo 0x1922320c > $DCC_PATH/config
    echo 0x19223214 2 > $DCC_PATH/config
    echo 0x19223308 > $DCC_PATH/config
    echo 0x19223318 > $DCC_PATH/config
    echo 0x19223318 > $DCC_PATH/config
    echo 0x1922358c > $DCC_PATH/config
    echo 0x19234010 > $DCC_PATH/config
    echo 0x1923801c 8 > $DCC_PATH/config
    echo 0x19238050 > $DCC_PATH/config
    echo 0x19238100 > $DCC_PATH/config
    echo 0x19238100 7 > $DCC_PATH/config
    echo 0x1923c004 > $DCC_PATH/config
    echo 0x1923c014 > $DCC_PATH/config
    echo 0x1923c020 > $DCC_PATH/config
    echo 0x1923c030 > $DCC_PATH/config
    echo 0x1923c05c 3 > $DCC_PATH/config
    echo 0x1923c074 > $DCC_PATH/config
    echo 0x1923c088 > $DCC_PATH/config
    echo 0x1923c0a0 > $DCC_PATH/config
    echo 0x1923c0b0 > $DCC_PATH/config
    echo 0x1923c0c0 > $DCC_PATH/config
    echo 0x1923c0d0 > $DCC_PATH/config
    echo 0x1923c0e0 > $DCC_PATH/config
    echo 0x1923c0f0 > $DCC_PATH/config
    echo 0x1923c100 > $DCC_PATH/config
    echo 0x1923d064 > $DCC_PATH/config
    echo 0x19240008 6 > $DCC_PATH/config
    echo 0x19240028 > $DCC_PATH/config
    echo 0x1924203c 3 > $DCC_PATH/config
    echo 0x19242044 2 > $DCC_PATH/config
    echo 0x19242048 2 > $DCC_PATH/config
    echo 0x1924204c 10 > $DCC_PATH/config
    echo 0x1924208c > $DCC_PATH/config
    echo 0x192420b0 > $DCC_PATH/config
    echo 0x192420b0 > $DCC_PATH/config
    echo 0x192420b8 3 > $DCC_PATH/config
    echo 0x192420f4 > $DCC_PATH/config
    echo 0x192420fc 3 > $DCC_PATH/config
    echo 0x19242104 5 > $DCC_PATH/config
    echo 0x19242114 > $DCC_PATH/config
    echo 0x19242324 14 > $DCC_PATH/config
    echo 0x19242410 > $DCC_PATH/config
    echo 0x192430a8 > $DCC_PATH/config
    echo 0x19248004 7 > $DCC_PATH/config
    echo 0x19248024 > $DCC_PATH/config
    echo 0x19248040 > $DCC_PATH/config
    echo 0x19248048 > $DCC_PATH/config
    echo 0x19249064 > $DCC_PATH/config
    echo 0x1924c000 > $DCC_PATH/config
    echo 0x1924c048 > $DCC_PATH/config
    echo 0x1924c054 2 > $DCC_PATH/config
    echo 0x1924c078 > $DCC_PATH/config
    echo 0x1924c108 > $DCC_PATH/config
    echo 0x1924c110 > $DCC_PATH/config
    echo 0x19250020 > $DCC_PATH/config
    echo 0x19250020 > $DCC_PATH/config
    echo 0x19251054 > $DCC_PATH/config
    echo 0x19252014 3 > $DCC_PATH/config
    echo 0x19252030 15 > $DCC_PATH/config
    echo 0x19252070 8 > $DCC_PATH/config
    echo 0x19252098 > $DCC_PATH/config
    echo 0x192520a0 > $DCC_PATH/config
    echo 0x192520b4 > $DCC_PATH/config
    echo 0x192520c0 > $DCC_PATH/config
    echo 0x192520d0 3 > $DCC_PATH/config
    echo 0x192520f4 10 > $DCC_PATH/config
    echo 0x19252120 12 > $DCC_PATH/config
    echo 0x1926004c > $DCC_PATH/config
    echo 0x1926004c 2 > $DCC_PATH/config
    echo 0x19260050 2 > $DCC_PATH/config
    echo 0x19260054 2 > $DCC_PATH/config
    echo 0x19260058 2 > $DCC_PATH/config
    echo 0x1926005c 2 > $DCC_PATH/config
    echo 0x19260060 2 > $DCC_PATH/config
    echo 0x19260064 2 > $DCC_PATH/config
    echo 0x19260068 3 > $DCC_PATH/config
    echo 0x19260078 > $DCC_PATH/config
    echo 0x1926020c > $DCC_PATH/config
    echo 0x19260214 > $DCC_PATH/config
    echo 0x19261084 > $DCC_PATH/config
    echo 0x19262020 > $DCC_PATH/config
    echo 0x19263020 > $DCC_PATH/config
    echo 0x19264020 > $DCC_PATH/config
    echo 0x19265020 > $DCC_PATH/config
    echo 0x19320344 2 > $DCC_PATH/config
    echo 0x19320348 8 > $DCC_PATH/config
    echo 0x19320370 7 > $DCC_PATH/config
    echo 0x19320480 > $DCC_PATH/config
    echo 0x19320480 > $DCC_PATH/config
    echo 0x19322400 > $DCC_PATH/config
    echo 0x19322400 26 > $DCC_PATH/config
    echo 0x19322470 5 > $DCC_PATH/config
    echo 0x1932320c > $DCC_PATH/config
    echo 0x19323214 2 > $DCC_PATH/config
    echo 0x19323308 > $DCC_PATH/config
    echo 0x19323308 > $DCC_PATH/config
    echo 0x19323318 > $DCC_PATH/config
    echo 0x19323318 > $DCC_PATH/config
    echo 0x1932358c > $DCC_PATH/config
    echo 0x19334010 > $DCC_PATH/config
    echo 0x1933801c 8 > $DCC_PATH/config
    echo 0x19338050 > $DCC_PATH/config
    echo 0x19338100 > $DCC_PATH/config
    echo 0x19338100 7 > $DCC_PATH/config
    echo 0x1933c004 > $DCC_PATH/config
    echo 0x1933c014 > $DCC_PATH/config
    echo 0x1933c020 > $DCC_PATH/config
    echo 0x1933c030 > $DCC_PATH/config
    echo 0x1933c05c 3 > $DCC_PATH/config
    echo 0x1933c074 > $DCC_PATH/config
    echo 0x1933c088 > $DCC_PATH/config
    echo 0x1933c0a0 > $DCC_PATH/config
    echo 0x1933c0b0 > $DCC_PATH/config
    echo 0x1933c0c0 > $DCC_PATH/config
    echo 0x1933c0d0 > $DCC_PATH/config
    echo 0x1933c0e0 > $DCC_PATH/config
    echo 0x1933c0f0 > $DCC_PATH/config
    echo 0x1933c100 > $DCC_PATH/config
    echo 0x1933d064 > $DCC_PATH/config
    echo 0x19340008 6 > $DCC_PATH/config
    echo 0x19340028 > $DCC_PATH/config
    echo 0x1934203c 3 > $DCC_PATH/config
    echo 0x19342044 2 > $DCC_PATH/config
    echo 0x19342048 2 > $DCC_PATH/config
    echo 0x1934204c 10 > $DCC_PATH/config
    echo 0x1934208c > $DCC_PATH/config
    echo 0x193420b0 > $DCC_PATH/config
    echo 0x193420b0 > $DCC_PATH/config
    echo 0x193420b8 3 > $DCC_PATH/config
    echo 0x193420f4 > $DCC_PATH/config
    echo 0x193420fc 3 > $DCC_PATH/config
    echo 0x19342104 5 > $DCC_PATH/config
    echo 0x19342114 > $DCC_PATH/config
    echo 0x19342324 14 > $DCC_PATH/config
    echo 0x19342410 > $DCC_PATH/config
    echo 0x193430a8 > $DCC_PATH/config
    echo 0x19348004 > $DCC_PATH/config
    echo 0x19348004 2 > $DCC_PATH/config
    echo 0x19348008 2 > $DCC_PATH/config
    echo 0x1934800c 2 > $DCC_PATH/config
    echo 0x19348010 4 > $DCC_PATH/config
    echo 0x19348024 > $DCC_PATH/config
    echo 0x19348040 > $DCC_PATH/config
    echo 0x19348048 > $DCC_PATH/config
    echo 0x19349064 > $DCC_PATH/config
    echo 0x1934c000 > $DCC_PATH/config
    echo 0x1934c048 > $DCC_PATH/config
    echo 0x1934c054 2 > $DCC_PATH/config
    echo 0x1934c078 > $DCC_PATH/config
    echo 0x1934c108 > $DCC_PATH/config
    echo 0x1934c110 > $DCC_PATH/config
    echo 0x19350020 > $DCC_PATH/config
    echo 0x19350020 > $DCC_PATH/config
    echo 0x19351054 > $DCC_PATH/config
    echo 0x19352014 3 > $DCC_PATH/config
    echo 0x19352030 15 > $DCC_PATH/config
    echo 0x19352070 8 > $DCC_PATH/config
    echo 0x19352098 > $DCC_PATH/config
    echo 0x193520a0 > $DCC_PATH/config
    echo 0x193520b4 > $DCC_PATH/config
    echo 0x193520c0 > $DCC_PATH/config
    echo 0x193520d0 3 > $DCC_PATH/config
    echo 0x193520f4 10 > $DCC_PATH/config
    echo 0x19352120 12 > $DCC_PATH/config
    echo 0x1936004c > $DCC_PATH/config
    echo 0x1936004c > $DCC_PATH/config
    echo 0x1936004c 2 > $DCC_PATH/config
    echo 0x19360050 > $DCC_PATH/config
    echo 0x19360050 2 > $DCC_PATH/config
    echo 0x19360054 > $DCC_PATH/config
    echo 0x19360054 2 > $DCC_PATH/config
    echo 0x19360058 > $DCC_PATH/config
    echo 0x19360058 2 > $DCC_PATH/config
    echo 0x1936005c > $DCC_PATH/config
    echo 0x1936005c 2 > $DCC_PATH/config
    echo 0x19360060 > $DCC_PATH/config
    echo 0x19360060 2 > $DCC_PATH/config
    echo 0x19360064 > $DCC_PATH/config
    echo 0x19360064 2 > $DCC_PATH/config
    echo 0x19360068 > $DCC_PATH/config
    echo 0x19360068 3 > $DCC_PATH/config
    echo 0x19360078 > $DCC_PATH/config
    echo 0x1936020c > $DCC_PATH/config
    echo 0x19360214 > $DCC_PATH/config
    echo 0x19361084 > $DCC_PATH/config
    echo 0x19362020 > $DCC_PATH/config
    echo 0x19363020 > $DCC_PATH/config
    echo 0x19364020 > $DCC_PATH/config
    echo 0x19365020 > $DCC_PATH/config
    echo 0x19200004 > $DCC_PATH/config
    echo 0x19201004 > $DCC_PATH/config
    echo 0x19202004 > $DCC_PATH/config
    echo 0x19203004 > $DCC_PATH/config
    echo 0x19204004 > $DCC_PATH/config
    echo 0x19205004 > $DCC_PATH/config
    echo 0x19206004 > $DCC_PATH/config
    echo 0x19207004 > $DCC_PATH/config
    echo 0x19208004 > $DCC_PATH/config
    echo 0x19209004 > $DCC_PATH/config
    echo 0x1920a004 > $DCC_PATH/config
    echo 0x1920b004 > $DCC_PATH/config
    echo 0x1920c004 > $DCC_PATH/config
    echo 0x1920d004 > $DCC_PATH/config
    echo 0x1920e004 > $DCC_PATH/config
    echo 0x1920f004 > $DCC_PATH/config
    echo 0x19210004 > $DCC_PATH/config
    echo 0x19211004 > $DCC_PATH/config
    echo 0x19212004 > $DCC_PATH/config
    echo 0x19213004 > $DCC_PATH/config
    echo 0x19214004 > $DCC_PATH/config
    echo 0x19215004 > $DCC_PATH/config
    echo 0x19216004 > $DCC_PATH/config
    echo 0x19217004 > $DCC_PATH/config
    echo 0x19218004 > $DCC_PATH/config
    echo 0x19219004 > $DCC_PATH/config
    echo 0x1921a004 > $DCC_PATH/config
    echo 0x1921b004 > $DCC_PATH/config
    echo 0x1921c004 > $DCC_PATH/config
    echo 0x1921d004 > $DCC_PATH/config
    echo 0x1921e004 > $DCC_PATH/config
    echo 0x1921f004 > $DCC_PATH/config

    # MC0_CABO
    echo 0x192c0080 > $DCC_PATH/config
    echo 0x192c0400 2 > $DCC_PATH/config
    echo 0x192c0410 3 > $DCC_PATH/config
    echo 0x192c0420 2 > $DCC_PATH/config
    echo 0x192c0430 > $DCC_PATH/config
    echo 0x192c0440 > $DCC_PATH/config
    echo 0x192c0448 > $DCC_PATH/config
    echo 0x192c04a0 > $DCC_PATH/config
    echo 0x192c04b0 4 > $DCC_PATH/config
    echo 0x192c04d0 2 > $DCC_PATH/config
    echo 0x192c1400 > $DCC_PATH/config
    echo 0x192c2400 2 > $DCC_PATH/config
    echo 0x192c3400 4 > $DCC_PATH/config
    echo 0x192c4700 > $DCC_PATH/config
    echo 0x192c53b0 > $DCC_PATH/config
    echo 0x192c5804 > $DCC_PATH/config
    echo 0x192c6400 > $DCC_PATH/config
    echo 0x192c6418 > $DCC_PATH/config
    echo 0x192c9100 > $DCC_PATH/config
    echo 0x192c9110 > $DCC_PATH/config
    echo 0x192c9120 > $DCC_PATH/config
    echo 0x192c9180 > $DCC_PATH/config
    echo 0x192c9180 2 > $DCC_PATH/config
    echo 0x192c9184 > $DCC_PATH/config
    echo 0x192c91a0 > $DCC_PATH/config
    echo 0x192c91c0 > $DCC_PATH/config
    echo 0x192c91e0 > $DCC_PATH/config

    # MCCC
    echo 0x190ba280 > $DCC_PATH/config
    echo 0x190ba288 8 > $DCC_PATH/config
    echo 0x192e0610 4 > $DCC_PATH/config
    echo 0x192e0680 4 > $DCC_PATH/config

    # DDRPHY
    echo 0x19281e64 > $DCC_PATH/config
    echo 0x19281ea0 > $DCC_PATH/config
    echo 0x19281f30 2 > $DCC_PATH/config
    echo 0x19283e64 > $DCC_PATH/config
    echo 0x19283ea0 > $DCC_PATH/config
    echo 0x19283f30 2 > $DCC_PATH/config
    echo 0x1928527c > $DCC_PATH/config
    echo 0x19285290 > $DCC_PATH/config
    echo 0x192854ec > $DCC_PATH/config
    echo 0x192854f4 > $DCC_PATH/config
    echo 0x19285514 > $DCC_PATH/config
    echo 0x1928551c > $DCC_PATH/config
    echo 0x19285524 > $DCC_PATH/config
    echo 0x19285548 > $DCC_PATH/config
    echo 0x19285550 > $DCC_PATH/config
    echo 0x19285558 > $DCC_PATH/config
    echo 0x192855b8 > $DCC_PATH/config
    echo 0x192855c0 > $DCC_PATH/config
    echo 0x192855ec > $DCC_PATH/config
    echo 0x19285860 > $DCC_PATH/config
    echo 0x19285870 > $DCC_PATH/config
    echo 0x192858a0 > $DCC_PATH/config
    echo 0x192858a8 > $DCC_PATH/config
    echo 0x192858b0 > $DCC_PATH/config
    echo 0x192858b8 > $DCC_PATH/config
    echo 0x192858d8 2 > $DCC_PATH/config
    echo 0x192858f4 > $DCC_PATH/config
    echo 0x192858fc > $DCC_PATH/config
    echo 0x19285920 > $DCC_PATH/config
    echo 0x19285928 > $DCC_PATH/config
    echo 0x19285944 > $DCC_PATH/config
    echo 0x19286604 > $DCC_PATH/config
    echo 0x1928660c > $DCC_PATH/config

    # SHRM2
    echo 0x19032020 2 > $DCC_PATH/config
    echo 0x1908e01c > $DCC_PATH/config
    echo 0x1908e030 > $DCC_PATH/config
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1908e008 > $DCC_PATH/config
    echo 0x19032020 > $DCC_PATH/config
    echo 0x1908e948 > $DCC_PATH/config
    echo 0x19032024 > $DCC_PATH/config

    echo 0x19030040 0x1 1 > $DCC_PATH/config_write
    echo 0x1903005c 0x22C000 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config

    echo 0x1903005c 0x22C001 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C002 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C003 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C004 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C005 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C006 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C007 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C008 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C009 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C00A 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C00B 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C00C 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C00D 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C00E 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C00F 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C010 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C011 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C012 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C013 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C014 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C015 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C016 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C017 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C018 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C019 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C01A 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C01B 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C01C 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C01D 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C01E 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C01F 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C300 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C341 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
    echo 0x1903005c 0x22C7B1 1 > $DCC_PATH/config_write
    echo 0x19030010 > $DCC_PATH/config
}

config_dcc_gic()
{
    echo 0x17200104 29 > $DCC_PATH/config
    echo 0x17200204 29 > $DCC_PATH/config
    echo 0x17200384 29 > $DCC_PATH/config
}

config_dcc_gpu()
{
    echo 0x129000 > $DCC_PATH/config
    echo 0x12903c > $DCC_PATH/config
    echo 0x19b004 > $DCC_PATH/config
    echo 0x19b00c 4 > $DCC_PATH/config
    echo 0x181154 > $DCC_PATH/config
    echo 0x18b000 > $DCC_PATH/config
    echo 0x18b03c > $DCC_PATH/config
    echo 0x18c000 > $DCC_PATH/config
    echo 0x18c03c > $DCC_PATH/config
    echo 0x18d000 > $DCC_PATH/config
    echo 0x18d03c > $DCC_PATH/config
    echo 0x18e000 > $DCC_PATH/config
    echo 0x18e03c > $DCC_PATH/config
    echo 0x3d99004 > $DCC_PATH/config
    echo 0x3d9905c 3 > $DCC_PATH/config
    echo 0x3d990a8 > $DCC_PATH/config
    echo 0x3d990b0 4 > $DCC_PATH/config
    echo 0x3d99108 3 > $DCC_PATH/config
    echo 0x3d9911c 5 > $DCC_PATH/config
    echo 0x3d99134 3 > $DCC_PATH/config
    echo 0x3d99144 > $DCC_PATH/config
    echo 0x3d9919c 2 > $DCC_PATH/config
    echo 0x3d99224 2 > $DCC_PATH/config
    echo 0x3d99274 > $DCC_PATH/config
    echo 0x3d99284 > $DCC_PATH/config
    echo 0x3d9935c 2 > $DCC_PATH/config
    echo 0x3d993a4 > $DCC_PATH/config
    echo 0x3d993e8 > $DCC_PATH/config
    echo 0x3d99148 > $DCC_PATH/config
    echo 0x3d99008 > $DCC_PATH/config
    echo 0x3d96000 2 > $DCC_PATH/config
    echo 0x3d95000 2 > $DCC_PATH/config
    echo 0x3d97000 2 > $DCC_PATH/config
    echo 0x3d99278 > $DCC_PATH/config
    echo 0x3d99288 2 > $DCC_PATH/config
    echo 0x3d99530 > $DCC_PATH/config
    echo 0x3d002b4 > $DCC_PATH/config
    echo 0x3d00410 2 > $DCC_PATH/config
    echo 0x3d00818 > $DCC_PATH/config
    echo 0x3d00800 > $DCC_PATH/config
    echo 0x3d00840 > $DCC_PATH/config
    echo 0x3d00804 > $DCC_PATH/config
    echo 0x3d000e0 > $DCC_PATH/config
    echo 0x3d8ec30 > $DCC_PATH/config
    echo 0x3d7e340 > $DCC_PATH/config
    echo 0x3d3c000 2 > $DCC_PATH/config
    echo 0x3da2300 > $DCC_PATH/config
    echo 0xaf09000 > $DCC_PATH/config
}

config_dcc_gcc()
{
    echo 0x100000 2 > $DCC_PATH/config
    echo 0x101000 2 > $DCC_PATH/config
    echo 0x102000 2 > $DCC_PATH/config
    echo 0x103000 2 > $DCC_PATH/config
    echo 0x104000 2 > $DCC_PATH/config
    echo 0x105000 2 > $DCC_PATH/config
    echo 0x106000 2 > $DCC_PATH/config
    echo 0x107000 2 > $DCC_PATH/config
    echo 0x108000 2 > $DCC_PATH/config
    echo 0x109000 2 > $DCC_PATH/config
    echo 0xc2a0000 2 > $DCC_PATH/config
    echo 0xc2a1000 2 > $DCC_PATH/config
    echo 0xc2b100c > $DCC_PATH/config
    echo 0x12802c > $DCC_PATH/config
    echo 0x128164 > $DCC_PATH/config
    echo 0x12829c > $DCC_PATH/config
    echo 0x1283d4 > $DCC_PATH/config
    echo 0x12850c > $DCC_PATH/config
    echo 0x128644 > $DCC_PATH/config
    echo 0x12e02c > $DCC_PATH/config
    echo 0x12e164 > $DCC_PATH/config
    echo 0x12e29c > $DCC_PATH/config
    echo 0x12e3d4 > $DCC_PATH/config
    echo 0x12e50c > $DCC_PATH/config
    echo 0x12e644 > $DCC_PATH/config
    echo 0x155028 > $DCC_PATH/config
    echo 0x15102c > $DCC_PATH/config
    echo 0x184058 > $DCC_PATH/config
    echo 0x12c094 > $DCC_PATH/config
    echo 0x146020 > $DCC_PATH/config
    echo 0x133054 > $DCC_PATH/config
    echo 0x15416c > $DCC_PATH/config
    echo 0x193028 > $DCC_PATH/config
    echo 0x186048 > $DCC_PATH/config
    echo 0x120004 2 > $DCC_PATH/config
    echo 0x12c004 2 > $DCC_PATH/config
    echo 0x17b004 2 > $DCC_PATH/config
    echo 0x17c000 2 > $DCC_PATH/config
    echo 0x19d004 2 > $DCC_PATH/config
    echo 0x19e000 2 > $DCC_PATH/config
    echo 0x1ad000 2 > $DCC_PATH/config
    echo 0x149008 > $DCC_PATH/config
    echo 0x149004 > $DCC_PATH/config
    echo 0x160018 2 > $DCC_PATH/config
    echo 0x162000 > $DCC_PATH/config
    echo 0x162008 > $DCC_PATH/config
    echo 0x162010 > $DCC_PATH/config
    echo 0x163024 > $DCC_PATH/config
    echo 0x16302c > $DCC_PATH/config
    echo 0x163034 > $DCC_PATH/config
    echo 0x166000 > $DCC_PATH/config
    echo 0x166008 > $DCC_PATH/config
    echo 0x166010 > $DCC_PATH/config
    echo 0x166204 > $DCC_PATH/config
    echo 0x165000 > $DCC_PATH/config
    echo 0x165008 > $DCC_PATH/config
    echo 0x165010 > $DCC_PATH/config
    echo 0x161000 > $DCC_PATH/config
    echo 0x161008 > $DCC_PATH/config
    echo 0x161010 > $DCC_PATH/config
    echo 0x16a000 > $DCC_PATH/config
    echo 0x16a008 > $DCC_PATH/config
    echo 0x16a010 > $DCC_PATH/config
    echo 0x167000 > $DCC_PATH/config
    echo 0x167008 > $DCC_PATH/config
    echo 0x167010 > $DCC_PATH/config
    echo 0x179024 > $DCC_PATH/config
    echo 0x17902c > $DCC_PATH/config
    echo 0x179034 > $DCC_PATH/config
    echo 0x16b000 > $DCC_PATH/config
    echo 0x16b008 > $DCC_PATH/config
    echo 0x16b010 > $DCC_PATH/config
    echo 0x1b1024 > $DCC_PATH/config
    echo 0x1b102c > $DCC_PATH/config
    echo 0x1b1034 > $DCC_PATH/config
    echo 0x120024 > $DCC_PATH/config
    echo 0x12c038 > $DCC_PATH/config
    echo 0x12c044 > $DCC_PATH/config
    echo 0x12c050 > $DCC_PATH/config
    echo 0x12c05c > $DCC_PATH/config
    echo 0x12c068 > $DCC_PATH/config
    echo 0x12c074 > $DCC_PATH/config
    echo 0x157004 > $DCC_PATH/config
    echo 0x1a0004 > $DCC_PATH/config
    echo 0x1a0010 > $DCC_PATH/config
    echo 0x1ad014 > $DCC_PATH/config
    echo 0x1ad020 > $DCC_PATH/config
    echo 0x193004 2 > $DCC_PATH/config
    echo 0x193144 > $DCC_PATH/config

    echo 0x18400c > $DCC_PATH/config
    echo 0x184018 2 > $DCC_PATH/config
    echo 0x17b038 > $DCC_PATH/config
    echo 0x17b02c > $DCC_PATH/config
    echo 0x17b054 > $DCC_PATH/config
    echo 0x17b048 > $DCC_PATH/config
    echo 0x17b020 > $DCC_PATH/config
    echo 0x17b01c > $DCC_PATH/config

    echo 0x129114 > $DCC_PATH/config
    echo 0x129214 > $DCC_PATH/config
    echo 0x18b114 > $DCC_PATH/config
    echo 0x18b214 > $DCC_PATH/config
    echo 0x18c114 > $DCC_PATH/config
    echo 0x18c214 > $DCC_PATH/config
    echo 0x18d114 > $DCC_PATH/config
    echo 0x18d214 > $DCC_PATH/config
    echo 0x18e114 > $DCC_PATH/config
    echo 0x18e214 > $DCC_PATH/config

    echo 0x121004 5 > $DCC_PATH/config
}

config_dcc_smmu()
{
    echo 0x151d0008 > $DCC_PATH/config
    echo 0x150025dc > $DCC_PATH/config
    echo 0x150055dc > $DCC_PATH/config
    echo 0x150075dc > $DCC_PATH/config
    echo 0x150075dc > $DCC_PATH/config
    echo 0x15002204 > $DCC_PATH/config
    echo 0x15002670 > $DCC_PATH/config
    echo 0x15002648 > $DCC_PATH/config
    echo 0x150022fc 3 > $DCC_PATH/config
    echo 0x150022fc > $DCC_PATH/config
    echo 0x15002304 > $DCC_PATH/config
}

config_dcc_gemnoc()
{
    # LLC0
    echo 0x19100010 > $DCC_PATH/config
    echo 0x19100020 6 > $DCC_PATH/config
    # LLC1
    echo 0x19140010 > $DCC_PATH/config
    echo 0x19140020 6 > $DCC_PATH/config
    # PCIe
    echo 0x19180010 > $DCC_PATH/config
    echo 0x19180020 6 > $DCC_PATH/config
    # CNOC
    echo 0x19180410 > $DCC_PATH/config
    echo 0x19180420 6 > $DCC_PATH/config
    echo 0x19180848 > $DCC_PATH/config
    # QOS Gen registers
    echo 0x1910e010 > $DCC_PATH/config
    echo 0x1910f010 > $DCC_PATH/config
    echo 0x19110010 > $DCC_PATH/config
    echo 0x1914e010 > $DCC_PATH/config
    echo 0x1914f010 > $DCC_PATH/config
    echo 0x19150010 > $DCC_PATH/config
    echo 0x1919c010 > $DCC_PATH/config
    echo 0x1919d010 > $DCC_PATH/config
    echo 0x1919e010 > $DCC_PATH/config
    echo 0x1919f010 > $DCC_PATH/config
    echo 0x191a0010 > $DCC_PATH/config
    echo 0x191a1010 > $DCC_PATH/config
    echo 0x191a2010 > $DCC_PATH/config

    #coherent even chain
    echo 0x19102018 > $DCC_PATH/config
    echo 0x19102008 > $DCC_PATH/config
    echo 0x3  > $DCC_PATH/loop
    echo 0x19102010 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #noncoherent even chain
    echo 0x19102098 > $DCC_PATH/config
    echo 0x19102088 > $DCC_PATH/config
    echo 0x4  > $DCC_PATH/loop
    echo 0x19102090 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #coherent odd chain
    echo 0x19142018 > $DCC_PATH/config
    echo 0x19142008 > $DCC_PATH/config
    echo 0x3  > $DCC_PATH/loop
    echo 0x19142010 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #noncoherent odd chain
    echo 0x19142098 > $DCC_PATH/config
    echo 0x19142088 > $DCC_PATH/config
    echo 0x4  > $DCC_PATH/loop
    echo 0x19142090 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #coherent sys chain
    echo 0x19182018 > $DCC_PATH/config
    echo 0x19182008 > $DCC_PATH/config
    echo 0x3  > $DCC_PATH/loop
    echo 0x19182010 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #noncoherent sys chain
    echo 0x19182098 > $DCC_PATH/config
    echo 0x19182088 > $DCC_PATH/config
    echo 0x9  > $DCC_PATH/loop
    echo 0x19182090 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #gem_noc_qns_llcc0_poc_dbg
    echo 0x19101010 > $DCC_PATH/config
    echo 0x40  > $DCC_PATH/loop
    echo 0x19101038 > $DCC_PATH/config
    echo 0x19101030 2 > $DCC_PATH/config
    echo 0x19101030 2 > $DCC_PATH/config
    echo 0x19101030 2 > $DCC_PATH/config
    echo 0x19101030 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #gem_noc_qns_llcc1_poc_dbg
    echo 0x19141010 > $DCC_PATH/config
    echo 0x40  > $DCC_PATH/loop
    echo 0x19141038 > $DCC_PATH/config
    echo 0x19141030 2 > $DCC_PATH/config
    echo 0x19141030 2 > $DCC_PATH/config
    echo 0x19141030 2 > $DCC_PATH/config
    echo 0x19141030 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #gem_noc_qns_pcie_poc_dbg
    echo 0x19181010 > $DCC_PATH/config
    echo 0x10  > $DCC_PATH/loop
    echo 0x19181038 > $DCC_PATH/config
    echo 0x19181030 2 > $DCC_PATH/config
    echo 0x19181030 2 > $DCC_PATH/config
    echo 0x19181030 2 > $DCC_PATH/config
    echo 0x19181030 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #gem_noc_qns_cnoc_poc_dbg
    echo 0x19181410 > $DCC_PATH/config
    echo 0x10  > $DCC_PATH/loop
    echo 0x19181438 > $DCC_PATH/config
    echo 0x19181430 2 > $DCC_PATH/config
    echo 0x19181430 2 > $DCC_PATH/config
    echo 0x19181430 2 > $DCC_PATH/config
    echo 0x19181430 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    echo 0x191920a0 > $DCC_PATH/config
    echo 0x191920a8 > $DCC_PATH/config
}

config_dcc_cnoc()
{
    echo 0x1500010 > $DCC_PATH/config
    echo 0x1500020 8 > $DCC_PATH/config
    echo 0x1500248 2 > $DCC_PATH/config
    echo 0x1500258 > $DCC_PATH/config
    echo 0x1500448 > $DCC_PATH/config

    #CNOC_config_noc_main_center_DebugChain
    echo 0x1502018 > $DCC_PATH/config
    echo 0x1502008 > $DCC_PATH/config
    echo 0x13  > $DCC_PATH/loop
    echo 0x1502010 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #CNOC_config_noc_main_mmnoc_DebugChain
    echo 0x1502098 > $DCC_PATH/config
    echo 0x1502088 > $DCC_PATH/config
    echo 0x2  > $DCC_PATH/loop
    echo 0x1502090 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #CNOC_config_noc_main_lpass_tile_DebugChain
    echo 0x1502118 > $DCC_PATH/config
    echo 0x1502108 > $DCC_PATH/config
    echo 0x3  > $DCC_PATH/loop
    echo 0x1502110 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #CNOC_config_noc_main_north_DebugChain
    echo 0x1502198 > $DCC_PATH/config
    echo 0x1502188 > $DCC_PATH/config
    echo 0x5  > $DCC_PATH/loop
    echo 0x1502190 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop
}

config_dcc_snoc()
{
    echo 0x1680010 > $DCC_PATH/config
    echo 0x1680020 8 > $DCC_PATH/config
    echo 0x1680248 > $DCC_PATH/config

    #system_noc_DebugChain_center
    echo 0x1681018 > $DCC_PATH/config
    echo 0x1681008 > $DCC_PATH/config
    echo 0x9  > $DCC_PATH/loop
    echo 0x1681010 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop

    #system_noc_DebugChain_north
    echo 0x1681098 > $DCC_PATH/config
    echo 0x1681088 > $DCC_PATH/config
    echo 0x3  > $DCC_PATH/loop
    echo 0x1681090 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop
}

config_dcc_pcie_agnoc()
{
    echo 0x16c0010 > $DCC_PATH/config
    echo 0x16c0020 8 > $DCC_PATH/config
    echo 0x16c0248 > $DCC_PATH/config

    #system_noc_DebugChain_pcie
    echo 0x16c1018 > $DCC_PATH/config
    echo 0x16c1008 > $DCC_PATH/config
    echo 0x4  > $DCC_PATH/loop
    echo 0x16c1010 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop
}

config_dcc_mmnoc()
{
    echo 0x1740010 > $DCC_PATH/config
    echo 0x1740020 8 > $DCC_PATH/config
    echo 0x1740248 > $DCC_PATH/config

    #system_noc_DebugChain_mmnoc
    echo 0x1741018 > $DCC_PATH/config
    echo 0x1741008 > $DCC_PATH/config
    echo 0x10  > $DCC_PATH/loop
    echo 0x1741010 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop
}

config_dcc_lpass_ag_noc()
{
    echo 0x3c40010 > $DCC_PATH/config
    echo 0x3c40020 8 > $DCC_PATH/config
    echo 0x3c4b048 > $DCC_PATH/config

    #lpass_ag_noc_agnoc_core_DebugChain
    echo 0x3c41018 > $DCC_PATH/config
    echo 0x3c41008 > $DCC_PATH/config
    echo 0x4  > $DCC_PATH/loop
    echo 0x3c41010 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop
}

config_dcc_dc_dch_noc()
{
    echo 0x190e0010 > $DCC_PATH/config
    echo 0x190e0020 8 > $DCC_PATH/config
    echo 0x190e0248 > $DCC_PATH/config

    #dc_noc_dch_debug_chain
    echo 0x190e5018 > $DCC_PATH/config
    echo 0x190e5008 > $DCC_PATH/config
    echo 0x7  > $DCC_PATH/loop
    echo 0x190e5010 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop
}

config_dcc_dc_ch01()
{
    echo 0x195f0010 > $DCC_PATH/config
    echo 0x195f0020 8 > $DCC_PATH/config
    echo 0x195f0248 > $DCC_PATH/config

    #DC_NOC_CH01_SERVICE_NETWORKdc_no_chain
    echo 0x195f2018 > $DCC_PATH/config
    echo 0x195f2008 > $DCC_PATH/config
    echo 0x3  > $DCC_PATH/loop
    echo 0x195f2010 2 > $DCC_PATH/config
    echo 0x1  > $DCC_PATH/loop
}

config_dcc_cdsp()
{
    echo 0x323b0208 > $DCC_PATH/config
    echo 0x323b0228 > $DCC_PATH/config
    echo 0x323b0248 > $DCC_PATH/config
    echo 0x323b0268 > $DCC_PATH/config
    echo 0x323b0288 > $DCC_PATH/config
    echo 0x323b02a8 > $DCC_PATH/config
    echo 0x323b020c > $DCC_PATH/config
    echo 0x323b022c > $DCC_PATH/config
    echo 0x323b024c > $DCC_PATH/config
    echo 0x323b026c > $DCC_PATH/config
    echo 0x323b028c > $DCC_PATH/config
    echo 0x323b02ac > $DCC_PATH/config
    echo 0x323b0210 > $DCC_PATH/config
    echo 0x323b0230 > $DCC_PATH/config
    echo 0x323b0250 > $DCC_PATH/config
    echo 0x323b0270 > $DCC_PATH/config
    echo 0x323b0290 > $DCC_PATH/config
    echo 0x323b02b0 > $DCC_PATH/config
    echo 0x323b0400 3 > $DCC_PATH/config
    echo 0x320a4400 3 > $DCC_PATH/config
    echo 0x32302028 > $DCC_PATH/config
    echo 0x32300304 > $DCC_PATH/config
    echo 0x320a4208 > $DCC_PATH/config
    echo 0x320a4228 > $DCC_PATH/config
    echo 0x320a4248 > $DCC_PATH/config
    echo 0x320a4268 > $DCC_PATH/config
    echo 0x320a4288 > $DCC_PATH/config
    echo 0x320a42a8 > $DCC_PATH/config
    echo 0x320a420c > $DCC_PATH/config
    echo 0x320a422c > $DCC_PATH/config
    echo 0x320a424c > $DCC_PATH/config
    echo 0x320a426c > $DCC_PATH/config
    echo 0x320a428c > $DCC_PATH/config
}

config_dcc_adsp()
{
    echo 0x30b0208 > $DCC_PATH/config
    echo 0x30b0228 > $DCC_PATH/config
    echo 0x30b0248 > $DCC_PATH/config
    echo 0x30b0268 > $DCC_PATH/config
    echo 0x30b0288 > $DCC_PATH/config
    echo 0x30b02a8 > $DCC_PATH/config
    echo 0x30b020c > $DCC_PATH/config
    echo 0x30b022c > $DCC_PATH/config
    echo 0x30b024c > $DCC_PATH/config
    echo 0x30b026c > $DCC_PATH/config
    echo 0x30b028c > $DCC_PATH/config
    echo 0x30b02ac > $DCC_PATH/config
    echo 0x30b0210 > $DCC_PATH/config
    echo 0x30b0230 > $DCC_PATH/config
    echo 0x30b0250 > $DCC_PATH/config
    echo 0x30b0270 > $DCC_PATH/config
    echo 0x30b0290 > $DCC_PATH/config
    echo 0x30b02b0 > $DCC_PATH/config
    echo 0x30b0400 3 > $DCC_PATH/config
    echo 0x3480400 3 > $DCC_PATH/config
    echo 0x3002028 > $DCC_PATH/config
    echo 0x3000304 > $DCC_PATH/config
}

config_dcc_wpss()
{
    echo 0x8ab0208 > $DCC_PATH/config
    echo 0x8ab0228 > $DCC_PATH/config
    echo 0x8ab0248 > $DCC_PATH/config
    echo 0x8ab0268 > $DCC_PATH/config
    echo 0x8ab0288 > $DCC_PATH/config
    echo 0x8ab02a8 > $DCC_PATH/config
    echo 0x8ab020c > $DCC_PATH/config
    echo 0x8ab022c > $DCC_PATH/config
    echo 0x8ab024c > $DCC_PATH/config
    echo 0x8ab026c > $DCC_PATH/config
    echo 0x8ab028c > $DCC_PATH/config
    echo 0x8ab02ac > $DCC_PATH/config
    echo 0x8ab0210 > $DCC_PATH/config
    echo 0x8ab0230 > $DCC_PATH/config
    echo 0x8ab0250 > $DCC_PATH/config
    echo 0x8ab0270 > $DCC_PATH/config
    echo 0x8ab0290 > $DCC_PATH/config
    echo 0x8ab02b0 > $DCC_PATH/config
    echo 0x8ab0400 3 > $DCC_PATH/config
    echo 0x8b00400 3 > $DCC_PATH/config
    echo 0x8a02028 > $DCC_PATH/config
    echo 0x8a00304 > $DCC_PATH/config
}

config_dcc_qup()
{
    echo 0x13316c > $DCC_PATH/config
    echo 0x13317c > $DCC_PATH/config
    echo 0x128004 > $DCC_PATH/config
    echo 0x128008 > $DCC_PATH/config
    echo 0x12e008 > $DCC_PATH/config
    echo 0x12e00c > $DCC_PATH/config
    echo 0x12e144 > $DCC_PATH/config
    echo 0x12e27c > $DCC_PATH/config
    echo 0x12e3b4 > $DCC_PATH/config
    echo 0x12e4ec > $DCC_PATH/config
    echo 0x12e624 > $DCC_PATH/config
}

config_dcc_cpucp()
{
    echo 0x1282802c > $DCC_PATH/config
    echo 0x17a30d38 > $DCC_PATH/config
    echo 0x17a30d3c > $DCC_PATH/config
    echo 0xb630010 > $DCC_PATH/config
}

config_dcc_turing_rscc()
{
    echo 0x323b0404 > $DCC_PATH/config
    echo 0x323b0408 > $DCC_PATH/config
    echo 0x323b0208 > $DCC_PATH/config
    echo 0x323b0228 > $DCC_PATH/config
    echo 0x323b0248 > $DCC_PATH/config
    echo 0x323b0268 > $DCC_PATH/config
    echo 0x323b0288 > $DCC_PATH/config
    echo 0x323b02a8 > $DCC_PATH/config
    echo 0x323b020c > $DCC_PATH/config
    echo 0x323b022c > $DCC_PATH/config
    echo 0x323b024c > $DCC_PATH/config
    echo 0x323b026c > $DCC_PATH/config
    echo 0x323b028c > $DCC_PATH/config
    echo 0x323b02ac > $DCC_PATH/config
}

enable_cpuss_register()
{
    echo 1 > /sys/bus/platform/devices/soc:mem_dump/register_reset

    format_ver=1
    if [ -r /sys/bus/platform/devices/soc:mem_dump/format_version ]; then
        format_ver=$(cat /sys/bus/platform/devices/soc:mem_dump/format_version)
    fi
    #MEM_DUM_PATH = "/sys/bus/platform/devices/soc:mem_dump"
    echo 0x17000000 > $MEM_DUMP_PATH/register_config
    echo 0x17000008 2 > $MEM_DUMP_PATH/register_config
    echo 0x17000008 18 > $MEM_DUMP_PATH/register_config
    echo 0x17000054 7 > $MEM_DUMP_PATH/register_config
    echo 0x170000f0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17000048 2 > $MEM_DUMP_PATH/register_config
    echo 0x17000054 5 > $MEM_DUMP_PATH/register_config
    echo 0x17000054 7 > $MEM_DUMP_PATH/register_config
    echo 0x170000f0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17008000 5 > $MEM_DUMP_PATH/register_config
    echo 0x17200000 3 > $MEM_DUMP_PATH/register_config
    echo 0x17200020 2 > $MEM_DUMP_PATH/register_config
    echo 0x17200020 2 > $MEM_DUMP_PATH/register_config
    echo 0x17200084 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200104 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200184 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200204 20 > $MEM_DUMP_PATH/register_config
    echo 0x17200104 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200184 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200204 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200284 20 > $MEM_DUMP_PATH/register_config
    echo 0x17200184 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200204 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200284 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200304 20 > $MEM_DUMP_PATH/register_config
    echo 0x17200204 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200284 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200304 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200384 20 > $MEM_DUMP_PATH/register_config
    echo 0x17200284 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200304 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200384 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200420 13 > $MEM_DUMP_PATH/register_config
    echo 0x17200304 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200384 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200420 45 > $MEM_DUMP_PATH/register_config
    echo 0x17200384 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200420 77 > $MEM_DUMP_PATH/register_config
    echo 0x17200420 232 > $MEM_DUMP_PATH/register_config
    echo 0x17200c08 58 > $MEM_DUMP_PATH/register_config
    echo 0x17200d04 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200e08 58 > $MEM_DUMP_PATH/register_config
    echo 0x17200c08 58 > $MEM_DUMP_PATH/register_config
    echo 0x17200d04 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200e08 58 > $MEM_DUMP_PATH/register_config
    echo 0x17200d04 29 > $MEM_DUMP_PATH/register_config
    echo 0x17200e08 51 > $MEM_DUMP_PATH/register_config
    echo 0x17200e08 58 > $MEM_DUMP_PATH/register_config
    echo 0x17206100 > $MEM_DUMP_PATH/register_config
    echo 0x17206108 > $MEM_DUMP_PATH/register_config
    echo 0x17206108 > $MEM_DUMP_PATH/register_config
    echo 0x17206110 > $MEM_DUMP_PATH/register_config
    echo 0x17206110 > $MEM_DUMP_PATH/register_config
    echo 0x17206118 > $MEM_DUMP_PATH/register_config
    echo 0x17206118 > $MEM_DUMP_PATH/register_config
    echo 0x17206120 > $MEM_DUMP_PATH/register_config
    echo 0x17206120 > $MEM_DUMP_PATH/register_config
    echo 0x17206128 > $MEM_DUMP_PATH/register_config
    echo 0x17206128 > $MEM_DUMP_PATH/register_config
    echo 0x17206130 > $MEM_DUMP_PATH/register_config
    echo 0x17206130 > $MEM_DUMP_PATH/register_config
    echo 0x17206138 > $MEM_DUMP_PATH/register_config
    echo 0x17206138 > $MEM_DUMP_PATH/register_config
    echo 0x17206140 > $MEM_DUMP_PATH/register_config
    echo 0x17206140 > $MEM_DUMP_PATH/register_config
    echo 0x17206148 > $MEM_DUMP_PATH/register_config
    echo 0x17206148 > $MEM_DUMP_PATH/register_config
    echo 0x17206150 > $MEM_DUMP_PATH/register_config
    echo 0x17206150 > $MEM_DUMP_PATH/register_config
    echo 0x17206158 > $MEM_DUMP_PATH/register_config
    echo 0x17206158 > $MEM_DUMP_PATH/register_config
    echo 0x17206160 > $MEM_DUMP_PATH/register_config
    echo 0x17206160 > $MEM_DUMP_PATH/register_config
    echo 0x17206168 > $MEM_DUMP_PATH/register_config
    echo 0x17206168 > $MEM_DUMP_PATH/register_config
    echo 0x17206170 > $MEM_DUMP_PATH/register_config
    echo 0x17206170 > $MEM_DUMP_PATH/register_config
    echo 0x17206178 > $MEM_DUMP_PATH/register_config
    echo 0x17206178 > $MEM_DUMP_PATH/register_config
    echo 0x17206180 > $MEM_DUMP_PATH/register_config
    echo 0x17206180 > $MEM_DUMP_PATH/register_config
    echo 0x17206188 > $MEM_DUMP_PATH/register_config
    echo 0x17206188 > $MEM_DUMP_PATH/register_config
    echo 0x17206190 > $MEM_DUMP_PATH/register_config
    echo 0x17206190 > $MEM_DUMP_PATH/register_config
    echo 0x17206198 > $MEM_DUMP_PATH/register_config
    echo 0x17206198 > $MEM_DUMP_PATH/register_config
    echo 0x172061a0 > $MEM_DUMP_PATH/register_config
    echo 0x172061a0 > $MEM_DUMP_PATH/register_config
    echo 0x172061a8 > $MEM_DUMP_PATH/register_config
    echo 0x172061a8 > $MEM_DUMP_PATH/register_config
    echo 0x172061b0 > $MEM_DUMP_PATH/register_config
    echo 0x172061b0 > $MEM_DUMP_PATH/register_config
    echo 0x172061b8 > $MEM_DUMP_PATH/register_config
    echo 0x172061b8 > $MEM_DUMP_PATH/register_config
    echo 0x172061c0 > $MEM_DUMP_PATH/register_config
    echo 0x172061c0 > $MEM_DUMP_PATH/register_config
    echo 0x172061c8 > $MEM_DUMP_PATH/register_config
    echo 0x172061c8 > $MEM_DUMP_PATH/register_config
    echo 0x172061d0 > $MEM_DUMP_PATH/register_config
    echo 0x172061d0 > $MEM_DUMP_PATH/register_config
    echo 0x172061d8 > $MEM_DUMP_PATH/register_config
    echo 0x172061d8 > $MEM_DUMP_PATH/register_config
    echo 0x172061e0 > $MEM_DUMP_PATH/register_config
    echo 0x172061e0 > $MEM_DUMP_PATH/register_config
    echo 0x172061e8 > $MEM_DUMP_PATH/register_config
    echo 0x172061e8 > $MEM_DUMP_PATH/register_config
    echo 0x172061f0 > $MEM_DUMP_PATH/register_config
    echo 0x172061f0 > $MEM_DUMP_PATH/register_config
    echo 0x172061f8 > $MEM_DUMP_PATH/register_config
    echo 0x172061f8 > $MEM_DUMP_PATH/register_config
    echo 0x17206200 > $MEM_DUMP_PATH/register_config
    echo 0x17206200 > $MEM_DUMP_PATH/register_config
    echo 0x17206208 > $MEM_DUMP_PATH/register_config
    echo 0x17206208 > $MEM_DUMP_PATH/register_config
    echo 0x17206210 > $MEM_DUMP_PATH/register_config
    echo 0x17206210 > $MEM_DUMP_PATH/register_config
    echo 0x17206218 > $MEM_DUMP_PATH/register_config
    echo 0x17206218 > $MEM_DUMP_PATH/register_config
    echo 0x17206220 > $MEM_DUMP_PATH/register_config
    echo 0x17206220 > $MEM_DUMP_PATH/register_config
    echo 0x17206228 > $MEM_DUMP_PATH/register_config
    echo 0x17206228 > $MEM_DUMP_PATH/register_config
    echo 0x17206230 > $MEM_DUMP_PATH/register_config
    echo 0x17206230 > $MEM_DUMP_PATH/register_config
    echo 0x17206238 > $MEM_DUMP_PATH/register_config
    echo 0x17206238 > $MEM_DUMP_PATH/register_config
    echo 0x17206240 > $MEM_DUMP_PATH/register_config
    echo 0x17206240 > $MEM_DUMP_PATH/register_config
    echo 0x17206248 > $MEM_DUMP_PATH/register_config
    echo 0x17206248 > $MEM_DUMP_PATH/register_config
    echo 0x17206250 > $MEM_DUMP_PATH/register_config
    echo 0x17206250 > $MEM_DUMP_PATH/register_config
    echo 0x17206258 > $MEM_DUMP_PATH/register_config
    echo 0x17206258 > $MEM_DUMP_PATH/register_config
    echo 0x17206260 > $MEM_DUMP_PATH/register_config
    echo 0x17206260 > $MEM_DUMP_PATH/register_config
    echo 0x17206268 > $MEM_DUMP_PATH/register_config
    echo 0x17206268 > $MEM_DUMP_PATH/register_config
    echo 0x17206270 > $MEM_DUMP_PATH/register_config
    echo 0x17206270 > $MEM_DUMP_PATH/register_config
    echo 0x17206278 > $MEM_DUMP_PATH/register_config
    echo 0x17206278 > $MEM_DUMP_PATH/register_config
    echo 0x17206280 > $MEM_DUMP_PATH/register_config
    echo 0x17206280 > $MEM_DUMP_PATH/register_config
    echo 0x17206288 > $MEM_DUMP_PATH/register_config
    echo 0x17206288 > $MEM_DUMP_PATH/register_config
    echo 0x17206290 > $MEM_DUMP_PATH/register_config
    echo 0x17206290 > $MEM_DUMP_PATH/register_config
    echo 0x17206298 > $MEM_DUMP_PATH/register_config
    echo 0x17206298 > $MEM_DUMP_PATH/register_config
    echo 0x172062a0 > $MEM_DUMP_PATH/register_config
    echo 0x172062a0 > $MEM_DUMP_PATH/register_config
    echo 0x172062a8 > $MEM_DUMP_PATH/register_config
    echo 0x172062a8 > $MEM_DUMP_PATH/register_config
    echo 0x172062b0 > $MEM_DUMP_PATH/register_config
    echo 0x172062b0 > $MEM_DUMP_PATH/register_config
    echo 0x172062b8 > $MEM_DUMP_PATH/register_config
    echo 0x172062b8 > $MEM_DUMP_PATH/register_config
    echo 0x172062c0 > $MEM_DUMP_PATH/register_config
    echo 0x172062c0 > $MEM_DUMP_PATH/register_config
    echo 0x172062c8 > $MEM_DUMP_PATH/register_config
    echo 0x172062c8 > $MEM_DUMP_PATH/register_config
    echo 0x172062d0 > $MEM_DUMP_PATH/register_config
    echo 0x172062d0 > $MEM_DUMP_PATH/register_config
    echo 0x172062d8 > $MEM_DUMP_PATH/register_config
    echo 0x172062d8 > $MEM_DUMP_PATH/register_config
    echo 0x172062e0 > $MEM_DUMP_PATH/register_config
    echo 0x172062e0 > $MEM_DUMP_PATH/register_config
    echo 0x172062e8 > $MEM_DUMP_PATH/register_config
    echo 0x172062e8 > $MEM_DUMP_PATH/register_config
    echo 0x172062f0 > $MEM_DUMP_PATH/register_config
    echo 0x172062f0 > $MEM_DUMP_PATH/register_config
    echo 0x172062f8 > $MEM_DUMP_PATH/register_config
    echo 0x172062f8 > $MEM_DUMP_PATH/register_config
    echo 0x17206300 > $MEM_DUMP_PATH/register_config
    echo 0x17206300 > $MEM_DUMP_PATH/register_config
    echo 0x17206308 > $MEM_DUMP_PATH/register_config
    echo 0x17206308 > $MEM_DUMP_PATH/register_config
    echo 0x17206310 > $MEM_DUMP_PATH/register_config
    echo 0x17206310 > $MEM_DUMP_PATH/register_config
    echo 0x17206318 > $MEM_DUMP_PATH/register_config
    echo 0x17206318 > $MEM_DUMP_PATH/register_config
    echo 0x17206320 > $MEM_DUMP_PATH/register_config
    echo 0x17206320 > $MEM_DUMP_PATH/register_config
    echo 0x17206328 > $MEM_DUMP_PATH/register_config
    echo 0x17206328 > $MEM_DUMP_PATH/register_config
    echo 0x17206330 > $MEM_DUMP_PATH/register_config
    echo 0x17206330 > $MEM_DUMP_PATH/register_config
    echo 0x17206338 > $MEM_DUMP_PATH/register_config
    echo 0x17206338 > $MEM_DUMP_PATH/register_config
    echo 0x17206340 > $MEM_DUMP_PATH/register_config
    echo 0x17206340 > $MEM_DUMP_PATH/register_config
    echo 0x17206348 > $MEM_DUMP_PATH/register_config
    echo 0x17206348 > $MEM_DUMP_PATH/register_config
    echo 0x17206350 > $MEM_DUMP_PATH/register_config
    echo 0x17206350 > $MEM_DUMP_PATH/register_config
    echo 0x17206358 > $MEM_DUMP_PATH/register_config
    echo 0x17206358 > $MEM_DUMP_PATH/register_config
    echo 0x17206360 > $MEM_DUMP_PATH/register_config
    echo 0x17206360 > $MEM_DUMP_PATH/register_config
    echo 0x17206368 > $MEM_DUMP_PATH/register_config
    echo 0x17206368 > $MEM_DUMP_PATH/register_config
    echo 0x17206370 > $MEM_DUMP_PATH/register_config
    echo 0x17206370 > $MEM_DUMP_PATH/register_config
    echo 0x17206378 > $MEM_DUMP_PATH/register_config
    echo 0x17206378 > $MEM_DUMP_PATH/register_config
    echo 0x17206380 > $MEM_DUMP_PATH/register_config
    echo 0x17206380 > $MEM_DUMP_PATH/register_config
    echo 0x17206388 > $MEM_DUMP_PATH/register_config
    echo 0x17206388 > $MEM_DUMP_PATH/register_config
    echo 0x17206390 > $MEM_DUMP_PATH/register_config
    echo 0x17206390 > $MEM_DUMP_PATH/register_config
    echo 0x17206398 > $MEM_DUMP_PATH/register_config
    echo 0x17206398 > $MEM_DUMP_PATH/register_config
    echo 0x172063a0 > $MEM_DUMP_PATH/register_config
    echo 0x172063a0 > $MEM_DUMP_PATH/register_config
    echo 0x172063a8 > $MEM_DUMP_PATH/register_config
    echo 0x172063a8 > $MEM_DUMP_PATH/register_config
    echo 0x172063b0 > $MEM_DUMP_PATH/register_config
    echo 0x172063b0 > $MEM_DUMP_PATH/register_config
    echo 0x172063b8 > $MEM_DUMP_PATH/register_config
    echo 0x172063b8 > $MEM_DUMP_PATH/register_config
    echo 0x172063c0 > $MEM_DUMP_PATH/register_config
    echo 0x172063c0 > $MEM_DUMP_PATH/register_config
    echo 0x172063c8 > $MEM_DUMP_PATH/register_config
    echo 0x172063c8 > $MEM_DUMP_PATH/register_config
    echo 0x172063d0 > $MEM_DUMP_PATH/register_config
    echo 0x172063d0 > $MEM_DUMP_PATH/register_config
    echo 0x172063d8 > $MEM_DUMP_PATH/register_config
    echo 0x172063d8 > $MEM_DUMP_PATH/register_config
    echo 0x172063e0 > $MEM_DUMP_PATH/register_config
    echo 0x172063e0 > $MEM_DUMP_PATH/register_config
    echo 0x172063e8 > $MEM_DUMP_PATH/register_config
    echo 0x172063e8 > $MEM_DUMP_PATH/register_config
    echo 0x172063f0 > $MEM_DUMP_PATH/register_config
    echo 0x172063f0 > $MEM_DUMP_PATH/register_config
    echo 0x172063f8 > $MEM_DUMP_PATH/register_config
    echo 0x172063f8 > $MEM_DUMP_PATH/register_config
    echo 0x17206400 > $MEM_DUMP_PATH/register_config
    echo 0x17206400 > $MEM_DUMP_PATH/register_config
    echo 0x17206408 > $MEM_DUMP_PATH/register_config
    echo 0x17206408 > $MEM_DUMP_PATH/register_config
    echo 0x17206410 > $MEM_DUMP_PATH/register_config
    echo 0x17206410 > $MEM_DUMP_PATH/register_config
    echo 0x17206418 > $MEM_DUMP_PATH/register_config
    echo 0x17206418 > $MEM_DUMP_PATH/register_config
    echo 0x17206420 > $MEM_DUMP_PATH/register_config
    echo 0x17206420 > $MEM_DUMP_PATH/register_config
    echo 0x17206428 > $MEM_DUMP_PATH/register_config
    echo 0x17206428 > $MEM_DUMP_PATH/register_config
    echo 0x17206430 > $MEM_DUMP_PATH/register_config
    echo 0x17206430 > $MEM_DUMP_PATH/register_config
    echo 0x17206438 > $MEM_DUMP_PATH/register_config
    echo 0x17206438 > $MEM_DUMP_PATH/register_config
    echo 0x17206440 > $MEM_DUMP_PATH/register_config
    echo 0x17206440 > $MEM_DUMP_PATH/register_config
    echo 0x17206448 > $MEM_DUMP_PATH/register_config
    echo 0x17206448 > $MEM_DUMP_PATH/register_config
    echo 0x17206450 > $MEM_DUMP_PATH/register_config
    echo 0x17206450 > $MEM_DUMP_PATH/register_config
    echo 0x17206458 > $MEM_DUMP_PATH/register_config
    echo 0x17206458 > $MEM_DUMP_PATH/register_config
    echo 0x17206460 > $MEM_DUMP_PATH/register_config
    echo 0x17206460 > $MEM_DUMP_PATH/register_config
    echo 0x17206468 > $MEM_DUMP_PATH/register_config
    echo 0x17206468 > $MEM_DUMP_PATH/register_config
    echo 0x17206470 > $MEM_DUMP_PATH/register_config
    echo 0x17206470 > $MEM_DUMP_PATH/register_config
    echo 0x17206478 > $MEM_DUMP_PATH/register_config
    echo 0x17206478 > $MEM_DUMP_PATH/register_config
    echo 0x17206480 > $MEM_DUMP_PATH/register_config
    echo 0x17206480 > $MEM_DUMP_PATH/register_config
    echo 0x17206488 > $MEM_DUMP_PATH/register_config
    echo 0x17206488 > $MEM_DUMP_PATH/register_config
    echo 0x17206490 > $MEM_DUMP_PATH/register_config
    echo 0x17206490 > $MEM_DUMP_PATH/register_config
    echo 0x17206498 > $MEM_DUMP_PATH/register_config
    echo 0x17206498 > $MEM_DUMP_PATH/register_config
    echo 0x172064a0 > $MEM_DUMP_PATH/register_config
    echo 0x172064a0 > $MEM_DUMP_PATH/register_config
    echo 0x172064a8 > $MEM_DUMP_PATH/register_config
    echo 0x172064a8 > $MEM_DUMP_PATH/register_config
    echo 0x172064b0 > $MEM_DUMP_PATH/register_config
    echo 0x172064b0 > $MEM_DUMP_PATH/register_config
    echo 0x172064b8 > $MEM_DUMP_PATH/register_config
    echo 0x172064b8 > $MEM_DUMP_PATH/register_config
    echo 0x172064c0 > $MEM_DUMP_PATH/register_config
    echo 0x172064c0 > $MEM_DUMP_PATH/register_config
    echo 0x172064c8 > $MEM_DUMP_PATH/register_config
    echo 0x172064c8 > $MEM_DUMP_PATH/register_config
    echo 0x172064d0 > $MEM_DUMP_PATH/register_config
    echo 0x172064d0 > $MEM_DUMP_PATH/register_config
    echo 0x172064d8 > $MEM_DUMP_PATH/register_config
    echo 0x172064d8 > $MEM_DUMP_PATH/register_config
    echo 0x172064e0 > $MEM_DUMP_PATH/register_config
    echo 0x172064e0 > $MEM_DUMP_PATH/register_config
    echo 0x172064e8 > $MEM_DUMP_PATH/register_config
    echo 0x172064e8 > $MEM_DUMP_PATH/register_config
    echo 0x172064f0 > $MEM_DUMP_PATH/register_config
    echo 0x172064f0 > $MEM_DUMP_PATH/register_config
    echo 0x172064f8 > $MEM_DUMP_PATH/register_config
    echo 0x172064f8 > $MEM_DUMP_PATH/register_config
    echo 0x17206500 > $MEM_DUMP_PATH/register_config
    echo 0x17206500 > $MEM_DUMP_PATH/register_config
    echo 0x17206508 > $MEM_DUMP_PATH/register_config
    echo 0x17206508 > $MEM_DUMP_PATH/register_config
    echo 0x17206510 > $MEM_DUMP_PATH/register_config
    echo 0x17206510 > $MEM_DUMP_PATH/register_config
    echo 0x17206518 > $MEM_DUMP_PATH/register_config
    echo 0x17206518 > $MEM_DUMP_PATH/register_config
    echo 0x17206520 > $MEM_DUMP_PATH/register_config
    echo 0x17206520 > $MEM_DUMP_PATH/register_config
    echo 0x17206528 > $MEM_DUMP_PATH/register_config
    echo 0x17206528 > $MEM_DUMP_PATH/register_config
    echo 0x17206530 > $MEM_DUMP_PATH/register_config
    echo 0x17206530 > $MEM_DUMP_PATH/register_config
    echo 0x17206538 > $MEM_DUMP_PATH/register_config
    echo 0x17206538 > $MEM_DUMP_PATH/register_config
    echo 0x17206540 > $MEM_DUMP_PATH/register_config
    echo 0x17206540 > $MEM_DUMP_PATH/register_config
    echo 0x17206548 > $MEM_DUMP_PATH/register_config
    echo 0x17206548 > $MEM_DUMP_PATH/register_config
    echo 0x17206550 > $MEM_DUMP_PATH/register_config
    echo 0x17206550 > $MEM_DUMP_PATH/register_config
    echo 0x17206558 > $MEM_DUMP_PATH/register_config
    echo 0x17206558 > $MEM_DUMP_PATH/register_config
    echo 0x17206560 > $MEM_DUMP_PATH/register_config
    echo 0x17206560 > $MEM_DUMP_PATH/register_config
    echo 0x17206568 > $MEM_DUMP_PATH/register_config
    echo 0x17206568 > $MEM_DUMP_PATH/register_config
    echo 0x17206570 > $MEM_DUMP_PATH/register_config
    echo 0x17206570 > $MEM_DUMP_PATH/register_config
    echo 0x17206578 > $MEM_DUMP_PATH/register_config
    echo 0x17206578 > $MEM_DUMP_PATH/register_config
    echo 0x17206580 > $MEM_DUMP_PATH/register_config
    echo 0x17206580 > $MEM_DUMP_PATH/register_config
    echo 0x17206588 > $MEM_DUMP_PATH/register_config
    echo 0x17206588 > $MEM_DUMP_PATH/register_config
    echo 0x17206590 > $MEM_DUMP_PATH/register_config
    echo 0x17206590 > $MEM_DUMP_PATH/register_config
    echo 0x17206598 > $MEM_DUMP_PATH/register_config
    echo 0x17206598 > $MEM_DUMP_PATH/register_config
    echo 0x172065a0 > $MEM_DUMP_PATH/register_config
    echo 0x172065a0 > $MEM_DUMP_PATH/register_config
    echo 0x172065a8 > $MEM_DUMP_PATH/register_config
    echo 0x172065a8 > $MEM_DUMP_PATH/register_config
    echo 0x172065b0 > $MEM_DUMP_PATH/register_config
    echo 0x172065b0 > $MEM_DUMP_PATH/register_config
    echo 0x172065b8 > $MEM_DUMP_PATH/register_config
    echo 0x172065b8 > $MEM_DUMP_PATH/register_config
    echo 0x172065c0 > $MEM_DUMP_PATH/register_config
    echo 0x172065c0 > $MEM_DUMP_PATH/register_config
    echo 0x172065c8 > $MEM_DUMP_PATH/register_config
    echo 0x172065c8 > $MEM_DUMP_PATH/register_config
    echo 0x172065d0 > $MEM_DUMP_PATH/register_config
    echo 0x172065d0 > $MEM_DUMP_PATH/register_config
    echo 0x172065d8 > $MEM_DUMP_PATH/register_config
    echo 0x172065d8 > $MEM_DUMP_PATH/register_config
    echo 0x172065e0 > $MEM_DUMP_PATH/register_config
    echo 0x172065e0 > $MEM_DUMP_PATH/register_config
    echo 0x172065e8 > $MEM_DUMP_PATH/register_config
    echo 0x172065e8 > $MEM_DUMP_PATH/register_config
    echo 0x172065f0 > $MEM_DUMP_PATH/register_config
    echo 0x172065f0 > $MEM_DUMP_PATH/register_config
    echo 0x172065f8 > $MEM_DUMP_PATH/register_config
    echo 0x172065f8 > $MEM_DUMP_PATH/register_config
    echo 0x17206600 > $MEM_DUMP_PATH/register_config
    echo 0x17206600 > $MEM_DUMP_PATH/register_config
    echo 0x17206608 > $MEM_DUMP_PATH/register_config
    echo 0x17206608 > $MEM_DUMP_PATH/register_config
    echo 0x17206610 > $MEM_DUMP_PATH/register_config
    echo 0x17206610 > $MEM_DUMP_PATH/register_config
    echo 0x17206618 > $MEM_DUMP_PATH/register_config
    echo 0x17206618 > $MEM_DUMP_PATH/register_config
    echo 0x17206620 > $MEM_DUMP_PATH/register_config
    echo 0x17206620 > $MEM_DUMP_PATH/register_config
    echo 0x17206628 > $MEM_DUMP_PATH/register_config
    echo 0x17206628 > $MEM_DUMP_PATH/register_config
    echo 0x17206630 > $MEM_DUMP_PATH/register_config
    echo 0x17206630 > $MEM_DUMP_PATH/register_config
    echo 0x17206638 > $MEM_DUMP_PATH/register_config
    echo 0x17206638 > $MEM_DUMP_PATH/register_config
    echo 0x17206640 > $MEM_DUMP_PATH/register_config
    echo 0x17206640 > $MEM_DUMP_PATH/register_config
    echo 0x17206648 > $MEM_DUMP_PATH/register_config
    echo 0x17206648 > $MEM_DUMP_PATH/register_config
    echo 0x17206650 > $MEM_DUMP_PATH/register_config
    echo 0x17206650 > $MEM_DUMP_PATH/register_config
    echo 0x17206658 > $MEM_DUMP_PATH/register_config
    echo 0x17206658 > $MEM_DUMP_PATH/register_config
    echo 0x17206660 > $MEM_DUMP_PATH/register_config
    echo 0x17206660 > $MEM_DUMP_PATH/register_config
    echo 0x17206668 > $MEM_DUMP_PATH/register_config
    echo 0x17206668 > $MEM_DUMP_PATH/register_config
    echo 0x17206670 > $MEM_DUMP_PATH/register_config
    echo 0x17206670 > $MEM_DUMP_PATH/register_config
    echo 0x17206678 > $MEM_DUMP_PATH/register_config
    echo 0x17206678 > $MEM_DUMP_PATH/register_config
    echo 0x17206680 > $MEM_DUMP_PATH/register_config
    echo 0x17206680 > $MEM_DUMP_PATH/register_config
    echo 0x17206688 > $MEM_DUMP_PATH/register_config
    echo 0x17206688 > $MEM_DUMP_PATH/register_config
    echo 0x17206690 > $MEM_DUMP_PATH/register_config
    echo 0x17206690 > $MEM_DUMP_PATH/register_config
    echo 0x17206698 > $MEM_DUMP_PATH/register_config
    echo 0x17206698 > $MEM_DUMP_PATH/register_config
    echo 0x172066a0 > $MEM_DUMP_PATH/register_config
    echo 0x172066a0 > $MEM_DUMP_PATH/register_config
    echo 0x172066a8 > $MEM_DUMP_PATH/register_config
    echo 0x172066a8 > $MEM_DUMP_PATH/register_config
    echo 0x172066b0 > $MEM_DUMP_PATH/register_config
    echo 0x172066b0 > $MEM_DUMP_PATH/register_config
    echo 0x172066b8 > $MEM_DUMP_PATH/register_config
    echo 0x172066b8 > $MEM_DUMP_PATH/register_config
    echo 0x172066c0 > $MEM_DUMP_PATH/register_config
    echo 0x172066c0 > $MEM_DUMP_PATH/register_config
    echo 0x172066c8 > $MEM_DUMP_PATH/register_config
    echo 0x172066c8 > $MEM_DUMP_PATH/register_config
    echo 0x172066d0 > $MEM_DUMP_PATH/register_config
    echo 0x172066d0 > $MEM_DUMP_PATH/register_config
    echo 0x172066d8 > $MEM_DUMP_PATH/register_config
    echo 0x172066d8 > $MEM_DUMP_PATH/register_config
    echo 0x172066e0 > $MEM_DUMP_PATH/register_config
    echo 0x172066e0 > $MEM_DUMP_PATH/register_config
    echo 0x172066e8 > $MEM_DUMP_PATH/register_config
    echo 0x172066e8 > $MEM_DUMP_PATH/register_config
    echo 0x172066f0 > $MEM_DUMP_PATH/register_config
    echo 0x172066f0 > $MEM_DUMP_PATH/register_config
    echo 0x172066f8 > $MEM_DUMP_PATH/register_config
    echo 0x172066f8 > $MEM_DUMP_PATH/register_config
    echo 0x17206700 > $MEM_DUMP_PATH/register_config
    echo 0x17206700 > $MEM_DUMP_PATH/register_config
    echo 0x17206708 > $MEM_DUMP_PATH/register_config
    echo 0x17206708 > $MEM_DUMP_PATH/register_config
    echo 0x17206710 > $MEM_DUMP_PATH/register_config
    echo 0x17206710 > $MEM_DUMP_PATH/register_config
    echo 0x17206718 > $MEM_DUMP_PATH/register_config
    echo 0x17206718 > $MEM_DUMP_PATH/register_config
    echo 0x17206720 > $MEM_DUMP_PATH/register_config
    echo 0x17206720 > $MEM_DUMP_PATH/register_config
    echo 0x17206728 > $MEM_DUMP_PATH/register_config
    echo 0x17206728 > $MEM_DUMP_PATH/register_config
    echo 0x17206730 > $MEM_DUMP_PATH/register_config
    echo 0x17206730 > $MEM_DUMP_PATH/register_config
    echo 0x17206738 > $MEM_DUMP_PATH/register_config
    echo 0x17206738 > $MEM_DUMP_PATH/register_config
    echo 0x17206740 > $MEM_DUMP_PATH/register_config
    echo 0x17206740 > $MEM_DUMP_PATH/register_config
    echo 0x17206748 > $MEM_DUMP_PATH/register_config
    echo 0x17206748 > $MEM_DUMP_PATH/register_config
    echo 0x17206750 > $MEM_DUMP_PATH/register_config
    echo 0x17206750 > $MEM_DUMP_PATH/register_config
    echo 0x17206758 > $MEM_DUMP_PATH/register_config
    echo 0x17206758 > $MEM_DUMP_PATH/register_config
    echo 0x17206760 > $MEM_DUMP_PATH/register_config
    echo 0x17206760 > $MEM_DUMP_PATH/register_config
    echo 0x17206768 > $MEM_DUMP_PATH/register_config
    echo 0x17206768 > $MEM_DUMP_PATH/register_config
    echo 0x17206770 > $MEM_DUMP_PATH/register_config
    echo 0x17206770 > $MEM_DUMP_PATH/register_config
    echo 0x17206778 > $MEM_DUMP_PATH/register_config
    echo 0x17206778 > $MEM_DUMP_PATH/register_config
    echo 0x17206780 > $MEM_DUMP_PATH/register_config
    echo 0x17206780 > $MEM_DUMP_PATH/register_config
    echo 0x17206788 > $MEM_DUMP_PATH/register_config
    echo 0x17206788 > $MEM_DUMP_PATH/register_config
    echo 0x17206790 > $MEM_DUMP_PATH/register_config
    echo 0x17206790 > $MEM_DUMP_PATH/register_config
    echo 0x17206798 > $MEM_DUMP_PATH/register_config
    echo 0x17206798 > $MEM_DUMP_PATH/register_config
    echo 0x172067a0 > $MEM_DUMP_PATH/register_config
    echo 0x172067a0 > $MEM_DUMP_PATH/register_config
    echo 0x172067a8 > $MEM_DUMP_PATH/register_config
    echo 0x172067a8 > $MEM_DUMP_PATH/register_config
    echo 0x172067b0 > $MEM_DUMP_PATH/register_config
    echo 0x172067b0 > $MEM_DUMP_PATH/register_config
    echo 0x172067b8 > $MEM_DUMP_PATH/register_config
    echo 0x172067b8 > $MEM_DUMP_PATH/register_config
    echo 0x172067c0 > $MEM_DUMP_PATH/register_config
    echo 0x172067c0 > $MEM_DUMP_PATH/register_config
    echo 0x172067c8 > $MEM_DUMP_PATH/register_config
    echo 0x172067c8 > $MEM_DUMP_PATH/register_config
    echo 0x172067d0 > $MEM_DUMP_PATH/register_config
    echo 0x172067d0 > $MEM_DUMP_PATH/register_config
    echo 0x172067d8 > $MEM_DUMP_PATH/register_config
    echo 0x172067d8 > $MEM_DUMP_PATH/register_config
    echo 0x172067e0 > $MEM_DUMP_PATH/register_config
    echo 0x172067e0 > $MEM_DUMP_PATH/register_config
    echo 0x172067e8 > $MEM_DUMP_PATH/register_config
    echo 0x172067e8 > $MEM_DUMP_PATH/register_config
    echo 0x172067f0 > $MEM_DUMP_PATH/register_config
    echo 0x172067f0 > $MEM_DUMP_PATH/register_config
    echo 0x172067f8 > $MEM_DUMP_PATH/register_config
    echo 0x172067f8 > $MEM_DUMP_PATH/register_config
    echo 0x17206800 > $MEM_DUMP_PATH/register_config
    echo 0x17206800 > $MEM_DUMP_PATH/register_config
    echo 0x17206808 > $MEM_DUMP_PATH/register_config
    echo 0x17206808 > $MEM_DUMP_PATH/register_config
    echo 0x17206810 > $MEM_DUMP_PATH/register_config
    echo 0x17206810 > $MEM_DUMP_PATH/register_config
    echo 0x17206818 > $MEM_DUMP_PATH/register_config
    echo 0x17206818 > $MEM_DUMP_PATH/register_config
    echo 0x17206820 > $MEM_DUMP_PATH/register_config
    echo 0x17206820 > $MEM_DUMP_PATH/register_config
    echo 0x17206828 > $MEM_DUMP_PATH/register_config
    echo 0x17206828 > $MEM_DUMP_PATH/register_config
    echo 0x17206830 > $MEM_DUMP_PATH/register_config
    echo 0x17206830 > $MEM_DUMP_PATH/register_config
    echo 0x17206838 > $MEM_DUMP_PATH/register_config
    echo 0x17206838 > $MEM_DUMP_PATH/register_config
    echo 0x17206840 > $MEM_DUMP_PATH/register_config
    echo 0x17206840 > $MEM_DUMP_PATH/register_config
    echo 0x17206848 > $MEM_DUMP_PATH/register_config
    echo 0x17206848 > $MEM_DUMP_PATH/register_config
    echo 0x17206850 > $MEM_DUMP_PATH/register_config
    echo 0x17206850 > $MEM_DUMP_PATH/register_config
    echo 0x17206858 > $MEM_DUMP_PATH/register_config
    echo 0x17206858 > $MEM_DUMP_PATH/register_config
    echo 0x17206860 > $MEM_DUMP_PATH/register_config
    echo 0x17206860 > $MEM_DUMP_PATH/register_config
    echo 0x17206868 > $MEM_DUMP_PATH/register_config
    echo 0x17206868 > $MEM_DUMP_PATH/register_config
    echo 0x17206870 > $MEM_DUMP_PATH/register_config
    echo 0x17206870 > $MEM_DUMP_PATH/register_config
    echo 0x17206878 > $MEM_DUMP_PATH/register_config
    echo 0x17206878 > $MEM_DUMP_PATH/register_config
    echo 0x17206880 > $MEM_DUMP_PATH/register_config
    echo 0x17206880 > $MEM_DUMP_PATH/register_config
    echo 0x17206888 > $MEM_DUMP_PATH/register_config
    echo 0x17206888 > $MEM_DUMP_PATH/register_config
    echo 0x17206890 > $MEM_DUMP_PATH/register_config
    echo 0x17206890 > $MEM_DUMP_PATH/register_config
    echo 0x17206898 > $MEM_DUMP_PATH/register_config
    echo 0x17206898 > $MEM_DUMP_PATH/register_config
    echo 0x172068a0 > $MEM_DUMP_PATH/register_config
    echo 0x172068a0 > $MEM_DUMP_PATH/register_config
    echo 0x172068a8 > $MEM_DUMP_PATH/register_config
    echo 0x172068a8 > $MEM_DUMP_PATH/register_config
    echo 0x172068b0 > $MEM_DUMP_PATH/register_config
    echo 0x172068b0 > $MEM_DUMP_PATH/register_config
    echo 0x172068b8 > $MEM_DUMP_PATH/register_config
    echo 0x172068b8 > $MEM_DUMP_PATH/register_config
    echo 0x172068c0 > $MEM_DUMP_PATH/register_config
    echo 0x172068c0 > $MEM_DUMP_PATH/register_config
    echo 0x172068c8 > $MEM_DUMP_PATH/register_config
    echo 0x172068c8 > $MEM_DUMP_PATH/register_config
    echo 0x172068d0 > $MEM_DUMP_PATH/register_config
    echo 0x172068d0 > $MEM_DUMP_PATH/register_config
    echo 0x172068d8 > $MEM_DUMP_PATH/register_config
    echo 0x172068d8 > $MEM_DUMP_PATH/register_config
    echo 0x172068e0 > $MEM_DUMP_PATH/register_config
    echo 0x172068e0 > $MEM_DUMP_PATH/register_config
    echo 0x172068e8 > $MEM_DUMP_PATH/register_config
    echo 0x172068e8 > $MEM_DUMP_PATH/register_config
    echo 0x172068f0 > $MEM_DUMP_PATH/register_config
    echo 0x172068f0 > $MEM_DUMP_PATH/register_config
    echo 0x172068f8 > $MEM_DUMP_PATH/register_config
    echo 0x172068f8 > $MEM_DUMP_PATH/register_config
    echo 0x17206900 > $MEM_DUMP_PATH/register_config
    echo 0x17206900 > $MEM_DUMP_PATH/register_config
    echo 0x17206908 > $MEM_DUMP_PATH/register_config
    echo 0x17206908 > $MEM_DUMP_PATH/register_config
    echo 0x17206910 > $MEM_DUMP_PATH/register_config
    echo 0x17206910 > $MEM_DUMP_PATH/register_config
    echo 0x17206918 > $MEM_DUMP_PATH/register_config
    echo 0x17206918 > $MEM_DUMP_PATH/register_config
    echo 0x17206920 > $MEM_DUMP_PATH/register_config
    echo 0x17206920 > $MEM_DUMP_PATH/register_config
    echo 0x17206928 > $MEM_DUMP_PATH/register_config
    echo 0x17206928 > $MEM_DUMP_PATH/register_config
    echo 0x17206930 > $MEM_DUMP_PATH/register_config
    echo 0x17206930 > $MEM_DUMP_PATH/register_config
    echo 0x17206938 > $MEM_DUMP_PATH/register_config
    echo 0x17206938 > $MEM_DUMP_PATH/register_config
    echo 0x17206940 > $MEM_DUMP_PATH/register_config
    echo 0x17206940 > $MEM_DUMP_PATH/register_config
    echo 0x17206948 > $MEM_DUMP_PATH/register_config
    echo 0x17206948 > $MEM_DUMP_PATH/register_config
    echo 0x17206950 > $MEM_DUMP_PATH/register_config
    echo 0x17206950 > $MEM_DUMP_PATH/register_config
    echo 0x17206958 > $MEM_DUMP_PATH/register_config
    echo 0x17206958 > $MEM_DUMP_PATH/register_config
    echo 0x17206960 > $MEM_DUMP_PATH/register_config
    echo 0x17206960 > $MEM_DUMP_PATH/register_config
    echo 0x17206968 > $MEM_DUMP_PATH/register_config
    echo 0x17206968 > $MEM_DUMP_PATH/register_config
    echo 0x17206970 > $MEM_DUMP_PATH/register_config
    echo 0x17206970 > $MEM_DUMP_PATH/register_config
    echo 0x17206978 > $MEM_DUMP_PATH/register_config
    echo 0x17206978 > $MEM_DUMP_PATH/register_config
    echo 0x17206980 > $MEM_DUMP_PATH/register_config
    echo 0x17206980 > $MEM_DUMP_PATH/register_config
    echo 0x17206988 > $MEM_DUMP_PATH/register_config
    echo 0x17206988 > $MEM_DUMP_PATH/register_config
    echo 0x17206990 > $MEM_DUMP_PATH/register_config
    echo 0x17206990 > $MEM_DUMP_PATH/register_config
    echo 0x17206998 > $MEM_DUMP_PATH/register_config
    echo 0x17206998 > $MEM_DUMP_PATH/register_config
    echo 0x172069a0 > $MEM_DUMP_PATH/register_config
    echo 0x172069a0 > $MEM_DUMP_PATH/register_config
    echo 0x172069a8 > $MEM_DUMP_PATH/register_config
    echo 0x172069a8 > $MEM_DUMP_PATH/register_config
    echo 0x172069b0 > $MEM_DUMP_PATH/register_config
    echo 0x172069b0 > $MEM_DUMP_PATH/register_config
    echo 0x172069b8 > $MEM_DUMP_PATH/register_config
    echo 0x172069b8 > $MEM_DUMP_PATH/register_config
    echo 0x172069c0 > $MEM_DUMP_PATH/register_config
    echo 0x172069c0 > $MEM_DUMP_PATH/register_config
    echo 0x172069c8 > $MEM_DUMP_PATH/register_config
    echo 0x172069c8 > $MEM_DUMP_PATH/register_config
    echo 0x172069d0 > $MEM_DUMP_PATH/register_config
    echo 0x172069d0 > $MEM_DUMP_PATH/register_config
    echo 0x172069d8 > $MEM_DUMP_PATH/register_config
    echo 0x172069d8 > $MEM_DUMP_PATH/register_config
    echo 0x172069e0 > $MEM_DUMP_PATH/register_config
    echo 0x172069e0 > $MEM_DUMP_PATH/register_config
    echo 0x172069e8 > $MEM_DUMP_PATH/register_config
    echo 0x172069e8 > $MEM_DUMP_PATH/register_config
    echo 0x172069f0 > $MEM_DUMP_PATH/register_config
    echo 0x172069f0 > $MEM_DUMP_PATH/register_config
    echo 0x172069f8 > $MEM_DUMP_PATH/register_config
    echo 0x172069f8 > $MEM_DUMP_PATH/register_config
    echo 0x17206a00 > $MEM_DUMP_PATH/register_config
    echo 0x17206a00 > $MEM_DUMP_PATH/register_config
    echo 0x17206a08 > $MEM_DUMP_PATH/register_config
    echo 0x17206a08 > $MEM_DUMP_PATH/register_config
    echo 0x17206a10 > $MEM_DUMP_PATH/register_config
    echo 0x17206a10 > $MEM_DUMP_PATH/register_config
    echo 0x17206a18 > $MEM_DUMP_PATH/register_config
    echo 0x17206a18 > $MEM_DUMP_PATH/register_config
    echo 0x17206a20 > $MEM_DUMP_PATH/register_config
    echo 0x17206a20 > $MEM_DUMP_PATH/register_config
    echo 0x17206a28 > $MEM_DUMP_PATH/register_config
    echo 0x17206a28 > $MEM_DUMP_PATH/register_config
    echo 0x17206a30 > $MEM_DUMP_PATH/register_config
    echo 0x17206a30 > $MEM_DUMP_PATH/register_config
    echo 0x17206a38 > $MEM_DUMP_PATH/register_config
    echo 0x17206a38 > $MEM_DUMP_PATH/register_config
    echo 0x17206a40 > $MEM_DUMP_PATH/register_config
    echo 0x17206a40 > $MEM_DUMP_PATH/register_config
    echo 0x17206a48 > $MEM_DUMP_PATH/register_config
    echo 0x17206a48 > $MEM_DUMP_PATH/register_config
    echo 0x17206a50 > $MEM_DUMP_PATH/register_config
    echo 0x17206a50 > $MEM_DUMP_PATH/register_config
    echo 0x17206a58 > $MEM_DUMP_PATH/register_config
    echo 0x17206a58 > $MEM_DUMP_PATH/register_config
    echo 0x17206a60 > $MEM_DUMP_PATH/register_config
    echo 0x17206a60 > $MEM_DUMP_PATH/register_config
    echo 0x17206a68 > $MEM_DUMP_PATH/register_config
    echo 0x17206a68 > $MEM_DUMP_PATH/register_config
    echo 0x17206a70 > $MEM_DUMP_PATH/register_config
    echo 0x17206a70 > $MEM_DUMP_PATH/register_config
    echo 0x17206a78 > $MEM_DUMP_PATH/register_config
    echo 0x17206a78 > $MEM_DUMP_PATH/register_config
    echo 0x17206a80 > $MEM_DUMP_PATH/register_config
    echo 0x17206a80 > $MEM_DUMP_PATH/register_config
    echo 0x17206a88 > $MEM_DUMP_PATH/register_config
    echo 0x17206a88 > $MEM_DUMP_PATH/register_config
    echo 0x17206a90 > $MEM_DUMP_PATH/register_config
    echo 0x17206a90 > $MEM_DUMP_PATH/register_config
    echo 0x17206a98 > $MEM_DUMP_PATH/register_config
    echo 0x17206a98 > $MEM_DUMP_PATH/register_config
    echo 0x17206aa0 > $MEM_DUMP_PATH/register_config
    echo 0x17206aa0 > $MEM_DUMP_PATH/register_config
    echo 0x17206aa8 > $MEM_DUMP_PATH/register_config
    echo 0x17206aa8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ab0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ab0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ab8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ab8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ac0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ac0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ac8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ac8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ad0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ad0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ad8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ad8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ae0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ae0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ae8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ae8 > $MEM_DUMP_PATH/register_config
    echo 0x17206af0 > $MEM_DUMP_PATH/register_config
    echo 0x17206af0 > $MEM_DUMP_PATH/register_config
    echo 0x17206af8 > $MEM_DUMP_PATH/register_config
    echo 0x17206af8 > $MEM_DUMP_PATH/register_config
    echo 0x17206b00 > $MEM_DUMP_PATH/register_config
    echo 0x17206b00 > $MEM_DUMP_PATH/register_config
    echo 0x17206b08 > $MEM_DUMP_PATH/register_config
    echo 0x17206b08 > $MEM_DUMP_PATH/register_config
    echo 0x17206b10 > $MEM_DUMP_PATH/register_config
    echo 0x17206b10 > $MEM_DUMP_PATH/register_config
    echo 0x17206b18 > $MEM_DUMP_PATH/register_config
    echo 0x17206b18 > $MEM_DUMP_PATH/register_config
    echo 0x17206b20 > $MEM_DUMP_PATH/register_config
    echo 0x17206b20 > $MEM_DUMP_PATH/register_config
    echo 0x17206b28 > $MEM_DUMP_PATH/register_config
    echo 0x17206b28 > $MEM_DUMP_PATH/register_config
    echo 0x17206b30 > $MEM_DUMP_PATH/register_config
    echo 0x17206b30 > $MEM_DUMP_PATH/register_config
    echo 0x17206b38 > $MEM_DUMP_PATH/register_config
    echo 0x17206b38 > $MEM_DUMP_PATH/register_config
    echo 0x17206b40 > $MEM_DUMP_PATH/register_config
    echo 0x17206b40 > $MEM_DUMP_PATH/register_config
    echo 0x17206b48 > $MEM_DUMP_PATH/register_config
    echo 0x17206b48 > $MEM_DUMP_PATH/register_config
    echo 0x17206b50 > $MEM_DUMP_PATH/register_config
    echo 0x17206b50 > $MEM_DUMP_PATH/register_config
    echo 0x17206b58 > $MEM_DUMP_PATH/register_config
    echo 0x17206b58 > $MEM_DUMP_PATH/register_config
    echo 0x17206b60 > $MEM_DUMP_PATH/register_config
    echo 0x17206b60 > $MEM_DUMP_PATH/register_config
    echo 0x17206b68 > $MEM_DUMP_PATH/register_config
    echo 0x17206b68 > $MEM_DUMP_PATH/register_config
    echo 0x17206b70 > $MEM_DUMP_PATH/register_config
    echo 0x17206b70 > $MEM_DUMP_PATH/register_config
    echo 0x17206b78 > $MEM_DUMP_PATH/register_config
    echo 0x17206b78 > $MEM_DUMP_PATH/register_config
    echo 0x17206b80 > $MEM_DUMP_PATH/register_config
    echo 0x17206b80 > $MEM_DUMP_PATH/register_config
    echo 0x17206b88 > $MEM_DUMP_PATH/register_config
    echo 0x17206b88 > $MEM_DUMP_PATH/register_config
    echo 0x17206b90 > $MEM_DUMP_PATH/register_config
    echo 0x17206b90 > $MEM_DUMP_PATH/register_config
    echo 0x17206b98 > $MEM_DUMP_PATH/register_config
    echo 0x17206b98 > $MEM_DUMP_PATH/register_config
    echo 0x17206ba0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ba0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ba8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ba8 > $MEM_DUMP_PATH/register_config
    echo 0x17206bb0 > $MEM_DUMP_PATH/register_config
    echo 0x17206bb0 > $MEM_DUMP_PATH/register_config
    echo 0x17206bb8 > $MEM_DUMP_PATH/register_config
    echo 0x17206bb8 > $MEM_DUMP_PATH/register_config
    echo 0x17206bc0 > $MEM_DUMP_PATH/register_config
    echo 0x17206bc0 > $MEM_DUMP_PATH/register_config
    echo 0x17206bc8 > $MEM_DUMP_PATH/register_config
    echo 0x17206bc8 > $MEM_DUMP_PATH/register_config
    echo 0x17206bd0 > $MEM_DUMP_PATH/register_config
    echo 0x17206bd0 > $MEM_DUMP_PATH/register_config
    echo 0x17206bd8 > $MEM_DUMP_PATH/register_config
    echo 0x17206bd8 > $MEM_DUMP_PATH/register_config
    echo 0x17206be0 > $MEM_DUMP_PATH/register_config
    echo 0x17206be0 > $MEM_DUMP_PATH/register_config
    echo 0x17206be8 > $MEM_DUMP_PATH/register_config
    echo 0x17206be8 > $MEM_DUMP_PATH/register_config
    echo 0x17206bf0 > $MEM_DUMP_PATH/register_config
    echo 0x17206bf0 > $MEM_DUMP_PATH/register_config
    echo 0x17206bf8 > $MEM_DUMP_PATH/register_config
    echo 0x17206bf8 > $MEM_DUMP_PATH/register_config
    echo 0x17206c00 > $MEM_DUMP_PATH/register_config
    echo 0x17206c00 > $MEM_DUMP_PATH/register_config
    echo 0x17206c08 > $MEM_DUMP_PATH/register_config
    echo 0x17206c08 > $MEM_DUMP_PATH/register_config
    echo 0x17206c10 > $MEM_DUMP_PATH/register_config
    echo 0x17206c10 > $MEM_DUMP_PATH/register_config
    echo 0x17206c18 > $MEM_DUMP_PATH/register_config
    echo 0x17206c18 > $MEM_DUMP_PATH/register_config
    echo 0x17206c20 > $MEM_DUMP_PATH/register_config
    echo 0x17206c20 > $MEM_DUMP_PATH/register_config
    echo 0x17206c28 > $MEM_DUMP_PATH/register_config
    echo 0x17206c28 > $MEM_DUMP_PATH/register_config
    echo 0x17206c30 > $MEM_DUMP_PATH/register_config
    echo 0x17206c30 > $MEM_DUMP_PATH/register_config
    echo 0x17206c38 > $MEM_DUMP_PATH/register_config
    echo 0x17206c38 > $MEM_DUMP_PATH/register_config
    echo 0x17206c40 > $MEM_DUMP_PATH/register_config
    echo 0x17206c40 > $MEM_DUMP_PATH/register_config
    echo 0x17206c48 > $MEM_DUMP_PATH/register_config
    echo 0x17206c48 > $MEM_DUMP_PATH/register_config
    echo 0x17206c50 > $MEM_DUMP_PATH/register_config
    echo 0x17206c50 > $MEM_DUMP_PATH/register_config
    echo 0x17206c58 > $MEM_DUMP_PATH/register_config
    echo 0x17206c58 > $MEM_DUMP_PATH/register_config
    echo 0x17206c60 > $MEM_DUMP_PATH/register_config
    echo 0x17206c60 > $MEM_DUMP_PATH/register_config
    echo 0x17206c68 > $MEM_DUMP_PATH/register_config
    echo 0x17206c68 > $MEM_DUMP_PATH/register_config
    echo 0x17206c70 > $MEM_DUMP_PATH/register_config
    echo 0x17206c70 > $MEM_DUMP_PATH/register_config
    echo 0x17206c78 > $MEM_DUMP_PATH/register_config
    echo 0x17206c78 > $MEM_DUMP_PATH/register_config
    echo 0x17206c80 > $MEM_DUMP_PATH/register_config
    echo 0x17206c80 > $MEM_DUMP_PATH/register_config
    echo 0x17206c88 > $MEM_DUMP_PATH/register_config
    echo 0x17206c88 > $MEM_DUMP_PATH/register_config
    echo 0x17206c90 > $MEM_DUMP_PATH/register_config
    echo 0x17206c90 > $MEM_DUMP_PATH/register_config
    echo 0x17206c98 > $MEM_DUMP_PATH/register_config
    echo 0x17206c98 > $MEM_DUMP_PATH/register_config
    echo 0x17206ca0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ca0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ca8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ca8 > $MEM_DUMP_PATH/register_config
    echo 0x17206cb0 > $MEM_DUMP_PATH/register_config
    echo 0x17206cb0 > $MEM_DUMP_PATH/register_config
    echo 0x17206cb8 > $MEM_DUMP_PATH/register_config
    echo 0x17206cb8 > $MEM_DUMP_PATH/register_config
    echo 0x17206cc0 > $MEM_DUMP_PATH/register_config
    echo 0x17206cc0 > $MEM_DUMP_PATH/register_config
    echo 0x17206cc8 > $MEM_DUMP_PATH/register_config
    echo 0x17206cc8 > $MEM_DUMP_PATH/register_config
    echo 0x17206cd0 > $MEM_DUMP_PATH/register_config
    echo 0x17206cd0 > $MEM_DUMP_PATH/register_config
    echo 0x17206cd8 > $MEM_DUMP_PATH/register_config
    echo 0x17206cd8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ce0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ce0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ce8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ce8 > $MEM_DUMP_PATH/register_config
    echo 0x17206cf0 > $MEM_DUMP_PATH/register_config
    echo 0x17206cf0 > $MEM_DUMP_PATH/register_config
    echo 0x17206cf8 > $MEM_DUMP_PATH/register_config
    echo 0x17206cf8 > $MEM_DUMP_PATH/register_config
    echo 0x17206d00 > $MEM_DUMP_PATH/register_config
    echo 0x17206d00 > $MEM_DUMP_PATH/register_config
    echo 0x17206d08 > $MEM_DUMP_PATH/register_config
    echo 0x17206d08 > $MEM_DUMP_PATH/register_config
    echo 0x17206d10 > $MEM_DUMP_PATH/register_config
    echo 0x17206d10 > $MEM_DUMP_PATH/register_config
    echo 0x17206d18 > $MEM_DUMP_PATH/register_config
    echo 0x17206d18 > $MEM_DUMP_PATH/register_config
    echo 0x17206d20 > $MEM_DUMP_PATH/register_config
    echo 0x17206d20 > $MEM_DUMP_PATH/register_config
    echo 0x17206d28 > $MEM_DUMP_PATH/register_config
    echo 0x17206d28 > $MEM_DUMP_PATH/register_config
    echo 0x17206d30 > $MEM_DUMP_PATH/register_config
    echo 0x17206d30 > $MEM_DUMP_PATH/register_config
    echo 0x17206d38 > $MEM_DUMP_PATH/register_config
    echo 0x17206d38 > $MEM_DUMP_PATH/register_config
    echo 0x17206d40 > $MEM_DUMP_PATH/register_config
    echo 0x17206d40 > $MEM_DUMP_PATH/register_config
    echo 0x17206d48 > $MEM_DUMP_PATH/register_config
    echo 0x17206d48 > $MEM_DUMP_PATH/register_config
    echo 0x17206d50 > $MEM_DUMP_PATH/register_config
    echo 0x17206d50 > $MEM_DUMP_PATH/register_config
    echo 0x17206d58 > $MEM_DUMP_PATH/register_config
    echo 0x17206d58 > $MEM_DUMP_PATH/register_config
    echo 0x17206d60 > $MEM_DUMP_PATH/register_config
    echo 0x17206d60 > $MEM_DUMP_PATH/register_config
    echo 0x17206d68 > $MEM_DUMP_PATH/register_config
    echo 0x17206d68 > $MEM_DUMP_PATH/register_config
    echo 0x17206d70 > $MEM_DUMP_PATH/register_config
    echo 0x17206d70 > $MEM_DUMP_PATH/register_config
    echo 0x17206d78 > $MEM_DUMP_PATH/register_config
    echo 0x17206d78 > $MEM_DUMP_PATH/register_config
    echo 0x17206d80 > $MEM_DUMP_PATH/register_config
    echo 0x17206d80 > $MEM_DUMP_PATH/register_config
    echo 0x17206d88 > $MEM_DUMP_PATH/register_config
    echo 0x17206d88 > $MEM_DUMP_PATH/register_config
    echo 0x17206d90 > $MEM_DUMP_PATH/register_config
    echo 0x17206d90 > $MEM_DUMP_PATH/register_config
    echo 0x17206d98 > $MEM_DUMP_PATH/register_config
    echo 0x17206d98 > $MEM_DUMP_PATH/register_config
    echo 0x17206da0 > $MEM_DUMP_PATH/register_config
    echo 0x17206da0 > $MEM_DUMP_PATH/register_config
    echo 0x17206da8 > $MEM_DUMP_PATH/register_config
    echo 0x17206da8 > $MEM_DUMP_PATH/register_config
    echo 0x17206db0 > $MEM_DUMP_PATH/register_config
    echo 0x17206db0 > $MEM_DUMP_PATH/register_config
    echo 0x17206db8 > $MEM_DUMP_PATH/register_config
    echo 0x17206db8 > $MEM_DUMP_PATH/register_config
    echo 0x17206dc0 > $MEM_DUMP_PATH/register_config
    echo 0x17206dc0 > $MEM_DUMP_PATH/register_config
    echo 0x17206dc8 > $MEM_DUMP_PATH/register_config
    echo 0x17206dc8 > $MEM_DUMP_PATH/register_config
    echo 0x17206dd0 > $MEM_DUMP_PATH/register_config
    echo 0x17206dd0 > $MEM_DUMP_PATH/register_config
    echo 0x17206dd8 > $MEM_DUMP_PATH/register_config
    echo 0x17206dd8 > $MEM_DUMP_PATH/register_config
    echo 0x17206de0 > $MEM_DUMP_PATH/register_config
    echo 0x17206de0 > $MEM_DUMP_PATH/register_config
    echo 0x17206de8 > $MEM_DUMP_PATH/register_config
    echo 0x17206de8 > $MEM_DUMP_PATH/register_config
    echo 0x17206df0 > $MEM_DUMP_PATH/register_config
    echo 0x17206df0 > $MEM_DUMP_PATH/register_config
    echo 0x17206df8 > $MEM_DUMP_PATH/register_config
    echo 0x17206df8 > $MEM_DUMP_PATH/register_config
    echo 0x17206e00 > $MEM_DUMP_PATH/register_config
    echo 0x17206e00 > $MEM_DUMP_PATH/register_config
    echo 0x17206e08 > $MEM_DUMP_PATH/register_config
    echo 0x17206e08 > $MEM_DUMP_PATH/register_config
    echo 0x17206e10 > $MEM_DUMP_PATH/register_config
    echo 0x17206e10 > $MEM_DUMP_PATH/register_config
    echo 0x17206e18 > $MEM_DUMP_PATH/register_config
    echo 0x17206e18 > $MEM_DUMP_PATH/register_config
    echo 0x17206e20 > $MEM_DUMP_PATH/register_config
    echo 0x17206e20 > $MEM_DUMP_PATH/register_config
    echo 0x17206e28 > $MEM_DUMP_PATH/register_config
    echo 0x17206e28 > $MEM_DUMP_PATH/register_config
    echo 0x17206e30 > $MEM_DUMP_PATH/register_config
    echo 0x17206e30 > $MEM_DUMP_PATH/register_config
    echo 0x17206e38 > $MEM_DUMP_PATH/register_config
    echo 0x17206e38 > $MEM_DUMP_PATH/register_config
    echo 0x17206e40 > $MEM_DUMP_PATH/register_config
    echo 0x17206e40 > $MEM_DUMP_PATH/register_config
    echo 0x17206e48 > $MEM_DUMP_PATH/register_config
    echo 0x17206e48 > $MEM_DUMP_PATH/register_config
    echo 0x17206e50 > $MEM_DUMP_PATH/register_config
    echo 0x17206e50 > $MEM_DUMP_PATH/register_config
    echo 0x17206e58 > $MEM_DUMP_PATH/register_config
    echo 0x17206e58 > $MEM_DUMP_PATH/register_config
    echo 0x17206e60 > $MEM_DUMP_PATH/register_config
    echo 0x17206e60 > $MEM_DUMP_PATH/register_config
    echo 0x17206e68 > $MEM_DUMP_PATH/register_config
    echo 0x17206e68 > $MEM_DUMP_PATH/register_config
    echo 0x17206e70 > $MEM_DUMP_PATH/register_config
    echo 0x17206e70 > $MEM_DUMP_PATH/register_config
    echo 0x17206e78 > $MEM_DUMP_PATH/register_config
    echo 0x17206e78 > $MEM_DUMP_PATH/register_config
    echo 0x17206e80 > $MEM_DUMP_PATH/register_config
    echo 0x17206e80 > $MEM_DUMP_PATH/register_config
    echo 0x17206e88 > $MEM_DUMP_PATH/register_config
    echo 0x17206e88 > $MEM_DUMP_PATH/register_config
    echo 0x17206e90 > $MEM_DUMP_PATH/register_config
    echo 0x17206e90 > $MEM_DUMP_PATH/register_config
    echo 0x17206e98 > $MEM_DUMP_PATH/register_config
    echo 0x17206e98 > $MEM_DUMP_PATH/register_config
    echo 0x17206ea0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ea0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ea8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ea8 > $MEM_DUMP_PATH/register_config
    echo 0x17206eb0 > $MEM_DUMP_PATH/register_config
    echo 0x17206eb0 > $MEM_DUMP_PATH/register_config
    echo 0x17206eb8 > $MEM_DUMP_PATH/register_config
    echo 0x17206eb8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ec0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ec0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ec8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ec8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ed0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ed0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ed8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ed8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ee0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ee0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ee8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ee8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ef0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ef0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ef8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ef8 > $MEM_DUMP_PATH/register_config
    echo 0x17206f00 > $MEM_DUMP_PATH/register_config
    echo 0x17206f00 > $MEM_DUMP_PATH/register_config
    echo 0x17206f08 > $MEM_DUMP_PATH/register_config
    echo 0x17206f08 > $MEM_DUMP_PATH/register_config
    echo 0x17206f10 > $MEM_DUMP_PATH/register_config
    echo 0x17206f10 > $MEM_DUMP_PATH/register_config
    echo 0x17206f18 > $MEM_DUMP_PATH/register_config
    echo 0x17206f18 > $MEM_DUMP_PATH/register_config
    echo 0x17206f20 > $MEM_DUMP_PATH/register_config
    echo 0x17206f20 > $MEM_DUMP_PATH/register_config
    echo 0x17206f28 > $MEM_DUMP_PATH/register_config
    echo 0x17206f28 > $MEM_DUMP_PATH/register_config
    echo 0x17206f30 > $MEM_DUMP_PATH/register_config
    echo 0x17206f30 > $MEM_DUMP_PATH/register_config
    echo 0x17206f38 > $MEM_DUMP_PATH/register_config
    echo 0x17206f38 > $MEM_DUMP_PATH/register_config
    echo 0x17206f40 > $MEM_DUMP_PATH/register_config
    echo 0x17206f40 > $MEM_DUMP_PATH/register_config
    echo 0x17206f48 > $MEM_DUMP_PATH/register_config
    echo 0x17206f48 > $MEM_DUMP_PATH/register_config
    echo 0x17206f50 > $MEM_DUMP_PATH/register_config
    echo 0x17206f50 > $MEM_DUMP_PATH/register_config
    echo 0x17206f58 > $MEM_DUMP_PATH/register_config
    echo 0x17206f58 > $MEM_DUMP_PATH/register_config
    echo 0x17206f60 > $MEM_DUMP_PATH/register_config
    echo 0x17206f60 > $MEM_DUMP_PATH/register_config
    echo 0x17206f68 > $MEM_DUMP_PATH/register_config
    echo 0x17206f68 > $MEM_DUMP_PATH/register_config
    echo 0x17206f70 > $MEM_DUMP_PATH/register_config
    echo 0x17206f70 > $MEM_DUMP_PATH/register_config
    echo 0x17206f78 > $MEM_DUMP_PATH/register_config
    echo 0x17206f78 > $MEM_DUMP_PATH/register_config
    echo 0x17206f80 > $MEM_DUMP_PATH/register_config
    echo 0x17206f80 > $MEM_DUMP_PATH/register_config
    echo 0x17206f88 > $MEM_DUMP_PATH/register_config
    echo 0x17206f88 > $MEM_DUMP_PATH/register_config
    echo 0x17206f90 > $MEM_DUMP_PATH/register_config
    echo 0x17206f90 > $MEM_DUMP_PATH/register_config
    echo 0x17206f98 > $MEM_DUMP_PATH/register_config
    echo 0x17206f98 > $MEM_DUMP_PATH/register_config
    echo 0x17206fa0 > $MEM_DUMP_PATH/register_config
    echo 0x17206fa0 > $MEM_DUMP_PATH/register_config
    echo 0x17206fa8 > $MEM_DUMP_PATH/register_config
    echo 0x17206fa8 > $MEM_DUMP_PATH/register_config
    echo 0x17206fb0 > $MEM_DUMP_PATH/register_config
    echo 0x17206fb0 > $MEM_DUMP_PATH/register_config
    echo 0x17206fb8 > $MEM_DUMP_PATH/register_config
    echo 0x17206fb8 > $MEM_DUMP_PATH/register_config
    echo 0x17206fc0 > $MEM_DUMP_PATH/register_config
    echo 0x17206fc0 > $MEM_DUMP_PATH/register_config
    echo 0x17206fc8 > $MEM_DUMP_PATH/register_config
    echo 0x17206fc8 > $MEM_DUMP_PATH/register_config
    echo 0x17206fd0 > $MEM_DUMP_PATH/register_config
    echo 0x17206fd0 > $MEM_DUMP_PATH/register_config
    echo 0x17206fd8 > $MEM_DUMP_PATH/register_config
    echo 0x17206fd8 > $MEM_DUMP_PATH/register_config
    echo 0x17206fe0 > $MEM_DUMP_PATH/register_config
    echo 0x17206fe0 > $MEM_DUMP_PATH/register_config
    echo 0x17206fe8 > $MEM_DUMP_PATH/register_config
    echo 0x17206fe8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ff0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ff0 > $MEM_DUMP_PATH/register_config
    echo 0x17206ff8 > $MEM_DUMP_PATH/register_config
    echo 0x17206ff8 > $MEM_DUMP_PATH/register_config
    echo 0x17207000 > $MEM_DUMP_PATH/register_config
    echo 0x17207000 > $MEM_DUMP_PATH/register_config
    echo 0x17207008 > $MEM_DUMP_PATH/register_config
    echo 0x17207008 > $MEM_DUMP_PATH/register_config
    echo 0x17207010 > $MEM_DUMP_PATH/register_config
    echo 0x17207010 > $MEM_DUMP_PATH/register_config
    echo 0x17207018 > $MEM_DUMP_PATH/register_config
    echo 0x17207018 > $MEM_DUMP_PATH/register_config
    echo 0x17207020 > $MEM_DUMP_PATH/register_config
    echo 0x17207020 > $MEM_DUMP_PATH/register_config
    echo 0x17207028 > $MEM_DUMP_PATH/register_config
    echo 0x17207028 > $MEM_DUMP_PATH/register_config
    echo 0x17207030 > $MEM_DUMP_PATH/register_config
    echo 0x17207030 > $MEM_DUMP_PATH/register_config
    echo 0x17207038 > $MEM_DUMP_PATH/register_config
    echo 0x17207038 > $MEM_DUMP_PATH/register_config
    echo 0x17207040 > $MEM_DUMP_PATH/register_config
    echo 0x17207040 > $MEM_DUMP_PATH/register_config
    echo 0x17207048 > $MEM_DUMP_PATH/register_config
    echo 0x17207048 > $MEM_DUMP_PATH/register_config
    echo 0x17207050 > $MEM_DUMP_PATH/register_config
    echo 0x17207050 > $MEM_DUMP_PATH/register_config
    echo 0x17207058 > $MEM_DUMP_PATH/register_config
    echo 0x17207058 > $MEM_DUMP_PATH/register_config
    echo 0x17207060 > $MEM_DUMP_PATH/register_config
    echo 0x17207060 > $MEM_DUMP_PATH/register_config
    echo 0x17207068 > $MEM_DUMP_PATH/register_config
    echo 0x17207068 > $MEM_DUMP_PATH/register_config
    echo 0x17207070 > $MEM_DUMP_PATH/register_config
    echo 0x17207070 > $MEM_DUMP_PATH/register_config
    echo 0x17207078 > $MEM_DUMP_PATH/register_config
    echo 0x17207078 > $MEM_DUMP_PATH/register_config
    echo 0x17207080 > $MEM_DUMP_PATH/register_config
    echo 0x17207080 > $MEM_DUMP_PATH/register_config
    echo 0x17207088 > $MEM_DUMP_PATH/register_config
    echo 0x17207088 > $MEM_DUMP_PATH/register_config
    echo 0x17207090 > $MEM_DUMP_PATH/register_config
    echo 0x17207090 > $MEM_DUMP_PATH/register_config
    echo 0x17207098 > $MEM_DUMP_PATH/register_config
    echo 0x17207098 > $MEM_DUMP_PATH/register_config
    echo 0x172070a0 > $MEM_DUMP_PATH/register_config
    echo 0x172070a0 > $MEM_DUMP_PATH/register_config
    echo 0x172070a8 > $MEM_DUMP_PATH/register_config
    echo 0x172070a8 > $MEM_DUMP_PATH/register_config
    echo 0x172070b0 > $MEM_DUMP_PATH/register_config
    echo 0x172070b0 > $MEM_DUMP_PATH/register_config
    echo 0x172070b8 > $MEM_DUMP_PATH/register_config
    echo 0x172070b8 > $MEM_DUMP_PATH/register_config
    echo 0x172070c0 > $MEM_DUMP_PATH/register_config
    echo 0x172070c0 > $MEM_DUMP_PATH/register_config
    echo 0x172070c8 > $MEM_DUMP_PATH/register_config
    echo 0x172070c8 > $MEM_DUMP_PATH/register_config
    echo 0x172070d0 > $MEM_DUMP_PATH/register_config
    echo 0x172070d0 > $MEM_DUMP_PATH/register_config
    echo 0x172070d8 > $MEM_DUMP_PATH/register_config
    echo 0x172070d8 > $MEM_DUMP_PATH/register_config
    echo 0x172070e0 > $MEM_DUMP_PATH/register_config
    echo 0x172070e0 > $MEM_DUMP_PATH/register_config
    echo 0x172070e8 > $MEM_DUMP_PATH/register_config
    echo 0x172070e8 > $MEM_DUMP_PATH/register_config
    echo 0x172070f0 > $MEM_DUMP_PATH/register_config
    echo 0x172070f0 > $MEM_DUMP_PATH/register_config
    echo 0x172070f8 > $MEM_DUMP_PATH/register_config
    echo 0x172070f8 > $MEM_DUMP_PATH/register_config
    echo 0x17207100 > $MEM_DUMP_PATH/register_config
    echo 0x17207100 > $MEM_DUMP_PATH/register_config
    echo 0x17207108 > $MEM_DUMP_PATH/register_config
    echo 0x17207108 > $MEM_DUMP_PATH/register_config
    echo 0x17207110 > $MEM_DUMP_PATH/register_config
    echo 0x17207110 > $MEM_DUMP_PATH/register_config
    echo 0x17207118 > $MEM_DUMP_PATH/register_config
    echo 0x17207118 > $MEM_DUMP_PATH/register_config
    echo 0x17207120 > $MEM_DUMP_PATH/register_config
    echo 0x17207120 > $MEM_DUMP_PATH/register_config
    echo 0x17207128 > $MEM_DUMP_PATH/register_config
    echo 0x17207128 > $MEM_DUMP_PATH/register_config
    echo 0x17207130 > $MEM_DUMP_PATH/register_config
    echo 0x17207130 > $MEM_DUMP_PATH/register_config
    echo 0x17207138 > $MEM_DUMP_PATH/register_config
    echo 0x17207138 > $MEM_DUMP_PATH/register_config
    echo 0x17207140 > $MEM_DUMP_PATH/register_config
    echo 0x17207140 > $MEM_DUMP_PATH/register_config
    echo 0x17207148 > $MEM_DUMP_PATH/register_config
    echo 0x17207148 > $MEM_DUMP_PATH/register_config
    echo 0x17207150 > $MEM_DUMP_PATH/register_config
    echo 0x17207150 > $MEM_DUMP_PATH/register_config
    echo 0x17207158 > $MEM_DUMP_PATH/register_config
    echo 0x17207158 > $MEM_DUMP_PATH/register_config
    echo 0x17207160 > $MEM_DUMP_PATH/register_config
    echo 0x17207160 > $MEM_DUMP_PATH/register_config
    echo 0x17207168 > $MEM_DUMP_PATH/register_config
    echo 0x17207168 > $MEM_DUMP_PATH/register_config
    echo 0x17207170 > $MEM_DUMP_PATH/register_config
    echo 0x17207170 > $MEM_DUMP_PATH/register_config
    echo 0x17207178 > $MEM_DUMP_PATH/register_config
    echo 0x17207178 > $MEM_DUMP_PATH/register_config
    echo 0x17207180 > $MEM_DUMP_PATH/register_config
    echo 0x17207180 > $MEM_DUMP_PATH/register_config
    echo 0x17207188 > $MEM_DUMP_PATH/register_config
    echo 0x17207188 > $MEM_DUMP_PATH/register_config
    echo 0x17207190 > $MEM_DUMP_PATH/register_config
    echo 0x17207190 > $MEM_DUMP_PATH/register_config
    echo 0x17207198 > $MEM_DUMP_PATH/register_config
    echo 0x17207198 > $MEM_DUMP_PATH/register_config
    echo 0x172071a0 > $MEM_DUMP_PATH/register_config
    echo 0x172071a0 > $MEM_DUMP_PATH/register_config
    echo 0x172071a8 > $MEM_DUMP_PATH/register_config
    echo 0x172071a8 > $MEM_DUMP_PATH/register_config
    echo 0x172071b0 > $MEM_DUMP_PATH/register_config
    echo 0x172071b0 > $MEM_DUMP_PATH/register_config
    echo 0x172071b8 > $MEM_DUMP_PATH/register_config
    echo 0x172071b8 > $MEM_DUMP_PATH/register_config
    echo 0x172071c0 > $MEM_DUMP_PATH/register_config
    echo 0x172071c0 > $MEM_DUMP_PATH/register_config
    echo 0x172071c8 > $MEM_DUMP_PATH/register_config
    echo 0x172071c8 > $MEM_DUMP_PATH/register_config
    echo 0x172071d0 > $MEM_DUMP_PATH/register_config
    echo 0x172071d0 > $MEM_DUMP_PATH/register_config
    echo 0x172071d8 > $MEM_DUMP_PATH/register_config
    echo 0x172071d8 > $MEM_DUMP_PATH/register_config
    echo 0x172071e0 > $MEM_DUMP_PATH/register_config
    echo 0x172071e0 > $MEM_DUMP_PATH/register_config
    echo 0x172071e8 > $MEM_DUMP_PATH/register_config
    echo 0x172071e8 > $MEM_DUMP_PATH/register_config
    echo 0x172071f0 > $MEM_DUMP_PATH/register_config
    echo 0x172071f0 > $MEM_DUMP_PATH/register_config
    echo 0x172071f8 > $MEM_DUMP_PATH/register_config
    echo 0x172071f8 > $MEM_DUMP_PATH/register_config
    echo 0x17207200 > $MEM_DUMP_PATH/register_config
    echo 0x17207200 > $MEM_DUMP_PATH/register_config
    echo 0x17207208 > $MEM_DUMP_PATH/register_config
    echo 0x17207208 > $MEM_DUMP_PATH/register_config
    echo 0x17207210 > $MEM_DUMP_PATH/register_config
    echo 0x17207210 > $MEM_DUMP_PATH/register_config
    echo 0x17207218 > $MEM_DUMP_PATH/register_config
    echo 0x17207218 > $MEM_DUMP_PATH/register_config
    echo 0x17207220 > $MEM_DUMP_PATH/register_config
    echo 0x17207220 > $MEM_DUMP_PATH/register_config
    echo 0x17207228 > $MEM_DUMP_PATH/register_config
    echo 0x17207228 > $MEM_DUMP_PATH/register_config
    echo 0x17207230 > $MEM_DUMP_PATH/register_config
    echo 0x17207230 > $MEM_DUMP_PATH/register_config
    echo 0x17207238 > $MEM_DUMP_PATH/register_config
    echo 0x17207238 > $MEM_DUMP_PATH/register_config
    echo 0x17207240 > $MEM_DUMP_PATH/register_config
    echo 0x17207240 > $MEM_DUMP_PATH/register_config
    echo 0x17207248 > $MEM_DUMP_PATH/register_config
    echo 0x17207248 > $MEM_DUMP_PATH/register_config
    echo 0x17207250 > $MEM_DUMP_PATH/register_config
    echo 0x17207250 > $MEM_DUMP_PATH/register_config
    echo 0x17207258 > $MEM_DUMP_PATH/register_config
    echo 0x17207258 > $MEM_DUMP_PATH/register_config
    echo 0x17207260 > $MEM_DUMP_PATH/register_config
    echo 0x17207260 > $MEM_DUMP_PATH/register_config
    echo 0x17207268 > $MEM_DUMP_PATH/register_config
    echo 0x17207268 > $MEM_DUMP_PATH/register_config
    echo 0x17207270 > $MEM_DUMP_PATH/register_config
    echo 0x17207270 > $MEM_DUMP_PATH/register_config
    echo 0x17207278 > $MEM_DUMP_PATH/register_config
    echo 0x17207278 > $MEM_DUMP_PATH/register_config
    echo 0x17207280 > $MEM_DUMP_PATH/register_config
    echo 0x17207280 > $MEM_DUMP_PATH/register_config
    echo 0x17207288 > $MEM_DUMP_PATH/register_config
    echo 0x17207288 > $MEM_DUMP_PATH/register_config
    echo 0x17207290 > $MEM_DUMP_PATH/register_config
    echo 0x17207290 > $MEM_DUMP_PATH/register_config
    echo 0x17207298 > $MEM_DUMP_PATH/register_config
    echo 0x17207298 > $MEM_DUMP_PATH/register_config
    echo 0x172072a0 > $MEM_DUMP_PATH/register_config
    echo 0x172072a0 > $MEM_DUMP_PATH/register_config
    echo 0x172072a8 > $MEM_DUMP_PATH/register_config
    echo 0x172072a8 > $MEM_DUMP_PATH/register_config
    echo 0x172072b0 > $MEM_DUMP_PATH/register_config
    echo 0x172072b0 > $MEM_DUMP_PATH/register_config
    echo 0x172072b8 > $MEM_DUMP_PATH/register_config
    echo 0x172072b8 > $MEM_DUMP_PATH/register_config
    echo 0x172072c0 > $MEM_DUMP_PATH/register_config
    echo 0x172072c0 > $MEM_DUMP_PATH/register_config
    echo 0x172072c8 > $MEM_DUMP_PATH/register_config
    echo 0x172072c8 > $MEM_DUMP_PATH/register_config
    echo 0x172072d0 > $MEM_DUMP_PATH/register_config
    echo 0x172072d0 > $MEM_DUMP_PATH/register_config
    echo 0x172072d8 > $MEM_DUMP_PATH/register_config
    echo 0x172072d8 > $MEM_DUMP_PATH/register_config
    echo 0x172072e0 > $MEM_DUMP_PATH/register_config
    echo 0x172072e0 > $MEM_DUMP_PATH/register_config
    echo 0x172072e8 > $MEM_DUMP_PATH/register_config
    echo 0x172072e8 > $MEM_DUMP_PATH/register_config
    echo 0x172072f0 > $MEM_DUMP_PATH/register_config
    echo 0x172072f0 > $MEM_DUMP_PATH/register_config
    echo 0x172072f8 > $MEM_DUMP_PATH/register_config
    echo 0x172072f8 > $MEM_DUMP_PATH/register_config
    echo 0x17207300 > $MEM_DUMP_PATH/register_config
    echo 0x17207300 > $MEM_DUMP_PATH/register_config
    echo 0x17207308 > $MEM_DUMP_PATH/register_config
    echo 0x17207308 > $MEM_DUMP_PATH/register_config
    echo 0x17207310 > $MEM_DUMP_PATH/register_config
    echo 0x17207310 > $MEM_DUMP_PATH/register_config
    echo 0x17207318 > $MEM_DUMP_PATH/register_config
    echo 0x17207318 > $MEM_DUMP_PATH/register_config
    echo 0x17207320 > $MEM_DUMP_PATH/register_config
    echo 0x17207320 > $MEM_DUMP_PATH/register_config
    echo 0x17207328 > $MEM_DUMP_PATH/register_config
    echo 0x17207328 > $MEM_DUMP_PATH/register_config
    echo 0x17207330 > $MEM_DUMP_PATH/register_config
    echo 0x17207330 > $MEM_DUMP_PATH/register_config
    echo 0x17207338 > $MEM_DUMP_PATH/register_config
    echo 0x17207338 > $MEM_DUMP_PATH/register_config
    echo 0x17207340 > $MEM_DUMP_PATH/register_config
    echo 0x17207340 > $MEM_DUMP_PATH/register_config
    echo 0x17207348 > $MEM_DUMP_PATH/register_config
    echo 0x17207348 > $MEM_DUMP_PATH/register_config
    echo 0x17207350 > $MEM_DUMP_PATH/register_config
    echo 0x17207350 > $MEM_DUMP_PATH/register_config
    echo 0x17207358 > $MEM_DUMP_PATH/register_config
    echo 0x17207358 > $MEM_DUMP_PATH/register_config
    echo 0x17207360 > $MEM_DUMP_PATH/register_config
    echo 0x17207360 > $MEM_DUMP_PATH/register_config
    echo 0x17207368 > $MEM_DUMP_PATH/register_config
    echo 0x17207368 > $MEM_DUMP_PATH/register_config
    echo 0x17207370 > $MEM_DUMP_PATH/register_config
    echo 0x17207370 > $MEM_DUMP_PATH/register_config
    echo 0x17207378 > $MEM_DUMP_PATH/register_config
    echo 0x17207378 > $MEM_DUMP_PATH/register_config
    echo 0x17207380 > $MEM_DUMP_PATH/register_config
    echo 0x17207380 > $MEM_DUMP_PATH/register_config
    echo 0x17207388 > $MEM_DUMP_PATH/register_config
    echo 0x17207388 > $MEM_DUMP_PATH/register_config
    echo 0x17207390 > $MEM_DUMP_PATH/register_config
    echo 0x17207390 > $MEM_DUMP_PATH/register_config
    echo 0x17207398 > $MEM_DUMP_PATH/register_config
    echo 0x17207398 > $MEM_DUMP_PATH/register_config
    echo 0x172073a0 > $MEM_DUMP_PATH/register_config
    echo 0x172073a0 > $MEM_DUMP_PATH/register_config
    echo 0x172073a8 > $MEM_DUMP_PATH/register_config
    echo 0x172073a8 > $MEM_DUMP_PATH/register_config
    echo 0x172073b0 > $MEM_DUMP_PATH/register_config
    echo 0x172073b0 > $MEM_DUMP_PATH/register_config
    echo 0x172073b8 > $MEM_DUMP_PATH/register_config
    echo 0x172073b8 > $MEM_DUMP_PATH/register_config
    echo 0x172073c0 > $MEM_DUMP_PATH/register_config
    echo 0x172073c0 > $MEM_DUMP_PATH/register_config
    echo 0x172073c8 > $MEM_DUMP_PATH/register_config
    echo 0x172073c8 > $MEM_DUMP_PATH/register_config
    echo 0x172073d0 > $MEM_DUMP_PATH/register_config
    echo 0x172073d0 > $MEM_DUMP_PATH/register_config
    echo 0x172073d8 > $MEM_DUMP_PATH/register_config
    echo 0x172073d8 > $MEM_DUMP_PATH/register_config
    echo 0x172073e0 > $MEM_DUMP_PATH/register_config
    echo 0x172073e0 > $MEM_DUMP_PATH/register_config
    echo 0x172073e8 > $MEM_DUMP_PATH/register_config
    echo 0x172073e8 > $MEM_DUMP_PATH/register_config
    echo 0x172073f0 > $MEM_DUMP_PATH/register_config
    echo 0x172073f0 > $MEM_DUMP_PATH/register_config
    echo 0x172073f8 > $MEM_DUMP_PATH/register_config
    echo 0x172073f8 > $MEM_DUMP_PATH/register_config
    echo 0x17207400 > $MEM_DUMP_PATH/register_config
    echo 0x17207400 > $MEM_DUMP_PATH/register_config
    echo 0x17207408 > $MEM_DUMP_PATH/register_config
    echo 0x17207408 > $MEM_DUMP_PATH/register_config
    echo 0x17207410 > $MEM_DUMP_PATH/register_config
    echo 0x17207410 > $MEM_DUMP_PATH/register_config
    echo 0x17207418 > $MEM_DUMP_PATH/register_config
    echo 0x17207418 > $MEM_DUMP_PATH/register_config
    echo 0x17207420 > $MEM_DUMP_PATH/register_config
    echo 0x17207420 > $MEM_DUMP_PATH/register_config
    echo 0x17207428 > $MEM_DUMP_PATH/register_config
    echo 0x17207428 > $MEM_DUMP_PATH/register_config
    echo 0x17207430 > $MEM_DUMP_PATH/register_config
    echo 0x17207430 > $MEM_DUMP_PATH/register_config
    echo 0x17207438 > $MEM_DUMP_PATH/register_config
    echo 0x17207438 > $MEM_DUMP_PATH/register_config
    echo 0x17207440 > $MEM_DUMP_PATH/register_config
    echo 0x17207440 > $MEM_DUMP_PATH/register_config
    echo 0x17207448 > $MEM_DUMP_PATH/register_config
    echo 0x17207448 > $MEM_DUMP_PATH/register_config
    echo 0x17207450 > $MEM_DUMP_PATH/register_config
    echo 0x17207450 > $MEM_DUMP_PATH/register_config
    echo 0x17207458 > $MEM_DUMP_PATH/register_config
    echo 0x17207458 > $MEM_DUMP_PATH/register_config
    echo 0x17207460 > $MEM_DUMP_PATH/register_config
    echo 0x17207460 > $MEM_DUMP_PATH/register_config
    echo 0x17207468 > $MEM_DUMP_PATH/register_config
    echo 0x17207468 > $MEM_DUMP_PATH/register_config
    echo 0x17207470 > $MEM_DUMP_PATH/register_config
    echo 0x17207470 > $MEM_DUMP_PATH/register_config
    echo 0x17207478 > $MEM_DUMP_PATH/register_config
    echo 0x17207478 > $MEM_DUMP_PATH/register_config
    echo 0x17207480 > $MEM_DUMP_PATH/register_config
    echo 0x17207480 > $MEM_DUMP_PATH/register_config
    echo 0x17207488 > $MEM_DUMP_PATH/register_config
    echo 0x17207488 > $MEM_DUMP_PATH/register_config
    echo 0x17207490 > $MEM_DUMP_PATH/register_config
    echo 0x17207490 > $MEM_DUMP_PATH/register_config
    echo 0x17207498 > $MEM_DUMP_PATH/register_config
    echo 0x17207498 > $MEM_DUMP_PATH/register_config
    echo 0x172074a0 > $MEM_DUMP_PATH/register_config
    echo 0x172074a0 > $MEM_DUMP_PATH/register_config
    echo 0x172074a8 > $MEM_DUMP_PATH/register_config
    echo 0x172074a8 > $MEM_DUMP_PATH/register_config
    echo 0x172074b0 > $MEM_DUMP_PATH/register_config
    echo 0x172074b0 > $MEM_DUMP_PATH/register_config
    echo 0x172074b8 > $MEM_DUMP_PATH/register_config
    echo 0x172074b8 > $MEM_DUMP_PATH/register_config
    echo 0x172074c0 > $MEM_DUMP_PATH/register_config
    echo 0x172074c0 > $MEM_DUMP_PATH/register_config
    echo 0x172074c8 > $MEM_DUMP_PATH/register_config
    echo 0x172074c8 > $MEM_DUMP_PATH/register_config
    echo 0x172074d0 > $MEM_DUMP_PATH/register_config
    echo 0x172074d0 > $MEM_DUMP_PATH/register_config
    echo 0x172074d8 > $MEM_DUMP_PATH/register_config
    echo 0x172074d8 > $MEM_DUMP_PATH/register_config
    echo 0x172074e0 > $MEM_DUMP_PATH/register_config
    echo 0x172074e0 > $MEM_DUMP_PATH/register_config
    echo 0x172074e8 > $MEM_DUMP_PATH/register_config
    echo 0x172074e8 > $MEM_DUMP_PATH/register_config
    echo 0x172074f0 > $MEM_DUMP_PATH/register_config
    echo 0x172074f0 > $MEM_DUMP_PATH/register_config
    echo 0x172074f8 > $MEM_DUMP_PATH/register_config
    echo 0x172074f8 > $MEM_DUMP_PATH/register_config
    echo 0x17207500 > $MEM_DUMP_PATH/register_config
    echo 0x17207500 > $MEM_DUMP_PATH/register_config
    echo 0x17207508 > $MEM_DUMP_PATH/register_config
    echo 0x17207508 > $MEM_DUMP_PATH/register_config
    echo 0x17207510 > $MEM_DUMP_PATH/register_config
    echo 0x17207510 > $MEM_DUMP_PATH/register_config
    echo 0x17207518 > $MEM_DUMP_PATH/register_config
    echo 0x17207518 > $MEM_DUMP_PATH/register_config
    echo 0x17207520 > $MEM_DUMP_PATH/register_config
    echo 0x17207520 > $MEM_DUMP_PATH/register_config
    echo 0x17207528 > $MEM_DUMP_PATH/register_config
    echo 0x17207528 > $MEM_DUMP_PATH/register_config
    echo 0x17207530 > $MEM_DUMP_PATH/register_config
    echo 0x17207530 > $MEM_DUMP_PATH/register_config
    echo 0x17207538 > $MEM_DUMP_PATH/register_config
    echo 0x17207538 > $MEM_DUMP_PATH/register_config
    echo 0x17207540 > $MEM_DUMP_PATH/register_config
    echo 0x17207540 > $MEM_DUMP_PATH/register_config
    echo 0x17207548 > $MEM_DUMP_PATH/register_config
    echo 0x17207548 > $MEM_DUMP_PATH/register_config
    echo 0x17207550 > $MEM_DUMP_PATH/register_config
    echo 0x17207550 > $MEM_DUMP_PATH/register_config
    echo 0x17207558 > $MEM_DUMP_PATH/register_config
    echo 0x17207558 > $MEM_DUMP_PATH/register_config
    echo 0x17207560 > $MEM_DUMP_PATH/register_config
    echo 0x17207560 > $MEM_DUMP_PATH/register_config
    echo 0x17207568 > $MEM_DUMP_PATH/register_config
    echo 0x17207568 > $MEM_DUMP_PATH/register_config
    echo 0x17207570 > $MEM_DUMP_PATH/register_config
    echo 0x17207570 > $MEM_DUMP_PATH/register_config
    echo 0x17207578 > $MEM_DUMP_PATH/register_config
    echo 0x17207578 > $MEM_DUMP_PATH/register_config
    echo 0x17207580 > $MEM_DUMP_PATH/register_config
    echo 0x17207580 > $MEM_DUMP_PATH/register_config
    echo 0x17207588 > $MEM_DUMP_PATH/register_config
    echo 0x17207588 > $MEM_DUMP_PATH/register_config
    echo 0x17207590 > $MEM_DUMP_PATH/register_config
    echo 0x17207590 > $MEM_DUMP_PATH/register_config
    echo 0x17207598 > $MEM_DUMP_PATH/register_config
    echo 0x17207598 > $MEM_DUMP_PATH/register_config
    echo 0x172075a0 > $MEM_DUMP_PATH/register_config
    echo 0x172075a0 > $MEM_DUMP_PATH/register_config
    echo 0x172075a8 > $MEM_DUMP_PATH/register_config
    echo 0x172075a8 > $MEM_DUMP_PATH/register_config
    echo 0x172075b0 > $MEM_DUMP_PATH/register_config
    echo 0x172075b0 > $MEM_DUMP_PATH/register_config
    echo 0x172075b8 > $MEM_DUMP_PATH/register_config
    echo 0x172075b8 > $MEM_DUMP_PATH/register_config
    echo 0x172075c0 > $MEM_DUMP_PATH/register_config
    echo 0x172075c0 > $MEM_DUMP_PATH/register_config
    echo 0x172075c8 > $MEM_DUMP_PATH/register_config
    echo 0x172075c8 > $MEM_DUMP_PATH/register_config
    echo 0x172075d0 > $MEM_DUMP_PATH/register_config
    echo 0x172075d0 > $MEM_DUMP_PATH/register_config
    echo 0x172075d8 > $MEM_DUMP_PATH/register_config
    echo 0x172075d8 > $MEM_DUMP_PATH/register_config
    echo 0x172075e0 > $MEM_DUMP_PATH/register_config
    echo 0x172075e0 > $MEM_DUMP_PATH/register_config
    echo 0x172075e8 > $MEM_DUMP_PATH/register_config
    echo 0x172075e8 > $MEM_DUMP_PATH/register_config
    echo 0x172075f0 > $MEM_DUMP_PATH/register_config
    echo 0x172075f0 > $MEM_DUMP_PATH/register_config
    echo 0x172075f8 > $MEM_DUMP_PATH/register_config
    echo 0x172075f8 > $MEM_DUMP_PATH/register_config
    echo 0x17207600 > $MEM_DUMP_PATH/register_config
    echo 0x17207600 > $MEM_DUMP_PATH/register_config
    echo 0x17207608 > $MEM_DUMP_PATH/register_config
    echo 0x17207608 > $MEM_DUMP_PATH/register_config
    echo 0x17207610 > $MEM_DUMP_PATH/register_config
    echo 0x17207610 > $MEM_DUMP_PATH/register_config
    echo 0x17207618 > $MEM_DUMP_PATH/register_config
    echo 0x17207618 > $MEM_DUMP_PATH/register_config
    echo 0x17207620 > $MEM_DUMP_PATH/register_config
    echo 0x17207620 > $MEM_DUMP_PATH/register_config
    echo 0x17207628 > $MEM_DUMP_PATH/register_config
    echo 0x17207628 > $MEM_DUMP_PATH/register_config
    echo 0x17207630 > $MEM_DUMP_PATH/register_config
    echo 0x17207630 > $MEM_DUMP_PATH/register_config
    echo 0x17207638 > $MEM_DUMP_PATH/register_config
    echo 0x17207638 > $MEM_DUMP_PATH/register_config
    echo 0x17207640 > $MEM_DUMP_PATH/register_config
    echo 0x17207640 > $MEM_DUMP_PATH/register_config
    echo 0x17207648 > $MEM_DUMP_PATH/register_config
    echo 0x17207648 > $MEM_DUMP_PATH/register_config
    echo 0x17207650 > $MEM_DUMP_PATH/register_config
    echo 0x17207650 > $MEM_DUMP_PATH/register_config
    echo 0x17207658 > $MEM_DUMP_PATH/register_config
    echo 0x17207658 > $MEM_DUMP_PATH/register_config
    echo 0x17207660 > $MEM_DUMP_PATH/register_config
    echo 0x17207660 > $MEM_DUMP_PATH/register_config
    echo 0x17207668 > $MEM_DUMP_PATH/register_config
    echo 0x17207668 > $MEM_DUMP_PATH/register_config
    echo 0x17207670 > $MEM_DUMP_PATH/register_config
    echo 0x17207670 > $MEM_DUMP_PATH/register_config
    echo 0x17207678 > $MEM_DUMP_PATH/register_config
    echo 0x17207678 > $MEM_DUMP_PATH/register_config
    echo 0x17207680 > $MEM_DUMP_PATH/register_config
    echo 0x17207680 > $MEM_DUMP_PATH/register_config
    echo 0x17207688 > $MEM_DUMP_PATH/register_config
    echo 0x17207688 > $MEM_DUMP_PATH/register_config
    echo 0x17207690 > $MEM_DUMP_PATH/register_config
    echo 0x17207690 > $MEM_DUMP_PATH/register_config
    echo 0x17207698 > $MEM_DUMP_PATH/register_config
    echo 0x17207698 > $MEM_DUMP_PATH/register_config
    echo 0x172076a0 > $MEM_DUMP_PATH/register_config
    echo 0x172076a0 > $MEM_DUMP_PATH/register_config
    echo 0x172076a8 > $MEM_DUMP_PATH/register_config
    echo 0x172076a8 > $MEM_DUMP_PATH/register_config
    echo 0x172076b0 > $MEM_DUMP_PATH/register_config
    echo 0x172076b0 > $MEM_DUMP_PATH/register_config
    echo 0x172076b8 > $MEM_DUMP_PATH/register_config
    echo 0x172076b8 > $MEM_DUMP_PATH/register_config
    echo 0x172076c0 > $MEM_DUMP_PATH/register_config
    echo 0x172076c0 > $MEM_DUMP_PATH/register_config
    echo 0x172076c8 > $MEM_DUMP_PATH/register_config
    echo 0x172076c8 > $MEM_DUMP_PATH/register_config
    echo 0x172076d0 > $MEM_DUMP_PATH/register_config
    echo 0x172076d0 > $MEM_DUMP_PATH/register_config
    echo 0x172076d8 > $MEM_DUMP_PATH/register_config
    echo 0x172076d8 > $MEM_DUMP_PATH/register_config
    echo 0x172076e0 > $MEM_DUMP_PATH/register_config
    echo 0x172076e0 > $MEM_DUMP_PATH/register_config
    echo 0x172076e8 > $MEM_DUMP_PATH/register_config
    echo 0x172076e8 > $MEM_DUMP_PATH/register_config
    echo 0x172076f0 > $MEM_DUMP_PATH/register_config
    echo 0x172076f0 > $MEM_DUMP_PATH/register_config
    echo 0x172076f8 > $MEM_DUMP_PATH/register_config
    echo 0x172076f8 > $MEM_DUMP_PATH/register_config
    echo 0x17207700 > $MEM_DUMP_PATH/register_config
    echo 0x17207700 > $MEM_DUMP_PATH/register_config
    echo 0x17207708 > $MEM_DUMP_PATH/register_config
    echo 0x17207708 > $MEM_DUMP_PATH/register_config
    echo 0x17207710 > $MEM_DUMP_PATH/register_config
    echo 0x17207710 > $MEM_DUMP_PATH/register_config
    echo 0x17207718 > $MEM_DUMP_PATH/register_config
    echo 0x17207718 > $MEM_DUMP_PATH/register_config
    echo 0x17207720 > $MEM_DUMP_PATH/register_config
    echo 0x17207720 > $MEM_DUMP_PATH/register_config
    echo 0x17207728 > $MEM_DUMP_PATH/register_config
    echo 0x17207728 > $MEM_DUMP_PATH/register_config
    echo 0x17207730 > $MEM_DUMP_PATH/register_config
    echo 0x17207730 > $MEM_DUMP_PATH/register_config
    echo 0x17207738 > $MEM_DUMP_PATH/register_config
    echo 0x17207738 > $MEM_DUMP_PATH/register_config
    echo 0x17207740 > $MEM_DUMP_PATH/register_config
    echo 0x17207740 > $MEM_DUMP_PATH/register_config
    echo 0x17207748 > $MEM_DUMP_PATH/register_config
    echo 0x17207748 > $MEM_DUMP_PATH/register_config
    echo 0x17207750 > $MEM_DUMP_PATH/register_config
    echo 0x17207750 > $MEM_DUMP_PATH/register_config
    echo 0x17207758 > $MEM_DUMP_PATH/register_config
    echo 0x17207758 > $MEM_DUMP_PATH/register_config
    echo 0x17207760 > $MEM_DUMP_PATH/register_config
    echo 0x17207760 > $MEM_DUMP_PATH/register_config
    echo 0x17207768 > $MEM_DUMP_PATH/register_config
    echo 0x17207768 > $MEM_DUMP_PATH/register_config
    echo 0x17207770 > $MEM_DUMP_PATH/register_config
    echo 0x17207770 > $MEM_DUMP_PATH/register_config
    echo 0x17207778 > $MEM_DUMP_PATH/register_config
    echo 0x17207778 > $MEM_DUMP_PATH/register_config
    echo 0x17207780 > $MEM_DUMP_PATH/register_config
    echo 0x17207780 > $MEM_DUMP_PATH/register_config
    echo 0x17207788 > $MEM_DUMP_PATH/register_config
    echo 0x17207788 > $MEM_DUMP_PATH/register_config
    echo 0x17207790 > $MEM_DUMP_PATH/register_config
    echo 0x17207790 > $MEM_DUMP_PATH/register_config
    echo 0x17207798 > $MEM_DUMP_PATH/register_config
    echo 0x17207798 > $MEM_DUMP_PATH/register_config
    echo 0x172077a0 > $MEM_DUMP_PATH/register_config
    echo 0x172077a0 > $MEM_DUMP_PATH/register_config
    echo 0x172077a8 > $MEM_DUMP_PATH/register_config
    echo 0x172077a8 > $MEM_DUMP_PATH/register_config
    echo 0x172077b0 > $MEM_DUMP_PATH/register_config
    echo 0x172077b0 > $MEM_DUMP_PATH/register_config
    echo 0x172077b8 > $MEM_DUMP_PATH/register_config
    echo 0x172077b8 > $MEM_DUMP_PATH/register_config
    echo 0x172077c0 > $MEM_DUMP_PATH/register_config
    echo 0x172077c0 > $MEM_DUMP_PATH/register_config
    echo 0x172077c8 > $MEM_DUMP_PATH/register_config
    echo 0x172077c8 > $MEM_DUMP_PATH/register_config
    echo 0x172077d0 > $MEM_DUMP_PATH/register_config
    echo 0x172077d0 > $MEM_DUMP_PATH/register_config
    echo 0x172077d8 > $MEM_DUMP_PATH/register_config
    echo 0x172077d8 > $MEM_DUMP_PATH/register_config
    echo 0x172077e0 > $MEM_DUMP_PATH/register_config
    echo 0x172077e0 > $MEM_DUMP_PATH/register_config
    echo 0x172077e8 > $MEM_DUMP_PATH/register_config
    echo 0x172077e8 > $MEM_DUMP_PATH/register_config
    echo 0x172077f0 > $MEM_DUMP_PATH/register_config
    echo 0x172077f0 > $MEM_DUMP_PATH/register_config
    echo 0x172077f8 > $MEM_DUMP_PATH/register_config
    echo 0x172077f8 > $MEM_DUMP_PATH/register_config
    echo 0x17207800 > $MEM_DUMP_PATH/register_config
    echo 0x17207800 > $MEM_DUMP_PATH/register_config
    echo 0x17207808 > $MEM_DUMP_PATH/register_config
    echo 0x17207808 > $MEM_DUMP_PATH/register_config
    echo 0x17207810 > $MEM_DUMP_PATH/register_config
    echo 0x17207810 > $MEM_DUMP_PATH/register_config
    echo 0x17207818 > $MEM_DUMP_PATH/register_config
    echo 0x17207818 > $MEM_DUMP_PATH/register_config
    echo 0x17207820 > $MEM_DUMP_PATH/register_config
    echo 0x17207820 > $MEM_DUMP_PATH/register_config
    echo 0x17207828 > $MEM_DUMP_PATH/register_config
    echo 0x17207828 > $MEM_DUMP_PATH/register_config
    echo 0x17207830 > $MEM_DUMP_PATH/register_config
    echo 0x17207830 > $MEM_DUMP_PATH/register_config
    echo 0x17207838 > $MEM_DUMP_PATH/register_config
    echo 0x17207838 > $MEM_DUMP_PATH/register_config
    echo 0x17207840 > $MEM_DUMP_PATH/register_config
    echo 0x17207840 > $MEM_DUMP_PATH/register_config
    echo 0x17207848 > $MEM_DUMP_PATH/register_config
    echo 0x17207848 > $MEM_DUMP_PATH/register_config
    echo 0x17207850 > $MEM_DUMP_PATH/register_config
    echo 0x17207850 > $MEM_DUMP_PATH/register_config
    echo 0x17207858 > $MEM_DUMP_PATH/register_config
    echo 0x17207858 > $MEM_DUMP_PATH/register_config
    echo 0x17207860 > $MEM_DUMP_PATH/register_config
    echo 0x17207860 > $MEM_DUMP_PATH/register_config
    echo 0x17207868 > $MEM_DUMP_PATH/register_config
    echo 0x17207868 > $MEM_DUMP_PATH/register_config
    echo 0x17207870 > $MEM_DUMP_PATH/register_config
    echo 0x17207870 > $MEM_DUMP_PATH/register_config
    echo 0x17207878 > $MEM_DUMP_PATH/register_config
    echo 0x17207878 > $MEM_DUMP_PATH/register_config
    echo 0x17207880 > $MEM_DUMP_PATH/register_config
    echo 0x17207880 > $MEM_DUMP_PATH/register_config
    echo 0x17207888 > $MEM_DUMP_PATH/register_config
    echo 0x17207888 > $MEM_DUMP_PATH/register_config
    echo 0x17207890 > $MEM_DUMP_PATH/register_config
    echo 0x17207890 > $MEM_DUMP_PATH/register_config
    echo 0x17207898 > $MEM_DUMP_PATH/register_config
    echo 0x17207898 > $MEM_DUMP_PATH/register_config
    echo 0x172078a0 > $MEM_DUMP_PATH/register_config
    echo 0x172078a0 > $MEM_DUMP_PATH/register_config
    echo 0x172078a8 > $MEM_DUMP_PATH/register_config
    echo 0x172078a8 > $MEM_DUMP_PATH/register_config
    echo 0x172078b0 > $MEM_DUMP_PATH/register_config
    echo 0x172078b0 > $MEM_DUMP_PATH/register_config
    echo 0x172078b8 > $MEM_DUMP_PATH/register_config
    echo 0x172078b8 > $MEM_DUMP_PATH/register_config
    echo 0x172078c0 > $MEM_DUMP_PATH/register_config
    echo 0x172078c0 > $MEM_DUMP_PATH/register_config
    echo 0x172078c8 > $MEM_DUMP_PATH/register_config
    echo 0x172078c8 > $MEM_DUMP_PATH/register_config
    echo 0x172078d0 > $MEM_DUMP_PATH/register_config
    echo 0x172078d0 > $MEM_DUMP_PATH/register_config
    echo 0x172078d8 > $MEM_DUMP_PATH/register_config
    echo 0x172078d8 > $MEM_DUMP_PATH/register_config
    echo 0x172078e0 > $MEM_DUMP_PATH/register_config
    echo 0x172078e0 > $MEM_DUMP_PATH/register_config
    echo 0x172078e8 > $MEM_DUMP_PATH/register_config
    echo 0x172078e8 > $MEM_DUMP_PATH/register_config
    echo 0x172078f0 > $MEM_DUMP_PATH/register_config
    echo 0x172078f0 > $MEM_DUMP_PATH/register_config
    echo 0x172078f8 > $MEM_DUMP_PATH/register_config
    echo 0x172078f8 > $MEM_DUMP_PATH/register_config
    echo 0x17207900 > $MEM_DUMP_PATH/register_config
    echo 0x17207900 > $MEM_DUMP_PATH/register_config
    echo 0x17207908 > $MEM_DUMP_PATH/register_config
    echo 0x17207908 > $MEM_DUMP_PATH/register_config
    echo 0x17207910 > $MEM_DUMP_PATH/register_config
    echo 0x17207910 > $MEM_DUMP_PATH/register_config
    echo 0x17207918 > $MEM_DUMP_PATH/register_config
    echo 0x17207918 > $MEM_DUMP_PATH/register_config
    echo 0x17207920 > $MEM_DUMP_PATH/register_config
    echo 0x17207920 > $MEM_DUMP_PATH/register_config
    echo 0x17207928 > $MEM_DUMP_PATH/register_config
    echo 0x17207928 > $MEM_DUMP_PATH/register_config
    echo 0x17207930 > $MEM_DUMP_PATH/register_config
    echo 0x17207930 > $MEM_DUMP_PATH/register_config
    echo 0x17207938 > $MEM_DUMP_PATH/register_config
    echo 0x17207938 > $MEM_DUMP_PATH/register_config
    echo 0x17207940 > $MEM_DUMP_PATH/register_config
    echo 0x17207940 > $MEM_DUMP_PATH/register_config
    echo 0x17207948 > $MEM_DUMP_PATH/register_config
    echo 0x17207948 > $MEM_DUMP_PATH/register_config
    echo 0x17207950 > $MEM_DUMP_PATH/register_config
    echo 0x17207950 > $MEM_DUMP_PATH/register_config
    echo 0x17207958 > $MEM_DUMP_PATH/register_config
    echo 0x17207958 > $MEM_DUMP_PATH/register_config
    echo 0x17207960 > $MEM_DUMP_PATH/register_config
    echo 0x17207960 > $MEM_DUMP_PATH/register_config
    echo 0x17207968 > $MEM_DUMP_PATH/register_config
    echo 0x17207968 > $MEM_DUMP_PATH/register_config
    echo 0x17207970 > $MEM_DUMP_PATH/register_config
    echo 0x17207970 > $MEM_DUMP_PATH/register_config
    echo 0x17207978 > $MEM_DUMP_PATH/register_config
    echo 0x17207978 > $MEM_DUMP_PATH/register_config
    echo 0x17207980 > $MEM_DUMP_PATH/register_config
    echo 0x17207980 > $MEM_DUMP_PATH/register_config
    echo 0x17207988 > $MEM_DUMP_PATH/register_config
    echo 0x17207988 > $MEM_DUMP_PATH/register_config
    echo 0x17207990 > $MEM_DUMP_PATH/register_config
    echo 0x17207990 > $MEM_DUMP_PATH/register_config
    echo 0x17207998 > $MEM_DUMP_PATH/register_config
    echo 0x17207998 > $MEM_DUMP_PATH/register_config
    echo 0x172079a0 > $MEM_DUMP_PATH/register_config
    echo 0x172079a0 > $MEM_DUMP_PATH/register_config
    echo 0x172079a8 > $MEM_DUMP_PATH/register_config
    echo 0x172079a8 > $MEM_DUMP_PATH/register_config
    echo 0x172079b0 > $MEM_DUMP_PATH/register_config
    echo 0x172079b0 > $MEM_DUMP_PATH/register_config
    echo 0x172079b8 > $MEM_DUMP_PATH/register_config
    echo 0x172079b8 > $MEM_DUMP_PATH/register_config
    echo 0x172079c0 > $MEM_DUMP_PATH/register_config
    echo 0x172079c0 > $MEM_DUMP_PATH/register_config
    echo 0x172079c8 > $MEM_DUMP_PATH/register_config
    echo 0x172079c8 > $MEM_DUMP_PATH/register_config
    echo 0x172079d0 > $MEM_DUMP_PATH/register_config
    echo 0x172079d0 > $MEM_DUMP_PATH/register_config
    echo 0x172079d8 > $MEM_DUMP_PATH/register_config
    echo 0x172079d8 > $MEM_DUMP_PATH/register_config
    echo 0x172079e0 > $MEM_DUMP_PATH/register_config
    echo 0x172079e0 > $MEM_DUMP_PATH/register_config
    echo 0x172079e8 > $MEM_DUMP_PATH/register_config
    echo 0x172079e8 > $MEM_DUMP_PATH/register_config
    echo 0x172079f0 > $MEM_DUMP_PATH/register_config
    echo 0x172079f0 > $MEM_DUMP_PATH/register_config
    echo 0x172079f8 > $MEM_DUMP_PATH/register_config
    echo 0x172079f8 > $MEM_DUMP_PATH/register_config
    echo 0x17207a00 > $MEM_DUMP_PATH/register_config
    echo 0x17207a00 > $MEM_DUMP_PATH/register_config
    echo 0x17207a08 > $MEM_DUMP_PATH/register_config
    echo 0x17207a08 > $MEM_DUMP_PATH/register_config
    echo 0x17207a10 > $MEM_DUMP_PATH/register_config
    echo 0x17207a10 > $MEM_DUMP_PATH/register_config
    echo 0x17207a18 > $MEM_DUMP_PATH/register_config
    echo 0x17207a18 > $MEM_DUMP_PATH/register_config
    echo 0x17207a20 > $MEM_DUMP_PATH/register_config
    echo 0x17207a20 > $MEM_DUMP_PATH/register_config
    echo 0x17207a28 > $MEM_DUMP_PATH/register_config
    echo 0x17207a28 > $MEM_DUMP_PATH/register_config
    echo 0x17207a30 > $MEM_DUMP_PATH/register_config
    echo 0x17207a30 > $MEM_DUMP_PATH/register_config
    echo 0x17207a38 > $MEM_DUMP_PATH/register_config
    echo 0x17207a38 > $MEM_DUMP_PATH/register_config
    echo 0x17207a40 > $MEM_DUMP_PATH/register_config
    echo 0x17207a40 > $MEM_DUMP_PATH/register_config
    echo 0x17207a48 > $MEM_DUMP_PATH/register_config
    echo 0x17207a48 > $MEM_DUMP_PATH/register_config
    echo 0x17207a50 > $MEM_DUMP_PATH/register_config
    echo 0x17207a50 > $MEM_DUMP_PATH/register_config
    echo 0x17207a58 > $MEM_DUMP_PATH/register_config
    echo 0x17207a58 > $MEM_DUMP_PATH/register_config
    echo 0x17207a60 > $MEM_DUMP_PATH/register_config
    echo 0x17207a60 > $MEM_DUMP_PATH/register_config
    echo 0x17207a68 > $MEM_DUMP_PATH/register_config
    echo 0x17207a68 > $MEM_DUMP_PATH/register_config
    echo 0x17207a70 > $MEM_DUMP_PATH/register_config
    echo 0x17207a70 > $MEM_DUMP_PATH/register_config
    echo 0x17207a78 > $MEM_DUMP_PATH/register_config
    echo 0x17207a78 > $MEM_DUMP_PATH/register_config
    echo 0x17207a80 > $MEM_DUMP_PATH/register_config
    echo 0x17207a80 > $MEM_DUMP_PATH/register_config
    echo 0x17207a88 > $MEM_DUMP_PATH/register_config
    echo 0x17207a88 > $MEM_DUMP_PATH/register_config
    echo 0x17207a90 > $MEM_DUMP_PATH/register_config
    echo 0x17207a90 > $MEM_DUMP_PATH/register_config
    echo 0x17207a98 > $MEM_DUMP_PATH/register_config
    echo 0x17207a98 > $MEM_DUMP_PATH/register_config
    echo 0x17207aa0 > $MEM_DUMP_PATH/register_config
    echo 0x17207aa0 > $MEM_DUMP_PATH/register_config
    echo 0x17207aa8 > $MEM_DUMP_PATH/register_config
    echo 0x17207aa8 > $MEM_DUMP_PATH/register_config
    echo 0x17207ab0 > $MEM_DUMP_PATH/register_config
    echo 0x17207ab0 > $MEM_DUMP_PATH/register_config
    echo 0x17207ab8 > $MEM_DUMP_PATH/register_config
    echo 0x17207ab8 > $MEM_DUMP_PATH/register_config
    echo 0x17207ac0 > $MEM_DUMP_PATH/register_config
    echo 0x17207ac0 > $MEM_DUMP_PATH/register_config
    echo 0x17207ac8 > $MEM_DUMP_PATH/register_config
    echo 0x17207ac8 > $MEM_DUMP_PATH/register_config
    echo 0x17207ad0 > $MEM_DUMP_PATH/register_config
    echo 0x17207ad0 > $MEM_DUMP_PATH/register_config
    echo 0x17207ad8 > $MEM_DUMP_PATH/register_config
    echo 0x17207ad8 > $MEM_DUMP_PATH/register_config
    echo 0x17207ae0 > $MEM_DUMP_PATH/register_config
    echo 0x17207ae0 > $MEM_DUMP_PATH/register_config
    echo 0x17207ae8 > $MEM_DUMP_PATH/register_config
    echo 0x17207ae8 > $MEM_DUMP_PATH/register_config
    echo 0x17207af0 > $MEM_DUMP_PATH/register_config
    echo 0x17207af0 > $MEM_DUMP_PATH/register_config
    echo 0x17207af8 > $MEM_DUMP_PATH/register_config
    echo 0x17207af8 > $MEM_DUMP_PATH/register_config
    echo 0x17207b00 > $MEM_DUMP_PATH/register_config
    echo 0x17207b00 > $MEM_DUMP_PATH/register_config
    echo 0x17207b08 > $MEM_DUMP_PATH/register_config
    echo 0x17207b08 > $MEM_DUMP_PATH/register_config
    echo 0x17207b10 > $MEM_DUMP_PATH/register_config
    echo 0x17207b10 > $MEM_DUMP_PATH/register_config
    echo 0x17207b18 > $MEM_DUMP_PATH/register_config
    echo 0x17207b18 > $MEM_DUMP_PATH/register_config
    echo 0x17207b20 > $MEM_DUMP_PATH/register_config
    echo 0x17207b20 > $MEM_DUMP_PATH/register_config
    echo 0x17207b28 > $MEM_DUMP_PATH/register_config
    echo 0x17207b28 > $MEM_DUMP_PATH/register_config
    echo 0x17207b30 > $MEM_DUMP_PATH/register_config
    echo 0x17207b30 > $MEM_DUMP_PATH/register_config
    echo 0x17207b38 > $MEM_DUMP_PATH/register_config
    echo 0x17207b38 > $MEM_DUMP_PATH/register_config
    echo 0x17207b40 > $MEM_DUMP_PATH/register_config
    echo 0x17207b40 > $MEM_DUMP_PATH/register_config
    echo 0x17207b48 > $MEM_DUMP_PATH/register_config
    echo 0x17207b48 > $MEM_DUMP_PATH/register_config
    echo 0x17207b50 > $MEM_DUMP_PATH/register_config
    echo 0x17207b50 > $MEM_DUMP_PATH/register_config
    echo 0x17207b58 > $MEM_DUMP_PATH/register_config
    echo 0x17207b58 > $MEM_DUMP_PATH/register_config
    echo 0x17207b60 > $MEM_DUMP_PATH/register_config
    echo 0x17207b60 > $MEM_DUMP_PATH/register_config
    echo 0x17207b68 > $MEM_DUMP_PATH/register_config
    echo 0x17207b68 > $MEM_DUMP_PATH/register_config
    echo 0x17207b70 > $MEM_DUMP_PATH/register_config
    echo 0x17207b70 > $MEM_DUMP_PATH/register_config
    echo 0x17207b78 > $MEM_DUMP_PATH/register_config
    echo 0x17207b78 > $MEM_DUMP_PATH/register_config
    echo 0x17207b80 > $MEM_DUMP_PATH/register_config
    echo 0x17207b80 > $MEM_DUMP_PATH/register_config
    echo 0x17207b88 > $MEM_DUMP_PATH/register_config
    echo 0x17207b88 > $MEM_DUMP_PATH/register_config
    echo 0x17207b90 > $MEM_DUMP_PATH/register_config
    echo 0x17207b90 > $MEM_DUMP_PATH/register_config
    echo 0x17207b98 > $MEM_DUMP_PATH/register_config
    echo 0x17207b98 > $MEM_DUMP_PATH/register_config
    echo 0x17207ba0 > $MEM_DUMP_PATH/register_config
    echo 0x17207ba0 > $MEM_DUMP_PATH/register_config
    echo 0x17207ba8 > $MEM_DUMP_PATH/register_config
    echo 0x17207ba8 > $MEM_DUMP_PATH/register_config
    echo 0x17207bb0 > $MEM_DUMP_PATH/register_config
    echo 0x17207bb0 > $MEM_DUMP_PATH/register_config
    echo 0x17207bb8 > $MEM_DUMP_PATH/register_config
    echo 0x17207bb8 > $MEM_DUMP_PATH/register_config
    echo 0x17207bc0 > $MEM_DUMP_PATH/register_config
    echo 0x17207bc0 > $MEM_DUMP_PATH/register_config
    echo 0x17207bc8 > $MEM_DUMP_PATH/register_config
    echo 0x17207bc8 > $MEM_DUMP_PATH/register_config
    echo 0x17207bd0 > $MEM_DUMP_PATH/register_config
    echo 0x17207bd0 > $MEM_DUMP_PATH/register_config
    echo 0x17207bd8 > $MEM_DUMP_PATH/register_config
    echo 0x17207bd8 > $MEM_DUMP_PATH/register_config
    echo 0x17207be0 > $MEM_DUMP_PATH/register_config
    echo 0x17207be0 > $MEM_DUMP_PATH/register_config
    echo 0x17207be8 > $MEM_DUMP_PATH/register_config
    echo 0x17207be8 > $MEM_DUMP_PATH/register_config
    echo 0x17207bf0 > $MEM_DUMP_PATH/register_config
    echo 0x17207bf0 > $MEM_DUMP_PATH/register_config
    echo 0x17207bf8 > $MEM_DUMP_PATH/register_config
    echo 0x17207bf8 > $MEM_DUMP_PATH/register_config
    echo 0x17207c00 > $MEM_DUMP_PATH/register_config
    echo 0x17207c00 > $MEM_DUMP_PATH/register_config
    echo 0x17207c08 > $MEM_DUMP_PATH/register_config
    echo 0x17207c08 > $MEM_DUMP_PATH/register_config
    echo 0x17207c10 > $MEM_DUMP_PATH/register_config
    echo 0x17207c10 > $MEM_DUMP_PATH/register_config
    echo 0x17207c18 > $MEM_DUMP_PATH/register_config
    echo 0x17207c18 > $MEM_DUMP_PATH/register_config
    echo 0x17207c20 > $MEM_DUMP_PATH/register_config
    echo 0x17207c20 > $MEM_DUMP_PATH/register_config
    echo 0x17207c28 > $MEM_DUMP_PATH/register_config
    echo 0x17207c28 > $MEM_DUMP_PATH/register_config
    echo 0x17207c30 > $MEM_DUMP_PATH/register_config
    echo 0x17207c30 > $MEM_DUMP_PATH/register_config
    echo 0x17207c38 > $MEM_DUMP_PATH/register_config
    echo 0x17207c38 > $MEM_DUMP_PATH/register_config
    echo 0x17207c40 > $MEM_DUMP_PATH/register_config
    echo 0x17207c40 > $MEM_DUMP_PATH/register_config
    echo 0x17207c48 > $MEM_DUMP_PATH/register_config
    echo 0x17207c48 > $MEM_DUMP_PATH/register_config
    echo 0x17207c50 > $MEM_DUMP_PATH/register_config
    echo 0x17207c50 > $MEM_DUMP_PATH/register_config
    echo 0x17207c58 > $MEM_DUMP_PATH/register_config
    echo 0x17207c58 > $MEM_DUMP_PATH/register_config
    echo 0x17207c60 > $MEM_DUMP_PATH/register_config
    echo 0x17207c60 > $MEM_DUMP_PATH/register_config
    echo 0x17207c68 > $MEM_DUMP_PATH/register_config
    echo 0x17207c68 > $MEM_DUMP_PATH/register_config
    echo 0x17207c70 > $MEM_DUMP_PATH/register_config
    echo 0x17207c70 > $MEM_DUMP_PATH/register_config
    echo 0x17207c78 > $MEM_DUMP_PATH/register_config
    echo 0x17207c78 > $MEM_DUMP_PATH/register_config
    echo 0x17207c80 > $MEM_DUMP_PATH/register_config
    echo 0x17207c80 > $MEM_DUMP_PATH/register_config
    echo 0x17207c88 > $MEM_DUMP_PATH/register_config
    echo 0x17207c88 > $MEM_DUMP_PATH/register_config
    echo 0x17207c90 > $MEM_DUMP_PATH/register_config
    echo 0x17207c90 > $MEM_DUMP_PATH/register_config
    echo 0x17207c98 > $MEM_DUMP_PATH/register_config
    echo 0x17207c98 > $MEM_DUMP_PATH/register_config
    echo 0x17207ca0 > $MEM_DUMP_PATH/register_config
    echo 0x17207ca0 > $MEM_DUMP_PATH/register_config
    echo 0x17207ca8 > $MEM_DUMP_PATH/register_config
    echo 0x17207ca8 > $MEM_DUMP_PATH/register_config
    echo 0x17207cb0 > $MEM_DUMP_PATH/register_config
    echo 0x17207cb0 > $MEM_DUMP_PATH/register_config
    echo 0x17207cb8 > $MEM_DUMP_PATH/register_config
    echo 0x17207cb8 > $MEM_DUMP_PATH/register_config
    echo 0x17207cc0 > $MEM_DUMP_PATH/register_config
    echo 0x17207cc0 > $MEM_DUMP_PATH/register_config
    echo 0x17207cc8 > $MEM_DUMP_PATH/register_config
    echo 0x17207cc8 > $MEM_DUMP_PATH/register_config
    echo 0x17207cd0 > $MEM_DUMP_PATH/register_config
    echo 0x17207cd0 > $MEM_DUMP_PATH/register_config
    echo 0x17207cd8 > $MEM_DUMP_PATH/register_config
    echo 0x17207cd8 > $MEM_DUMP_PATH/register_config
    echo 0x17207ce0 > $MEM_DUMP_PATH/register_config
    echo 0x17207ce0 > $MEM_DUMP_PATH/register_config
    echo 0x17207ce8 > $MEM_DUMP_PATH/register_config
    echo 0x17207ce8 > $MEM_DUMP_PATH/register_config
    echo 0x17207cf0 > $MEM_DUMP_PATH/register_config
    echo 0x17207cf0 > $MEM_DUMP_PATH/register_config
    echo 0x17207cf8 > $MEM_DUMP_PATH/register_config
    echo 0x17207cf8 > $MEM_DUMP_PATH/register_config
    echo 0x17207d00 > $MEM_DUMP_PATH/register_config
    echo 0x17207d00 > $MEM_DUMP_PATH/register_config
    echo 0x17207d08 > $MEM_DUMP_PATH/register_config
    echo 0x17207d08 > $MEM_DUMP_PATH/register_config
    echo 0x17207d10 > $MEM_DUMP_PATH/register_config
    echo 0x17207d10 > $MEM_DUMP_PATH/register_config
    echo 0x17207d18 > $MEM_DUMP_PATH/register_config
    echo 0x17207d18 > $MEM_DUMP_PATH/register_config
    echo 0x17207d20 > $MEM_DUMP_PATH/register_config
    echo 0x17207d20 > $MEM_DUMP_PATH/register_config
    echo 0x17207d28 > $MEM_DUMP_PATH/register_config
    echo 0x17207d28 > $MEM_DUMP_PATH/register_config
    echo 0x17207d30 > $MEM_DUMP_PATH/register_config
    echo 0x17207d30 > $MEM_DUMP_PATH/register_config
    echo 0x17207d38 > $MEM_DUMP_PATH/register_config
    echo 0x17207d38 > $MEM_DUMP_PATH/register_config
    echo 0x17207d40 > $MEM_DUMP_PATH/register_config
    echo 0x17207d40 > $MEM_DUMP_PATH/register_config
    echo 0x17207d48 > $MEM_DUMP_PATH/register_config
    echo 0x17207d48 > $MEM_DUMP_PATH/register_config
    echo 0x17207d50 > $MEM_DUMP_PATH/register_config
    echo 0x17207d50 > $MEM_DUMP_PATH/register_config
    echo 0x17207d58 > $MEM_DUMP_PATH/register_config
    echo 0x17207d58 > $MEM_DUMP_PATH/register_config
    echo 0x17207d60 > $MEM_DUMP_PATH/register_config
    echo 0x17207d60 > $MEM_DUMP_PATH/register_config
    echo 0x17207d68 > $MEM_DUMP_PATH/register_config
    echo 0x17207d68 > $MEM_DUMP_PATH/register_config
    echo 0x17207d70 > $MEM_DUMP_PATH/register_config
    echo 0x17207d70 > $MEM_DUMP_PATH/register_config
    echo 0x17207d78 > $MEM_DUMP_PATH/register_config
    echo 0x17207d78 > $MEM_DUMP_PATH/register_config
    echo 0x17207d80 > $MEM_DUMP_PATH/register_config
    echo 0x17207d80 > $MEM_DUMP_PATH/register_config
    echo 0x17207d88 > $MEM_DUMP_PATH/register_config
    echo 0x17207d88 > $MEM_DUMP_PATH/register_config
    echo 0x17207d90 > $MEM_DUMP_PATH/register_config
    echo 0x17207d90 > $MEM_DUMP_PATH/register_config
    echo 0x17207d98 > $MEM_DUMP_PATH/register_config
    echo 0x17207d98 > $MEM_DUMP_PATH/register_config
    echo 0x17207da0 > $MEM_DUMP_PATH/register_config
    echo 0x17207da0 > $MEM_DUMP_PATH/register_config
    echo 0x17207da8 > $MEM_DUMP_PATH/register_config
    echo 0x17207da8 > $MEM_DUMP_PATH/register_config
    echo 0x17207db0 > $MEM_DUMP_PATH/register_config
    echo 0x17207db0 > $MEM_DUMP_PATH/register_config
    echo 0x17207db8 > $MEM_DUMP_PATH/register_config
    echo 0x17207db8 > $MEM_DUMP_PATH/register_config
    echo 0x17207dc0 > $MEM_DUMP_PATH/register_config
    echo 0x17207dc0 > $MEM_DUMP_PATH/register_config
    echo 0x17207dc8 > $MEM_DUMP_PATH/register_config
    echo 0x17207dc8 > $MEM_DUMP_PATH/register_config
    echo 0x17207dd0 > $MEM_DUMP_PATH/register_config
    echo 0x17207dd0 > $MEM_DUMP_PATH/register_config
    echo 0x17207dd8 > $MEM_DUMP_PATH/register_config
    echo 0x17207dd8 > $MEM_DUMP_PATH/register_config
    echo 0x17207de0 > $MEM_DUMP_PATH/register_config
    echo 0x17207de0 > $MEM_DUMP_PATH/register_config
    echo 0x17207de8 > $MEM_DUMP_PATH/register_config
    echo 0x17207de8 > $MEM_DUMP_PATH/register_config
    echo 0x17207df0 > $MEM_DUMP_PATH/register_config
    echo 0x17207df0 > $MEM_DUMP_PATH/register_config
    echo 0x17207df8 > $MEM_DUMP_PATH/register_config
    echo 0x17207df8 > $MEM_DUMP_PATH/register_config
    echo 0x1720e008 58 > $MEM_DUMP_PATH/register_config
    echo 0x1720e104 29 > $MEM_DUMP_PATH/register_config
    echo 0x1720e104 29 > $MEM_DUMP_PATH/register_config
    echo 0x1720f000 > $MEM_DUMP_PATH/register_config
    echo 0x1720ffd0 12 > $MEM_DUMP_PATH/register_config
    echo 0x17210040 > $MEM_DUMP_PATH/register_config
    echo 0x17210048 > $MEM_DUMP_PATH/register_config
    echo 0x17210050 > $MEM_DUMP_PATH/register_config
    echo 0x17210058 > $MEM_DUMP_PATH/register_config
    echo 0x17220000 > $MEM_DUMP_PATH/register_config
    echo 0x17220008 > $MEM_DUMP_PATH/register_config
    echo 0x17220008 > $MEM_DUMP_PATH/register_config
    echo 0x17220010 > $MEM_DUMP_PATH/register_config
    echo 0x17220010 > $MEM_DUMP_PATH/register_config
    echo 0x17220018 > $MEM_DUMP_PATH/register_config
    echo 0x17220018 > $MEM_DUMP_PATH/register_config
    echo 0x17220020 > $MEM_DUMP_PATH/register_config
    echo 0x17220020 > $MEM_DUMP_PATH/register_config
    echo 0x17220028 > $MEM_DUMP_PATH/register_config
    echo 0x17220028 > $MEM_DUMP_PATH/register_config
    echo 0x17220040 > $MEM_DUMP_PATH/register_config
    echo 0x17220048 > $MEM_DUMP_PATH/register_config
    echo 0x17220048 > $MEM_DUMP_PATH/register_config
    echo 0x17220050 > $MEM_DUMP_PATH/register_config
    echo 0x17220050 > $MEM_DUMP_PATH/register_config
    echo 0x17220060 > $MEM_DUMP_PATH/register_config
    echo 0x17220068 > $MEM_DUMP_PATH/register_config
    echo 0x17220068 > $MEM_DUMP_PATH/register_config
    echo 0x17220080 > $MEM_DUMP_PATH/register_config
    echo 0x17220088 > $MEM_DUMP_PATH/register_config
    echo 0x17220088 > $MEM_DUMP_PATH/register_config
    echo 0x17220090 > $MEM_DUMP_PATH/register_config
    echo 0x17220090 > $MEM_DUMP_PATH/register_config
    echo 0x172200a0 > $MEM_DUMP_PATH/register_config
    echo 0x172200a8 > $MEM_DUMP_PATH/register_config
    echo 0x172200a8 > $MEM_DUMP_PATH/register_config
    echo 0x172200c0 > $MEM_DUMP_PATH/register_config
    echo 0x172200c8 > $MEM_DUMP_PATH/register_config
    echo 0x172200c8 > $MEM_DUMP_PATH/register_config
    echo 0x172200d0 > $MEM_DUMP_PATH/register_config
    echo 0x172200d0 > $MEM_DUMP_PATH/register_config
    echo 0x172200e0 > $MEM_DUMP_PATH/register_config
    echo 0x172200e8 > $MEM_DUMP_PATH/register_config
    echo 0x172200e8 > $MEM_DUMP_PATH/register_config
    echo 0x17220100 > $MEM_DUMP_PATH/register_config
    echo 0x17220108 > $MEM_DUMP_PATH/register_config
    echo 0x17220108 > $MEM_DUMP_PATH/register_config
    echo 0x17220110 > $MEM_DUMP_PATH/register_config
    echo 0x17220110 > $MEM_DUMP_PATH/register_config
    echo 0x17220120 > $MEM_DUMP_PATH/register_config
    echo 0x17220128 > $MEM_DUMP_PATH/register_config
    echo 0x17220128 > $MEM_DUMP_PATH/register_config
    echo 0x17220140 > $MEM_DUMP_PATH/register_config
    echo 0x17220148 > $MEM_DUMP_PATH/register_config
    echo 0x17220148 > $MEM_DUMP_PATH/register_config
    echo 0x17220150 > $MEM_DUMP_PATH/register_config
    echo 0x17220150 > $MEM_DUMP_PATH/register_config
    echo 0x17220160 > $MEM_DUMP_PATH/register_config
    echo 0x17220168 > $MEM_DUMP_PATH/register_config
    echo 0x17220168 > $MEM_DUMP_PATH/register_config
    echo 0x17220180 > $MEM_DUMP_PATH/register_config
    echo 0x17220188 > $MEM_DUMP_PATH/register_config
    echo 0x17220188 > $MEM_DUMP_PATH/register_config
    echo 0x17220190 > $MEM_DUMP_PATH/register_config
    echo 0x17220190 > $MEM_DUMP_PATH/register_config
    echo 0x172201a0 > $MEM_DUMP_PATH/register_config
    echo 0x172201a8 > $MEM_DUMP_PATH/register_config
    echo 0x172201a8 > $MEM_DUMP_PATH/register_config
    echo 0x172201c0 > $MEM_DUMP_PATH/register_config
    echo 0x172201c8 > $MEM_DUMP_PATH/register_config
    echo 0x172201c8 > $MEM_DUMP_PATH/register_config
    echo 0x172201d0 > $MEM_DUMP_PATH/register_config
    echo 0x172201d0 > $MEM_DUMP_PATH/register_config
    echo 0x172201e0 > $MEM_DUMP_PATH/register_config
    echo 0x172201e8 > $MEM_DUMP_PATH/register_config
    echo 0x172201e8 > $MEM_DUMP_PATH/register_config
    echo 0x17220200 > $MEM_DUMP_PATH/register_config
    echo 0x17220208 > $MEM_DUMP_PATH/register_config
    echo 0x17220208 > $MEM_DUMP_PATH/register_config
    echo 0x17220210 > $MEM_DUMP_PATH/register_config
    echo 0x17220210 > $MEM_DUMP_PATH/register_config
    echo 0x17220220 > $MEM_DUMP_PATH/register_config
    echo 0x17220228 > $MEM_DUMP_PATH/register_config
    echo 0x17220228 > $MEM_DUMP_PATH/register_config
    echo 0x17220240 > $MEM_DUMP_PATH/register_config
    echo 0x17220248 > $MEM_DUMP_PATH/register_config
    echo 0x17220248 > $MEM_DUMP_PATH/register_config
    echo 0x17220250 > $MEM_DUMP_PATH/register_config
    echo 0x17220250 > $MEM_DUMP_PATH/register_config
    echo 0x17220260 > $MEM_DUMP_PATH/register_config
    echo 0x17220268 > $MEM_DUMP_PATH/register_config
    echo 0x17220268 > $MEM_DUMP_PATH/register_config
    echo 0x17220280 > $MEM_DUMP_PATH/register_config
    echo 0x17220288 > $MEM_DUMP_PATH/register_config
    echo 0x17220288 > $MEM_DUMP_PATH/register_config
    echo 0x17220290 > $MEM_DUMP_PATH/register_config
    echo 0x17220290 > $MEM_DUMP_PATH/register_config
    echo 0x172202a0 > $MEM_DUMP_PATH/register_config
    echo 0x172202a8 > $MEM_DUMP_PATH/register_config
    echo 0x172202a8 > $MEM_DUMP_PATH/register_config
    echo 0x172202c0 > $MEM_DUMP_PATH/register_config
    echo 0x172202c8 > $MEM_DUMP_PATH/register_config
    echo 0x172202c8 > $MEM_DUMP_PATH/register_config
    echo 0x172202d0 > $MEM_DUMP_PATH/register_config
    echo 0x172202d0 > $MEM_DUMP_PATH/register_config
    echo 0x172202e0 > $MEM_DUMP_PATH/register_config
    echo 0x172202e8 > $MEM_DUMP_PATH/register_config
    echo 0x172202e8 > $MEM_DUMP_PATH/register_config
    echo 0x17220300 > $MEM_DUMP_PATH/register_config
    echo 0x17220308 > $MEM_DUMP_PATH/register_config
    echo 0x17220308 > $MEM_DUMP_PATH/register_config
    echo 0x17220310 > $MEM_DUMP_PATH/register_config
    echo 0x17220310 > $MEM_DUMP_PATH/register_config
    echo 0x17220320 > $MEM_DUMP_PATH/register_config
    echo 0x17220328 > $MEM_DUMP_PATH/register_config
    echo 0x17220328 > $MEM_DUMP_PATH/register_config
    echo 0x17220340 > $MEM_DUMP_PATH/register_config
    echo 0x17220348 > $MEM_DUMP_PATH/register_config
    echo 0x17220348 > $MEM_DUMP_PATH/register_config
    echo 0x17220350 > $MEM_DUMP_PATH/register_config
    echo 0x17220350 > $MEM_DUMP_PATH/register_config
    echo 0x17220360 > $MEM_DUMP_PATH/register_config
    echo 0x17220368 > $MEM_DUMP_PATH/register_config
    echo 0x17220368 > $MEM_DUMP_PATH/register_config
    echo 0x1722e000 > $MEM_DUMP_PATH/register_config
    echo 0x1722e800 > $MEM_DUMP_PATH/register_config
    echo 0x1722e808 > $MEM_DUMP_PATH/register_config
    echo 0x1722e808 > $MEM_DUMP_PATH/register_config
    echo 0x1722ffbc > $MEM_DUMP_PATH/register_config
    echo 0x1722ffc8 > $MEM_DUMP_PATH/register_config
    echo 0x1722ffc8 > $MEM_DUMP_PATH/register_config
    echo 0x1722ffd0 2 > $MEM_DUMP_PATH/register_config
    echo 0x1722ffd0 17 > $MEM_DUMP_PATH/register_config
    echo 0x17230400 5 > $MEM_DUMP_PATH/register_config
    echo 0x17230600 5 > $MEM_DUMP_PATH/register_config
    echo 0x17230a00 5 > $MEM_DUMP_PATH/register_config
    echo 0x17230c00 > $MEM_DUMP_PATH/register_config
    echo 0x17230c20 > $MEM_DUMP_PATH/register_config
    echo 0x17230c40 > $MEM_DUMP_PATH/register_config
    echo 0x17230c60 > $MEM_DUMP_PATH/register_config
    echo 0x17230c80 > $MEM_DUMP_PATH/register_config
    echo 0x17230cc0 > $MEM_DUMP_PATH/register_config
    echo 0x17230e00 2 > $MEM_DUMP_PATH/register_config
    echo 0x17230e50 > $MEM_DUMP_PATH/register_config
    echo 0x17230fb8 2 > $MEM_DUMP_PATH/register_config
    echo 0x17230fcc 3 > $MEM_DUMP_PATH/register_config
    echo 0x17230fcc 13 > $MEM_DUMP_PATH/register_config
    echo 0x17240000 3 > $MEM_DUMP_PATH/register_config
    echo 0x17240020 > $MEM_DUMP_PATH/register_config
    echo 0x17240028 > $MEM_DUMP_PATH/register_config
    echo 0x17240020 > $MEM_DUMP_PATH/register_config
    echo 0x17240028 > $MEM_DUMP_PATH/register_config
    echo 0x17240028 > $MEM_DUMP_PATH/register_config
    echo 0x17240030 > $MEM_DUMP_PATH/register_config
    echo 0x17240030 > $MEM_DUMP_PATH/register_config
    echo 0x17240080 > $MEM_DUMP_PATH/register_config
    echo 0x17240088 > $MEM_DUMP_PATH/register_config
    echo 0x17240088 > $MEM_DUMP_PATH/register_config
    echo 0x17240090 > $MEM_DUMP_PATH/register_config
    echo 0x17240090 > $MEM_DUMP_PATH/register_config
    echo 0x17240100 > $MEM_DUMP_PATH/register_config
    echo 0x17240108 > $MEM_DUMP_PATH/register_config
    echo 0x17240108 > $MEM_DUMP_PATH/register_config
    echo 0x1724f000 > $MEM_DUMP_PATH/register_config
    echo 0x1724ffd0 12 > $MEM_DUMP_PATH/register_config
    echo 0x17250040 > $MEM_DUMP_PATH/register_config
    echo 0x17260000 3 > $MEM_DUMP_PATH/register_config
    echo 0x17260014 > $MEM_DUMP_PATH/register_config
    echo 0x17260020 3 > $MEM_DUMP_PATH/register_config
    echo 0x17260014 > $MEM_DUMP_PATH/register_config
    echo 0x17260020 > $MEM_DUMP_PATH/register_config
    echo 0x17260020 3 > $MEM_DUMP_PATH/register_config
    echo 0x17260070 > $MEM_DUMP_PATH/register_config
    echo 0x17260078 > $MEM_DUMP_PATH/register_config
    echo 0x17260078 > $MEM_DUMP_PATH/register_config
    echo 0x1726ffd0 12 > $MEM_DUMP_PATH/register_config
    echo 0x17270080 > $MEM_DUMP_PATH/register_config
    echo 0x17270080 > $MEM_DUMP_PATH/register_config
    echo 0x17270100 > $MEM_DUMP_PATH/register_config
    echo 0x17270180 > $MEM_DUMP_PATH/register_config
    echo 0x17270200 > $MEM_DUMP_PATH/register_config
    echo 0x17270280 > $MEM_DUMP_PATH/register_config
    echo 0x17270300 > $MEM_DUMP_PATH/register_config
    echo 0x17270380 > $MEM_DUMP_PATH/register_config
    echo 0x17270400 8 > $MEM_DUMP_PATH/register_config
    echo 0x17270c00 2 > $MEM_DUMP_PATH/register_config
    echo 0x17270d00 > $MEM_DUMP_PATH/register_config
    echo 0x17270e00 > $MEM_DUMP_PATH/register_config
    echo 0x1727c000 > $MEM_DUMP_PATH/register_config
    echo 0x1727c008 > $MEM_DUMP_PATH/register_config
    echo 0x1727c008 > $MEM_DUMP_PATH/register_config
    echo 0x1727c010 > $MEM_DUMP_PATH/register_config
    echo 0x1727c010 > $MEM_DUMP_PATH/register_config
    echo 0x1727f000 2 > $MEM_DUMP_PATH/register_config
    echo 0x17280000 3 > $MEM_DUMP_PATH/register_config
    echo 0x17280014 > $MEM_DUMP_PATH/register_config
    echo 0x17280020 3 > $MEM_DUMP_PATH/register_config
    echo 0x17280014 > $MEM_DUMP_PATH/register_config
    echo 0x17280020 > $MEM_DUMP_PATH/register_config
    echo 0x17280020 3 > $MEM_DUMP_PATH/register_config
    echo 0x17280070 > $MEM_DUMP_PATH/register_config
    echo 0x17280078 > $MEM_DUMP_PATH/register_config
    echo 0x17280078 > $MEM_DUMP_PATH/register_config
    echo 0x1728ffd0 12 > $MEM_DUMP_PATH/register_config
    echo 0x17290080 > $MEM_DUMP_PATH/register_config
    echo 0x17290080 > $MEM_DUMP_PATH/register_config
    echo 0x17290100 > $MEM_DUMP_PATH/register_config
    echo 0x17290180 > $MEM_DUMP_PATH/register_config
    echo 0x17290200 > $MEM_DUMP_PATH/register_config
    echo 0x17290280 > $MEM_DUMP_PATH/register_config
    echo 0x17290300 > $MEM_DUMP_PATH/register_config
    echo 0x17290380 > $MEM_DUMP_PATH/register_config
    echo 0x17290400 8 > $MEM_DUMP_PATH/register_config
    echo 0x17290c00 2 > $MEM_DUMP_PATH/register_config
    echo 0x17290d00 > $MEM_DUMP_PATH/register_config
    echo 0x17290e00 > $MEM_DUMP_PATH/register_config
    echo 0x1729c000 > $MEM_DUMP_PATH/register_config
    echo 0x1729c008 > $MEM_DUMP_PATH/register_config
    echo 0x1729c008 > $MEM_DUMP_PATH/register_config
    echo 0x1729c010 > $MEM_DUMP_PATH/register_config
    echo 0x1729c010 > $MEM_DUMP_PATH/register_config
    echo 0x1729f000 2 > $MEM_DUMP_PATH/register_config
    echo 0x172a0000 3 > $MEM_DUMP_PATH/register_config
    echo 0x172a0014 > $MEM_DUMP_PATH/register_config
    echo 0x172a0020 3 > $MEM_DUMP_PATH/register_config
    echo 0x172a0014 > $MEM_DUMP_PATH/register_config
    echo 0x172a0020 > $MEM_DUMP_PATH/register_config
    echo 0x172a0020 3 > $MEM_DUMP_PATH/register_config
    echo 0x172a0070 > $MEM_DUMP_PATH/register_config
    echo 0x172a0078 > $MEM_DUMP_PATH/register_config
    echo 0x172a0078 > $MEM_DUMP_PATH/register_config
    echo 0x172affd0 12 > $MEM_DUMP_PATH/register_config
    echo 0x172b0080 > $MEM_DUMP_PATH/register_config
    echo 0x172b0080 > $MEM_DUMP_PATH/register_config
    echo 0x172b0100 > $MEM_DUMP_PATH/register_config
    echo 0x172b0180 > $MEM_DUMP_PATH/register_config
    echo 0x172b0200 > $MEM_DUMP_PATH/register_config
    echo 0x172b0280 > $MEM_DUMP_PATH/register_config
    echo 0x172b0300 > $MEM_DUMP_PATH/register_config
    echo 0x172b0380 > $MEM_DUMP_PATH/register_config
    echo 0x172b0400 8 > $MEM_DUMP_PATH/register_config
    echo 0x172b0c00 2 > $MEM_DUMP_PATH/register_config
    echo 0x172b0d00 > $MEM_DUMP_PATH/register_config
    echo 0x172b0e00 > $MEM_DUMP_PATH/register_config
    echo 0x172bc000 > $MEM_DUMP_PATH/register_config
    echo 0x172bc008 > $MEM_DUMP_PATH/register_config
    echo 0x172bc008 > $MEM_DUMP_PATH/register_config
    echo 0x172bc010 > $MEM_DUMP_PATH/register_config
    echo 0x172bc010 > $MEM_DUMP_PATH/register_config
    echo 0x172bf000 2 > $MEM_DUMP_PATH/register_config
    echo 0x172c0000 3 > $MEM_DUMP_PATH/register_config
    echo 0x172c0014 > $MEM_DUMP_PATH/register_config
    echo 0x172c0020 3 > $MEM_DUMP_PATH/register_config
    echo 0x172c0014 > $MEM_DUMP_PATH/register_config
    echo 0x172c0020 > $MEM_DUMP_PATH/register_config
    echo 0x172c0020 3 > $MEM_DUMP_PATH/register_config
    echo 0x172c0070 > $MEM_DUMP_PATH/register_config
    echo 0x172c0078 > $MEM_DUMP_PATH/register_config
    echo 0x172c0078 > $MEM_DUMP_PATH/register_config
    echo 0x172cffd0 12 > $MEM_DUMP_PATH/register_config
    echo 0x172d0080 > $MEM_DUMP_PATH/register_config
    echo 0x172d0080 > $MEM_DUMP_PATH/register_config
    echo 0x172d0100 > $MEM_DUMP_PATH/register_config
    echo 0x172d0180 > $MEM_DUMP_PATH/register_config
    echo 0x172d0200 > $MEM_DUMP_PATH/register_config
    echo 0x172d0280 > $MEM_DUMP_PATH/register_config
    echo 0x172d0300 > $MEM_DUMP_PATH/register_config
    echo 0x172d0380 > $MEM_DUMP_PATH/register_config
    echo 0x172d0400 8 > $MEM_DUMP_PATH/register_config
    echo 0x172d0c00 2 > $MEM_DUMP_PATH/register_config
    echo 0x172d0d00 > $MEM_DUMP_PATH/register_config
    echo 0x172d0e00 > $MEM_DUMP_PATH/register_config
    echo 0x172dc000 > $MEM_DUMP_PATH/register_config
    echo 0x172dc008 > $MEM_DUMP_PATH/register_config
    echo 0x172dc008 > $MEM_DUMP_PATH/register_config
    echo 0x172dc010 > $MEM_DUMP_PATH/register_config
    echo 0x172dc010 > $MEM_DUMP_PATH/register_config
    echo 0x172df000 2 > $MEM_DUMP_PATH/register_config
    echo 0x172e0000 3 > $MEM_DUMP_PATH/register_config
    echo 0x172e0020 2 > $MEM_DUMP_PATH/register_config
    echo 0x172e0020 2 > $MEM_DUMP_PATH/register_config
    echo 0x172e0084 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0104 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0184 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0204 20 > $MEM_DUMP_PATH/register_config
    echo 0x172e0104 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0184 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0204 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0284 20 > $MEM_DUMP_PATH/register_config
    echo 0x172e0184 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0204 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0284 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0304 20 > $MEM_DUMP_PATH/register_config
    echo 0x172e0204 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0284 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0304 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0384 20 > $MEM_DUMP_PATH/register_config
    echo 0x172e0284 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0304 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0384 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0420 13 > $MEM_DUMP_PATH/register_config
    echo 0x172e0304 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0384 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0420 45 > $MEM_DUMP_PATH/register_config
    echo 0x172e0384 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0420 77 > $MEM_DUMP_PATH/register_config
    echo 0x172e0420 232 > $MEM_DUMP_PATH/register_config
    echo 0x172e0c08 58 > $MEM_DUMP_PATH/register_config
    echo 0x172e0d04 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0e10 58 > $MEM_DUMP_PATH/register_config
    echo 0x172e0c08 58 > $MEM_DUMP_PATH/register_config
    echo 0x172e0d04 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0e10 58 > $MEM_DUMP_PATH/register_config
    echo 0x172e0d04 29 > $MEM_DUMP_PATH/register_config
    echo 0x172e0e10 49 > $MEM_DUMP_PATH/register_config
    echo 0x172e0e10 58 > $MEM_DUMP_PATH/register_config
    echo 0x172e6100 > $MEM_DUMP_PATH/register_config
    echo 0x172e6108 > $MEM_DUMP_PATH/register_config
    echo 0x172e6108 > $MEM_DUMP_PATH/register_config
    echo 0x172e6110 > $MEM_DUMP_PATH/register_config
    echo 0x172e6110 > $MEM_DUMP_PATH/register_config
    echo 0x172e6118 > $MEM_DUMP_PATH/register_config
    echo 0x172e6118 > $MEM_DUMP_PATH/register_config
    echo 0x172e6120 > $MEM_DUMP_PATH/register_config
    echo 0x172e6120 > $MEM_DUMP_PATH/register_config
    echo 0x172e6128 > $MEM_DUMP_PATH/register_config
    echo 0x172e6128 > $MEM_DUMP_PATH/register_config
    echo 0x172e6130 > $MEM_DUMP_PATH/register_config
    echo 0x172e6130 > $MEM_DUMP_PATH/register_config
    echo 0x172e6138 > $MEM_DUMP_PATH/register_config
    echo 0x172e6138 > $MEM_DUMP_PATH/register_config
    echo 0x172e6140 > $MEM_DUMP_PATH/register_config
    echo 0x172e6140 > $MEM_DUMP_PATH/register_config
    echo 0x172e6148 > $MEM_DUMP_PATH/register_config
    echo 0x172e6148 > $MEM_DUMP_PATH/register_config
    echo 0x172e6150 > $MEM_DUMP_PATH/register_config
    echo 0x172e6150 > $MEM_DUMP_PATH/register_config
    echo 0x172e6158 > $MEM_DUMP_PATH/register_config
    echo 0x172e6158 > $MEM_DUMP_PATH/register_config
    echo 0x172e6160 > $MEM_DUMP_PATH/register_config
    echo 0x172e6160 > $MEM_DUMP_PATH/register_config
    echo 0x172e6168 > $MEM_DUMP_PATH/register_config
    echo 0x172e6168 > $MEM_DUMP_PATH/register_config
    echo 0x172e6170 > $MEM_DUMP_PATH/register_config
    echo 0x172e6170 > $MEM_DUMP_PATH/register_config
    echo 0x172e6178 > $MEM_DUMP_PATH/register_config
    echo 0x172e6178 > $MEM_DUMP_PATH/register_config
    echo 0x172e6180 > $MEM_DUMP_PATH/register_config
    echo 0x172e6180 > $MEM_DUMP_PATH/register_config
    echo 0x172e6188 > $MEM_DUMP_PATH/register_config
    echo 0x172e6188 > $MEM_DUMP_PATH/register_config
    echo 0x172e6190 > $MEM_DUMP_PATH/register_config
    echo 0x172e6190 > $MEM_DUMP_PATH/register_config
    echo 0x172e6198 > $MEM_DUMP_PATH/register_config
    echo 0x172e6198 > $MEM_DUMP_PATH/register_config
    echo 0x172e61a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e61a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e61a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e61a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e61b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e61b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e61b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e61b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e61c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e61c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e61c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e61c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e61d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e61d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e61d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e61d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e61e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e61e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e61e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e61e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e61f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e61f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e61f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e61f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6200 > $MEM_DUMP_PATH/register_config
    echo 0x172e6200 > $MEM_DUMP_PATH/register_config
    echo 0x172e6208 > $MEM_DUMP_PATH/register_config
    echo 0x172e6208 > $MEM_DUMP_PATH/register_config
    echo 0x172e6210 > $MEM_DUMP_PATH/register_config
    echo 0x172e6210 > $MEM_DUMP_PATH/register_config
    echo 0x172e6218 > $MEM_DUMP_PATH/register_config
    echo 0x172e6218 > $MEM_DUMP_PATH/register_config
    echo 0x172e6220 > $MEM_DUMP_PATH/register_config
    echo 0x172e6220 > $MEM_DUMP_PATH/register_config
    echo 0x172e6228 > $MEM_DUMP_PATH/register_config
    echo 0x172e6228 > $MEM_DUMP_PATH/register_config
    echo 0x172e6230 > $MEM_DUMP_PATH/register_config
    echo 0x172e6230 > $MEM_DUMP_PATH/register_config
    echo 0x172e6238 > $MEM_DUMP_PATH/register_config
    echo 0x172e6238 > $MEM_DUMP_PATH/register_config
    echo 0x172e6240 > $MEM_DUMP_PATH/register_config
    echo 0x172e6240 > $MEM_DUMP_PATH/register_config
    echo 0x172e6248 > $MEM_DUMP_PATH/register_config
    echo 0x172e6248 > $MEM_DUMP_PATH/register_config
    echo 0x172e6250 > $MEM_DUMP_PATH/register_config
    echo 0x172e6250 > $MEM_DUMP_PATH/register_config
    echo 0x172e6258 > $MEM_DUMP_PATH/register_config
    echo 0x172e6258 > $MEM_DUMP_PATH/register_config
    echo 0x172e6260 > $MEM_DUMP_PATH/register_config
    echo 0x172e6260 > $MEM_DUMP_PATH/register_config
    echo 0x172e6268 > $MEM_DUMP_PATH/register_config
    echo 0x172e6268 > $MEM_DUMP_PATH/register_config
    echo 0x172e6270 > $MEM_DUMP_PATH/register_config
    echo 0x172e6270 > $MEM_DUMP_PATH/register_config
    echo 0x172e6278 > $MEM_DUMP_PATH/register_config
    echo 0x172e6278 > $MEM_DUMP_PATH/register_config
    echo 0x172e6280 > $MEM_DUMP_PATH/register_config
    echo 0x172e6280 > $MEM_DUMP_PATH/register_config
    echo 0x172e6288 > $MEM_DUMP_PATH/register_config
    echo 0x172e6288 > $MEM_DUMP_PATH/register_config
    echo 0x172e6290 > $MEM_DUMP_PATH/register_config
    echo 0x172e6290 > $MEM_DUMP_PATH/register_config
    echo 0x172e6298 > $MEM_DUMP_PATH/register_config
    echo 0x172e6298 > $MEM_DUMP_PATH/register_config
    echo 0x172e62a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e62a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e62a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e62a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e62b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e62b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e62b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e62b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e62c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e62c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e62c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e62c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e62d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e62d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e62d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e62d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e62e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e62e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e62e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e62e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e62f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e62f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e62f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e62f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6300 > $MEM_DUMP_PATH/register_config
    echo 0x172e6300 > $MEM_DUMP_PATH/register_config
    echo 0x172e6308 > $MEM_DUMP_PATH/register_config
    echo 0x172e6308 > $MEM_DUMP_PATH/register_config
    echo 0x172e6310 > $MEM_DUMP_PATH/register_config
    echo 0x172e6310 > $MEM_DUMP_PATH/register_config
    echo 0x172e6318 > $MEM_DUMP_PATH/register_config
    echo 0x172e6318 > $MEM_DUMP_PATH/register_config
    echo 0x172e6320 > $MEM_DUMP_PATH/register_config
    echo 0x172e6320 > $MEM_DUMP_PATH/register_config
    echo 0x172e6328 > $MEM_DUMP_PATH/register_config
    echo 0x172e6328 > $MEM_DUMP_PATH/register_config
    echo 0x172e6330 > $MEM_DUMP_PATH/register_config
    echo 0x172e6330 > $MEM_DUMP_PATH/register_config
    echo 0x172e6338 > $MEM_DUMP_PATH/register_config
    echo 0x172e6338 > $MEM_DUMP_PATH/register_config
    echo 0x172e6340 > $MEM_DUMP_PATH/register_config
    echo 0x172e6340 > $MEM_DUMP_PATH/register_config
    echo 0x172e6348 > $MEM_DUMP_PATH/register_config
    echo 0x172e6348 > $MEM_DUMP_PATH/register_config
    echo 0x172e6350 > $MEM_DUMP_PATH/register_config
    echo 0x172e6350 > $MEM_DUMP_PATH/register_config
    echo 0x172e6358 > $MEM_DUMP_PATH/register_config
    echo 0x172e6358 > $MEM_DUMP_PATH/register_config
    echo 0x172e6360 > $MEM_DUMP_PATH/register_config
    echo 0x172e6360 > $MEM_DUMP_PATH/register_config
    echo 0x172e6368 > $MEM_DUMP_PATH/register_config
    echo 0x172e6368 > $MEM_DUMP_PATH/register_config
    echo 0x172e6370 > $MEM_DUMP_PATH/register_config
    echo 0x172e6370 > $MEM_DUMP_PATH/register_config
    echo 0x172e6378 > $MEM_DUMP_PATH/register_config
    echo 0x172e6378 > $MEM_DUMP_PATH/register_config
    echo 0x172e6380 > $MEM_DUMP_PATH/register_config
    echo 0x172e6380 > $MEM_DUMP_PATH/register_config
    echo 0x172e6388 > $MEM_DUMP_PATH/register_config
    echo 0x172e6388 > $MEM_DUMP_PATH/register_config
    echo 0x172e6390 > $MEM_DUMP_PATH/register_config
    echo 0x172e6390 > $MEM_DUMP_PATH/register_config
    echo 0x172e6398 > $MEM_DUMP_PATH/register_config
    echo 0x172e6398 > $MEM_DUMP_PATH/register_config
    echo 0x172e63a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e63a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e63a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e63a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e63b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e63b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e63b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e63b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e63c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e63c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e63c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e63c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e63d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e63d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e63d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e63d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e63e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e63e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e63e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e63e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e63f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e63f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e63f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e63f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6400 > $MEM_DUMP_PATH/register_config
    echo 0x172e6400 > $MEM_DUMP_PATH/register_config
    echo 0x172e6408 > $MEM_DUMP_PATH/register_config
    echo 0x172e6408 > $MEM_DUMP_PATH/register_config
    echo 0x172e6410 > $MEM_DUMP_PATH/register_config
    echo 0x172e6410 > $MEM_DUMP_PATH/register_config
    echo 0x172e6418 > $MEM_DUMP_PATH/register_config
    echo 0x172e6418 > $MEM_DUMP_PATH/register_config
    echo 0x172e6420 > $MEM_DUMP_PATH/register_config
    echo 0x172e6420 > $MEM_DUMP_PATH/register_config
    echo 0x172e6428 > $MEM_DUMP_PATH/register_config
    echo 0x172e6428 > $MEM_DUMP_PATH/register_config
    echo 0x172e6430 > $MEM_DUMP_PATH/register_config
    echo 0x172e6430 > $MEM_DUMP_PATH/register_config
    echo 0x172e6438 > $MEM_DUMP_PATH/register_config
    echo 0x172e6438 > $MEM_DUMP_PATH/register_config
    echo 0x172e6440 > $MEM_DUMP_PATH/register_config
    echo 0x172e6440 > $MEM_DUMP_PATH/register_config
    echo 0x172e6448 > $MEM_DUMP_PATH/register_config
    echo 0x172e6448 > $MEM_DUMP_PATH/register_config
    echo 0x172e6450 > $MEM_DUMP_PATH/register_config
    echo 0x172e6450 > $MEM_DUMP_PATH/register_config
    echo 0x172e6458 > $MEM_DUMP_PATH/register_config
    echo 0x172e6458 > $MEM_DUMP_PATH/register_config
    echo 0x172e6460 > $MEM_DUMP_PATH/register_config
    echo 0x172e6460 > $MEM_DUMP_PATH/register_config
    echo 0x172e6468 > $MEM_DUMP_PATH/register_config
    echo 0x172e6468 > $MEM_DUMP_PATH/register_config
    echo 0x172e6470 > $MEM_DUMP_PATH/register_config
    echo 0x172e6470 > $MEM_DUMP_PATH/register_config
    echo 0x172e6478 > $MEM_DUMP_PATH/register_config
    echo 0x172e6478 > $MEM_DUMP_PATH/register_config
    echo 0x172e6480 > $MEM_DUMP_PATH/register_config
    echo 0x172e6480 > $MEM_DUMP_PATH/register_config
    echo 0x172e6488 > $MEM_DUMP_PATH/register_config
    echo 0x172e6488 > $MEM_DUMP_PATH/register_config
    echo 0x172e6490 > $MEM_DUMP_PATH/register_config
    echo 0x172e6490 > $MEM_DUMP_PATH/register_config
    echo 0x172e6498 > $MEM_DUMP_PATH/register_config
    echo 0x172e6498 > $MEM_DUMP_PATH/register_config
    echo 0x172e64a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e64a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e64a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e64a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e64b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e64b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e64b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e64b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e64c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e64c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e64c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e64c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e64d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e64d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e64d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e64d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e64e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e64e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e64e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e64e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e64f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e64f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e64f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e64f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6500 > $MEM_DUMP_PATH/register_config
    echo 0x172e6500 > $MEM_DUMP_PATH/register_config
    echo 0x172e6508 > $MEM_DUMP_PATH/register_config
    echo 0x172e6508 > $MEM_DUMP_PATH/register_config
    echo 0x172e6510 > $MEM_DUMP_PATH/register_config
    echo 0x172e6510 > $MEM_DUMP_PATH/register_config
    echo 0x172e6518 > $MEM_DUMP_PATH/register_config
    echo 0x172e6518 > $MEM_DUMP_PATH/register_config
    echo 0x172e6520 > $MEM_DUMP_PATH/register_config
    echo 0x172e6520 > $MEM_DUMP_PATH/register_config
    echo 0x172e6528 > $MEM_DUMP_PATH/register_config
    echo 0x172e6528 > $MEM_DUMP_PATH/register_config
    echo 0x172e6530 > $MEM_DUMP_PATH/register_config
    echo 0x172e6530 > $MEM_DUMP_PATH/register_config
    echo 0x172e6538 > $MEM_DUMP_PATH/register_config
    echo 0x172e6538 > $MEM_DUMP_PATH/register_config
    echo 0x172e6540 > $MEM_DUMP_PATH/register_config
    echo 0x172e6540 > $MEM_DUMP_PATH/register_config
    echo 0x172e6548 > $MEM_DUMP_PATH/register_config
    echo 0x172e6548 > $MEM_DUMP_PATH/register_config
    echo 0x172e6550 > $MEM_DUMP_PATH/register_config
    echo 0x172e6550 > $MEM_DUMP_PATH/register_config
    echo 0x172e6558 > $MEM_DUMP_PATH/register_config
    echo 0x172e6558 > $MEM_DUMP_PATH/register_config
    echo 0x172e6560 > $MEM_DUMP_PATH/register_config
    echo 0x172e6560 > $MEM_DUMP_PATH/register_config
    echo 0x172e6568 > $MEM_DUMP_PATH/register_config
    echo 0x172e6568 > $MEM_DUMP_PATH/register_config
    echo 0x172e6570 > $MEM_DUMP_PATH/register_config
    echo 0x172e6570 > $MEM_DUMP_PATH/register_config
    echo 0x172e6578 > $MEM_DUMP_PATH/register_config
    echo 0x172e6578 > $MEM_DUMP_PATH/register_config
    echo 0x172e6580 > $MEM_DUMP_PATH/register_config
    echo 0x172e6580 > $MEM_DUMP_PATH/register_config
    echo 0x172e6588 > $MEM_DUMP_PATH/register_config
    echo 0x172e6588 > $MEM_DUMP_PATH/register_config
    echo 0x172e6590 > $MEM_DUMP_PATH/register_config
    echo 0x172e6590 > $MEM_DUMP_PATH/register_config
    echo 0x172e6598 > $MEM_DUMP_PATH/register_config
    echo 0x172e6598 > $MEM_DUMP_PATH/register_config
    echo 0x172e65a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e65a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e65a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e65a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e65b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e65b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e65b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e65b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e65c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e65c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e65c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e65c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e65d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e65d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e65d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e65d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e65e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e65e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e65e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e65e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e65f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e65f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e65f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e65f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6600 > $MEM_DUMP_PATH/register_config
    echo 0x172e6600 > $MEM_DUMP_PATH/register_config
    echo 0x172e6608 > $MEM_DUMP_PATH/register_config
    echo 0x172e6608 > $MEM_DUMP_PATH/register_config
    echo 0x172e6610 > $MEM_DUMP_PATH/register_config
    echo 0x172e6610 > $MEM_DUMP_PATH/register_config
    echo 0x172e6618 > $MEM_DUMP_PATH/register_config
    echo 0x172e6618 > $MEM_DUMP_PATH/register_config
    echo 0x172e6620 > $MEM_DUMP_PATH/register_config
    echo 0x172e6620 > $MEM_DUMP_PATH/register_config
    echo 0x172e6628 > $MEM_DUMP_PATH/register_config
    echo 0x172e6628 > $MEM_DUMP_PATH/register_config
    echo 0x172e6630 > $MEM_DUMP_PATH/register_config
    echo 0x172e6630 > $MEM_DUMP_PATH/register_config
    echo 0x172e6638 > $MEM_DUMP_PATH/register_config
    echo 0x172e6638 > $MEM_DUMP_PATH/register_config
    echo 0x172e6640 > $MEM_DUMP_PATH/register_config
    echo 0x172e6640 > $MEM_DUMP_PATH/register_config
    echo 0x172e6648 > $MEM_DUMP_PATH/register_config
    echo 0x172e6648 > $MEM_DUMP_PATH/register_config
    echo 0x172e6650 > $MEM_DUMP_PATH/register_config
    echo 0x172e6650 > $MEM_DUMP_PATH/register_config
    echo 0x172e6658 > $MEM_DUMP_PATH/register_config
    echo 0x172e6658 > $MEM_DUMP_PATH/register_config
    echo 0x172e6660 > $MEM_DUMP_PATH/register_config
    echo 0x172e6660 > $MEM_DUMP_PATH/register_config
    echo 0x172e6668 > $MEM_DUMP_PATH/register_config
    echo 0x172e6668 > $MEM_DUMP_PATH/register_config
    echo 0x172e6670 > $MEM_DUMP_PATH/register_config
    echo 0x172e6670 > $MEM_DUMP_PATH/register_config
    echo 0x172e6678 > $MEM_DUMP_PATH/register_config
    echo 0x172e6678 > $MEM_DUMP_PATH/register_config
    echo 0x172e6680 > $MEM_DUMP_PATH/register_config
    echo 0x172e6680 > $MEM_DUMP_PATH/register_config
    echo 0x172e6688 > $MEM_DUMP_PATH/register_config
    echo 0x172e6688 > $MEM_DUMP_PATH/register_config
    echo 0x172e6690 > $MEM_DUMP_PATH/register_config
    echo 0x172e6690 > $MEM_DUMP_PATH/register_config
    echo 0x172e6698 > $MEM_DUMP_PATH/register_config
    echo 0x172e6698 > $MEM_DUMP_PATH/register_config
    echo 0x172e66a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e66a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e66a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e66a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e66b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e66b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e66b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e66b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e66c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e66c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e66c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e66c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e66d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e66d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e66d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e66d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e66e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e66e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e66e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e66e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e66f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e66f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e66f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e66f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6700 > $MEM_DUMP_PATH/register_config
    echo 0x172e6700 > $MEM_DUMP_PATH/register_config
    echo 0x172e6708 > $MEM_DUMP_PATH/register_config
    echo 0x172e6708 > $MEM_DUMP_PATH/register_config
    echo 0x172e6710 > $MEM_DUMP_PATH/register_config
    echo 0x172e6710 > $MEM_DUMP_PATH/register_config
    echo 0x172e6718 > $MEM_DUMP_PATH/register_config
    echo 0x172e6718 > $MEM_DUMP_PATH/register_config
    echo 0x172e6720 > $MEM_DUMP_PATH/register_config
    echo 0x172e6720 > $MEM_DUMP_PATH/register_config
    echo 0x172e6728 > $MEM_DUMP_PATH/register_config
    echo 0x172e6728 > $MEM_DUMP_PATH/register_config
    echo 0x172e6730 > $MEM_DUMP_PATH/register_config
    echo 0x172e6730 > $MEM_DUMP_PATH/register_config
    echo 0x172e6738 > $MEM_DUMP_PATH/register_config
    echo 0x172e6738 > $MEM_DUMP_PATH/register_config
    echo 0x172e6740 > $MEM_DUMP_PATH/register_config
    echo 0x172e6740 > $MEM_DUMP_PATH/register_config
    echo 0x172e6748 > $MEM_DUMP_PATH/register_config
    echo 0x172e6748 > $MEM_DUMP_PATH/register_config
    echo 0x172e6750 > $MEM_DUMP_PATH/register_config
    echo 0x172e6750 > $MEM_DUMP_PATH/register_config
    echo 0x172e6758 > $MEM_DUMP_PATH/register_config
    echo 0x172e6758 > $MEM_DUMP_PATH/register_config
    echo 0x172e6760 > $MEM_DUMP_PATH/register_config
    echo 0x172e6760 > $MEM_DUMP_PATH/register_config
    echo 0x172e6768 > $MEM_DUMP_PATH/register_config
    echo 0x172e6768 > $MEM_DUMP_PATH/register_config
    echo 0x172e6770 > $MEM_DUMP_PATH/register_config
    echo 0x172e6770 > $MEM_DUMP_PATH/register_config
    echo 0x172e6778 > $MEM_DUMP_PATH/register_config
    echo 0x172e6778 > $MEM_DUMP_PATH/register_config
    echo 0x172e6780 > $MEM_DUMP_PATH/register_config
    echo 0x172e6780 > $MEM_DUMP_PATH/register_config
    echo 0x172e6788 > $MEM_DUMP_PATH/register_config
    echo 0x172e6788 > $MEM_DUMP_PATH/register_config
    echo 0x172e6790 > $MEM_DUMP_PATH/register_config
    echo 0x172e6790 > $MEM_DUMP_PATH/register_config
    echo 0x172e6798 > $MEM_DUMP_PATH/register_config
    echo 0x172e6798 > $MEM_DUMP_PATH/register_config
    echo 0x172e67a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e67a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e67a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e67a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e67b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e67b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e67b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e67b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e67c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e67c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e67c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e67c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e67d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e67d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e67d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e67d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e67e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e67e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e67e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e67e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e67f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e67f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e67f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e67f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6800 > $MEM_DUMP_PATH/register_config
    echo 0x172e6800 > $MEM_DUMP_PATH/register_config
    echo 0x172e6808 > $MEM_DUMP_PATH/register_config
    echo 0x172e6808 > $MEM_DUMP_PATH/register_config
    echo 0x172e6810 > $MEM_DUMP_PATH/register_config
    echo 0x172e6810 > $MEM_DUMP_PATH/register_config
    echo 0x172e6818 > $MEM_DUMP_PATH/register_config
    echo 0x172e6818 > $MEM_DUMP_PATH/register_config
    echo 0x172e6820 > $MEM_DUMP_PATH/register_config
    echo 0x172e6820 > $MEM_DUMP_PATH/register_config
    echo 0x172e6828 > $MEM_DUMP_PATH/register_config
    echo 0x172e6828 > $MEM_DUMP_PATH/register_config
    echo 0x172e6830 > $MEM_DUMP_PATH/register_config
    echo 0x172e6830 > $MEM_DUMP_PATH/register_config
    echo 0x172e6838 > $MEM_DUMP_PATH/register_config
    echo 0x172e6838 > $MEM_DUMP_PATH/register_config
    echo 0x172e6840 > $MEM_DUMP_PATH/register_config
    echo 0x172e6840 > $MEM_DUMP_PATH/register_config
    echo 0x172e6848 > $MEM_DUMP_PATH/register_config
    echo 0x172e6848 > $MEM_DUMP_PATH/register_config
    echo 0x172e6850 > $MEM_DUMP_PATH/register_config
    echo 0x172e6850 > $MEM_DUMP_PATH/register_config
    echo 0x172e6858 > $MEM_DUMP_PATH/register_config
    echo 0x172e6858 > $MEM_DUMP_PATH/register_config
    echo 0x172e6860 > $MEM_DUMP_PATH/register_config
    echo 0x172e6860 > $MEM_DUMP_PATH/register_config
    echo 0x172e6868 > $MEM_DUMP_PATH/register_config
    echo 0x172e6868 > $MEM_DUMP_PATH/register_config
    echo 0x172e6870 > $MEM_DUMP_PATH/register_config
    echo 0x172e6870 > $MEM_DUMP_PATH/register_config
    echo 0x172e6878 > $MEM_DUMP_PATH/register_config
    echo 0x172e6878 > $MEM_DUMP_PATH/register_config
    echo 0x172e6880 > $MEM_DUMP_PATH/register_config
    echo 0x172e6880 > $MEM_DUMP_PATH/register_config
    echo 0x172e6888 > $MEM_DUMP_PATH/register_config
    echo 0x172e6888 > $MEM_DUMP_PATH/register_config
    echo 0x172e6890 > $MEM_DUMP_PATH/register_config
    echo 0x172e6890 > $MEM_DUMP_PATH/register_config
    echo 0x172e6898 > $MEM_DUMP_PATH/register_config
    echo 0x172e6898 > $MEM_DUMP_PATH/register_config
    echo 0x172e68a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e68a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e68a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e68a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e68b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e68b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e68b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e68b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e68c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e68c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e68c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e68c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e68d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e68d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e68d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e68d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e68e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e68e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e68e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e68e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e68f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e68f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e68f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e68f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6900 > $MEM_DUMP_PATH/register_config
    echo 0x172e6900 > $MEM_DUMP_PATH/register_config
    echo 0x172e6908 > $MEM_DUMP_PATH/register_config
    echo 0x172e6908 > $MEM_DUMP_PATH/register_config
    echo 0x172e6910 > $MEM_DUMP_PATH/register_config
    echo 0x172e6910 > $MEM_DUMP_PATH/register_config
    echo 0x172e6918 > $MEM_DUMP_PATH/register_config
    echo 0x172e6918 > $MEM_DUMP_PATH/register_config
    echo 0x172e6920 > $MEM_DUMP_PATH/register_config
    echo 0x172e6920 > $MEM_DUMP_PATH/register_config
    echo 0x172e6928 > $MEM_DUMP_PATH/register_config
    echo 0x172e6928 > $MEM_DUMP_PATH/register_config
    echo 0x172e6930 > $MEM_DUMP_PATH/register_config
    echo 0x172e6930 > $MEM_DUMP_PATH/register_config
    echo 0x172e6938 > $MEM_DUMP_PATH/register_config
    echo 0x172e6938 > $MEM_DUMP_PATH/register_config
    echo 0x172e6940 > $MEM_DUMP_PATH/register_config
    echo 0x172e6940 > $MEM_DUMP_PATH/register_config
    echo 0x172e6948 > $MEM_DUMP_PATH/register_config
    echo 0x172e6948 > $MEM_DUMP_PATH/register_config
    echo 0x172e6950 > $MEM_DUMP_PATH/register_config
    echo 0x172e6950 > $MEM_DUMP_PATH/register_config
    echo 0x172e6958 > $MEM_DUMP_PATH/register_config
    echo 0x172e6958 > $MEM_DUMP_PATH/register_config
    echo 0x172e6960 > $MEM_DUMP_PATH/register_config
    echo 0x172e6960 > $MEM_DUMP_PATH/register_config
    echo 0x172e6968 > $MEM_DUMP_PATH/register_config
    echo 0x172e6968 > $MEM_DUMP_PATH/register_config
    echo 0x172e6970 > $MEM_DUMP_PATH/register_config
    echo 0x172e6970 > $MEM_DUMP_PATH/register_config
    echo 0x172e6978 > $MEM_DUMP_PATH/register_config
    echo 0x172e6978 > $MEM_DUMP_PATH/register_config
    echo 0x172e6980 > $MEM_DUMP_PATH/register_config
    echo 0x172e6980 > $MEM_DUMP_PATH/register_config
    echo 0x172e6988 > $MEM_DUMP_PATH/register_config
    echo 0x172e6988 > $MEM_DUMP_PATH/register_config
    echo 0x172e6990 > $MEM_DUMP_PATH/register_config
    echo 0x172e6990 > $MEM_DUMP_PATH/register_config
    echo 0x172e6998 > $MEM_DUMP_PATH/register_config
    echo 0x172e6998 > $MEM_DUMP_PATH/register_config
    echo 0x172e69a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e69a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e69a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e69a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e69b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e69b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e69b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e69b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e69c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e69c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e69c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e69c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e69d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e69d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e69d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e69d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e69e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e69e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e69e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e69e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e69f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e69f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e69f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e69f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a00 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a00 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a08 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a08 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a10 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a10 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a18 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a18 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a20 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a20 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a28 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a28 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a30 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a30 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a38 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a38 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a40 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a40 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a48 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a48 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a50 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a50 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a58 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a58 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a60 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a60 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a68 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a68 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a70 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a70 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a78 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a78 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a80 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a80 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a88 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a88 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a90 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a90 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a98 > $MEM_DUMP_PATH/register_config
    echo 0x172e6a98 > $MEM_DUMP_PATH/register_config
    echo 0x172e6aa0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6aa0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6aa8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6aa8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ab0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ab0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ab8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ab8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ac0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ac0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ac8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ac8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ad0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ad0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ad8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ad8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ae0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ae0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ae8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ae8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6af0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6af0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6af8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6af8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b00 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b00 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b08 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b08 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b10 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b10 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b18 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b18 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b20 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b20 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b28 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b28 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b30 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b30 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b38 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b38 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b40 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b40 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b48 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b48 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b50 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b50 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b58 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b58 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b60 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b60 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b68 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b68 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b70 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b70 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b78 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b78 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b80 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b80 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b88 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b88 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b90 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b90 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b98 > $MEM_DUMP_PATH/register_config
    echo 0x172e6b98 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ba0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ba0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ba8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ba8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6bb0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6bb0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6bb8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6bb8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6bc0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6bc0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6bc8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6bc8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6bd0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6bd0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6bd8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6bd8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6be0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6be0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6be8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6be8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6bf0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6bf0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6bf8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6bf8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c00 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c00 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c08 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c08 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c10 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c10 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c18 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c18 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c20 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c20 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c28 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c28 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c30 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c30 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c38 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c38 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c40 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c40 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c48 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c48 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c50 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c50 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c58 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c58 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c60 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c60 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c68 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c68 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c70 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c70 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c78 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c78 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c80 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c80 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c88 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c88 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c90 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c90 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c98 > $MEM_DUMP_PATH/register_config
    echo 0x172e6c98 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ca0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ca0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ca8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ca8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6cb0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6cb0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6cb8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6cb8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6cc0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6cc0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6cc8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6cc8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6cd0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6cd0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6cd8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6cd8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ce0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ce0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ce8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ce8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6cf0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6cf0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6cf8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6cf8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d00 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d00 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d08 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d08 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d10 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d10 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d18 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d18 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d20 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d20 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d28 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d28 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d30 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d30 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d38 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d38 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d40 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d40 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d48 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d48 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d50 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d50 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d58 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d58 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d60 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d60 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d68 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d68 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d70 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d70 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d78 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d78 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d80 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d80 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d88 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d88 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d90 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d90 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d98 > $MEM_DUMP_PATH/register_config
    echo 0x172e6d98 > $MEM_DUMP_PATH/register_config
    echo 0x172e6da0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6da0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6da8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6da8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6db0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6db0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6db8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6db8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6dc0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6dc0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6dc8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6dc8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6dd0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6dd0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6dd8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6dd8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6de0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6de0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6de8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6de8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6df0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6df0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6df8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6df8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e00 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e00 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e08 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e08 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e10 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e10 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e18 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e18 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e20 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e20 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e28 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e28 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e30 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e30 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e38 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e38 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e40 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e40 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e48 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e48 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e50 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e50 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e58 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e58 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e60 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e60 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e68 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e68 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e70 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e70 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e78 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e78 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e80 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e80 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e88 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e88 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e90 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e90 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e98 > $MEM_DUMP_PATH/register_config
    echo 0x172e6e98 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ea0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ea0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ea8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ea8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6eb0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6eb0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6eb8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6eb8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ec0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ec0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ec8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ec8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ed0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ed0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ed8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ed8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ee0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ee0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ee8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ee8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ef0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ef0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ef8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ef8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f00 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f00 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f08 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f08 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f10 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f10 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f18 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f18 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f20 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f20 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f28 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f28 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f30 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f30 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f38 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f38 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f40 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f40 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f48 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f48 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f50 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f50 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f58 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f58 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f60 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f60 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f68 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f68 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f70 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f70 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f78 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f78 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f80 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f80 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f88 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f88 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f90 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f90 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f98 > $MEM_DUMP_PATH/register_config
    echo 0x172e6f98 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fa0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fa0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fa8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fa8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fb0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fb0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fb8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fb8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fc0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fc0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fc8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fc8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fd0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fd0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fd8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fd8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fe0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fe0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fe8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6fe8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ff0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ff0 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ff8 > $MEM_DUMP_PATH/register_config
    echo 0x172e6ff8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7000 > $MEM_DUMP_PATH/register_config
    echo 0x172e7000 > $MEM_DUMP_PATH/register_config
    echo 0x172e7008 > $MEM_DUMP_PATH/register_config
    echo 0x172e7008 > $MEM_DUMP_PATH/register_config
    echo 0x172e7010 > $MEM_DUMP_PATH/register_config
    echo 0x172e7010 > $MEM_DUMP_PATH/register_config
    echo 0x172e7018 > $MEM_DUMP_PATH/register_config
    echo 0x172e7018 > $MEM_DUMP_PATH/register_config
    echo 0x172e7020 > $MEM_DUMP_PATH/register_config
    echo 0x172e7020 > $MEM_DUMP_PATH/register_config
    echo 0x172e7028 > $MEM_DUMP_PATH/register_config
    echo 0x172e7028 > $MEM_DUMP_PATH/register_config
    echo 0x172e7030 > $MEM_DUMP_PATH/register_config
    echo 0x172e7030 > $MEM_DUMP_PATH/register_config
    echo 0x172e7038 > $MEM_DUMP_PATH/register_config
    echo 0x172e7038 > $MEM_DUMP_PATH/register_config
    echo 0x172e7040 > $MEM_DUMP_PATH/register_config
    echo 0x172e7040 > $MEM_DUMP_PATH/register_config
    echo 0x172e7048 > $MEM_DUMP_PATH/register_config
    echo 0x172e7048 > $MEM_DUMP_PATH/register_config
    echo 0x172e7050 > $MEM_DUMP_PATH/register_config
    echo 0x172e7050 > $MEM_DUMP_PATH/register_config
    echo 0x172e7058 > $MEM_DUMP_PATH/register_config
    echo 0x172e7058 > $MEM_DUMP_PATH/register_config
    echo 0x172e7060 > $MEM_DUMP_PATH/register_config
    echo 0x172e7060 > $MEM_DUMP_PATH/register_config
    echo 0x172e7068 > $MEM_DUMP_PATH/register_config
    echo 0x172e7068 > $MEM_DUMP_PATH/register_config
    echo 0x172e7070 > $MEM_DUMP_PATH/register_config
    echo 0x172e7070 > $MEM_DUMP_PATH/register_config
    echo 0x172e7078 > $MEM_DUMP_PATH/register_config
    echo 0x172e7078 > $MEM_DUMP_PATH/register_config
    echo 0x172e7080 > $MEM_DUMP_PATH/register_config
    echo 0x172e7080 > $MEM_DUMP_PATH/register_config
    echo 0x172e7088 > $MEM_DUMP_PATH/register_config
    echo 0x172e7088 > $MEM_DUMP_PATH/register_config
    echo 0x172e7090 > $MEM_DUMP_PATH/register_config
    echo 0x172e7090 > $MEM_DUMP_PATH/register_config
    echo 0x172e7098 > $MEM_DUMP_PATH/register_config
    echo 0x172e7098 > $MEM_DUMP_PATH/register_config
    echo 0x172e70a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e70a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e70a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e70a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e70b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e70b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e70b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e70b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e70c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e70c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e70c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e70c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e70d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e70d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e70d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e70d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e70e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e70e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e70e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e70e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e70f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e70f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e70f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e70f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7100 > $MEM_DUMP_PATH/register_config
    echo 0x172e7100 > $MEM_DUMP_PATH/register_config
    echo 0x172e7108 > $MEM_DUMP_PATH/register_config
    echo 0x172e7108 > $MEM_DUMP_PATH/register_config
    echo 0x172e7110 > $MEM_DUMP_PATH/register_config
    echo 0x172e7110 > $MEM_DUMP_PATH/register_config
    echo 0x172e7118 > $MEM_DUMP_PATH/register_config
    echo 0x172e7118 > $MEM_DUMP_PATH/register_config
    echo 0x172e7120 > $MEM_DUMP_PATH/register_config
    echo 0x172e7120 > $MEM_DUMP_PATH/register_config
    echo 0x172e7128 > $MEM_DUMP_PATH/register_config
    echo 0x172e7128 > $MEM_DUMP_PATH/register_config
    echo 0x172e7130 > $MEM_DUMP_PATH/register_config
    echo 0x172e7130 > $MEM_DUMP_PATH/register_config
    echo 0x172e7138 > $MEM_DUMP_PATH/register_config
    echo 0x172e7138 > $MEM_DUMP_PATH/register_config
    echo 0x172e7140 > $MEM_DUMP_PATH/register_config
    echo 0x172e7140 > $MEM_DUMP_PATH/register_config
    echo 0x172e7148 > $MEM_DUMP_PATH/register_config
    echo 0x172e7148 > $MEM_DUMP_PATH/register_config
    echo 0x172e7150 > $MEM_DUMP_PATH/register_config
    echo 0x172e7150 > $MEM_DUMP_PATH/register_config
    echo 0x172e7158 > $MEM_DUMP_PATH/register_config
    echo 0x172e7158 > $MEM_DUMP_PATH/register_config
    echo 0x172e7160 > $MEM_DUMP_PATH/register_config
    echo 0x172e7160 > $MEM_DUMP_PATH/register_config
    echo 0x172e7168 > $MEM_DUMP_PATH/register_config
    echo 0x172e7168 > $MEM_DUMP_PATH/register_config
    echo 0x172e7170 > $MEM_DUMP_PATH/register_config
    echo 0x172e7170 > $MEM_DUMP_PATH/register_config
    echo 0x172e7178 > $MEM_DUMP_PATH/register_config
    echo 0x172e7178 > $MEM_DUMP_PATH/register_config
    echo 0x172e7180 > $MEM_DUMP_PATH/register_config
    echo 0x172e7180 > $MEM_DUMP_PATH/register_config
    echo 0x172e7188 > $MEM_DUMP_PATH/register_config
    echo 0x172e7188 > $MEM_DUMP_PATH/register_config
    echo 0x172e7190 > $MEM_DUMP_PATH/register_config
    echo 0x172e7190 > $MEM_DUMP_PATH/register_config
    echo 0x172e7198 > $MEM_DUMP_PATH/register_config
    echo 0x172e7198 > $MEM_DUMP_PATH/register_config
    echo 0x172e71a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e71a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e71a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e71a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e71b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e71b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e71b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e71b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e71c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e71c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e71c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e71c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e71d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e71d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e71d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e71d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e71e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e71e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e71e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e71e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e71f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e71f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e71f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e71f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7200 > $MEM_DUMP_PATH/register_config
    echo 0x172e7200 > $MEM_DUMP_PATH/register_config
    echo 0x172e7208 > $MEM_DUMP_PATH/register_config
    echo 0x172e7208 > $MEM_DUMP_PATH/register_config
    echo 0x172e7210 > $MEM_DUMP_PATH/register_config
    echo 0x172e7210 > $MEM_DUMP_PATH/register_config
    echo 0x172e7218 > $MEM_DUMP_PATH/register_config
    echo 0x172e7218 > $MEM_DUMP_PATH/register_config
    echo 0x172e7220 > $MEM_DUMP_PATH/register_config
    echo 0x172e7220 > $MEM_DUMP_PATH/register_config
    echo 0x172e7228 > $MEM_DUMP_PATH/register_config
    echo 0x172e7228 > $MEM_DUMP_PATH/register_config
    echo 0x172e7230 > $MEM_DUMP_PATH/register_config
    echo 0x172e7230 > $MEM_DUMP_PATH/register_config
    echo 0x172e7238 > $MEM_DUMP_PATH/register_config
    echo 0x172e7238 > $MEM_DUMP_PATH/register_config
    echo 0x172e7240 > $MEM_DUMP_PATH/register_config
    echo 0x172e7240 > $MEM_DUMP_PATH/register_config
    echo 0x172e7248 > $MEM_DUMP_PATH/register_config
    echo 0x172e7248 > $MEM_DUMP_PATH/register_config
    echo 0x172e7250 > $MEM_DUMP_PATH/register_config
    echo 0x172e7250 > $MEM_DUMP_PATH/register_config
    echo 0x172e7258 > $MEM_DUMP_PATH/register_config
    echo 0x172e7258 > $MEM_DUMP_PATH/register_config
    echo 0x172e7260 > $MEM_DUMP_PATH/register_config
    echo 0x172e7260 > $MEM_DUMP_PATH/register_config
    echo 0x172e7268 > $MEM_DUMP_PATH/register_config
    echo 0x172e7268 > $MEM_DUMP_PATH/register_config
    echo 0x172e7270 > $MEM_DUMP_PATH/register_config
    echo 0x172e7270 > $MEM_DUMP_PATH/register_config
    echo 0x172e7278 > $MEM_DUMP_PATH/register_config
    echo 0x172e7278 > $MEM_DUMP_PATH/register_config
    echo 0x172e7280 > $MEM_DUMP_PATH/register_config
    echo 0x172e7280 > $MEM_DUMP_PATH/register_config
    echo 0x172e7288 > $MEM_DUMP_PATH/register_config
    echo 0x172e7288 > $MEM_DUMP_PATH/register_config
    echo 0x172e7290 > $MEM_DUMP_PATH/register_config
    echo 0x172e7290 > $MEM_DUMP_PATH/register_config
    echo 0x172e7298 > $MEM_DUMP_PATH/register_config
    echo 0x172e7298 > $MEM_DUMP_PATH/register_config
    echo 0x172e72a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e72a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e72a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e72a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e72b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e72b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e72b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e72b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e72c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e72c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e72c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e72c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e72d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e72d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e72d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e72d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e72e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e72e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e72e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e72e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e72f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e72f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e72f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e72f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7300 > $MEM_DUMP_PATH/register_config
    echo 0x172e7300 > $MEM_DUMP_PATH/register_config
    echo 0x172e7308 > $MEM_DUMP_PATH/register_config
    echo 0x172e7308 > $MEM_DUMP_PATH/register_config
    echo 0x172e7310 > $MEM_DUMP_PATH/register_config
    echo 0x172e7310 > $MEM_DUMP_PATH/register_config
    echo 0x172e7318 > $MEM_DUMP_PATH/register_config
    echo 0x172e7318 > $MEM_DUMP_PATH/register_config
    echo 0x172e7320 > $MEM_DUMP_PATH/register_config
    echo 0x172e7320 > $MEM_DUMP_PATH/register_config
    echo 0x172e7328 > $MEM_DUMP_PATH/register_config
    echo 0x172e7328 > $MEM_DUMP_PATH/register_config
    echo 0x172e7330 > $MEM_DUMP_PATH/register_config
    echo 0x172e7330 > $MEM_DUMP_PATH/register_config
    echo 0x172e7338 > $MEM_DUMP_PATH/register_config
    echo 0x172e7338 > $MEM_DUMP_PATH/register_config
    echo 0x172e7340 > $MEM_DUMP_PATH/register_config
    echo 0x172e7340 > $MEM_DUMP_PATH/register_config
    echo 0x172e7348 > $MEM_DUMP_PATH/register_config
    echo 0x172e7348 > $MEM_DUMP_PATH/register_config
    echo 0x172e7350 > $MEM_DUMP_PATH/register_config
    echo 0x172e7350 > $MEM_DUMP_PATH/register_config
    echo 0x172e7358 > $MEM_DUMP_PATH/register_config
    echo 0x172e7358 > $MEM_DUMP_PATH/register_config
    echo 0x172e7360 > $MEM_DUMP_PATH/register_config
    echo 0x172e7360 > $MEM_DUMP_PATH/register_config
    echo 0x172e7368 > $MEM_DUMP_PATH/register_config
    echo 0x172e7368 > $MEM_DUMP_PATH/register_config
    echo 0x172e7370 > $MEM_DUMP_PATH/register_config
    echo 0x172e7370 > $MEM_DUMP_PATH/register_config
    echo 0x172e7378 > $MEM_DUMP_PATH/register_config
    echo 0x172e7378 > $MEM_DUMP_PATH/register_config
    echo 0x172e7380 > $MEM_DUMP_PATH/register_config
    echo 0x172e7380 > $MEM_DUMP_PATH/register_config
    echo 0x172e7388 > $MEM_DUMP_PATH/register_config
    echo 0x172e7388 > $MEM_DUMP_PATH/register_config
    echo 0x172e7390 > $MEM_DUMP_PATH/register_config
    echo 0x172e7390 > $MEM_DUMP_PATH/register_config
    echo 0x172e7398 > $MEM_DUMP_PATH/register_config
    echo 0x172e7398 > $MEM_DUMP_PATH/register_config
    echo 0x172e73a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e73a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e73a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e73a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e73b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e73b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e73b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e73b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e73c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e73c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e73c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e73c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e73d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e73d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e73d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e73d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e73e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e73e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e73e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e73e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e73f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e73f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e73f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e73f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7400 > $MEM_DUMP_PATH/register_config
    echo 0x172e7400 > $MEM_DUMP_PATH/register_config
    echo 0x172e7408 > $MEM_DUMP_PATH/register_config
    echo 0x172e7408 > $MEM_DUMP_PATH/register_config
    echo 0x172e7410 > $MEM_DUMP_PATH/register_config
    echo 0x172e7410 > $MEM_DUMP_PATH/register_config
    echo 0x172e7418 > $MEM_DUMP_PATH/register_config
    echo 0x172e7418 > $MEM_DUMP_PATH/register_config
    echo 0x172e7420 > $MEM_DUMP_PATH/register_config
    echo 0x172e7420 > $MEM_DUMP_PATH/register_config
    echo 0x172e7428 > $MEM_DUMP_PATH/register_config
    echo 0x172e7428 > $MEM_DUMP_PATH/register_config
    echo 0x172e7430 > $MEM_DUMP_PATH/register_config
    echo 0x172e7430 > $MEM_DUMP_PATH/register_config
    echo 0x172e7438 > $MEM_DUMP_PATH/register_config
    echo 0x172e7438 > $MEM_DUMP_PATH/register_config
    echo 0x172e7440 > $MEM_DUMP_PATH/register_config
    echo 0x172e7440 > $MEM_DUMP_PATH/register_config
    echo 0x172e7448 > $MEM_DUMP_PATH/register_config
    echo 0x172e7448 > $MEM_DUMP_PATH/register_config
    echo 0x172e7450 > $MEM_DUMP_PATH/register_config
    echo 0x172e7450 > $MEM_DUMP_PATH/register_config
    echo 0x172e7458 > $MEM_DUMP_PATH/register_config
    echo 0x172e7458 > $MEM_DUMP_PATH/register_config
    echo 0x172e7460 > $MEM_DUMP_PATH/register_config
    echo 0x172e7460 > $MEM_DUMP_PATH/register_config
    echo 0x172e7468 > $MEM_DUMP_PATH/register_config
    echo 0x172e7468 > $MEM_DUMP_PATH/register_config
    echo 0x172e7470 > $MEM_DUMP_PATH/register_config
    echo 0x172e7470 > $MEM_DUMP_PATH/register_config
    echo 0x172e7478 > $MEM_DUMP_PATH/register_config
    echo 0x172e7478 > $MEM_DUMP_PATH/register_config
    echo 0x172e7480 > $MEM_DUMP_PATH/register_config
    echo 0x172e7480 > $MEM_DUMP_PATH/register_config
    echo 0x172e7488 > $MEM_DUMP_PATH/register_config
    echo 0x172e7488 > $MEM_DUMP_PATH/register_config
    echo 0x172e7490 > $MEM_DUMP_PATH/register_config
    echo 0x172e7490 > $MEM_DUMP_PATH/register_config
    echo 0x172e7498 > $MEM_DUMP_PATH/register_config
    echo 0x172e7498 > $MEM_DUMP_PATH/register_config
    echo 0x172e74a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e74a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e74a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e74a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e74b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e74b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e74b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e74b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e74c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e74c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e74c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e74c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e74d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e74d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e74d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e74d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e74e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e74e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e74e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e74e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e74f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e74f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e74f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e74f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7500 > $MEM_DUMP_PATH/register_config
    echo 0x172e7500 > $MEM_DUMP_PATH/register_config
    echo 0x172e7508 > $MEM_DUMP_PATH/register_config
    echo 0x172e7508 > $MEM_DUMP_PATH/register_config
    echo 0x172e7510 > $MEM_DUMP_PATH/register_config
    echo 0x172e7510 > $MEM_DUMP_PATH/register_config
    echo 0x172e7518 > $MEM_DUMP_PATH/register_config
    echo 0x172e7518 > $MEM_DUMP_PATH/register_config
    echo 0x172e7520 > $MEM_DUMP_PATH/register_config
    echo 0x172e7520 > $MEM_DUMP_PATH/register_config
    echo 0x172e7528 > $MEM_DUMP_PATH/register_config
    echo 0x172e7528 > $MEM_DUMP_PATH/register_config
    echo 0x172e7530 > $MEM_DUMP_PATH/register_config
    echo 0x172e7530 > $MEM_DUMP_PATH/register_config
    echo 0x172e7538 > $MEM_DUMP_PATH/register_config
    echo 0x172e7538 > $MEM_DUMP_PATH/register_config
    echo 0x172e7540 > $MEM_DUMP_PATH/register_config
    echo 0x172e7540 > $MEM_DUMP_PATH/register_config
    echo 0x172e7548 > $MEM_DUMP_PATH/register_config
    echo 0x172e7548 > $MEM_DUMP_PATH/register_config
    echo 0x172e7550 > $MEM_DUMP_PATH/register_config
    echo 0x172e7550 > $MEM_DUMP_PATH/register_config
    echo 0x172e7558 > $MEM_DUMP_PATH/register_config
    echo 0x172e7558 > $MEM_DUMP_PATH/register_config
    echo 0x172e7560 > $MEM_DUMP_PATH/register_config
    echo 0x172e7560 > $MEM_DUMP_PATH/register_config
    echo 0x172e7568 > $MEM_DUMP_PATH/register_config
    echo 0x172e7568 > $MEM_DUMP_PATH/register_config
    echo 0x172e7570 > $MEM_DUMP_PATH/register_config
    echo 0x172e7570 > $MEM_DUMP_PATH/register_config
    echo 0x172e7578 > $MEM_DUMP_PATH/register_config
    echo 0x172e7578 > $MEM_DUMP_PATH/register_config
    echo 0x172e7580 > $MEM_DUMP_PATH/register_config
    echo 0x172e7580 > $MEM_DUMP_PATH/register_config
    echo 0x172e7588 > $MEM_DUMP_PATH/register_config
    echo 0x172e7588 > $MEM_DUMP_PATH/register_config
    echo 0x172e7590 > $MEM_DUMP_PATH/register_config
    echo 0x172e7590 > $MEM_DUMP_PATH/register_config
    echo 0x172e7598 > $MEM_DUMP_PATH/register_config
    echo 0x172e7598 > $MEM_DUMP_PATH/register_config
    echo 0x172e75a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e75a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e75a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e75a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e75b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e75b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e75b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e75b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e75c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e75c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e75c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e75c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e75d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e75d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e75d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e75d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e75e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e75e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e75e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e75e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e75f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e75f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e75f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e75f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7600 > $MEM_DUMP_PATH/register_config
    echo 0x172e7600 > $MEM_DUMP_PATH/register_config
    echo 0x172e7608 > $MEM_DUMP_PATH/register_config
    echo 0x172e7608 > $MEM_DUMP_PATH/register_config
    echo 0x172e7610 > $MEM_DUMP_PATH/register_config
    echo 0x172e7610 > $MEM_DUMP_PATH/register_config
    echo 0x172e7618 > $MEM_DUMP_PATH/register_config
    echo 0x172e7618 > $MEM_DUMP_PATH/register_config
    echo 0x172e7620 > $MEM_DUMP_PATH/register_config
    echo 0x172e7620 > $MEM_DUMP_PATH/register_config
    echo 0x172e7628 > $MEM_DUMP_PATH/register_config
    echo 0x172e7628 > $MEM_DUMP_PATH/register_config
    echo 0x172e7630 > $MEM_DUMP_PATH/register_config
    echo 0x172e7630 > $MEM_DUMP_PATH/register_config
    echo 0x172e7638 > $MEM_DUMP_PATH/register_config
    echo 0x172e7638 > $MEM_DUMP_PATH/register_config
    echo 0x172e7640 > $MEM_DUMP_PATH/register_config
    echo 0x172e7640 > $MEM_DUMP_PATH/register_config
    echo 0x172e7648 > $MEM_DUMP_PATH/register_config
    echo 0x172e7648 > $MEM_DUMP_PATH/register_config
    echo 0x172e7650 > $MEM_DUMP_PATH/register_config
    echo 0x172e7650 > $MEM_DUMP_PATH/register_config
    echo 0x172e7658 > $MEM_DUMP_PATH/register_config
    echo 0x172e7658 > $MEM_DUMP_PATH/register_config
    echo 0x172e7660 > $MEM_DUMP_PATH/register_config
    echo 0x172e7660 > $MEM_DUMP_PATH/register_config
    echo 0x172e7668 > $MEM_DUMP_PATH/register_config
    echo 0x172e7668 > $MEM_DUMP_PATH/register_config
    echo 0x172e7670 > $MEM_DUMP_PATH/register_config
    echo 0x172e7670 > $MEM_DUMP_PATH/register_config
    echo 0x172e7678 > $MEM_DUMP_PATH/register_config
    echo 0x172e7678 > $MEM_DUMP_PATH/register_config
    echo 0x172e7680 > $MEM_DUMP_PATH/register_config
    echo 0x172e7680 > $MEM_DUMP_PATH/register_config
    echo 0x172e7688 > $MEM_DUMP_PATH/register_config
    echo 0x172e7688 > $MEM_DUMP_PATH/register_config
    echo 0x172e7690 > $MEM_DUMP_PATH/register_config
    echo 0x172e7690 > $MEM_DUMP_PATH/register_config
    echo 0x172e7698 > $MEM_DUMP_PATH/register_config
    echo 0x172e7698 > $MEM_DUMP_PATH/register_config
    echo 0x172e76a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e76a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e76a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e76a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e76b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e76b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e76b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e76b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e76c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e76c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e76c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e76c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e76d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e76d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e76d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e76d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e76e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e76e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e76e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e76e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e76f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e76f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e76f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e76f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7700 > $MEM_DUMP_PATH/register_config
    echo 0x172e7700 > $MEM_DUMP_PATH/register_config
    echo 0x172e7708 > $MEM_DUMP_PATH/register_config
    echo 0x172e7708 > $MEM_DUMP_PATH/register_config
    echo 0x172e7710 > $MEM_DUMP_PATH/register_config
    echo 0x172e7710 > $MEM_DUMP_PATH/register_config
    echo 0x172e7718 > $MEM_DUMP_PATH/register_config
    echo 0x172e7718 > $MEM_DUMP_PATH/register_config
    echo 0x172e7720 > $MEM_DUMP_PATH/register_config
    echo 0x172e7720 > $MEM_DUMP_PATH/register_config
    echo 0x172e7728 > $MEM_DUMP_PATH/register_config
    echo 0x172e7728 > $MEM_DUMP_PATH/register_config
    echo 0x172e7730 > $MEM_DUMP_PATH/register_config
    echo 0x172e7730 > $MEM_DUMP_PATH/register_config
    echo 0x172e7738 > $MEM_DUMP_PATH/register_config
    echo 0x172e7738 > $MEM_DUMP_PATH/register_config
    echo 0x172e7740 > $MEM_DUMP_PATH/register_config
    echo 0x172e7740 > $MEM_DUMP_PATH/register_config
    echo 0x172e7748 > $MEM_DUMP_PATH/register_config
    echo 0x172e7748 > $MEM_DUMP_PATH/register_config
    echo 0x172e7750 > $MEM_DUMP_PATH/register_config
    echo 0x172e7750 > $MEM_DUMP_PATH/register_config
    echo 0x172e7758 > $MEM_DUMP_PATH/register_config
    echo 0x172e7758 > $MEM_DUMP_PATH/register_config
    echo 0x172e7760 > $MEM_DUMP_PATH/register_config
    echo 0x172e7760 > $MEM_DUMP_PATH/register_config
    echo 0x172e7768 > $MEM_DUMP_PATH/register_config
    echo 0x172e7768 > $MEM_DUMP_PATH/register_config
    echo 0x172e7770 > $MEM_DUMP_PATH/register_config
    echo 0x172e7770 > $MEM_DUMP_PATH/register_config
    echo 0x172e7778 > $MEM_DUMP_PATH/register_config
    echo 0x172e7778 > $MEM_DUMP_PATH/register_config
    echo 0x172e7780 > $MEM_DUMP_PATH/register_config
    echo 0x172e7780 > $MEM_DUMP_PATH/register_config
    echo 0x172e7788 > $MEM_DUMP_PATH/register_config
    echo 0x172e7788 > $MEM_DUMP_PATH/register_config
    echo 0x172e7790 > $MEM_DUMP_PATH/register_config
    echo 0x172e7790 > $MEM_DUMP_PATH/register_config
    echo 0x172e7798 > $MEM_DUMP_PATH/register_config
    echo 0x172e7798 > $MEM_DUMP_PATH/register_config
    echo 0x172e77a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e77a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e77a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e77a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e77b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e77b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e77b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e77b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e77c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e77c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e77c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e77c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e77d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e77d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e77d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e77d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e77e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e77e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e77e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e77e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e77f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e77f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e77f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e77f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7800 > $MEM_DUMP_PATH/register_config
    echo 0x172e7800 > $MEM_DUMP_PATH/register_config
    echo 0x172e7808 > $MEM_DUMP_PATH/register_config
    echo 0x172e7808 > $MEM_DUMP_PATH/register_config
    echo 0x172e7810 > $MEM_DUMP_PATH/register_config
    echo 0x172e7810 > $MEM_DUMP_PATH/register_config
    echo 0x172e7818 > $MEM_DUMP_PATH/register_config
    echo 0x172e7818 > $MEM_DUMP_PATH/register_config
    echo 0x172e7820 > $MEM_DUMP_PATH/register_config
    echo 0x172e7820 > $MEM_DUMP_PATH/register_config
    echo 0x172e7828 > $MEM_DUMP_PATH/register_config
    echo 0x172e7828 > $MEM_DUMP_PATH/register_config
    echo 0x172e7830 > $MEM_DUMP_PATH/register_config
    echo 0x172e7830 > $MEM_DUMP_PATH/register_config
    echo 0x172e7838 > $MEM_DUMP_PATH/register_config
    echo 0x172e7838 > $MEM_DUMP_PATH/register_config
    echo 0x172e7840 > $MEM_DUMP_PATH/register_config
    echo 0x172e7840 > $MEM_DUMP_PATH/register_config
    echo 0x172e7848 > $MEM_DUMP_PATH/register_config
    echo 0x172e7848 > $MEM_DUMP_PATH/register_config
    echo 0x172e7850 > $MEM_DUMP_PATH/register_config
    echo 0x172e7850 > $MEM_DUMP_PATH/register_config
    echo 0x172e7858 > $MEM_DUMP_PATH/register_config
    echo 0x172e7858 > $MEM_DUMP_PATH/register_config
    echo 0x172e7860 > $MEM_DUMP_PATH/register_config
    echo 0x172e7860 > $MEM_DUMP_PATH/register_config
    echo 0x172e7868 > $MEM_DUMP_PATH/register_config
    echo 0x172e7868 > $MEM_DUMP_PATH/register_config
    echo 0x172e7870 > $MEM_DUMP_PATH/register_config
    echo 0x172e7870 > $MEM_DUMP_PATH/register_config
    echo 0x172e7878 > $MEM_DUMP_PATH/register_config
    echo 0x172e7878 > $MEM_DUMP_PATH/register_config
    echo 0x172e7880 > $MEM_DUMP_PATH/register_config
    echo 0x172e7880 > $MEM_DUMP_PATH/register_config
    echo 0x172e7888 > $MEM_DUMP_PATH/register_config
    echo 0x172e7888 > $MEM_DUMP_PATH/register_config
    echo 0x172e7890 > $MEM_DUMP_PATH/register_config
    echo 0x172e7890 > $MEM_DUMP_PATH/register_config
    echo 0x172e7898 > $MEM_DUMP_PATH/register_config
    echo 0x172e7898 > $MEM_DUMP_PATH/register_config
    echo 0x172e78a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e78a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e78a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e78a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e78b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e78b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e78b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e78b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e78c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e78c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e78c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e78c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e78d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e78d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e78d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e78d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e78e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e78e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e78e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e78e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e78f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e78f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e78f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e78f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7900 > $MEM_DUMP_PATH/register_config
    echo 0x172e7900 > $MEM_DUMP_PATH/register_config
    echo 0x172e7908 > $MEM_DUMP_PATH/register_config
    echo 0x172e7908 > $MEM_DUMP_PATH/register_config
    echo 0x172e7910 > $MEM_DUMP_PATH/register_config
    echo 0x172e7910 > $MEM_DUMP_PATH/register_config
    echo 0x172e7918 > $MEM_DUMP_PATH/register_config
    echo 0x172e7918 > $MEM_DUMP_PATH/register_config
    echo 0x172e7920 > $MEM_DUMP_PATH/register_config
    echo 0x172e7920 > $MEM_DUMP_PATH/register_config
    echo 0x172e7928 > $MEM_DUMP_PATH/register_config
    echo 0x172e7928 > $MEM_DUMP_PATH/register_config
    echo 0x172e7930 > $MEM_DUMP_PATH/register_config
    echo 0x172e7930 > $MEM_DUMP_PATH/register_config
    echo 0x172e7938 > $MEM_DUMP_PATH/register_config
    echo 0x172e7938 > $MEM_DUMP_PATH/register_config
    echo 0x172e7940 > $MEM_DUMP_PATH/register_config
    echo 0x172e7940 > $MEM_DUMP_PATH/register_config
    echo 0x172e7948 > $MEM_DUMP_PATH/register_config
    echo 0x172e7948 > $MEM_DUMP_PATH/register_config
    echo 0x172e7950 > $MEM_DUMP_PATH/register_config
    echo 0x172e7950 > $MEM_DUMP_PATH/register_config
    echo 0x172e7958 > $MEM_DUMP_PATH/register_config
    echo 0x172e7958 > $MEM_DUMP_PATH/register_config
    echo 0x172e7960 > $MEM_DUMP_PATH/register_config
    echo 0x172e7960 > $MEM_DUMP_PATH/register_config
    echo 0x172e7968 > $MEM_DUMP_PATH/register_config
    echo 0x172e7968 > $MEM_DUMP_PATH/register_config
    echo 0x172e7970 > $MEM_DUMP_PATH/register_config
    echo 0x172e7970 > $MEM_DUMP_PATH/register_config
    echo 0x172e7978 > $MEM_DUMP_PATH/register_config
    echo 0x172e7978 > $MEM_DUMP_PATH/register_config
    echo 0x172e7980 > $MEM_DUMP_PATH/register_config
    echo 0x172e7980 > $MEM_DUMP_PATH/register_config
    echo 0x172e7988 > $MEM_DUMP_PATH/register_config
    echo 0x172e7988 > $MEM_DUMP_PATH/register_config
    echo 0x172e7990 > $MEM_DUMP_PATH/register_config
    echo 0x172e7990 > $MEM_DUMP_PATH/register_config
    echo 0x172e7998 > $MEM_DUMP_PATH/register_config
    echo 0x172e7998 > $MEM_DUMP_PATH/register_config
    echo 0x172e79a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e79a0 > $MEM_DUMP_PATH/register_config
    echo 0x172e79a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e79a8 > $MEM_DUMP_PATH/register_config
    echo 0x172e79b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e79b0 > $MEM_DUMP_PATH/register_config
    echo 0x172e79b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e79b8 > $MEM_DUMP_PATH/register_config
    echo 0x172e79c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e79c0 > $MEM_DUMP_PATH/register_config
    echo 0x172e79c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e79c8 > $MEM_DUMP_PATH/register_config
    echo 0x172e79d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e79d0 > $MEM_DUMP_PATH/register_config
    echo 0x172e79d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e79d8 > $MEM_DUMP_PATH/register_config
    echo 0x172e79e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e79e0 > $MEM_DUMP_PATH/register_config
    echo 0x172e79e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e79e8 > $MEM_DUMP_PATH/register_config
    echo 0x172e79f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e79f0 > $MEM_DUMP_PATH/register_config
    echo 0x172e79f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e79f8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a00 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a00 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a08 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a08 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a10 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a10 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a18 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a18 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a20 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a20 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a28 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a28 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a30 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a30 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a38 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a38 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a40 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a40 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a48 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a48 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a50 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a50 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a58 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a58 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a60 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a60 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a68 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a68 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a70 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a70 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a78 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a78 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a80 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a80 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a88 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a88 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a90 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a90 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a98 > $MEM_DUMP_PATH/register_config
    echo 0x172e7a98 > $MEM_DUMP_PATH/register_config
    echo 0x172e7aa0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7aa0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7aa8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7aa8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ab0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ab0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ab8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ab8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ac0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ac0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ac8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ac8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ad0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ad0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ad8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ad8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ae0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ae0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ae8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ae8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7af0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7af0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7af8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7af8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b00 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b00 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b08 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b08 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b10 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b10 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b18 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b18 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b20 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b20 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b28 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b28 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b30 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b30 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b38 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b38 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b40 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b40 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b48 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b48 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b50 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b50 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b58 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b58 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b60 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b60 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b68 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b68 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b70 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b70 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b78 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b78 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b80 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b80 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b88 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b88 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b90 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b90 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b98 > $MEM_DUMP_PATH/register_config
    echo 0x172e7b98 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ba0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ba0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ba8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ba8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7bb0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7bb0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7bb8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7bb8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7bc0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7bc0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7bc8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7bc8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7bd0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7bd0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7bd8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7bd8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7be0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7be0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7be8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7be8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7bf0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7bf0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7bf8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7bf8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c00 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c00 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c08 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c08 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c10 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c10 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c18 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c18 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c20 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c20 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c28 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c28 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c30 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c30 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c38 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c38 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c40 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c40 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c48 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c48 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c50 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c50 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c58 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c58 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c60 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c60 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c68 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c68 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c70 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c70 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c78 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c78 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c80 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c80 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c88 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c88 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c90 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c90 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c98 > $MEM_DUMP_PATH/register_config
    echo 0x172e7c98 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ca0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ca0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ca8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ca8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7cb0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7cb0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7cb8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7cb8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7cc0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7cc0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7cc8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7cc8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7cd0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7cd0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7cd8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7cd8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ce0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ce0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ce8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7ce8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7cf0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7cf0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7cf8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7cf8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d00 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d00 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d08 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d08 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d10 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d10 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d18 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d18 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d20 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d20 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d28 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d28 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d30 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d30 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d38 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d38 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d40 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d40 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d48 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d48 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d50 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d50 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d58 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d58 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d60 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d60 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d68 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d68 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d70 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d70 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d78 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d78 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d80 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d80 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d88 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d88 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d90 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d90 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d98 > $MEM_DUMP_PATH/register_config
    echo 0x172e7d98 > $MEM_DUMP_PATH/register_config
    echo 0x172e7da0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7da0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7da8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7da8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7db0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7db0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7db8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7db8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7dc0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7dc0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7dc8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7dc8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7dd0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7dd0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7dd8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7dd8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7de0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7de0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7de8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7de8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7df0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7df0 > $MEM_DUMP_PATH/register_config
    echo 0x172e7df8 > $MEM_DUMP_PATH/register_config
    echo 0x172e7df8 > $MEM_DUMP_PATH/register_config
    echo 0x172ee008 58 > $MEM_DUMP_PATH/register_config
    echo 0x172ee104 29 > $MEM_DUMP_PATH/register_config
    echo 0x172ee104 29 > $MEM_DUMP_PATH/register_config
    echo 0x172ef000 > $MEM_DUMP_PATH/register_config
    echo 0x172effd0 12 > $MEM_DUMP_PATH/register_config
    echo 0x17400004 4 > $MEM_DUMP_PATH/register_config
    echo 0x17400038 2 > $MEM_DUMP_PATH/register_config
    echo 0x17400044 3 > $MEM_DUMP_PATH/register_config
    echo 0x17400044 3 > $MEM_DUMP_PATH/register_config
    echo 0x174000f0 29 > $MEM_DUMP_PATH/register_config
    echo 0x17400200 25 > $MEM_DUMP_PATH/register_config
    echo 0x17400200 25 > $MEM_DUMP_PATH/register_config
    echo 0x17400438 > $MEM_DUMP_PATH/register_config
    echo 0x17400444 > $MEM_DUMP_PATH/register_config
    echo 0x17400444 > $MEM_DUMP_PATH/register_config
    echo 0x17410000 4 > $MEM_DUMP_PATH/register_config
    echo 0x1741000c 7 > $MEM_DUMP_PATH/register_config
    echo 0x17410020 2 > $MEM_DUMP_PATH/register_config
    echo 0x17411000 4 > $MEM_DUMP_PATH/register_config
    echo 0x1741100c 7 > $MEM_DUMP_PATH/register_config
    echo 0x17411020 2 > $MEM_DUMP_PATH/register_config
    echo 0x17420000 3 > $MEM_DUMP_PATH/register_config
    echo 0x17420040 7 > $MEM_DUMP_PATH/register_config
    echo 0x17420080 12 > $MEM_DUMP_PATH/register_config
    echo 0x17420080 14 > $MEM_DUMP_PATH/register_config
    echo 0x17420fc0 > $MEM_DUMP_PATH/register_config
    echo 0x17420fd0 > $MEM_DUMP_PATH/register_config
    echo 0x17420fe0 > $MEM_DUMP_PATH/register_config
    echo 0x17420ff0 > $MEM_DUMP_PATH/register_config
    echo 0x17421000 16 > $MEM_DUMP_PATH/register_config
    echo 0x17421fd0 > $MEM_DUMP_PATH/register_config
    echo 0x17422000 5 > $MEM_DUMP_PATH/register_config
    echo 0x17422020 8 > $MEM_DUMP_PATH/register_config
    echo 0x17422020 8 > $MEM_DUMP_PATH/register_config
    echo 0x17422fd0 > $MEM_DUMP_PATH/register_config
    echo 0x17423000 16 > $MEM_DUMP_PATH/register_config
    echo 0x17423fd0 > $MEM_DUMP_PATH/register_config
    echo 0x17425000 16 > $MEM_DUMP_PATH/register_config
    echo 0x17425fd0 > $MEM_DUMP_PATH/register_config
    echo 0x17426000 5 > $MEM_DUMP_PATH/register_config
    echo 0x17426020 8 > $MEM_DUMP_PATH/register_config
    echo 0x17426020 8 > $MEM_DUMP_PATH/register_config
    echo 0x17426fd0 > $MEM_DUMP_PATH/register_config
    echo 0x17427000 16 > $MEM_DUMP_PATH/register_config
    echo 0x17427fd0 > $MEM_DUMP_PATH/register_config
    echo 0x17429000 16 > $MEM_DUMP_PATH/register_config
    echo 0x17429fd0 > $MEM_DUMP_PATH/register_config
    echo 0x1742b000 16 > $MEM_DUMP_PATH/register_config
    echo 0x1742bfd0 > $MEM_DUMP_PATH/register_config
    echo 0x1742d000 16 > $MEM_DUMP_PATH/register_config
    echo 0x1742dfd0 > $MEM_DUMP_PATH/register_config
    echo 0x17600004 2 > $MEM_DUMP_PATH/register_config
    echo 0x17600010 5 > $MEM_DUMP_PATH/register_config
    echo 0x17600010 10 > $MEM_DUMP_PATH/register_config
    echo 0x17600040 2 > $MEM_DUMP_PATH/register_config
    echo 0x17600050 3 > $MEM_DUMP_PATH/register_config
    echo 0x17600040 2 > $MEM_DUMP_PATH/register_config
    echo 0x17600050 3 > $MEM_DUMP_PATH/register_config
    echo 0x17600050 3 > $MEM_DUMP_PATH/register_config
    echo 0x17600160 2 > $MEM_DUMP_PATH/register_config
    echo 0x17600174 2 > $MEM_DUMP_PATH/register_config
    echo 0x17600174 2 > $MEM_DUMP_PATH/register_config
    echo 0x17600180 5 > $MEM_DUMP_PATH/register_config
    echo 0x17600180 8 > $MEM_DUMP_PATH/register_config
    echo 0x17600204 2 > $MEM_DUMP_PATH/register_config
    echo 0x17600210 2 > $MEM_DUMP_PATH/register_config
    echo 0x17600220 > $MEM_DUMP_PATH/register_config
    echo 0x17600210 2 > $MEM_DUMP_PATH/register_config
    echo 0x17600220 3 > $MEM_DUMP_PATH/register_config
    echo 0x17600220 3 > $MEM_DUMP_PATH/register_config
    echo 0x17600230 3 > $MEM_DUMP_PATH/register_config
    echo 0x17600240 4 > $MEM_DUMP_PATH/register_config
    echo 0x17600230 3 > $MEM_DUMP_PATH/register_config
    echo 0x17600240 4 > $MEM_DUMP_PATH/register_config
    echo 0x1760025c > $MEM_DUMP_PATH/register_config
    echo 0x17600240 4 > $MEM_DUMP_PATH/register_config
    echo 0x1760025c 4 > $MEM_DUMP_PATH/register_config
    echo 0x17600270 4 > $MEM_DUMP_PATH/register_config
    echo 0x1760025c 4 > $MEM_DUMP_PATH/register_config
    echo 0x17600270 4 > $MEM_DUMP_PATH/register_config
    echo 0x17600270 4 > $MEM_DUMP_PATH/register_config
    echo 0x176002a4 > $MEM_DUMP_PATH/register_config
    echo 0x176002ac > $MEM_DUMP_PATH/register_config
    echo 0x176002a4 > $MEM_DUMP_PATH/register_config
    echo 0x176002ac > $MEM_DUMP_PATH/register_config
    echo 0x176002ac > $MEM_DUMP_PATH/register_config
    echo 0x176002b4 2 > $MEM_DUMP_PATH/register_config
    echo 0x176002b4 2 > $MEM_DUMP_PATH/register_config
    echo 0x17600404 > $MEM_DUMP_PATH/register_config
    echo 0x1760041c 2 > $MEM_DUMP_PATH/register_config
    echo 0x17600434 > $MEM_DUMP_PATH/register_config
    echo 0x17600434 > $MEM_DUMP_PATH/register_config
    echo 0x1760043c 2 > $MEM_DUMP_PATH/register_config
    echo 0x1760043c 2 > $MEM_DUMP_PATH/register_config
    echo 0x17600448 > $MEM_DUMP_PATH/register_config
    echo 0x17600448 40 > $MEM_DUMP_PATH/register_config
    echo 0x17600474 36 > $MEM_DUMP_PATH/register_config
    echo 0x17600514 4 > $MEM_DUMP_PATH/register_config
    echo 0x17600534 5 > $MEM_DUMP_PATH/register_config
    echo 0x17600558 35 > $MEM_DUMP_PATH/register_config
    echo 0x176004d4 12 > $MEM_DUMP_PATH/register_config
    echo 0x17600514 4 > $MEM_DUMP_PATH/register_config
    echo 0x17600534 5 > $MEM_DUMP_PATH/register_config
    echo 0x17600558 15 > $MEM_DUMP_PATH/register_config
    echo 0x17600514 4 > $MEM_DUMP_PATH/register_config
    echo 0x17600534 5 > $MEM_DUMP_PATH/register_config
    echo 0x17600534 5 > $MEM_DUMP_PATH/register_config
    echo 0x17600558 11 > $MEM_DUMP_PATH/register_config
    echo 0x17600558 37 > $MEM_DUMP_PATH/register_config
    echo 0x176005fc 2 > $MEM_DUMP_PATH/register_config
    echo 0x17600608 4 > $MEM_DUMP_PATH/register_config
    echo 0x1760061c 8 > $MEM_DUMP_PATH/register_config
    echo 0x17600640 6 > $MEM_DUMP_PATH/register_config
    echo 0x17600660 5 > $MEM_DUMP_PATH/register_config
    echo 0x176005fc 2 > $MEM_DUMP_PATH/register_config
    echo 0x17600608 4 > $MEM_DUMP_PATH/register_config
    echo 0x17600608 4 > $MEM_DUMP_PATH/register_config
    echo 0x1760061c 8 > $MEM_DUMP_PATH/register_config
    echo 0x17600640 2 > $MEM_DUMP_PATH/register_config
    echo 0x1760061c 8 > $MEM_DUMP_PATH/register_config
    echo 0x17600640 6 > $MEM_DUMP_PATH/register_config
    echo 0x17600660 5 > $MEM_DUMP_PATH/register_config
    echo 0x17600640 6 > $MEM_DUMP_PATH/register_config
    echo 0x17600660 5 > $MEM_DUMP_PATH/register_config
    echo 0x17600660 5 > $MEM_DUMP_PATH/register_config
    echo 0x17606000 > $MEM_DUMP_PATH/register_config
    echo 0x17607000 > $MEM_DUMP_PATH/register_config
    echo 0x17800000 > $MEM_DUMP_PATH/register_config
    echo 0x17800008 2 > $MEM_DUMP_PATH/register_config
    echo 0x17800008 18 > $MEM_DUMP_PATH/register_config
    echo 0x17800054 7 > $MEM_DUMP_PATH/register_config
    echo 0x178000f0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17800048 2 > $MEM_DUMP_PATH/register_config
    echo 0x17800054 5 > $MEM_DUMP_PATH/register_config
    echo 0x17800054 7 > $MEM_DUMP_PATH/register_config
    echo 0x178000f0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17810000 > $MEM_DUMP_PATH/register_config
    echo 0x17810008 2 > $MEM_DUMP_PATH/register_config
    echo 0x17810008 18 > $MEM_DUMP_PATH/register_config
    echo 0x17810054 7 > $MEM_DUMP_PATH/register_config
    echo 0x178100f0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17810048 2 > $MEM_DUMP_PATH/register_config
    echo 0x17810054 5 > $MEM_DUMP_PATH/register_config
    echo 0x17810054 7 > $MEM_DUMP_PATH/register_config
    echo 0x178100f0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17820000 > $MEM_DUMP_PATH/register_config
    echo 0x17820008 2 > $MEM_DUMP_PATH/register_config
    echo 0x17820008 18 > $MEM_DUMP_PATH/register_config
    echo 0x17820054 7 > $MEM_DUMP_PATH/register_config
    echo 0x178200f0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17820048 2 > $MEM_DUMP_PATH/register_config
    echo 0x17820054 5 > $MEM_DUMP_PATH/register_config
    echo 0x17820054 7 > $MEM_DUMP_PATH/register_config
    echo 0x178200f0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17830000 > $MEM_DUMP_PATH/register_config
    echo 0x17830008 2 > $MEM_DUMP_PATH/register_config
    echo 0x17830008 18 > $MEM_DUMP_PATH/register_config
    echo 0x17830054 7 > $MEM_DUMP_PATH/register_config
    echo 0x178300f0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17830048 2 > $MEM_DUMP_PATH/register_config
    echo 0x17830054 5 > $MEM_DUMP_PATH/register_config
    echo 0x17830054 7 > $MEM_DUMP_PATH/register_config
    echo 0x178300f0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17880000 > $MEM_DUMP_PATH/register_config
    echo 0x17880008 2 > $MEM_DUMP_PATH/register_config
    echo 0x17880008 18 > $MEM_DUMP_PATH/register_config
    echo 0x17880054 13 > $MEM_DUMP_PATH/register_config
    echo 0x17880094 29 > $MEM_DUMP_PATH/register_config
    echo 0x17880054 13 > $MEM_DUMP_PATH/register_config
    echo 0x17880094 29 > $MEM_DUMP_PATH/register_config
    echo 0x17880094 29 > $MEM_DUMP_PATH/register_config
    echo 0x17880134 2 > $MEM_DUMP_PATH/register_config
    echo 0x17880140 2 > $MEM_DUMP_PATH/register_config
    echo 0x1788019c > $MEM_DUMP_PATH/register_config
    echo 0x178801b4 > $MEM_DUMP_PATH/register_config
    echo 0x178801bc 4 > $MEM_DUMP_PATH/register_config
    echo 0x178801f0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17880250 4 > $MEM_DUMP_PATH/register_config
    echo 0x17880134 2 > $MEM_DUMP_PATH/register_config
    echo 0x17880140 2 > $MEM_DUMP_PATH/register_config
    echo 0x17880140 2 > $MEM_DUMP_PATH/register_config
    echo 0x1788019c > $MEM_DUMP_PATH/register_config
    echo 0x178801b4 > $MEM_DUMP_PATH/register_config
    echo 0x178801bc 2 > $MEM_DUMP_PATH/register_config
    echo 0x178801bc 4 > $MEM_DUMP_PATH/register_config
    echo 0x178801f0 3 > $MEM_DUMP_PATH/register_config
    echo 0x178801f0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17880250 4 > $MEM_DUMP_PATH/register_config
    echo 0x178c0000 146 > $MEM_DUMP_PATH/register_config
    echo 0x178c8000 > $MEM_DUMP_PATH/register_config
    echo 0x178c8008 > $MEM_DUMP_PATH/register_config
    echo 0x178c8008 > $MEM_DUMP_PATH/register_config
    echo 0x178c8010 > $MEM_DUMP_PATH/register_config
    echo 0x178c8010 > $MEM_DUMP_PATH/register_config
    echo 0x178c8018 > $MEM_DUMP_PATH/register_config
    echo 0x178c8018 > $MEM_DUMP_PATH/register_config
    echo 0x178c8020 > $MEM_DUMP_PATH/register_config
    echo 0x178c8020 > $MEM_DUMP_PATH/register_config
    echo 0x178c8028 > $MEM_DUMP_PATH/register_config
    echo 0x178c8028 > $MEM_DUMP_PATH/register_config
    echo 0x178c8030 > $MEM_DUMP_PATH/register_config
    echo 0x178c8030 > $MEM_DUMP_PATH/register_config
    echo 0x178c8038 > $MEM_DUMP_PATH/register_config
    echo 0x178c8038 > $MEM_DUMP_PATH/register_config
    echo 0x178c8040 > $MEM_DUMP_PATH/register_config
    echo 0x178c8040 > $MEM_DUMP_PATH/register_config
    echo 0x178c8048 > $MEM_DUMP_PATH/register_config
    echo 0x178c8048 > $MEM_DUMP_PATH/register_config
    echo 0x178c8050 > $MEM_DUMP_PATH/register_config
    echo 0x178c8050 > $MEM_DUMP_PATH/register_config
    echo 0x178c8058 > $MEM_DUMP_PATH/register_config
    echo 0x178c8058 > $MEM_DUMP_PATH/register_config
    echo 0x178c8060 > $MEM_DUMP_PATH/register_config
    echo 0x178c8060 > $MEM_DUMP_PATH/register_config
    echo 0x178c8068 > $MEM_DUMP_PATH/register_config
    echo 0x178c8068 > $MEM_DUMP_PATH/register_config
    echo 0x178c8070 > $MEM_DUMP_PATH/register_config
    echo 0x178c8070 > $MEM_DUMP_PATH/register_config
    echo 0x178c8078 > $MEM_DUMP_PATH/register_config
    echo 0x178c8078 > $MEM_DUMP_PATH/register_config
    echo 0x178c8080 > $MEM_DUMP_PATH/register_config
    echo 0x178c8080 > $MEM_DUMP_PATH/register_config
    echo 0x178c8088 > $MEM_DUMP_PATH/register_config
    echo 0x178c8088 > $MEM_DUMP_PATH/register_config
    echo 0x178c8090 > $MEM_DUMP_PATH/register_config
    echo 0x178c8090 > $MEM_DUMP_PATH/register_config
    echo 0x178c8098 > $MEM_DUMP_PATH/register_config
    echo 0x178c8098 > $MEM_DUMP_PATH/register_config
    echo 0x178cc000 5 > $MEM_DUMP_PATH/register_config
    echo 0x178cc030 > $MEM_DUMP_PATH/register_config
    echo 0x178cc040 4 > $MEM_DUMP_PATH/register_config
    echo 0x178cc030 > $MEM_DUMP_PATH/register_config
    echo 0x178cc040 10 > $MEM_DUMP_PATH/register_config
    echo 0x178cc090 8 > $MEM_DUMP_PATH/register_config
    echo 0x178cc0d0 4 > $MEM_DUMP_PATH/register_config
    echo 0x178cc090 8 > $MEM_DUMP_PATH/register_config
    echo 0x178cc0d0 10 > $MEM_DUMP_PATH/register_config
    echo 0x178cc0d0 10 > $MEM_DUMP_PATH/register_config
    echo 0x17900000 > $MEM_DUMP_PATH/register_config
    echo 0x1790000c > $MEM_DUMP_PATH/register_config
    echo 0x1790000c > $MEM_DUMP_PATH/register_config
    echo 0x17900040 6 > $MEM_DUMP_PATH/register_config
    echo 0x17900900 5 > $MEM_DUMP_PATH/register_config
    echo 0x17900c00 2 > $MEM_DUMP_PATH/register_config
    echo 0x17900c0c 5 > $MEM_DUMP_PATH/register_config
    echo 0x17900c0c 6 > $MEM_DUMP_PATH/register_config
    echo 0x17900c40 2 > $MEM_DUMP_PATH/register_config
    echo 0x17900c40 2 > $MEM_DUMP_PATH/register_config
    echo 0x17900fd0 > $MEM_DUMP_PATH/register_config
    echo 0x17a00000 55 > $MEM_DUMP_PATH/register_config
    echo 0x17a00100 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a00200 5 > $MEM_DUMP_PATH/register_config
    echo 0x17a00224 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00244 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00264 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00284 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a002a4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a002c4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a002e4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a000d8 > $MEM_DUMP_PATH/register_config
    echo 0x17a00100 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a00200 5 > $MEM_DUMP_PATH/register_config
    echo 0x17a00224 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00244 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a00224 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00244 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00244 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00264 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00264 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00284 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00284 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a002a4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a002a4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a002c4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a002c4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a002e4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a002e4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00400 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a00450 6 > $MEM_DUMP_PATH/register_config
    echo 0x17a00490 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a004a0 7 > $MEM_DUMP_PATH/register_config
    echo 0x17a00500 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00500 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00600 128 > $MEM_DUMP_PATH/register_config
    echo 0x17a00d00 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a00d10 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00d30 52 > $MEM_DUMP_PATH/register_config
    echo 0x17a00d00 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a00d10 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00d10 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00d30 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a00d30 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a00fb0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00fd0 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a00fb0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a00fd0 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a00fd0 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a01250 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a01270 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a01250 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a01270 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a01270 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a014f0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a01510 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a014f0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a01510 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a01510 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a03d44 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a03d4c 2 > $MEM_DUMP_PATH/register_config
    echo 0x17a10000 19 > $MEM_DUMP_PATH/register_config
    echo 0x17a10050 33 > $MEM_DUMP_PATH/register_config
    echo 0x17a100d8 > $MEM_DUMP_PATH/register_config
    echo 0x17a10100 > $MEM_DUMP_PATH/register_config
    echo 0x17a10108 > $MEM_DUMP_PATH/register_config
    echo 0x17a10050 33 > $MEM_DUMP_PATH/register_config
    echo 0x17a100d8 > $MEM_DUMP_PATH/register_config
    echo 0x17a10100 > $MEM_DUMP_PATH/register_config
    echo 0x17a10108 > $MEM_DUMP_PATH/register_config
    echo 0x17a10204 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10224 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10244 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a100d8 > $MEM_DUMP_PATH/register_config
    echo 0x17a10100 > $MEM_DUMP_PATH/register_config
    echo 0x17a10108 > $MEM_DUMP_PATH/register_config
    echo 0x17a10108 > $MEM_DUMP_PATH/register_config
    echo 0x17a10204 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10224 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10224 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10244 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10244 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10264 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10264 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10284 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10284 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a102a4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a102a4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a102c4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a102c4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a102e4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a102e4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10400 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a10450 6 > $MEM_DUMP_PATH/register_config
    echo 0x17a104a0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a104a0 7 > $MEM_DUMP_PATH/register_config
    echo 0x17a10500 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10500 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10600 128 > $MEM_DUMP_PATH/register_config
    echo 0x17a10d00 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a10d10 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10d30 52 > $MEM_DUMP_PATH/register_config
    echo 0x17a10d00 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a10d10 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10d10 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10d30 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a10d30 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a10fb0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10fd0 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a10fb0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a10fd0 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a10fd0 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a11250 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a11270 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a11250 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a11270 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a11270 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a114f0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a11510 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a114f0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a11510 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a11510 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a13d44 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a13d4c 2 > $MEM_DUMP_PATH/register_config
    echo 0x17a13e00 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a20000 19 > $MEM_DUMP_PATH/register_config
    echo 0x17a20050 33 > $MEM_DUMP_PATH/register_config
    echo 0x17a200d8 > $MEM_DUMP_PATH/register_config
    echo 0x17a20100 > $MEM_DUMP_PATH/register_config
    echo 0x17a20108 > $MEM_DUMP_PATH/register_config
    echo 0x17a20050 33 > $MEM_DUMP_PATH/register_config
    echo 0x17a200d8 > $MEM_DUMP_PATH/register_config
    echo 0x17a20100 > $MEM_DUMP_PATH/register_config
    echo 0x17a20108 > $MEM_DUMP_PATH/register_config
    echo 0x17a20204 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20224 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20244 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a200d8 > $MEM_DUMP_PATH/register_config
    echo 0x17a20100 > $MEM_DUMP_PATH/register_config
    echo 0x17a20108 > $MEM_DUMP_PATH/register_config
    echo 0x17a20108 > $MEM_DUMP_PATH/register_config
    echo 0x17a20204 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20224 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20224 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20244 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20244 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20264 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20264 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20284 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20284 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a202a4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a202a4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a202c4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a202c4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a202e4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a202e4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20400 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a20450 6 > $MEM_DUMP_PATH/register_config
    echo 0x17a204a0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a204a0 7 > $MEM_DUMP_PATH/register_config
    echo 0x17a20500 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20500 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20600 128 > $MEM_DUMP_PATH/register_config
    echo 0x17a20d00 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a20d10 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20d30 52 > $MEM_DUMP_PATH/register_config
    echo 0x17a20d00 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a20d10 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20d10 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20d30 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a20d30 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a20fb0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20fd0 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a20fb0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a20fd0 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a20fd0 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a21250 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a21270 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a21250 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a21270 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a21270 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a214f0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a21510 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a214f0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a21510 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a21510 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a21790 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a217b0 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a21790 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a217b0 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a217b0 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a21a30 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a21a50 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a21a30 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a21a50 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a21a50 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a21cd0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a21cf0 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a21cd0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a21cf0 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a21cf0 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a21f70 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a21f90 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a21f70 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a21f90 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a21f90 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a23d44 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a23d4c 2 > $MEM_DUMP_PATH/register_config
    echo 0x17a23e00 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a30000 19 > $MEM_DUMP_PATH/register_config
    echo 0x17a30050 33 > $MEM_DUMP_PATH/register_config
    echo 0x17a300d8 > $MEM_DUMP_PATH/register_config
    echo 0x17a30100 > $MEM_DUMP_PATH/register_config
    echo 0x17a30108 > $MEM_DUMP_PATH/register_config
    echo 0x17a30050 33 > $MEM_DUMP_PATH/register_config
    echo 0x17a300d8 > $MEM_DUMP_PATH/register_config
    echo 0x17a30100 > $MEM_DUMP_PATH/register_config
    echo 0x17a30108 > $MEM_DUMP_PATH/register_config
    echo 0x17a30204 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30224 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30244 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a300d8 > $MEM_DUMP_PATH/register_config
    echo 0x17a30100 > $MEM_DUMP_PATH/register_config
    echo 0x17a30108 > $MEM_DUMP_PATH/register_config
    echo 0x17a30108 > $MEM_DUMP_PATH/register_config
    echo 0x17a30204 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30224 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30224 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30244 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30244 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30264 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30264 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30284 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30284 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a302a4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a302a4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a302c4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a302c4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a302e4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a302e4 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30400 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a30450 6 > $MEM_DUMP_PATH/register_config
    echo 0x17a304a0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a304a0 7 > $MEM_DUMP_PATH/register_config
    echo 0x17a30500 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30500 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30600 128 > $MEM_DUMP_PATH/register_config
    echo 0x17a30d00 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a30d10 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30d30 52 > $MEM_DUMP_PATH/register_config
    echo 0x17a30d00 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a30d10 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30d10 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30d30 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a30d30 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a30fb0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30fd0 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a30fb0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a30fd0 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a30fd0 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a31250 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a31270 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a31250 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a31270 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a31270 80 > $MEM_DUMP_PATH/register_config
    echo 0x17a33d44 4 > $MEM_DUMP_PATH/register_config
    echo 0x17a33d4c 2 > $MEM_DUMP_PATH/register_config
    echo 0x17a33e00 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a80000 17 > $MEM_DUMP_PATH/register_config
    echo 0x17a81000 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a81028 2 > $MEM_DUMP_PATH/register_config
    echo 0x17a81028 2 > $MEM_DUMP_PATH/register_config
    echo 0x17a82000 6 > $MEM_DUMP_PATH/register_config
    echo 0x17a83000 256 > $MEM_DUMP_PATH/register_config
    echo 0x17a84000 16 > $MEM_DUMP_PATH/register_config
    echo 0x17a85000 8 > $MEM_DUMP_PATH/register_config
    echo 0x17a85028 2 > $MEM_DUMP_PATH/register_config
    echo 0x17a85028 2 > $MEM_DUMP_PATH/register_config
    echo 0x17a86000 6 > $MEM_DUMP_PATH/register_config
    echo 0x17a87000 256 > $MEM_DUMP_PATH/register_config
    echo 0x17a90000 23 > $MEM_DUMP_PATH/register_config
    echo 0x17a90080 9 > $MEM_DUMP_PATH/register_config
    echo 0x17a900ac 2 > $MEM_DUMP_PATH/register_config
    echo 0x17a900cc 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a90048 5 > $MEM_DUMP_PATH/register_config
    echo 0x17a90080 6 > $MEM_DUMP_PATH/register_config
    echo 0x17a90080 9 > $MEM_DUMP_PATH/register_config
    echo 0x17a900ac 2 > $MEM_DUMP_PATH/register_config
    echo 0x17a900cc 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a90100 > $MEM_DUMP_PATH/register_config
    echo 0x17a900ac 2 > $MEM_DUMP_PATH/register_config
    echo 0x17a900cc 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a90100 > $MEM_DUMP_PATH/register_config
    echo 0x17a94000 23 > $MEM_DUMP_PATH/register_config
    echo 0x17a94080 9 > $MEM_DUMP_PATH/register_config
    echo 0x17a940ac 2 > $MEM_DUMP_PATH/register_config
    echo 0x17a940cc 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a94048 5 > $MEM_DUMP_PATH/register_config
    echo 0x17a94080 6 > $MEM_DUMP_PATH/register_config
    echo 0x17a94080 9 > $MEM_DUMP_PATH/register_config
    echo 0x17a940ac 2 > $MEM_DUMP_PATH/register_config
    echo 0x17a940cc 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a94100 > $MEM_DUMP_PATH/register_config
    echo 0x17a940ac 2 > $MEM_DUMP_PATH/register_config
    echo 0x17a940cc 3 > $MEM_DUMP_PATH/register_config
    echo 0x17a94100 > $MEM_DUMP_PATH/register_config
    echo 0x17aa0004 8 > $MEM_DUMP_PATH/register_config
    echo 0x17aa0028 3 > $MEM_DUMP_PATH/register_config
    echo 0x17aa003c 3 > $MEM_DUMP_PATH/register_config
    echo 0x17aa0054 12 > $MEM_DUMP_PATH/register_config
    echo 0x17aa0028 3 > $MEM_DUMP_PATH/register_config
    echo 0x17aa003c 3 > $MEM_DUMP_PATH/register_config
    echo 0x17aa0054 > $MEM_DUMP_PATH/register_config
    echo 0x17aa003c 3 > $MEM_DUMP_PATH/register_config
    echo 0x17aa0054 6 > $MEM_DUMP_PATH/register_config
    echo 0x17aa0054 30 > $MEM_DUMP_PATH/register_config
    echo 0x17b00000 72 > $MEM_DUMP_PATH/register_config
    echo 0x17b78000 3 > $MEM_DUMP_PATH/register_config
    echo 0x17b78010 4 > $MEM_DUMP_PATH/register_config
    echo 0x17b78010 4 > $MEM_DUMP_PATH/register_config
    echo 0x17b78090 5 > $MEM_DUMP_PATH/register_config
    echo 0x17b78100 > $MEM_DUMP_PATH/register_config
    echo 0x17b78110 2 > $MEM_DUMP_PATH/register_config
    echo 0x17b78190 > $MEM_DUMP_PATH/register_config
    echo 0x17b781a0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17b78220 2 > $MEM_DUMP_PATH/register_config
    echo 0x17b782a0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17b78320 > $MEM_DUMP_PATH/register_config
    echo 0x17b78380 > $MEM_DUMP_PATH/register_config
    echo 0x17b78390 12 > $MEM_DUMP_PATH/register_config
    echo 0x17b78410 > $MEM_DUMP_PATH/register_config
    echo 0x17b78420 12 > $MEM_DUMP_PATH/register_config
    echo 0x17b78410 > $MEM_DUMP_PATH/register_config
    echo 0x17b78420 12 > $MEM_DUMP_PATH/register_config
    echo 0x17b784a0 12 > $MEM_DUMP_PATH/register_config
    echo 0x17b784a0 12 > $MEM_DUMP_PATH/register_config
    echo 0x17b78520 2 > $MEM_DUMP_PATH/register_config
    echo 0x17b78520 2 > $MEM_DUMP_PATH/register_config
    echo 0x17b78580 6 > $MEM_DUMP_PATH/register_config
    echo 0x17b78600 > $MEM_DUMP_PATH/register_config
    echo 0x17b78610 8 > $MEM_DUMP_PATH/register_config
    echo 0x17b78690 8 > $MEM_DUMP_PATH/register_config
    echo 0x17b78710 8 > $MEM_DUMP_PATH/register_config
    echo 0x17b78790 8 > $MEM_DUMP_PATH/register_config
    echo 0x17b78810 8 > $MEM_DUMP_PATH/register_config
    echo 0x17b78890 8 > $MEM_DUMP_PATH/register_config
    echo 0x17b78910 8 > $MEM_DUMP_PATH/register_config
    echo 0x17b78990 8 > $MEM_DUMP_PATH/register_config
    echo 0x17b78a10 8 > $MEM_DUMP_PATH/register_config
    echo 0x17b78a90 8 > $MEM_DUMP_PATH/register_config
    echo 0x17b78b00 > $MEM_DUMP_PATH/register_config
    echo 0x17b78b00 > $MEM_DUMP_PATH/register_config
    echo 0x17b78b10 8 > $MEM_DUMP_PATH/register_config
    echo 0x17b78b90 8 > $MEM_DUMP_PATH/register_config
    echo 0x17b78c10 8 > $MEM_DUMP_PATH/register_config
    echo 0x17b78c90 8 > $MEM_DUMP_PATH/register_config
    echo 0x17b78d10 8 > $MEM_DUMP_PATH/register_config
    echo 0x17b78d90 8 > $MEM_DUMP_PATH/register_config
    echo 0x17b78e00 > $MEM_DUMP_PATH/register_config
    echo 0x17b78e00 > $MEM_DUMP_PATH/register_config
    echo 0x17b78e10 6 > $MEM_DUMP_PATH/register_config
    echo 0x17b78e90 6 > $MEM_DUMP_PATH/register_config
    echo 0x17b78f10 6 > $MEM_DUMP_PATH/register_config
    echo 0x17b78f90 6 > $MEM_DUMP_PATH/register_config
    echo 0x17b79010 6 > $MEM_DUMP_PATH/register_config
    echo 0x17b79090 6 > $MEM_DUMP_PATH/register_config
    echo 0x17b79100 > $MEM_DUMP_PATH/register_config
    echo 0x17b79110 4 > $MEM_DUMP_PATH/register_config
    echo 0x17b79190 4 > $MEM_DUMP_PATH/register_config
    echo 0x17b79210 64 > $MEM_DUMP_PATH/register_config
    echo 0x17b79a10 4 > $MEM_DUMP_PATH/register_config
    echo 0x17b79a90 4 > $MEM_DUMP_PATH/register_config
    echo 0x17b79b00 2 > $MEM_DUMP_PATH/register_config
    echo 0x17b79b10 4 > $MEM_DUMP_PATH/register_config
    echo 0x17b79b10 4 > $MEM_DUMP_PATH/register_config
    echo 0x17b79b90 5 > $MEM_DUMP_PATH/register_config
    echo 0x17b79bb0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17b79bb0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17b79c30 2 > $MEM_DUMP_PATH/register_config
    echo 0x17b79c40 4 > $MEM_DUMP_PATH/register_config
    echo 0x17b79c40 4 > $MEM_DUMP_PATH/register_config
    echo 0x17b79cc0 4 > $MEM_DUMP_PATH/register_config
    echo 0x17b79d00 11 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0000 6 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0020 5 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0050 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0020 5 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0050 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0050 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0070 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0080 25 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0100 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0120 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0140 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0200 4 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0100 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0120 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0140 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0200 6 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0700 > $MEM_DUMP_PATH/register_config
    echo 0x17ba070c > $MEM_DUMP_PATH/register_config
    echo 0x17ba070c 3 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0780 33 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0808 10 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0840 16 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0808 10 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0840 10 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0824 3 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0840 5 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0840 16 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0c48 10 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0c80 10 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0c64 3 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0c80 5 > $MEM_DUMP_PATH/register_config
    echo 0x17ba0c80 16 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3000 80 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3500 80 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3a00 3 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3a24 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3a2c > $MEM_DUMP_PATH/register_config
    echo 0x17ba3a24 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3a2c 2 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3a2c 49 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3b00 2 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3b20 3 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3b30 11 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3b64 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3b70 2 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3b90 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3c00 35 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3b00 2 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3b20 3 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3b30 8 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3b30 11 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3b64 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3b70 2 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3b90 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3b64 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3b70 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3b70 2 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3b90 > $MEM_DUMP_PATH/register_config
    echo 0x17ba3c00 35 > $MEM_DUMP_PATH/register_config
    echo 0x17c20000 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c40000 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c40010 > $MEM_DUMP_PATH/register_config
    echo 0x17c40018 > $MEM_DUMP_PATH/register_config
    echo 0x17c40020 4 > $MEM_DUMP_PATH/register_config
    echo 0x17c40010 > $MEM_DUMP_PATH/register_config
    echo 0x17c40018 > $MEM_DUMP_PATH/register_config
    echo 0x17c40020 8 > $MEM_DUMP_PATH/register_config
    echo 0x17c41000 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c41010 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c41010 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c44000 4 > $MEM_DUMP_PATH/register_config
    echo 0x17c44100 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c44208 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c44304 > $MEM_DUMP_PATH/register_config
    echo 0x17c44500 6 > $MEM_DUMP_PATH/register_config
    echo 0x17c45000 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4500c > $MEM_DUMP_PATH/register_config
    echo 0x17c45014 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4500c > $MEM_DUMP_PATH/register_config
    echo 0x17c45014 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45014 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c45030 > $MEM_DUMP_PATH/register_config
    echo 0x17c45040 > $MEM_DUMP_PATH/register_config
    echo 0x17c45030 > $MEM_DUMP_PATH/register_config
    echo 0x17c45040 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4504c > $MEM_DUMP_PATH/register_config
    echo 0x17c45054 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4504c > $MEM_DUMP_PATH/register_config
    echo 0x17c45054 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45054 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c45070 > $MEM_DUMP_PATH/register_config
    echo 0x17c45080 > $MEM_DUMP_PATH/register_config
    echo 0x17c45070 > $MEM_DUMP_PATH/register_config
    echo 0x17c45080 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4508c > $MEM_DUMP_PATH/register_config
    echo 0x17c45094 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4508c > $MEM_DUMP_PATH/register_config
    echo 0x17c45094 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45094 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c450b0 > $MEM_DUMP_PATH/register_config
    echo 0x17c450c0 > $MEM_DUMP_PATH/register_config
    echo 0x17c450b0 > $MEM_DUMP_PATH/register_config
    echo 0x17c450c0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c450cc > $MEM_DUMP_PATH/register_config
    echo 0x17c450d4 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c450cc > $MEM_DUMP_PATH/register_config
    echo 0x17c450d4 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c450d4 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c450f0 > $MEM_DUMP_PATH/register_config
    echo 0x17c45100 > $MEM_DUMP_PATH/register_config
    echo 0x17c450f0 > $MEM_DUMP_PATH/register_config
    echo 0x17c45100 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4510c > $MEM_DUMP_PATH/register_config
    echo 0x17c45114 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4510c > $MEM_DUMP_PATH/register_config
    echo 0x17c45114 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45114 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c45130 > $MEM_DUMP_PATH/register_config
    echo 0x17c45140 > $MEM_DUMP_PATH/register_config
    echo 0x17c45130 > $MEM_DUMP_PATH/register_config
    echo 0x17c45140 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4514c > $MEM_DUMP_PATH/register_config
    echo 0x17c45154 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4514c > $MEM_DUMP_PATH/register_config
    echo 0x17c45154 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45154 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c45170 > $MEM_DUMP_PATH/register_config
    echo 0x17c45180 > $MEM_DUMP_PATH/register_config
    echo 0x17c45170 > $MEM_DUMP_PATH/register_config
    echo 0x17c45180 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4518c > $MEM_DUMP_PATH/register_config
    echo 0x17c45194 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4518c > $MEM_DUMP_PATH/register_config
    echo 0x17c45194 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45194 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c451b0 > $MEM_DUMP_PATH/register_config
    echo 0x17c451c0 > $MEM_DUMP_PATH/register_config
    echo 0x17c451b0 > $MEM_DUMP_PATH/register_config
    echo 0x17c451c0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c451cc > $MEM_DUMP_PATH/register_config
    echo 0x17c451d4 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c451cc > $MEM_DUMP_PATH/register_config
    echo 0x17c451d4 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c451d4 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c451f0 > $MEM_DUMP_PATH/register_config
    echo 0x17c45200 > $MEM_DUMP_PATH/register_config
    echo 0x17c451f0 > $MEM_DUMP_PATH/register_config
    echo 0x17c45200 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4520c > $MEM_DUMP_PATH/register_config
    echo 0x17c45214 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4520c > $MEM_DUMP_PATH/register_config
    echo 0x17c45214 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45214 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c45230 > $MEM_DUMP_PATH/register_config
    echo 0x17c45240 > $MEM_DUMP_PATH/register_config
    echo 0x17c45230 > $MEM_DUMP_PATH/register_config
    echo 0x17c45240 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4524c > $MEM_DUMP_PATH/register_config
    echo 0x17c45254 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4524c > $MEM_DUMP_PATH/register_config
    echo 0x17c45254 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45254 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c45270 > $MEM_DUMP_PATH/register_config
    echo 0x17c45280 > $MEM_DUMP_PATH/register_config
    echo 0x17c45270 > $MEM_DUMP_PATH/register_config
    echo 0x17c45280 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4528c > $MEM_DUMP_PATH/register_config
    echo 0x17c45294 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4528c > $MEM_DUMP_PATH/register_config
    echo 0x17c45294 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45294 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c452b0 > $MEM_DUMP_PATH/register_config
    echo 0x17c452c0 > $MEM_DUMP_PATH/register_config
    echo 0x17c452b0 > $MEM_DUMP_PATH/register_config
    echo 0x17c452c0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c452cc > $MEM_DUMP_PATH/register_config
    echo 0x17c452d4 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c452cc > $MEM_DUMP_PATH/register_config
    echo 0x17c452d4 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c452d4 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c452f0 > $MEM_DUMP_PATH/register_config
    echo 0x17c45300 > $MEM_DUMP_PATH/register_config
    echo 0x17c452f0 > $MEM_DUMP_PATH/register_config
    echo 0x17c45300 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4530c > $MEM_DUMP_PATH/register_config
    echo 0x17c45314 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4530c > $MEM_DUMP_PATH/register_config
    echo 0x17c45314 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45314 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c45330 > $MEM_DUMP_PATH/register_config
    echo 0x17c45340 > $MEM_DUMP_PATH/register_config
    echo 0x17c45330 > $MEM_DUMP_PATH/register_config
    echo 0x17c45340 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4534c > $MEM_DUMP_PATH/register_config
    echo 0x17c45354 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4534c > $MEM_DUMP_PATH/register_config
    echo 0x17c45354 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45354 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c45370 > $MEM_DUMP_PATH/register_config
    echo 0x17c45380 > $MEM_DUMP_PATH/register_config
    echo 0x17c45370 > $MEM_DUMP_PATH/register_config
    echo 0x17c45380 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4538c > $MEM_DUMP_PATH/register_config
    echo 0x17c45394 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4538c > $MEM_DUMP_PATH/register_config
    echo 0x17c45394 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45394 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c453b0 > $MEM_DUMP_PATH/register_config
    echo 0x17c453c0 > $MEM_DUMP_PATH/register_config
    echo 0x17c453b0 > $MEM_DUMP_PATH/register_config
    echo 0x17c453c0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c453cc > $MEM_DUMP_PATH/register_config
    echo 0x17c453d4 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c453cc > $MEM_DUMP_PATH/register_config
    echo 0x17c453d4 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c453d4 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c453f0 > $MEM_DUMP_PATH/register_config
    echo 0x17c45400 > $MEM_DUMP_PATH/register_config
    echo 0x17c453f0 > $MEM_DUMP_PATH/register_config
    echo 0x17c45400 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4540c > $MEM_DUMP_PATH/register_config
    echo 0x17c45414 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4540c > $MEM_DUMP_PATH/register_config
    echo 0x17c45414 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45414 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c45430 > $MEM_DUMP_PATH/register_config
    echo 0x17c45440 > $MEM_DUMP_PATH/register_config
    echo 0x17c45430 > $MEM_DUMP_PATH/register_config
    echo 0x17c45440 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4544c > $MEM_DUMP_PATH/register_config
    echo 0x17c45454 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4544c > $MEM_DUMP_PATH/register_config
    echo 0x17c45454 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45454 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c45470 > $MEM_DUMP_PATH/register_config
    echo 0x17c45480 > $MEM_DUMP_PATH/register_config
    echo 0x17c45470 > $MEM_DUMP_PATH/register_config
    echo 0x17c45480 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4548c > $MEM_DUMP_PATH/register_config
    echo 0x17c45494 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4548c > $MEM_DUMP_PATH/register_config
    echo 0x17c45494 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45494 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c454b0 > $MEM_DUMP_PATH/register_config
    echo 0x17c454c0 > $MEM_DUMP_PATH/register_config
    echo 0x17c454b0 > $MEM_DUMP_PATH/register_config
    echo 0x17c454c0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c454cc > $MEM_DUMP_PATH/register_config
    echo 0x17c454d4 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c454cc > $MEM_DUMP_PATH/register_config
    echo 0x17c454d4 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c454d4 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c454f0 > $MEM_DUMP_PATH/register_config
    echo 0x17c45500 > $MEM_DUMP_PATH/register_config
    echo 0x17c454f0 > $MEM_DUMP_PATH/register_config
    echo 0x17c45500 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4550c > $MEM_DUMP_PATH/register_config
    echo 0x17c45514 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4550c > $MEM_DUMP_PATH/register_config
    echo 0x17c45514 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45514 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c45530 > $MEM_DUMP_PATH/register_config
    echo 0x17c45540 > $MEM_DUMP_PATH/register_config
    echo 0x17c45530 > $MEM_DUMP_PATH/register_config
    echo 0x17c45540 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4554c > $MEM_DUMP_PATH/register_config
    echo 0x17c45554 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4554c > $MEM_DUMP_PATH/register_config
    echo 0x17c45554 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45554 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c45570 > $MEM_DUMP_PATH/register_config
    echo 0x17c45580 > $MEM_DUMP_PATH/register_config
    echo 0x17c45570 > $MEM_DUMP_PATH/register_config
    echo 0x17c45580 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4558c > $MEM_DUMP_PATH/register_config
    echo 0x17c45594 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4558c > $MEM_DUMP_PATH/register_config
    echo 0x17c45594 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45594 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c455b0 > $MEM_DUMP_PATH/register_config
    echo 0x17c455c0 > $MEM_DUMP_PATH/register_config
    echo 0x17c455b0 > $MEM_DUMP_PATH/register_config
    echo 0x17c455c0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c455cc > $MEM_DUMP_PATH/register_config
    echo 0x17c455d4 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c455cc > $MEM_DUMP_PATH/register_config
    echo 0x17c455d4 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c455d4 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c455f0 > $MEM_DUMP_PATH/register_config
    echo 0x17c45600 > $MEM_DUMP_PATH/register_config
    echo 0x17c455f0 > $MEM_DUMP_PATH/register_config
    echo 0x17c45600 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4560c > $MEM_DUMP_PATH/register_config
    echo 0x17c45614 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4560c > $MEM_DUMP_PATH/register_config
    echo 0x17c45614 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45614 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c45630 > $MEM_DUMP_PATH/register_config
    echo 0x17c45640 > $MEM_DUMP_PATH/register_config
    echo 0x17c45630 > $MEM_DUMP_PATH/register_config
    echo 0x17c45640 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4564c > $MEM_DUMP_PATH/register_config
    echo 0x17c45654 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4564c > $MEM_DUMP_PATH/register_config
    echo 0x17c45654 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45654 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c45670 > $MEM_DUMP_PATH/register_config
    echo 0x17c45680 > $MEM_DUMP_PATH/register_config
    echo 0x17c45670 > $MEM_DUMP_PATH/register_config
    echo 0x17c45680 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4568c > $MEM_DUMP_PATH/register_config
    echo 0x17c45694 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4568c > $MEM_DUMP_PATH/register_config
    echo 0x17c45694 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45694 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c456b0 > $MEM_DUMP_PATH/register_config
    echo 0x17c456c0 > $MEM_DUMP_PATH/register_config
    echo 0x17c456b0 > $MEM_DUMP_PATH/register_config
    echo 0x17c456c0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c456cc > $MEM_DUMP_PATH/register_config
    echo 0x17c456d4 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c456cc > $MEM_DUMP_PATH/register_config
    echo 0x17c456d4 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c456d4 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c456f0 > $MEM_DUMP_PATH/register_config
    echo 0x17c45700 > $MEM_DUMP_PATH/register_config
    echo 0x17c456f0 > $MEM_DUMP_PATH/register_config
    echo 0x17c45700 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4570c > $MEM_DUMP_PATH/register_config
    echo 0x17c45714 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4570c > $MEM_DUMP_PATH/register_config
    echo 0x17c45714 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45714 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c45730 > $MEM_DUMP_PATH/register_config
    echo 0x17c45740 > $MEM_DUMP_PATH/register_config
    echo 0x17c45730 > $MEM_DUMP_PATH/register_config
    echo 0x17c45740 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4574c > $MEM_DUMP_PATH/register_config
    echo 0x17c45754 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4574c > $MEM_DUMP_PATH/register_config
    echo 0x17c45754 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45754 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c45770 > $MEM_DUMP_PATH/register_config
    echo 0x17c45780 > $MEM_DUMP_PATH/register_config
    echo 0x17c45770 > $MEM_DUMP_PATH/register_config
    echo 0x17c45780 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c4578c > $MEM_DUMP_PATH/register_config
    echo 0x17c45794 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c4578c > $MEM_DUMP_PATH/register_config
    echo 0x17c45794 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c45794 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c457b0 > $MEM_DUMP_PATH/register_config
    echo 0x17c457c0 > $MEM_DUMP_PATH/register_config
    echo 0x17c457b0 > $MEM_DUMP_PATH/register_config
    echo 0x17c457c0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c457cc > $MEM_DUMP_PATH/register_config
    echo 0x17c457d4 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c457cc > $MEM_DUMP_PATH/register_config
    echo 0x17c457d4 2 > $MEM_DUMP_PATH/register_config
    echo 0x17c457d4 3 > $MEM_DUMP_PATH/register_config
    echo 0x17c457f0 > $MEM_DUMP_PATH/register_config
    echo 0x17c457f0 > $MEM_DUMP_PATH/register_config
    echo 0x17d80000 7 > $MEM_DUMP_PATH/register_config
    echo 0x17d80020 2 > $MEM_DUMP_PATH/register_config
    echo 0x17d80040 2 > $MEM_DUMP_PATH/register_config
    echo 0x17d80060 2 > $MEM_DUMP_PATH/register_config
    echo 0x17d80100 144 > $MEM_DUMP_PATH/register_config
    echo 0x17d90000 4 > $MEM_DUMP_PATH/register_config
    echo 0x17d90014 11 > $MEM_DUMP_PATH/register_config
    echo 0x17d90014 26 > $MEM_DUMP_PATH/register_config
    echo 0x17d90080 5 > $MEM_DUMP_PATH/register_config
    echo 0x17d900b0 > $MEM_DUMP_PATH/register_config
    echo 0x17d900b8 2 > $MEM_DUMP_PATH/register_config
    echo 0x17d900d0 9 > $MEM_DUMP_PATH/register_config
    echo 0x17d90100 40 > $MEM_DUMP_PATH/register_config
    echo 0x17d90080 5 > $MEM_DUMP_PATH/register_config
    echo 0x17d900b0 > $MEM_DUMP_PATH/register_config
    echo 0x17d900b8 2 > $MEM_DUMP_PATH/register_config
    echo 0x17d900b0 > $MEM_DUMP_PATH/register_config
    echo 0x17d900b8 2 > $MEM_DUMP_PATH/register_config
    echo 0x17d900b8 2 > $MEM_DUMP_PATH/register_config
    echo 0x17d900d0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17d900d0 9 > $MEM_DUMP_PATH/register_config
    echo 0x17d90100 24 > $MEM_DUMP_PATH/register_config
    echo 0x17d90100 40 > $MEM_DUMP_PATH/register_config
    echo 0x17d90200 40 > $MEM_DUMP_PATH/register_config
    echo 0x17d90300 5 > $MEM_DUMP_PATH/register_config
    echo 0x17d90320 > $MEM_DUMP_PATH/register_config
    echo 0x17d90340 > $MEM_DUMP_PATH/register_config
    echo 0x17d9034c 13 > $MEM_DUMP_PATH/register_config
    echo 0x17d90200 40 > $MEM_DUMP_PATH/register_config
    echo 0x17d90300 5 > $MEM_DUMP_PATH/register_config
    echo 0x17d90320 > $MEM_DUMP_PATH/register_config
    echo 0x17d90340 > $MEM_DUMP_PATH/register_config
    echo 0x17d9034c 31 > $MEM_DUMP_PATH/register_config
    echo 0x17d903e0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17d90404 > $MEM_DUMP_PATH/register_config
    echo 0x17d90410 > $MEM_DUMP_PATH/register_config
    echo 0x17d90300 5 > $MEM_DUMP_PATH/register_config
    echo 0x17d90320 > $MEM_DUMP_PATH/register_config
    echo 0x17d90340 > $MEM_DUMP_PATH/register_config
    echo 0x17d9034c > $MEM_DUMP_PATH/register_config
    echo 0x17d90320 > $MEM_DUMP_PATH/register_config
    echo 0x17d90340 > $MEM_DUMP_PATH/register_config
    echo 0x17d9034c > $MEM_DUMP_PATH/register_config
    echo 0x17d9034c 31 > $MEM_DUMP_PATH/register_config
    echo 0x17d903e0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17d90404 > $MEM_DUMP_PATH/register_config
    echo 0x17d90410 > $MEM_DUMP_PATH/register_config
    echo 0x17d903e0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17d90404 > $MEM_DUMP_PATH/register_config
    echo 0x17d90410 > $MEM_DUMP_PATH/register_config
    echo 0x17d90410 > $MEM_DUMP_PATH/register_config
    echo 0x17d91000 4 > $MEM_DUMP_PATH/register_config
    echo 0x17d91014 11 > $MEM_DUMP_PATH/register_config
    echo 0x17d91014 26 > $MEM_DUMP_PATH/register_config
    echo 0x17d91080 8 > $MEM_DUMP_PATH/register_config
    echo 0x17d910b0 > $MEM_DUMP_PATH/register_config
    echo 0x17d910b8 2 > $MEM_DUMP_PATH/register_config
    echo 0x17d910d0 9 > $MEM_DUMP_PATH/register_config
    echo 0x17d91100 40 > $MEM_DUMP_PATH/register_config
    echo 0x17d91080 8 > $MEM_DUMP_PATH/register_config
    echo 0x17d910b0 > $MEM_DUMP_PATH/register_config
    echo 0x17d910b8 2 > $MEM_DUMP_PATH/register_config
    echo 0x17d910d0 9 > $MEM_DUMP_PATH/register_config
    echo 0x17d910b0 > $MEM_DUMP_PATH/register_config
    echo 0x17d910b8 2 > $MEM_DUMP_PATH/register_config
    echo 0x17d910b8 2 > $MEM_DUMP_PATH/register_config
    echo 0x17d910d0 2 > $MEM_DUMP_PATH/register_config
    echo 0x17d910d0 9 > $MEM_DUMP_PATH/register_config
    echo 0x17d91100 24 > $MEM_DUMP_PATH/register_config
    echo 0x17d91100 40 > $MEM_DUMP_PATH/register_config
    echo 0x17d91200 40 > $MEM_DUMP_PATH/register_config
    echo 0x17d91300 5 > $MEM_DUMP_PATH/register_config
    echo 0x17d91320 4 > $MEM_DUMP_PATH/register_config
    echo 0x17d91340 > $MEM_DUMP_PATH/register_config
    echo 0x17d9134c 13 > $MEM_DUMP_PATH/register_config
    echo 0x17d91200 40 > $MEM_DUMP_PATH/register_config
    echo 0x17d91300 5 > $MEM_DUMP_PATH/register_config
    echo 0x17d91320 4 > $MEM_DUMP_PATH/register_config
    echo 0x17d91340 > $MEM_DUMP_PATH/register_config
    echo 0x17d9134c 35 > $MEM_DUMP_PATH/register_config
    echo 0x17d913e0 5 > $MEM_DUMP_PATH/register_config
    echo 0x17d91404 > $MEM_DUMP_PATH/register_config
    echo 0x17d91410 > $MEM_DUMP_PATH/register_config
    echo 0x17d91300 5 > $MEM_DUMP_PATH/register_config
    echo 0x17d91320 4 > $MEM_DUMP_PATH/register_config
    echo 0x17d91340 > $MEM_DUMP_PATH/register_config
    echo 0x17d9134c > $MEM_DUMP_PATH/register_config
    echo 0x17d91320 4 > $MEM_DUMP_PATH/register_config
    echo 0x17d91340 > $MEM_DUMP_PATH/register_config
    echo 0x17d9134c 5 > $MEM_DUMP_PATH/register_config
    echo 0x17d91340 > $MEM_DUMP_PATH/register_config
    echo 0x17d9134c > $MEM_DUMP_PATH/register_config
    echo 0x17d9134c 35 > $MEM_DUMP_PATH/register_config
    echo 0x17d913e0 5 > $MEM_DUMP_PATH/register_config
    echo 0x17d91404 > $MEM_DUMP_PATH/register_config
    echo 0x17d91410 > $MEM_DUMP_PATH/register_config
    echo 0x17d913e0 5 > $MEM_DUMP_PATH/register_config
    echo 0x17d91404 > $MEM_DUMP_PATH/register_config
    echo 0x17d91410 > $MEM_DUMP_PATH/register_config
    echo 0x17d91404 > $MEM_DUMP_PATH/register_config
    echo 0x17d91410 > $MEM_DUMP_PATH/register_config
    echo 0x17d91410 > $MEM_DUMP_PATH/register_config
    echo 0x17d98000 10 > $MEM_DUMP_PATH/register_config
    echo 0x17d98030 2 > $MEM_DUMP_PATH/register_config
    echo 0x17d98030 2 > $MEM_DUMP_PATH/register_config
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

    config_dcc_thermal
    config_dcc_core
    config_dcc_lpm_pcu
    config_dcc_rpmh
    config_dcc_apss_rscc
    config_dcc_epss
    config_dcc_misc
    config_dcc_ddr
    config_dcc_gpu

    echo 4 > $DCC_PATH/curr_list
    echo cap > $DCC_PATH/func_type
    echo sram > $DCC_PATH/data_sink

    config_dcc_gic
    config_dcc_gcc
    config_dcc_smmu

    config_dcc_gemnoc
    config_dcc_cnoc
    config_dcc_snoc
    #config_dcc_pcie_agnoc
    config_dcc_mmnoc
    config_dcc_lpass_ag_noc
    config_dcc_dc_dch_noc
    config_dcc_dc_ch01
    #config_dcc_cdsp
    config_dcc_adsp
    #config_dcc_wpss
    config_dcc_qup
    config_dcc_cpucp
    config_dcc_turing_rscc

    echo  1 > $DCC_PATH/enable
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
    echo "neo61 debug"
    find_build_type
    etr_size="0x2000000"
    srcenable="enable_source"
    sinkenable="enable_sink"
    create_stp_policy
    echo "Enabling STM events on neo61."
    adjust_permission
    enable_stm_events
    enable_cti_flush_for_etf
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
