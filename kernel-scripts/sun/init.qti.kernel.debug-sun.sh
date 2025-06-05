#=============================================================================
# Copyright (c) 2020-2023 Qualcomm Technologies, Inc.
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

config_dcc_timer()
{
    echo 0x16801000 2 > $DCC_PATH/config
}

config_dcc_core()
{
    # NCC0_NCC_NCC_ARCH reg
    echo 0x18880258 10 > $DCC_PATH/config
    echo 0x18880288 6 > $DCC_PATH/config
    echo 0x188802a8 10 > $DCC_PATH/config
    echo 0x18880328 6 > $DCC_PATH/config
    echo 0x188803d8 10 > $DCC_PATH/config
    echo 0x18880408 2 > $DCC_PATH/config

    # NCC1_NCC_NCC_ARCH reg
    echo 0x19880258 10 > $DCC_PATH/config
    echo 0x19880288 6 > $DCC_PATH/config
    echo 0x198802a8 10 > $DCC_PATH/config
    echo 0x19880328 6 > $DCC_PATH/config
    echo 0x198803d8 10 > $DCC_PATH/config
    echo 0x19880408 2 > $DCC_PATH/config

    #NCC0_NCC_DVFS0
    echo 0x188b0000 4 > $DCC_PATH/config
    echo 0x188b0050 2 > $DCC_PATH/config
    echo 0x188b00e8 36 > $DCC_PATH/config
    echo 0x188b0788 2 > $DCC_PATH/config
    echo 0x188b0c18 2 > $DCC_PATH/config

    #NCC1_NCC_DVFS0
    echo 0x198b0000 4 > $DCC_PATH/config
    echo 0x198b0050 2 > $DCC_PATH/config
    echo 0x198b00e8 36 > $DCC_PATH/config
    echo 0x198b0788 2 > $DCC_PATH/config
    echo 0x198b0c18 2 > $DCC_PATH/config

    #APSS_CPUCP_BE
    echo 0x17988814 2 > $DCC_PATH/config
    echo 0x17998814 2 > $DCC_PATH/config
    #APSS_CPUCP_CL_LPM
    echo 0x179d2000 3 > $DCC_PATH/config
    echo 0x179d2020 3 > $DCC_PATH/config
    echo 0x179d2040 2 > $DCC_PATH/config
    echo 0x179d2060 2 > $DCC_PATH/config
    echo 0x179d2080 2 > $DCC_PATH/config
    echo 0x179d20a0 > $DCC_PATH/config
    echo 0x179d20b0 > $DCC_PATH/config
    echo 0x179d20c0 > $DCC_PATH/config
    echo 0x179d2200 2 > $DCC_PATH/config
    echo 0x179d2220 2 > $DCC_PATH/config
    echo 0x179d2280 2 > $DCC_PATH/config
    echo 0x179d22f0 > $DCC_PATH/config
    echo 0x179d2304 > $DCC_PATH/config
    echo 0x179d2310 > $DCC_PATH/config
    echo 0x179d2400 4 > $DCC_PATH/config
    echo 0x179d2420 > $DCC_PATH/config
    echo 0x179d2428 2 > $DCC_PATH/config
    echo 0x179d2440 2 > $DCC_PATH/config
    echo 0x179d2520 2 > $DCC_PATH/config
    echo 0x179d2600 2 > $DCC_PATH/config
    echo 0x179d2710 2 > $DCC_PATH/config
    echo 0x179d2720 2 > $DCC_PATH/config
    echo 0x179d2740 2 > $DCC_PATH/config
    echo 0x179d3080 2 > $DCC_PATH/config

    #APSS_CPUCP
    echo 0x17846018 2 > $DCC_PATH/config
    echo 0x17846060 > $DCC_PATH/config
    echo 0x17846100 2 > $DCC_PATH/config
    echo 0x17846110 > $DCC_PATH/config
    echo 0x17847030 2 > $DCC_PATH/config
    echo 0x17847040 2 > $DCC_PATH/config
    echo 0x17847050 2 > $DCC_PATH/config
    echo 0x17847060 2 > $DCC_PATH/config
    echo 0x17847070 2 > $DCC_PATH/config
    echo 0x17847080 2 > $DCC_PATH/config
    echo 0x17847090 2 > $DCC_PATH/config
    echo 0x178470a0 2 > $DCC_PATH/config
    echo 0x178470b0 2 > $DCC_PATH/config
    echo 0x178470c0 2 > $DCC_PATH/config
    echo 0x17850000 2 > $DCC_PATH/config
    echo 0x17850010 2 > $DCC_PATH/config
    echo 0x17850030 2 > $DCC_PATH/config
    echo 0x17850040 > $DCC_PATH/config
    echo 0x17854000 > $DCC_PATH/config
    echo 0x17854008 4 > $DCC_PATH/config
    echo 0x179c8814 2 > $DCC_PATH/config
    echo 0x179d0104 > $DCC_PATH/config
    echo 0x179d0118 2 > $DCC_PATH/config
    echo 0x179d0148 2 > $DCC_PATH/config
    echo 0x179d1600 > $DCC_PATH/config
    echo 0x179d1678 > $DCC_PATH/config
    echo 0x179d1688 2 > $DCC_PATH/config
    echo 0x179d1694 3 > $DCC_PATH/config
    echo 0x179d1820 3 > $DCC_PATH/config
    echo 0x17b70000 > $DCC_PATH/config
    echo 0x17b70008 2 > $DCC_PATH/config
    echo 0x17b71000 > $DCC_PATH/config
    echo 0x17b71008 2 > $DCC_PATH/config

    #APSS_CPUCX
    echo 0x164807f8 > $DCC_PATH/config
    echo 0x16480810 3 > $DCC_PATH/config
    echo 0x16483000 40 > $DCC_PATH/config
    echo 0x16483a00 2 > $DCC_PATH/config
    echo 0x16488908 > $DCC_PATH/config
    echo 0x16488C18 > $DCC_PATH/config
    echo 0x164A8908 > $DCC_PATH/config
    echo 0x164A8C18 > $DCC_PATH/config
    echo 0x164a07f8 > $DCC_PATH/config
    echo 0x164a0810 3 > $DCC_PATH/config
    echo 0x164a3000 40 > $DCC_PATH/config
    echo 0x164a3a00 2 > $DCC_PATH/config

    #APSS_CPUMX
    echo 0x16493000 40 > $DCC_PATH/config
    echo 0x16493a00 2 > $DCC_PATH/config
    echo 0x16498C18 > $DCC_PATH/config
    echo 0x164B8C18 > $DCC_PATH/config
    echo 0x164b3a00 2 > $DCC_PATH/config

    #APSS_INTU
    echo 0x16440000 2 > $DCC_PATH/config
    echo 0x16440020 3 > $DCC_PATH/config
    echo 0x16440030 > $DCC_PATH/config
    echo 0x1644003c > $DCC_PATH/config
    echo 0x16440044 3 > $DCC_PATH/config
    echo 0x16440438 > $DCC_PATH/config
    echo 0x16440500 5 > $DCC_PATH/config
    echo 0x16562000 2 > $DCC_PATH/config
    echo 0x16565004 > $DCC_PATH/config

    #APSS_NSINW
    echo 0x17000BD0 > $DCC_PATH/config
    echo 0x170404A0 > $DCC_PATH/config
    echo 0x170A0590 > $DCC_PATH/config
    echo 0x170C0330 > $DCC_PATH/config
    echo 0x170C0338 > $DCC_PATH/config
    echo 0x170C0340 > $DCC_PATH/config
    echo 0x170C0518 > $DCC_PATH/config
    echo 0x170C0528 > $DCC_PATH/config
    echo 0x170C0538 > $DCC_PATH/config
    echo 0x170C0560 4 > $DCC_PATH/config
    echo 0x17200BD0 > $DCC_PATH/config
    echo 0x172404A0 > $DCC_PATH/config
    echo 0x172A0590 > $DCC_PATH/config
    echo 0x172C0330 > $DCC_PATH/config
    echo 0x172C0338 > $DCC_PATH/config
    echo 0x172C0340 > $DCC_PATH/config
    echo 0x172C0518 > $DCC_PATH/config
    echo 0x172C0528 > $DCC_PATH/config
    echo 0x172C0538 > $DCC_PATH/config
    echo 0x172C0560 4 > $DCC_PATH/config

    #APSS_WDOG_STATUS
    echo 0x1641000C > $DCC_PATH/config
    echo 0x1641400C > $DCC_PATH/config

    #NCC_GBL_WDOG_THRESHOLD
    echo 0x18830320 2 > $DCC_PATH/config
    echo 0x19830320 2 > $DCC_PATH/config

    #NCC0_CORE0_NCC_RAS/URAS
    echo 0x18040010 6 > $DCC_PATH/config
    echo 0x18040040 10 > $DCC_PATH/config
    echo 0x18040090 6 > $DCC_PATH/config
    echo 0x18850020 2 > $DCC_PATH/config
    echo 0x18850060 2 > $DCC_PATH/config
    echo 0x188500A0 2 > $DCC_PATH/config
    echo 0x188500E0 2 > $DCC_PATH/config
    echo 0x18850120 2 > $DCC_PATH/config
    echo 0x18850160 2 > $DCC_PATH/config
    echo 0x189C1000 > $DCC_PATH/config
    echo 0x199C1000 > $DCC_PATH/config

    #NCC PLL
    echo 0x18A30000 9 > $DCC_PATH/config
    echo 0x18A30030 > $DCC_PATH/config
    echo 0x18A3003C 2 > $DCC_PATH/config
    echo 0x19A30000 9 > $DCC_PATH/config
    echo 0x19A30030 > $DCC_PATH/config
    echo 0x19A3003C 2 > $DCC_PATH/config
}

