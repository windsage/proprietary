/*
 * Copyright (c) 2019, 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __CNSS_GENL_H__
#define __CNSS_GENL_H__

#define MAX_FILE_SIZE ((128) * (1024) * (1024))
#define DEFAULT_FILE_SIZE ((128) * (1024) * (1024))

#define MAX_FILE_COUNT 30
#define DEFAULT_FILE_COUNT 10

int cnss_fw_data_init(unsigned int max_file_count, unsigned long max_file_size);
void cnss_fw_data_deinit(void);
int cnss_genl_init(void);
void cnss_genl_exit(void);
int cnss_genl_recvmsgs(void);
int cnss_genl_get_fd(void);
int cnss_genl_send_data(uint8_t type, uint32_t instance_id, uint32_t value);
#endif
