#=============================================================================
# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#=============================================================================


# function to enable ftrace events

enable_sched_events()
{
    local instance=/sys/kernel/tracing

    echo > $instance/trace
    echo > $instance/set_event

    # timer
    echo 1 > $instance/events/timer/timer_expire_entry/enable
    echo 1 > $instance/events/timer/timer_expire_exit/enable
    echo 1 > $instance/events/timer/hrtimer_cancel/enable
    echo 1 > $instance/events/timer/hrtimer_expire_entry/enable
    echo 1 > $instance/events/timer/hrtimer_expire_exit/enable
    echo 1 > $instance/events/timer/hrtimer_init/enable
    echo 1 > $instance/events/timer/hrtimer_start/enable
    #enble FTRACE for softirq events
    echo 1 > $instance/events/irq/enable
    #enble FTRACE for Workqueue events
    echo 1 > $instance/events/workqueue/enable
    # sched
    echo 1 > $instance/events/sched/sched_migrate_task/enable
    echo 1 > $instance/events/sched/sched_pi_setprio/enable
    echo 1 > $instance/events/sched/sched_switch/enable
    echo 1 > $instance/events/sched/sched_wakeup/enable
    echo 1 > $instance/events/sched/sched_wakeup_new/enable

    # hot-plug
    echo 1 > $instance/events/cpuhp/enable

    echo 1 > $instance/events/power/cpu_frequency/enable

    echo 1 > $instance/tracing_on
}

enable_rproc_events()
{
    local instance=/sys/kernel/tracing/instances/rproc_qcom

    mkdir $instance
    echo > $instance/trace
    echo > $instance/set_event

    # enable rproc events as soon as available
    /vendor/bin/init.qti.write.sh $instance/events/rproc_qcom/enable 1

    echo 1 > $instance/tracing_on
}

# Suspend events are also noisy when going into suspend/resume
enable_suspend_events()
{
    local instance=/sys/kernel/tracing/instances/suspend

    mkdir $instance
    echo > $instance/trace
    echo > $instance/set_event

    echo 1 > $instance/events/power/suspend_resume/enable
    echo 1 > $instance/events/power/device_pm_callback_start/enable
    echo 1 > $instance/events/power/device_pm_callback_end/enable

    echo 1 > $instance/tracing_on
}

enable_clock_reg_events()
{
    local instance=/sys/kernel/tracing/instances/clock_reg

    mkdir $instance
    echo > $instance/trace
    echo > $instance/set_event

    # clock
    echo 1 > $instance/events/clk/enable
    echo 1 > $instance/events/clk_qcom/enable

    # interconnect
    echo 1 > $instance/events/interconnect/enable

    # regulator
    echo 1 > $instance/events/regulator/enable

    #thermal
    echo 1 > /sys/kernel/tracing/events/thermal/thermal_pre_core_offline/enable
    echo 1 > /sys/kernel/tracing/events/thermal/thermal_post_core_offline/enable
    echo 1 > /sys/kernel/tracing/events/thermal/thermal_pre_core_online/enable
    echo 1 > /sys/kernel/tracing/events/thermal/thermal_post_core_online/enable
    echo 1 > /sys/kernel/tracing/events/thermal/thermal_pre_frequency_mit/enable
    echo 1 > /sys/kernel/tracing/events/thermal/thermal_post_frequency_mit/enable

    # rpmh
    echo 1 > $instance/events/rpmh/enable

    echo 1 > $instance/tracing_on
}

enable_memory_events()
{
    local instance=/sys/kernel/tracing/instances/memory

    mkdir $instance
    echo > $instance/trace
    echo > $instance/set_event

    #memory pressure events/oom
    echo 1 > $instance/events/psi/psi_event/enable
    echo 1 > $instance/events/psi/psi_window_vmstat/enable
    echo 1 > $instance/events/arm_smmu/enable

    echo 1 > $instance/tracing_on
}

# binder tracing can be noisy
enable_binder_events()
{
    local instance=/sys/kernel/tracing/instances/binder

    mkdir $instance
    echo > $instance/trace
    echo > $instance/set_event

    echo 1 > $instance/events/binder/enable

    echo 1 > $instance/tracing_on
}

enable_rwmmio_events()
{
    if [ ! -d /sys/kernel/tracing/events/rwmmio ]
    then
        return
    fi

    local instance=/sys/kernel/tracing/instances/rwmmio

    mkdir $instance
    echo > $instance/trace
    echo > $instance/set_event

    echo 1 > $instance/events/rwmmio/rwmmio_read/enable
    echo 1 > $instance/events/rwmmio/rwmmio_write/enable

    echo 1 > $instance/tracing_on
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
enable_ftrace_event_tracing()
{
    # bail out if its perf config
     find_build_type
     if [ "$debug_build" = false ]
     then
        return
    fi

    # bail out if ftrace events aren't present
    if [ ! -d /sys/kernel/tracing/events ]
    then
        return
    fi

    enable_sched_events
    enable_rproc_events
    enable_suspend_events
    enable_binder_events
    enable_clock_reg_events
    enable_memory_events
    enable_rwmmio_events
}

ftrace_disable=`getprop persist.debug.ftrace_events_disable`
if [ "$ftrace_disable" != "Yes" ]; then
    enable_ftrace_event_tracing
fi