gemnoc_dump()
{
    # gem_noc_fault_sbm
    echo 0x24201040 1 > $DCC_PATH/config
    echo 0x24201048 1 > $DCC_PATH/config

    #; gem_noc_qns_llcc_even_poc_err
    echo 0x24100010 1 > $DCC_PATH/config
    echo 0x24100020 6 > $DCC_PATH/config
    #; gem_noc_qns_llcc_odd_poc_err
    echo 0x24180010 1 > $DCC_PATH/config
    echo 0x24180020 6 > $DCC_PATH/config

    #; gem_noc_qns_cnoc_poc_err
    echo 0x24200010 1 > $DCC_PATH/config
    echo 0x24200020 6 > $DCC_PATH/config
    #; gem_noc_qns_pcie_poc_err
    echo 0x24200410 1 > $DCC_PATH/config
    echo 0x24200420 6 > $DCC_PATH/config

    #; gem_noc_qns_llcc_even_poc_dbg
    echo 0x24102010 1 > $DCC_PATH/config
    echo 0x24102038 1 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102038 1 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102038 1 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102038 1 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102008 2 > $DCC_PATH/config

    #; gem_noc_qns_llcc_odd_poc_dbg
    echo 0x24181010 1 > $DCC_PATH/config
    echo 0x24181038 1 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181038 1 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181038 1 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181038 1 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181008 2 > $DCC_PATH/config

    #; gem_noc_qns_cnoc_poc_dbg
    echo 0x24203010 1 > $DCC_PATH/config
    echo 0x24203038 1 > $DCC_PATH/config
    echo 0x24203030 2 > $DCC_PATH/config
    echo 0x24203030 2 > $DCC_PATH/config
    echo 0x24203030 2 > $DCC_PATH/config
    echo 0x24203030 2 > $DCC_PATH/config
    echo 0x24203038 1 > $DCC_PATH/config
    echo 0x24203030 2 > $DCC_PATH/config
    echo 0x24203030 2 > $DCC_PATH/config
    echo 0x24203030 2 > $DCC_PATH/config
    echo 0x24203030 2 > $DCC_PATH/config
    echo 0x24203038 1 > $DCC_PATH/config
    echo 0x24203030 2 > $DCC_PATH/config
    echo 0x24203030 2 > $DCC_PATH/config
    echo 0x24203030 2 > $DCC_PATH/config
    echo 0x24203030 2 > $DCC_PATH/config
    echo 0x24203038 1 > $DCC_PATH/config
    echo 0x24203030 2 > $DCC_PATH/config
    echo 0x24203030 2 > $DCC_PATH/config
    echo 0x24203030 2 > $DCC_PATH/config
    echo 0x24203030 2 > $DCC_PATH/config
    echo 0x24203008 2 > $DCC_PATH/config
    #; gem_noc_qns_pcie_poc_dbg
    echo 0x24203410 1 > $DCC_PATH/config
    echo 0x24203438 1 > $DCC_PATH/config
    echo 0x24203430 2 > $DCC_PATH/config
    echo 0x24203430 2 > $DCC_PATH/config
    echo 0x24203430 2 > $DCC_PATH/config
    echo 0x24203430 2 > $DCC_PATH/config
    echo 0x24203438 1 > $DCC_PATH/config
    echo 0x24203430 2 > $DCC_PATH/config
    echo 0x24203430 2 > $DCC_PATH/config
    echo 0x24203430 2 > $DCC_PATH/config
    echo 0x24203430 2 > $DCC_PATH/config
    echo 0x24203438 1 > $DCC_PATH/config
    echo 0x24203430 2 > $DCC_PATH/config
    echo 0x24203430 2 > $DCC_PATH/config
    echo 0x24203430 2 > $DCC_PATH/config
    echo 0x24203430 2 > $DCC_PATH/config
    echo 0x24203438 1 > $DCC_PATH/config
    echo 0x24203430 2 > $DCC_PATH/config
    echo 0x24203430 2 > $DCC_PATH/config
    echo 0x24203430 2 > $DCC_PATH/config
    echo 0x24203430 2 > $DCC_PATH/config
    echo 0x24203408 2 > $DCC_PATH/config

    #; Coherent_even_chain
    echo 0x24104018 1 > $DCC_PATH/config
    echo 0x24104008 1 > $DCC_PATH/config
    echo 0x24104010 2 > $DCC_PATH/config
    echo 0x24104010 2 > $DCC_PATH/config
    echo 0x24104010 2 > $DCC_PATH/config
    echo 0x24104010 2 > $DCC_PATH/config
    #; NonCoherent_even_chain
    echo 0x24104098 1 > $DCC_PATH/config
    echo 0x24104088 1 > $DCC_PATH/config
    echo 0x24104090 2 > $DCC_PATH/config
    echo 0x24104090 2 > $DCC_PATH/config
    echo 0x24104090 2 > $DCC_PATH/config
    echo 0x24104090 2 > $DCC_PATH/config
    echo 0x24104090 2 > $DCC_PATH/config
    #; Coherent_odd_chain
    echo 0x24182018 1 > $DCC_PATH/config
    echo 0x24182008 1 > $DCC_PATH/config
    echo 0x24182010 2 > $DCC_PATH/config
    echo 0x24182010 2 > $DCC_PATH/config
    echo 0x24182010 2 > $DCC_PATH/config
    echo 0x24182010 2 > $DCC_PATH/config
    #; NonCoherent_odd_chain
    echo 0x24182098 1 > $DCC_PATH/config
    echo 0x24182088 1 > $DCC_PATH/config
    echo 0x24182090 2 > $DCC_PATH/config
    echo 0x24182090 2 > $DCC_PATH/config
    echo 0x24182090 2 > $DCC_PATH/config
    echo 0x24182090 2 > $DCC_PATH/config
    echo 0x24182090 2 > $DCC_PATH/config
    #; Coherent_sys_chain
    echo 0x24204018 1 > $DCC_PATH/config
    echo 0x24204008 1 > $DCC_PATH/config
    echo 0x24204010 2 > $DCC_PATH/config
    echo 0x24204010 2 > $DCC_PATH/config
    echo 0x24204010 2 > $DCC_PATH/config
    #; NonCoherent_sys_chain
    echo 0x24204098 1 > $DCC_PATH/config
    echo 0x24204088 1 > $DCC_PATH/config
    echo 0x24204090 2 > $DCC_PATH/config
    echo 0x24204090 2 > $DCC_PATH/config
    echo 0x24204090 2 > $DCC_PATH/config
    echo 0x24204090 2 > $DCC_PATH/config
    echo 0x24204090 2 > $DCC_PATH/config
    echo 0x24204090 2 > $DCC_PATH/config
    echo 0x24204090 2 > $DCC_PATH/config
    echo 0x24204090 2 > $DCC_PATH/config
    echo 0x24204090 2 > $DCC_PATH/config
    echo 0x24204090 2 > $DCC_PATH/config
    echo 0x24204090 2 > $DCC_PATH/config
    echo 0x24204090 2 > $DCC_PATH/config
}

