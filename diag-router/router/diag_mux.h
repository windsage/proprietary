/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Internal Header File for diag mux layer

GENERAL DESCRIPTION

Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef DIAG_MUX_H
#define DIAG_MUX_H
#define DIAG_USB_MODE			0
#define DIAG_MEMORY_DEVICE_MODE		1
#define DIAG_NO_LOGGING_MODE		2
#define DIAG_MULTI_MODE			3
#define DIAG_PCIE_MODE		4

#define DIAG_MUX_LOCAL		0
#define DIAG_MUX_LOCAL_LAST	1
#define DIAG_MUX_BRIDGE_BASE	DIAG_MUX_LOCAL_LAST
#define DIAG_MUX_MDM		(DIAG_MUX_BRIDGE_BASE)
#define DIAG_MUX_MDM2		(DIAG_MUX_BRIDGE_BASE + 1)
#define DIAG_MUX_SMUX		(DIAG_MUX_BRIDGE_BASE + 2)
#define DIAG_MUX_BRIDGE_LAST	(DIAG_MUX_BRIDGE_BASE + 3)

#define NUM_MUX_PROC		DIAG_MUX_BRIDGE_LAST
#define BRIDGE_TO_MUX(x)	(x + DIAG_MUX_BRIDGE_BASE)
struct diag_mux_state_t {
	struct diag_logger_t *logger[NUM_MUX_PROC];
	struct diag_logger_t *usb_ptr;
	struct diag_logger_t *md_ptr;
	struct diag_logger_t *socket_ptr;
	struct diag_logger_t *pcie_ptr;
	struct diag_logger_t *gvm_ptr;
	unsigned int mux_mask[NUM_MUX_PROC];
	unsigned int mode[NUM_MUX_PROC];
};

struct diag_logger_ops {
	int (*write)(struct diag_client *dm, int proc, int peripheral, unsigned char *buf, int len, struct watch_flow *flow, int cmd_rsp_flag);
	int (*open)(void);
	int (*close)(void);
	int (*open_device)(int id);
	int (*close_device)(int id);
};

struct diag_logger_t {
	int mode;
	struct diag_mux_ops *ops[NUM_MUX_PROC];
	struct diag_logger_ops *log_ops;
};

extern struct diag_mux_state_t *diag_mux;

int diag_mux_init(void);
void diag_mux_exit(void);
int diag_md_write(struct diag_client *dm, int proc, int peripheral,unsigned char *buf, int len, struct watch_flow *flow, int cmd_rsp_flag);
int diag_usb_write(struct diag_client *dm, int proc, int peripheral, unsigned char *buf, int len, struct watch_flow *flow, int cmd_rsp_flag);
int diag_socket_write(struct diag_client *dm, int proc, int peripheral, unsigned char *buf, int len, struct watch_flow *flow, int cmd_rsp_flag);
int diag_pcie_write(struct diag_client *dm, int proc, int peripheral, unsigned char *buf, int len, struct watch_flow *flow, int cmd_rsp_flag);
int diag_vm_write(struct diag_client *dm, int proc, int peripheral, unsigned char *buf, int len, struct watch_flow *flow, int cmd_rsp_flag);
int diag_mux_switch_logging(struct diag_client *dm, int proc, int *new_mode, unsigned int *peripheral_mask);
void diag_usb_mux_switch_cleanup(void);
int diag_usb_disconnect(int proc);
int diag_usb_connect(int proc);
int diag_usb_disconnect_all(void);
int diag_usb_connect_all(void);
int diag_pcie_open(void);
int diag_pcie_close(void);
int diag_mux_close_peripheral(int proc, uint8_t peripheral);
int diag_apps_rsp_send(int pid, unsigned char *resp, unsigned int write_len);

#ifdef __cplusplus
extern "C" {
#endif
int diag_mux_write(struct diag_client *dm, int proc, int peripheral, unsigned char *buf, int len, struct watch_flow *flow, int cmd_rsp_flag);
#ifdef __cplusplus
}
#endif
#endif


