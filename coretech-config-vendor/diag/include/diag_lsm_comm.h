#ifndef DIAG_LSM_COMM_H
#define DIAG_LSM_COMM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Header File for communication between diag lib, driver

GENERAL DESCRIPTION

Copyright (c) 2020-2021, 2023 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* The various IOCTLs */
#define DIAG_IOCTL_COMMAND_REG       0 /* IOCTL for packet registration
                                 Clients can use this to register to respond to packets from host tool */
#define DIAG_IOCTL_COMMAND_DEREG     1 /* IOCTL for de-registration */
/* Client process uses this to de-register itself, while unloading gracefully. */
#define DIAG_IOCTL_MASK_REG          2 /* IOCTL for registration for mask-change */
#define DIAG_IOCTL_MASK_DEREG        3
#define DIAG_IOCTL_GETEVENTMASK      4 /* For Client process to get event mask from DCM */
#define DIAG_IOCTL_GETLOGMASK        5
#define DIAG_IOCTL_GETMSGMASK        6
#define DIAG_IOCTL_GET_DELAYED_RSP_ID  8 /* Diag_LSM uses this IOCTL to get the next delayed response id
                                          in the system. */
#define DIAG_IOCTL_LSM_DEINIT		9
#define DIAG_IOCTL_SWITCH_LOGGING	7
#define DIAG_IOCTL_DCI_INIT		20
#define DIAG_IOCTL_DCI_DEINIT		21
#define DIAG_IOCTL_DCI_SUPPORT		22
#define DIAG_IOCTL_DCI_REG		23
#define DIAG_IOCTL_DCI_STREAM_INIT	24
#define DIAG_IOCTL_DCI_HEALTH_STATS	25
#define DIAG_IOCTL_DCI_LOG_STATUS	26
#define DIAG_IOCTL_DCI_EVENT_STATUS	27
#define DIAG_IOCTL_DCI_CLEAR_LOGS	28
#define DIAG_IOCTL_DCI_CLEAR_EVENTS	29
#define DIAG_IOCTL_REMOTE_DEV		32
#define DIAG_IOCTL_VOTE_REAL_TIME	33
#define DIAG_IOCTL_GET_REAL_TIME	34
#define DIAG_IOCTL_CONFIG_BUFFERING_TX_MODE	35
#define DIAG_IOCTL_BUFFERING_DRAIN_IMMEDIATE	36
#define DIAG_IOCTL_REGISTER_CALLBACK	37
#define DIAG_IOCTL_HDLC_TOGGLE	38
#define DIAG_IOCTL_QUERY_PD_LOGGING	39
#define DIAG_IOCTL_QUERY_CON_ALL	40
#define DIAG_IOCTL_QUERY_MD_PID	41
#define DIAG_IOCTL_QUERY_PD_FEATUREMASK	42
#define DIAG_IOCTL_PASSTHRU_CONTROL	43
#define DIAG_IOCTL_MDM_HDLC_TOGGLE	44
#define DIAG_IOCTL_QUERY_MASK		45
#define DIAG_IOCTL_SET_OVERRIDE_PID	47
#define DIAG_IOCTL_UPDATE_QDSS_ETR1_SUP	48
#define DIAG_IOCTL_VOTE_DCI_BUFFERING_MODE	49
#define DIAG_IOCTL_DCI_DRAIN_IMMEDIATE	50

#define DIAG_IOCTL_QUERY_REG_TABLE	75
#define DIAG_IOCTL_QUERY_DEBUG_ALL	76
#define DIAG_IOCTL_UPDATE_DYN_DBG_MASK	77
#define DIAG_IOCTL_GET_TRANSPORT_MODE	78

int diag_lsm_comm_open(void);
int diag_lsm_comm_write(int fd, unsigned char buf[], int bytes, int flags);
int diag_lsm_comm_ioctl(int fd, unsigned long request, void *buf, unsigned int len);

#endif /* DIAG_LSM_COMM_H */