gemnoc_dump_full_cxt()
{
    # gem_noc_qns_llcc_even_poc_dbg
    echo 0x24102028 0x2 > $DCC_PATH/config_write
    echo 0x40 > $DCC_PATH/loop
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x24102030 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x24102028 0x1 > $DCC_PATH/config_write

    # gem_noc_qns_llcc_odd_poc_dbg
    echo 0x24181028 0x2 > $DCC_PATH/config_write
    echo 0x40 > $DCC_PATH/loop
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x24181030 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    echo 0x24181028 0x1 > $DCC_PATH/config_write

}

dc_noc_dump()
{
    #; dc_noc_dch_erl
    echo 0x240e0010 1 > $DCC_PATH/config
    echo 0x240e0020 8 > $DCC_PATH/config
    echo 0x240e0248 1 > $DCC_PATH/config
    #; dc_noc_ch_hm02_erl
    # echo 0x245f0010 1 > $DCC_PATH/config
    # echo 0x245f0020 8 > $DCC_PATH/config
    # echo 0x245f0248 1 > $DCC_PATH/config
    #; dc_noc_ch_hm13_erl
    # echo 0x247f0010 1 > $DCC_PATH/config
    # echo 0x247f0020 8 > $DCC_PATH/config
    # echo 0x247f0248 1 > $DCC_PATH/config
    #; llclpi_noc_erl
    echo 0x24330010 1 > $DCC_PATH/config
    echo 0x24330020 8 > $DCC_PATH/config
    echo 0x24330248 1 > $DCC_PATH/config

    #; dch/DebugChain
    echo 0x240e1018 1 > $DCC_PATH/config
    echo 0x240e1008 1 > $DCC_PATH/config
    echo 0x9  > $DCC_PATH/loop
    echo 0x240e1010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    #; ch_hm02/DebugChain
    # echo 0x245f2018 1 > $DCC_PATH/config
    # echo 0x245f2008 1 > $DCC_PATH/config
    # echo 0x3  > $DCC_PATH/loop
    # echo 0x245f2010 2 > $DCC_PATH/config
    # echo 0x1 > $DCC_PATH/loop
    #; ch_hm13/DebugChain
    # echo 0x247f2018 1 > $DCC_PATH/config
    # echo 0x247f2008 1 > $DCC_PATH/config
    # echo 0x3  > $DCC_PATH/loop
    # echo 0x247f2010 2 > $DCC_PATH/config
    # echo 0x1 > $DCC_PATH/loop
    #; llclpi_noc/DebugChain
    echo 0x24331018 1 > $DCC_PATH/config
    echo 0x24331008 1 > $DCC_PATH/config
    echo 0x8  > $DCC_PATH/loop
    echo 0x24331010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
}

