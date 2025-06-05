/*
 * Copyright (c) 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef LIMITS_NETLINK_H__
#define LIMITS_NETLINK_H__

#include <thread>
#include <netlink/genl/genl.h>
#include <netlink/genl/mngt.h>
#include <netlink/genl/ctrl.h>
#include <netlink/netlink.h>

namespace vendor::qti::hardware::limits::implementation {

using eventMonitorCB = std::function<void(int, int)>;

class LimitsNetlink {
	public:
		LimitsNetlink(const eventMonitorCB &inp_event_cb);
		~LimitsNetlink();

		void parse_and_notify(char *inp_buf, ssize_t len);
		bool stopPolling()
		{
			return monitor_shutdown;
		}
		void start();
		int family_msg_cb(struct nl_msg *msg, void *data);
		int event_parse(struct nl_msg *n, void *data);
	private:
		std::thread event_th;
		struct nl_sock *event_soc;
		int event_group;
		bool monitor_shutdown;
		eventMonitorCB event_cb;

		int fetch_group_id();
		int send_nl_msg(struct nl_msg *msg);
};

}  // vendor::qti::hardware::limits::implementation

#endif  // LIMITS_NETLINK_H__
