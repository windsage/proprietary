/*
 * image version cmd handler A module to handle commands(diag) that can be
 * processed in native code.
 *
 * Copyright (c) 2013-2014, 2017-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * image_version.c : Main implementation of image_version
 */

#include <poll.h>
#include <unistd.h>
#include "diag.h"
#include "diag_mux.h"
#include "image_version.h"

typedef PACK(struct)
{
	uint8 command_code;
	uint8 subsys_id;
	uint16 subsys_cmd_code;
	uint32 status;
	uint16 delayed_rsp_id;
	uint16 rsp_cnt;         /* 0, means one response and 1, means two responses */
} diagpkt_version_subsys_hdr_v2_type;

typedef PACK(struct)
{
	diagpkt_version_subsys_hdr_v2_type hdr;
	uint16 version_data_len;
	unsigned char version_data[VERSION_TABLE_S+1];
} diagpkt_version_delayed_rsp;

#define DIAG_SUBSYS_CMD_VER_2_F    128

static int fd_select_image = -1;
static int fd_image_version = -1;
static int fd_image_variant = -1;
static int fd_image_crm_version = -1;

static int img_ver_open_image_files(void)
{
	fd_select_image = open(SELECT_IMAGE_FILE, O_WRONLY);
	if (fd_select_image < 0) {
		ALOGE("img_ver: could not open select image file: %s", strerror(errno));
		return -1;
	}
	fd_image_version = open(IMAGE_VERSION_FILE, O_RDONLY);
	if (fd_image_version < 0) {
		ALOGE("img_ver: could not open image version file: %s", strerror(errno));
		close(fd_select_image);
		return -1;
	}

	fd_image_variant = open(IMAGE_VARIANT_FILE, O_RDONLY);
	if (fd_image_variant < 0) {
		ALOGE("img_ver: could not open image variant file: %s", strerror(errno));
		close(fd_select_image);
		close(fd_image_version);
		return -1;
	}

	fd_image_crm_version = open(IMAGE_CRM_VERSION_FILE, O_RDONLY);
	if (fd_image_crm_version < 0) {
		ALOGE("img_ver: could not open image crm version file: %s", strerror(errno));
		close(fd_select_image);
		close(fd_image_version);
		close(fd_image_variant);
		return -1;
	}
	return 0;
}

void img_ver_close_image_files(void)
{
	if (fd_select_image >= 0){
		close(fd_select_image);
		fd_select_image = -1;
	}
	if (fd_image_version >= 0){
		close(fd_image_version);
		fd_image_version = -1;
	}
	if (fd_image_variant >= 0){
		close(fd_image_variant);
		fd_image_variant = -1;
	}
	if (fd_image_crm_version >= 0){
		close(fd_image_crm_version);
		fd_image_crm_version = -1;
	}
}

static int img_ver_read_image_files(unsigned char *temp_version_table_p)
{
	int ret = 0;
	unsigned char *temp;

	if (!temp_version_table_p) {
		ALOGE("img_ver: Bad Address for image version: %s", strerror(errno));
		return -1;
	}

	temp = temp_version_table_p;
	if (fd_image_version >= 0) {
		ret = read(fd_image_version, temp, IMAGE_VERSION_NAME_SIZE);
		if (ret < 0) {
			ALOGE("img_ver: Unable to read image version file: %s", strerror(errno));
			return -1;
		}
	} else {
		ALOGE("img_ver: Fail to read, invalid fd_image_version");
		return -1;
	}

	temp += (IMAGE_VERSION_NAME_SIZE - 1);
	*temp++ = '\0';
	if (fd_image_variant >= 0) {
		ret = read(fd_image_variant, temp, IMAGE_VERSION_VARIANT_SIZE);
		if (ret < 0) {
			ALOGE("img_ver: Unable to read image variant file: %s", strerror(errno));
			return -1;
		}
	} else {
		ALOGE("img_ver: Fail to read, invalid fd_image_variant");
		return -1;
	}

	temp += (IMAGE_VERSION_VARIANT_SIZE - 1);
	*temp++ = '\0';
	if (fd_image_crm_version >= 0) {
		ret = read(fd_image_crm_version, temp, IMAGE_VERSION_OEM_SIZE);
		if (ret < 0) {
			ALOGE("img_ver: Unable to read image crm version file: %s", strerror(errno));
			return -1;
		}
	} else {
		ALOGE("img_ver: Fail to read, invalid fd_image_crm_version");
		return -1;
	}
	return 0;
}