mmss_noc_dump()
{
    #; mmss_noc_erl
    echo 0x01780010 1 > $DCC_PATH/config
    echo 0x01780020 8 > $DCC_PATH/config
    echo 0x01780248 1 > $DCC_PATH/config
    #; mmss_noc/DebugChain
    echo 0x01782018 1 > $DCC_PATH/config
    echo 0x01782008 1 > $DCC_PATH/config
    echo 0xc  > $DCC_PATH/loop
    echo 0x01782010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    #; mmss_noc_QTB500/DebugChain
    echo 0x01783018 1 > $DCC_PATH/config
    echo 0x01783008 1 > $DCC_PATH/config
    echo 0x11  > $DCC_PATH/loop
    echo 0x01783010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
}

system_noc_dump()
{
    #; system_noc_erl
    echo 0x01680010 1 > $DCC_PATH/config
    echo 0x01680020 8 > $DCC_PATH/config
    echo 0x01681048 1 > $DCC_PATH/config
    #; system_noc/DebugChain
    echo 0x01682018 1 > $DCC_PATH/config
    echo 0x01682008 1 > $DCC_PATH/config
    echo 0x6  > $DCC_PATH/loop
    echo 0x01682010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
}

aggre_noc_dump()
{
    #; a1_noc_aggre_noc_erl
    echo 0x016e0010 1 > $DCC_PATH/config
    echo 0x016e0020 8 > $DCC_PATH/config
    echo 0x016e0248 1 > $DCC_PATH/config
    #; a1_noc_aggre_noc_south/DebugChain
    echo 0x016e1018 1 > $DCC_PATH/config
    echo 0x016e1008 1 > $DCC_PATH/config
    echo 0x4  > $DCC_PATH/loop
    echo 0x016e1010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    #; a1_noc_aggre_noc_ANOC_NIU/DebugChain
    echo 0x016e1098 1 > $DCC_PATH/config
    echo 0x016e1088 1 > $DCC_PATH/config
    echo 0x3  > $DCC_PATH/loop
    echo 0x016e1090 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    #; a1_noc_aggre_noc_ANOC_QTB/DebugChain
    echo 0x016e1118 1 > $DCC_PATH/config
    echo 0x016e1108 1 > $DCC_PATH/config
    echo 0x7  > $DCC_PATH/loop
    echo 0x016e1110 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop

    #; a2_noc_aggre_noc_erl
    echo 0x01700010 1 > $DCC_PATH/config
    echo 0x01700020 8 > $DCC_PATH/config
    echo 0x01700248 1 > $DCC_PATH/config
    #; a2_noc_aggre_noc_center/DebugChain
    echo 0x1702018 1 > $DCC_PATH/config
    echo 0x1702008 1 > $DCC_PATH/config
    echo 0x4  > $DCC_PATH/loop
    echo 0x1702010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    #; a2_noc_aggre_noc_east/DebugChain
    echo 0x1702218 1 > $DCC_PATH/config
    echo 0x1702208 1 > $DCC_PATH/config
    echo 0x2  > $DCC_PATH/loop
    echo 0x1702210 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    #; a2_noc_aggre_noc_north/DebugChain
    echo 0x1702118 1 > $DCC_PATH/config
    echo 0x1702108 1 > $DCC_PATH/config
    echo 0x3  > $DCC_PATH/loop
    echo 0x1702110 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
}

