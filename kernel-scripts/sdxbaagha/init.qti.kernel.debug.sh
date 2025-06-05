#=============================================================================
# Copyright (c) 2023 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#=============================================================================

adjust_sdxbaagha_tmc_permission()
{
    #add permission for block_size, mem_type, mem_size nodes to collect diag over QDSS by ODL
    echo "++++ $0 -> adjust_permission START" > /dev/kmsg
    chmod 660 /sys/devices/platform/soc/24048000.tmc/coresight-tmc-etr/block_size
    chmod 660 /sys/devices/platform/soc/24048000.tmc/coresight-tmc-etr/buffer_size
    #chown diag:root /dev/byte-cntr
    echo "++++ $0 -> adjust_permission END" > /dev/kmsg
}

create_sdxbaagha_stm_stp_policy()
{
    echo "++++ $0 -> create_stp_policy START" > /dev/kmsg
    mkdir /sys/kernel/config/stp-policy/coresight-stm:p_ost.policy
    chmod 660 /sys/kernel/config/stp-policy/coresight-stm:p_ost.policy
    mkdir /sys/kernel/config/stp-policy/coresight-stm:p_ost.policy/default
    chmod 660 /sys/kernel/config/stp-policy/coresight-stm:p_ost.policy/default
    echo 0x10 > /sys/bus/coresight/devices/coresight-stm/traceid
    echo "++++ $0 -> create_stp_policy END" > /dev/kmsg
}

#function to enable cti flush for etf
enable_sdxbaagha_cti_flush_for_etf()
{
    echo "++++ $0 -> enable_cti_flush_for_etf START" > /dev/kmsg
    # bail out if its perf config
    if [ ! -d /sys/module/coresight_etm3x ]
    then
        return
    fi

    echo 1 >/sys/bus/coresight/devices/coresight-cti-swao_cti/enable
    echo 0 24 >/sys/bus/coresight/devices/coresight-cti-swao_cti/channels/trigin_attach
    echo 0 1 >/sys/bus/coresight/devices/coresight-cti-swao_cti/channels/trigout_attach
    echo "++++ $0 -> enable_cti_flush_for_etf END" > /dev/kmsg
}

#function to enable modem cmb at boot time
enable_sdxbaagha_modem_cmb()
{
    echo "++++ $0 -> enable modem cmb START" > /dev/kmsg
    # bail out if its perf config
    if [ ! -d /sys/module/coresight_etm3x ]
    then
        return
    fi

    echo 0x400000 >/sys/bus/coresight/devices/coresight-tmc-etr/buffer_size
    echo 1 >/sys/bus/coresight/devices/coresight-tmc-etr/enable_sink
    #Enabling MSS_TPDM1_CMB source to trace sink:
    echo 1 >/sys/bus/coresight/devices/coresight-tpdm-modem-1/enable_source
    qdss_qmi_helper modem tpdm 0x02 dataset 0x04 enable

    #Enabling MSS_TPDM2_CMB source to trace sink:
    echo 1 >/sys/bus/coresight/devices/coresight-tpdm-modem-2/enable_source
    qdss_qmi_helper modem tpdm 0x1B dataset 0x04 enable

    #Enabling MSS_TPDM3_CMB source to trace sink:
    echo 1 >/sys/bus/coresight/devices/coresight-tpdm-modem-3/enable_source
    qdss_qmi_helper modem tpdm 0x42 dataset 0x04 enable

    #Enabling MSS_TPDM4_CMB source to trace sink:
    echo 1 >/sys/bus/coresight/devices/coresight-tpdm-modem-4/enable_source
    qdss_qmi_helper modem tpdm 0x70 dataset 0x04 enable

    echo "++++ $0 -> enable mdoem cmb END" > /dev/kmsg
}

enable_sdxbaagha_debug()
{
    echo "++++ $0 -> enable_sdxbaagha_debug START" > /dev/kmsg
    adjust_sdxbaagha_tmc_permission
    create_sdxbaagha_stm_stp_policy
    enable_sdxbaagha_cti_flush_for_etf
    enable_sdxbaagha_modem_cmb
    echo "++++ $0 -> enable_sdxbaagha_debug_debug END" > /dev/kmsg
}

case "$1" in
"start")
    echo -n "Starting qdssconfig for SDXBAAGHA: "
    echo "++++ $0 -> START qdssconfig for SDXBAAGHA" > /dev/kmsg
    enable_sdxbaagha_debug
    echo "++++ $0 -> END qdssconfig for SDXBAAGHA" > /dev/kmsg
    echo "done";
    ;;
esac

case "$1" in
"restart")
    echo "deep sleep exit..." > /dev/kmsg
    echo "START run needed reinitialization for SDXBAAGHA" > /dev/kmsg
    echo "END run needed reinitialization for SDXBAAGHA" > /dev/kmsg
    ;;
esac