static int get_version_info(diagpkt_version_delayed_rsp *version_table_p)
{
	int err = 0, ret, i, count = 0;
	unsigned char *temp_version_table_p;
	char image_index[3];

	temp_version_table_p = version_table_p->version_data;

	for (i = 0; i < (VERSION_TABLE_S/IMAGE_VERSION_SINGLE_BLOCK_SIZE); i++) {
		err = img_ver_open_image_files();
		if (err < 0) {
			ALOGE("img_ver: could not open image files %d", i);
			return -1;
		}
		count = (i < 10) ? 1 : 2;
		snprintf(image_index, sizeof(image_index), "%d", i);
		ret = write(fd_select_image, image_index, count);
		if (ret < 0) {
			ALOGE("img_ver: Unable to write %d in select image file: %s", i, strerror(errno));
			img_ver_close_image_files();
			return -1;
		}
		ret = img_ver_read_image_files(temp_version_table_p);
		if (ret < 0) {
			ALOGE("img_ver: Unable to read image file %d", i);
			img_ver_close_image_files();
			return -1;
		}
		temp_version_table_p += (IMAGE_VERSION_SINGLE_BLOCK_SIZE - 1);
		*temp_version_table_p++ = '\0';

		img_ver_close_image_files();
	}
	return 0;
}

int diagpkt_version_handler(struct diag_client *client, const void *buf, size_t len, int pid)
{
	diagpkt_version_subsys_hdr_v2_type rsp1 = {0};
	diagpkt_version_delayed_rsp rsp2 = {0};
	int err = 0;
	diagpkt_version_delayed_rsp *version_table_p = NULL;
	uint16 delay_rsp_id = 0;

	rsp1.command_code = DIAG_SUBSYS_CMD_VER_2_F;
	rsp1.subsys_id = VERSION_DIAGPKT_SUBSYS;
	rsp1.subsys_cmd_code = VERSION_DIAGPKT_PREFIX;
	rsp1.delayed_rsp_id = diag_get_next_delayed_rsp_id();
	rsp1.rsp_cnt = 0;
	rsp1.status = 0;
	if(!rsp1.delayed_rsp_id)
	{
		return -EINVAL;
	}

	/* Get the delayed_rsp_id that was allocated by diag to
	* use for the delayed response we're going to send next.
	* This id is unique in the system.
	*/
	delay_rsp_id = rsp1.delayed_rsp_id;//diagpkt_subsys_get_delayed_rsp_id(rsp);

	err = diag_apps_rsp_send(pid, (unsigned char *)&rsp1, 12);

	rsp2.hdr.command_code = DIAG_SUBSYS_CMD_VER_2_F;
	rsp2.hdr.subsys_id = VERSION_DIAGPKT_SUBSYS;
	rsp2.hdr.subsys_cmd_code = VERSION_DIAGPKT_PREFIX;
	rsp2.hdr.delayed_rsp_id = delay_rsp_id;
	rsp2.hdr.rsp_cnt = 1;
	rsp2.hdr.status = 0;

	version_table_p = (diagpkt_version_delayed_rsp*) &rsp2;

	err = get_version_info(version_table_p);
	if (err < 0) {
		ALOGE("img_ver: could not get image version info");
		return -EINVAL;
	}
	version_table_p->version_data_len =
		sizeof(version_table_p->version_data);

	diag_apps_rsp_send(pid, (unsigned char *)&rsp2, sizeof(diagpkt_version_delayed_rsp));

	return 0;
}

void image_version_init(void)
{

	register_fallback_subsys_cmd_v2(VERSION_DIAGPKT_SUBSYS, VERSION_DIAGPKT_PREFIX,
		diagpkt_version_handler);

	return;
}