config_noc_dump()
{
    #; cnoc_cfg_erl
    echo 0x01600010 1 > $DCC_PATH/config
    echo 0x01600020 8 > $DCC_PATH/config
    echo 0x01600248 2 > $DCC_PATH/config
    echo 0x01600258 1 > $DCC_PATH/config
    #; cnoc_cfg_center/DebugChain
    echo 0x01602018 1 > $DCC_PATH/config
    echo 0x01602008 1 > $DCC_PATH/config
    echo 0x7  > $DCC_PATH/loop
    echo 0x01602010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    #; cnoc_cfg_west/DebugChain
    echo 0x01602098 1 > $DCC_PATH/config
    echo 0x01602088 1 > $DCC_PATH/config
    echo 0x2  > $DCC_PATH/loop
    echo 0x01602090 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    #; cnoc_cfg_mmnoc/DebugChain
    echo 0x01602118 1 > $DCC_PATH/config
    echo 0x01602108 1 > $DCC_PATH/config
    echo 0x3  > $DCC_PATH/loop
    echo 0x01602110 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    #; cnoc_cfg_north/DebugChain
    echo 0x01602198 1 > $DCC_PATH/config
    echo 0x01602188 1 > $DCC_PATH/config
    echo 0x3  > $DCC_PATH/loop
    echo 0x01602190 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    #; cnoc_cfg_south/DebugChain
    echo 0x01602218 1 > $DCC_PATH/config
    echo 0x01602208 1 > $DCC_PATH/config
    echo 0x2  > $DCC_PATH/loop
    echo 0x01602210 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    #; cnoc_cfg_east/DebugChain
    echo 0x1602098 1 > $DCC_PATH/config
    echo 0x1602088 1 > $DCC_PATH/config
    echo 0x2  > $DCC_PATH/loop
    echo 0x1602090 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop

    #; cnoc_main_erl
    echo 0x01500010 1 > $DCC_PATH/config
    echo 0x01500020 8 > $DCC_PATH/config
    echo 0x01500248 1 > $DCC_PATH/config
    echo 0x01500448 1 > $DCC_PATH/config
    #; cnoc_main_center/DebugChain
    echo 0x01502018 1 > $DCC_PATH/config
    echo 0x01502008 1 > $DCC_PATH/config
    echo 0x7  > $DCC_PATH/loop
    echo 0x01502010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
    #; cnoc_main_north/DebugChain
    echo 0x01502098 1 > $DCC_PATH/config
    echo 0x01502088 1 > $DCC_PATH/config
    echo 0x7  > $DCC_PATH/loop
    echo 0x01502090 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
}

apss_noc_dump()
{
    # apps noc
    echo 0x16E00010 1 > $DCC_PATH/config
    echo 0x16E00020 8 > $DCC_PATH/config
    echo 0x16E00248 1 > $DCC_PATH/config
    echo 0x16E01018 1 > $DCC_PATH/config
    echo 0x16E01008 1 > $DCC_PATH/config
    echo 0x6  > $DCC_PATH/loop
    echo 0x16E01010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop

    #apss_nsinoc
    echo 0x1B600010 1 > $DCC_PATH/config
    echo 0x1B600020 8 > $DCC_PATH/config
    echo 0x1B600248 1 > $DCC_PATH/config
    echo 0x1B601018 1 > $DCC_PATH/config
    echo 0x1B601008 1 > $DCC_PATH/config
    echo 0x6  > $DCC_PATH/loop
    echo 0x1B601010 2 > $DCC_PATH/config
    echo 0x1 > $DCC_PATH/loop
}

config_dcc_gic()
{
    echo 0x16000104 30 > $DCC_PATH/config
    echo 0x16000204 29 > $DCC_PATH/config
    echo 0x16000384 30 > $DCC_PATH/config
}

config_dcc_rpmh()
{
    echo 0xB291024 > $DCC_PATH/config
    #CX/MX/DDRAUX_SEQ
    echo 0xC201244 > $DCC_PATH/config
    echo 0xC202244 > $DCC_PATH/config
    echo 0xBDE1034 > $DCC_PATH/config
    echo 0xBDE1038 > $DCC_PATH/config

    #RPMH_PDC_APSS
    echo 0xB201020 2 > $DCC_PATH/config
    echo 0xB211020 2 > $DCC_PATH/config
    echo 0xB221020 2 > $DCC_PATH/config
    echo 0xB231020 2 > $DCC_PATH/config
    echo 0xB204520 > $DCC_PATH/config
    echo 0xB200000 1 > $DCC_PATH/config
    echo 0xB210000 1 > $DCC_PATH/config
    echo 0xB220000 1 > $DCC_PATH/config
    echo 0xB230000 1 > $DCC_PATH/config
}

config_dcc_apss_rscc()
{
    #APSS_RSCC_RSC register
    echo 0x16500010 > $DCC_PATH/config
    echo 0x16510010 > $DCC_PATH/config
    echo 0x16520010 > $DCC_PATH/config
    echo 0x16530010 > $DCC_PATH/config
    echo 0x16500030 > $DCC_PATH/config
    echo 0x16510030 > $DCC_PATH/config
    echo 0x16520030 > $DCC_PATH/config
    echo 0x16530030 > $DCC_PATH/config
    echo 0x16500038 > $DCC_PATH/config
    echo 0x16510038 > $DCC_PATH/config
    echo 0x16520038 > $DCC_PATH/config
    echo 0x16530038 > $DCC_PATH/config
    echo 0x16500040 > $DCC_PATH/config
    echo 0x16510040 > $DCC_PATH/config
    echo 0x16520040 > $DCC_PATH/config
    echo 0x16530040 > $DCC_PATH/config
    echo 0x16500048 > $DCC_PATH/config
    echo 0x16500400 3 > $DCC_PATH/config
    echo 0x16510400 3 > $DCC_PATH/config
    echo 0x16520400 3 > $DCC_PATH/config
    echo 0x16530400 3 > $DCC_PATH/config
    echo 0x16510d3c  > $DCC_PATH/config
    echo 0x16510d54  > $DCC_PATH/config
    echo 0x16510d6c  > $DCC_PATH/config
    echo 0x16510d84  > $DCC_PATH/config
    echo 0x16510d9c  > $DCC_PATH/config
    echo 0x16510db4  > $DCC_PATH/config
    echo 0x16510dcc  > $DCC_PATH/config
    echo 0x16510de4  > $DCC_PATH/config
    echo 0x16510dfc  > $DCC_PATH/config
    echo 0x16510e14  > $DCC_PATH/config
    echo 0x16510e2c  > $DCC_PATH/config
    echo 0x16510e44  > $DCC_PATH/config
    echo 0x16510e5c  > $DCC_PATH/config
    echo 0x16510e74  > $DCC_PATH/config
    echo 0x16510e8c  > $DCC_PATH/config
    echo 0x16510ea4  > $DCC_PATH/config
    echo 0x16510fdc  > $DCC_PATH/config
    echo 0x16510ff4  > $DCC_PATH/config
    echo 0x1651100c  > $DCC_PATH/config
    echo 0x16511024  > $DCC_PATH/config
    echo 0x1651103c  > $DCC_PATH/config
    echo 0x16511054  > $DCC_PATH/config
    echo 0x1651106c  > $DCC_PATH/config
    echo 0x16511084  > $DCC_PATH/config
    echo 0x1651109c  > $DCC_PATH/config
    echo 0x165110b4  > $DCC_PATH/config
    echo 0x165110cc  > $DCC_PATH/config
    echo 0x165110e4  > $DCC_PATH/config
    echo 0x165110fc  > $DCC_PATH/config
    echo 0x16511114  > $DCC_PATH/config
    echo 0x1651112c  > $DCC_PATH/config
    echo 0x16511144  > $DCC_PATH/config
    echo 0x1651127c  > $DCC_PATH/config
    echo 0x16511294  > $DCC_PATH/config
    echo 0x165112ac  > $DCC_PATH/config
    echo 0x165112c4  > $DCC_PATH/config
    echo 0x165112dc  > $DCC_PATH/config
    echo 0x165112f4  > $DCC_PATH/config
    echo 0x1651130c  > $DCC_PATH/config
    echo 0x16511324  > $DCC_PATH/config
    echo 0x1651133c  > $DCC_PATH/config
    echo 0x16511354  > $DCC_PATH/config
    echo 0x1651136c  > $DCC_PATH/config
    echo 0x16511384  > $DCC_PATH/config
    echo 0x1651139c  > $DCC_PATH/config
    echo 0x165113b4  > $DCC_PATH/config
    echo 0x165113cc  > $DCC_PATH/config
    echo 0x165113e4  > $DCC_PATH/config
    echo 0x1651151c  > $DCC_PATH/config
    echo 0x16511534  > $DCC_PATH/config
    echo 0x1651154c  > $DCC_PATH/config
    echo 0x16511564  > $DCC_PATH/config
    echo 0x1651157c  > $DCC_PATH/config
    echo 0x16511594  > $DCC_PATH/config
    echo 0x165115ac  > $DCC_PATH/config
    echo 0x165115c4  > $DCC_PATH/config
    echo 0x165115dc  > $DCC_PATH/config
    echo 0x165115f4  > $DCC_PATH/config
    echo 0x1651160c  > $DCC_PATH/config
    echo 0x16511624  > $DCC_PATH/config
    echo 0x1651163c  > $DCC_PATH/config
    echo 0x16511654  > $DCC_PATH/config
    echo 0x1651166c  > $DCC_PATH/config
    echo 0x16511684  > $DCC_PATH/config
}

config_dcc_anoc_pcie()
{
    echo 0x110004 2 > $DCC_PATH/config
    echo 0x11003C 3 > $DCC_PATH/config
    #RPMH_SYS_NOC_CMD_DFSR
    echo 0x176040 1 > $DCC_PATH/config
}

config_dcc_rng()
{
    echo 0x10C0000 4  > $DCC_PATH/config
    echo 0x10C1000 2  > $DCC_PATH/config
    echo 0x10C1010 7  > $DCC_PATH/config
    echo 0x10C1100 3  > $DCC_PATH/config
    echo 0x10C1110 5  > $DCC_PATH/config
    echo 0x10C1130 2  > $DCC_PATH/config
    echo 0x10C113C 2  > $DCC_PATH/config
    echo 0x10C1148 3  > $DCC_PATH/config
    echo 0x10C1800 11 > $DCC_PATH/config
    echo 0x10C2000 1  > $DCC_PATH/config
    echo 0x10CF004 1  > $DCC_PATH/config
}

config_clk_gdsc()
{
    echo 0x0ADF00C8 1 > $DCC_PATH/config
    echo 0x0ADF017C 1 > $DCC_PATH/config
    echo 0x0ADF1004 1 > $DCC_PATH/config
    echo 0x0ADF1084 1 > $DCC_PATH/config
    echo 0x0ADF10EC 1 > $DCC_PATH/config
    echo 0x0ADF134C 1 > $DCC_PATH/config
    echo 0x0AF09000 1 > $DCC_PATH/config
    echo 0x0AF0B000 1 > $DCC_PATH/config
    echo 0x015214C  1 > $DCC_PATH/config
    echo 0x017914C  1 > $DCC_PATH/config
    echo 0x0150018  1 > $DCC_PATH/config
    echo 0x016B004  1 > $DCC_PATH/config
    echo 0x016C000  1 > $DCC_PATH/config
    echo 0x0177004  1 > $DCC_PATH/config
    echo 0x0139004  1 > $DCC_PATH/config
    echo 0x03D99080 1 > $DCC_PATH/config
    echo 0x03D99504 1 > $DCC_PATH/config
    echo 0x03D99094 1 > $DCC_PATH/config
    echo 0x03D68024 1 > $DCC_PATH/config
    echo 0x0ABF8034 1 > $DCC_PATH/config
    echo 0x0ABF8068 1 > $DCC_PATH/config
    echo 0xAAF8034  1 > $DCC_PATH/config
    echo 0xAAF8068  1 > $DCC_PATH/config
    echo 0x0126004  1 > $DCC_PATH/config
    echo 0x0126034  1 > $DCC_PATH/config
    echo 0x01A0004  1 > $DCC_PATH/config
    echo 0x019F004  1 > $DCC_PATH/config
    echo 0x019F01C  1 > $DCC_PATH/config
    echo 0x0111004  1 > $DCC_PATH/config
    echo 0x0111008  1 > $DCC_PATH/config
    echo 0x014402C  1 > $DCC_PATH/config
    echo 0x0127004  1 > $DCC_PATH/config
    echo 0x0171004  1 > $DCC_PATH/config
    echo 0x0132004  1 > $DCC_PATH/config
    echo 0x0132038  1 > $DCC_PATH/config
    echo 0x3D99008  1 > $DCC_PATH/config
    echo 0x3D99010  1 > $DCC_PATH/config
    echo 0x3D993A4  1 > $DCC_PATH/config
    echo 0x3D99064  1 > $DCC_PATH/config
    echo 0x3D993A8  1 > $DCC_PATH/config
    echo 0x3D99004  1 > $DCC_PATH/config
    echo 0x3D990CC  1 > $DCC_PATH/config
    echo 0xAF0E05C  1 > $DCC_PATH/config
    echo 0xAF0E07C  1 > $DCC_PATH/config
    echo 0xAF0C00C  1 > $DCC_PATH/config
    echo 0xAF0C008  1 > $DCC_PATH/config
    echo 0xADF137C  1 > $DCC_PATH/config
    echo 0xADF13C4  1 > $DCC_PATH/config
    echo 0xADF13C0  1 > $DCC_PATH/config
    echo 0xADF1398  1 > $DCC_PATH/config
    echo 0xABF80A4  1 > $DCC_PATH/config
    echo 0xABF80F8  1 > $DCC_PATH/config
    echo 0xABF80D4  1 > $DCC_PATH/config
    echo 0xAAF80A4  1 > $DCC_PATH/config
    echo 0xAAF80F8  1 > $DCC_PATH/config
    echo 0xAAF80D4  1 > $DCC_PATH/config
    echo 0x0152000  1 > $DCC_PATH/config
    echo 0x0152008  1 > $DCC_PATH/config
}

enable_dcc()
{
    #TODO: Add DCC configuration
    DCC_PATH="/sys/bus/platform/devices/100ff000.dcc"
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
    echo 1 > $DCC_PATH/ap_ns_qad_override_en
    config_dcc_timer
    # config_qdsp_lpm
    config_dcc_core

    gemnoc_dump
    gemnoc_dump_full_cxt
    # config_dcc_ddr
    # config_turing
    config_dcc_timer

    echo 4 > $DCC_PATH/curr_list
    echo cap > $DCC_PATH/func_type
    echo sram > $DCC_PATH/data_sink
    echo 1 > $DCC_PATH/ap_ns_qad_override_en
    config_dcc_timer
    dc_noc_dump
    # config_lpass
    mmss_noc_dump
    system_noc_dump
    aggre_noc_dump
    config_noc_dump
    apss_noc_dump

    config_dcc_gic
    config_dcc_rpmh
    config_dcc_apss_rscc
    config_dcc_anoc_pcie
    config_dcc_rng
    config_clk_gdsc
    config_dcc_timer

    echo  1 > $DCC_PATH/enable
}

create_stp_policy()
{
    mkdir /config/stp-policy/coresight-stm:p_ost.policy
    chmod 660 /config/stp-policy/coresight-stm:p_ost.policy
    mkdir /config/stp-policy/coresight-stm:p_ost.policy/default
    chmod 660 /config/stp-policy/coresight-stm:p_ost.policy/default
    echo ftrace > /config/stp-policy/coresight-stm:p_ost.policy/default/entity
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
    chmod 660 /sys/devices/platform/soc/soc:modem_diag/coresight-modem-diag/enable_source
}

find_build_type()
{
    linux_banner=`cat /proc/version`
    if [[ "$linux_banner" == *"-consolidate"* ]]
    then
        debug_build=true
    fi
}

debug_build=false
enable_debug()
{
    echo "sun debug"
    find_build_type
    create_stp_policy
    adjust_permission
    enable_dcc
}

enable_debug
