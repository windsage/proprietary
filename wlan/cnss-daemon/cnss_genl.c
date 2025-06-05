/*
 * Copyright (c) 2019, 2021-2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <dirent.h>
#include <ctype.h>
#include <limits.h>

#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>

#include "debug.h"
#include "cnss_genl.h"
#include "cnss_genl_msg.h"
#include "cnss_plat.h"

struct cnss_nl_data {
	struct nl_sock *sock;
	int family_id;
	int mcgrp_id;
	struct cnss_evt_queue evt_q;
};

static struct cnss_nl_data cnss_nl;

#ifndef IPQ
struct fw_data {
	char file_name[QMI_WLFW_MAX_STR_LEN_V01 + 1];
	int fidx;
	unsigned long max_file_size;
	unsigned long free_size;
	unsigned int max_file_count;
	struct fw_data *next;
	FILE *fp;
};

static struct fw_data g_fw_data;

static void extract_file_ext(char *file_name, char *pure_name,
			     char *ext, int size)
{
	char *last;

	if (!file_name) {
		*pure_name = '\0';
		*ext = '\0';
		return;
	}

	strlcpy(pure_name, file_name, size);
	last = strrchr(pure_name, '.');
	if (last) {
		strlcpy(ext, last + 1, size);
		*last = '\0';
	} else {
		*ext = '\0';
	}
}

static struct fw_data *fw_data_struct_init(char *file_name)
{
	struct fw_data *cur;

	cur = (struct fw_data *)malloc(sizeof(struct fw_data));
	if (!cur)
		return NULL;

	memset(cur, 0, sizeof(struct fw_data));
	strlcpy(cur->file_name, file_name, sizeof(cur->file_name));
	cur->max_file_size = g_fw_data.max_file_size;
	cur->max_file_count = g_fw_data.max_file_count;
	cur->free_size = UINT_MAX;
	cur->next = NULL;
	return cur;
}

static bool file_exist(const char *filename)
{
	struct stat st;
	int result = stat(filename, &st);

	return result == 0;
}

struct fw_data *get_fw_data_struct(char *file_name)
{
	struct fw_data *cur, *p, *end;

	cur = g_fw_data.next;
	end = &g_fw_data;
	while (cur) {
		end = cur;
		p = cur->next;
		if (!strcmp(cur->file_name, file_name)) {
			wsvc_printf_dbg("found file %s", file_name);
			return cur;
		}
		cur = p;
	}

	cur = fw_data_struct_init(file_name);
	end->next = cur;
	return cur;
}

static int set_next_file_to_store(struct fw_data *fw_data, char *folder,
				  char *pure_name, char *ext)
{
	int idx;
	DIR *fdir;
	struct dirent *dirent;
	int i = 0;
	int pos = 0;
	int cur_idx = 0;
	bool is_num;
	time_t last = 0;
	struct stat statbuf;

	fdir = opendir(folder);
	if (!fdir) {
		wsvc_printf_err("failed to open folder %s", folder);
		return -EINVAL;
	}

	chdir(folder);

	/*
	 * file name pattern is filename_xxx or
	 * filename_xxx.ext if ext is not null,
	 * here xxx is index.
	 */
	while ((dirent = readdir(fdir)) != NULL) {
		pos = 0;
		if (strncmp(dirent->d_name, pure_name, strlen(pure_name)))
			continue;
		pos += strlen(pure_name);
		if (dirent->d_name[pos] != '_')
			continue;
		pos += 1;

		/* next three char must be digits */
		is_num = true;
		idx = 0;
		i = 0;
		while (i < 3) {
			if (!isdigit(dirent->d_name[pos])) {
				is_num = false;
				break;
			}
			idx = dirent->d_name[pos] - '0' + idx * 10;
			pos++;
			i++;
		}

		if (!is_num || idx > fw_data->max_file_count)
			continue;

		if (dirent->d_name[pos] == '.')
			pos++;

		if (*ext && strcmp(&dirent->d_name[pos], ext))
			continue;

		if (stat(dirent->d_name, &statbuf) == -1)
			continue;

		if (statbuf.st_mtime > last) {
			cur_idx = idx;
			last = statbuf.st_mtime;
		}
	}

	fw_data->fidx = cur_idx % fw_data->max_file_count + 1;
	wsvc_printf_err("index of next store file : %d", fw_data->fidx);
	return 0;
}

static int cnss_genl_save_file(char *file_name, uint32_t total_size)
{
	FILE *fp = NULL;
	char filename[CNSS_MAX_FILE_PATH];
	char next_filename[CNSS_MAX_FILE_PATH];
	int count = 0, wr_size = 0;
	struct cnss_evt *evt;
	char cur_file[CNSS_FW_DATA_FILE_NAME_MAX];
	char pure_name[QMI_WLFW_MAX_STR_LEN_V01 + 1];
	char ext[QMI_WLFW_MAX_STR_LEN_V01 + 1];
	struct fw_data *fw_data;
	unsigned long free_size;
	int ret;

	/* still use legacy way, default for qdss */
	if (strcmp(file_name, "default") == 0)
		file_name = "qdss_trace.bin";

	fw_data = get_fw_data_struct(file_name);
	if (!fw_data) {
		wsvc_printf_err("failed to get fw data struct");
		goto drop_evt;
	}

	if (total_size > fw_data->max_file_size) {
		wsvc_printf_err("message size %u is over max file size %lu",
				total_size, fw_data->max_file_size);
		goto drop_evt;
	}

	extract_file_ext(file_name, pure_name, ext, QMI_WLFW_MAX_STR_LEN_V01 + 1);
	if (!*pure_name) {
		wsvc_printf_err("file name is empty");
		goto drop_evt;
	}

	if (*ext)
		snprintf(cur_file, sizeof(cur_file), "%s_current.%s",
			 pure_name, ext);
	else
		snprintf(cur_file, sizeof(cur_file), "%s_current", pure_name);

	snprintf(filename, sizeof(filename),
		 "/data/vendor/wifi/%s", cur_file);

	/* set free_size according to file status in file system. */
	if (fw_data->free_size == UINT_MAX) {
		if (file_exist(filename)) {
			fp = fopen(filename, "r");
			if (!fp) {
				wsvc_printf_err("%d failed to open file %s", __LINE__, filename);
				goto drop_evt;
			}
			fseek(fp, 0, SEEK_END);
			ret = ftell(fp);
			fclose(fp);
			if (ret == -1) {
				wsvc_printf_err("can't get size of file %s",
						filename);
				goto drop_evt;
			}
			if (ret <= fw_data->max_file_size) {
				fw_data->free_size = fw_data->max_file_size - ret;
			} else {
				wsvc_printf_err("%s is over max size, del it",
						filename);
				goto drop_evt;
			}
		} else {
			fw_data->free_size = fw_data->max_file_size;
		}
	}

	if (!fw_data->fidx)
		set_next_file_to_store(fw_data, "/data/vendor/wifi/",
				       pure_name, ext);

	free_size = fw_data->free_size;
	if (free_size < total_size) {
		if (*ext)
			snprintf(next_filename, sizeof(next_filename),
				 "/data/vendor/wifi/%s_%03d.%s",
				 pure_name, fw_data->fidx, ext);
		else
			snprintf(next_filename, sizeof(next_filename),
				 "/data/vendor/wifi/%s_%03d",
				 pure_name, fw_data->fidx);

		if (fw_data->fp) {
			fclose(fw_data->fp);
			fw_data->fp = NULL;
		}

		ret = rename(filename, next_filename);
		if (ret < 0)
			wsvc_printf_err("file %s rename to %s failed",
					filename, next_filename);

		fw_data->fp = fopen(filename, "wb");
		if (!fw_data->fp) {
			wsvc_printf_err("%d failed to open file %s", __LINE__, filename);
			goto drop_evt;
		}

		fw_data->fidx = fw_data->fidx % fw_data->max_file_count + 1;
		fw_data->free_size = fw_data->max_file_size;
	}

	if (!fw_data->fp || !file_exist(filename)) {
		if (fw_data->fp) {
			fclose(fw_data->fp);
			fw_data->fp = NULL;
		}
		fw_data->fp = fopen(filename, "ab");
		if (!fw_data->fp) {
			wsvc_printf_err("%d failed to open file %s", __LINE__, filename);
			goto drop_evt;
		}
	}

	evt = cnss_evt_dequeue(&cnss_nl.evt_q);
	while (evt) {
		wsvc_printf_dbg("%s: seg_id: %d data_len %u", __func__,
				count++, evt->data_len);
		wr_size += evt->data_len;

		if (evt->data) {
			fwrite(evt->data, 1, evt->data_len, fw_data->fp);
			free(evt->data);
		}
		free(evt);
		evt = cnss_evt_dequeue(&cnss_nl.evt_q);
	}
	if (wr_size != total_size)
		wsvc_printf_err("%s: Total size mismatch. Check seq_id",
				 __func__);

	fflush(fw_data->fp);
	fsync(fileno(fw_data->fp));
	fw_data->free_size -= total_size;
	wsvc_printf_info_high("%s: saved data to file: %s size: %u",
			      __func__, filename, total_size);
	return 0;

drop_evt:
	cnss_evt_free_queue(&cnss_nl.evt_q);
	return -EINVAL;
}

/* file name and offset hard-coded by target */
#define PCSS_DUMP_FILE	"pcss_ssr.bin"
#define PCSS_SFR_OFFSET	(144)
#define PCSS_SFR_LEN	(80)

/* Print SFR (Subsystem Failure Record) for PCSS SSR */
static inline void print_pcss_sfr(char *file_name,
				  unsigned int seg_id,
				  unsigned int data_len,
				  void *msg_buf)
{
	/* PCSS header will start from seg_id 0 always. */
	if (!seg_id && msg_buf &&
	    !strcmp(PCSS_DUMP_FILE, file_name) &&
	    data_len >= (PCSS_SFR_OFFSET + PCSS_SFR_LEN)) {
		const char *sfr = NULL;
		char sfr_string[PCSS_SFR_LEN];

		sfr = (const char *)msg_buf + PCSS_SFR_OFFSET;
		strlcpy(sfr_string, sfr, PCSS_SFR_LEN);

		wsvc_printf_info_high("PCSS SFR(VARIANT): %s", sfr_string);
	}
}
#else
static int cnss_genl_save_file(char *file_name, uint32_t total_size)
{
	FILE *fp = NULL;
	char filename[CNSS_MAX_FILE_PATH];
	int count = 0, wr_size = 0;
	struct cnss_evt *evt;

	if (strcmp(file_name, "default") == 0)
		snprintf(filename, sizeof(filename),
			 CNSS_DEFAULT_QDSS_TRACE_FILE);
	else
		snprintf(filename, sizeof(filename),
			 "/data/vendor/wifi/%s", file_name);
	fp = fopen(filename, "ab");
	if (fp == NULL) {
		wsvc_printf_err("%s: Failed to open file: %s Error:%s", __func__,
				filename, strerror(errno));
		cnss_evt_free_queue(&cnss_nl.evt_q);
		return 0;
	}

	wsvc_printf_err("%s: Storing QDSS data to file: %s: total_size %u",
			__func__,  filename, total_size);

	evt = cnss_evt_dequeue(&cnss_nl.evt_q);
	while (evt) {
		wsvc_printf_err("%s: seg_id: %d data_len %u", __func__,
				count++, evt->data_len);
		wr_size += evt->data_len;

		fwrite(evt->data, 1, evt->data_len, fp);
		evt = cnss_evt_dequeue(&cnss_nl.evt_q);
	}
	cnss_evt_free_queue(&cnss_nl.evt_q);
	if (wr_size != total_size)
		wsvc_printf_err("%s: Total size mismatch. Check seq_id",
				 __func__);
	fclose(fp);
	wsvc_printf_err("%s: saved data to file: %s size: %u",
			__func__, filename, total_size);
	return 0;
}

static inline void print_pcss_sfr(char *file_name,
				  unsigned int seg_id,
				  unsigned int data_len,
				  void *msg_buf)
{
}
#endif

static int cnss_genl_recv_msg(struct nl_msg *nl_msg, void *data)
{
	UNUSED(data);
	struct nlmsghdr *nlh = nlmsg_hdr(nl_msg);
	struct genlmsghdr *gnlh = nlmsg_data(nlh);
	struct nlattr *attrs[CNSS_GENL_ATTR_MAX + 1];
	int ret = 0;
	unsigned char type = 0;
	char *file_name = NULL;
	unsigned int total_size = 0;
	unsigned int seg_id = 0;
	unsigned char end = 0;
	unsigned int data_len = 0;
	void *msg_buf = NULL;
	struct cnss_evt *evt;

	if (gnlh->cmd != CNSS_GENL_CMD_MSG)
		return NL_SKIP;

	wsvc_printf_dbg("Received CNSS_GENL_CMD_MSG");

	ret = genlmsg_parse(nlh, 0, attrs, CNSS_GENL_ATTR_MAX, NULL);
	if (ret < 0) {
		wsvc_printf_err("RX NLMSG: Parse fail %d", ret);
		return 0;
	}

	type = nla_get_u8(attrs[CNSS_GENL_ATTR_MSG_TYPE]);
	file_name = nla_get_string(attrs[CNSS_GENL_ATTR_MSG_FILE_NAME]);
	total_size = nla_get_u32(attrs[CNSS_GENL_ATTR_MSG_TOTAL_SIZE]);
	seg_id = nla_get_u32(attrs[CNSS_GENL_ATTR_MSG_SEG_ID]);
	end = nla_get_u8(attrs[CNSS_GENL_ATTR_MSG_END]);
	data_len = nla_get_u32(attrs[CNSS_GENL_ATTR_MSG_DATA_LEN]);
	msg_buf = nla_data(attrs[CNSS_GENL_ATTR_MSG_DATA]);

	wsvc_printf_dbg("RX NLMSG: type %u, file_name %s, total_size %u, seg_id %u, end %u, data_len %u",
			type, file_name, total_size, seg_id, end, data_len);

	if (type != CNSS_GENL_MSG_TYPE_QDSS ||
	    data_len > CNSS_GENL_DATA_LEN_MAX) {
		wsvc_printf_err("%s: Invalid CNSS_GENL_CMD_MSG\n", __func__);
		return 0;
	}

	evt = cnss_evt_alloc(CNSS_GENL_MSG_TYPE_QDSS, msg_buf, data_len);
	if (!evt) {
		wsvc_printf_err("%s: No memory for evt", __func__);
		return 0;
	}
	cnss_evt_enqueue(&cnss_nl.evt_q, evt);

	print_pcss_sfr(file_name, seg_id, data_len, msg_buf);
	if (end)
		ret = cnss_genl_save_file(file_name, total_size);

	return ret;
}

int cnss_genl_recvmsgs(void)
{
	int ret = 0;

	if (!cnss_nl.sock)
		return -EINVAL;

	ret = nl_recvmsgs_default(cnss_nl.sock);
	if (ret < 0)
		wsvc_printf_err("NL msg recv error %d", ret);

	return ret;
}

#ifdef IPQ
int cnss_genl_send_data(uint8_t type, uint32_t instance_id, uint32_t value)
{
	struct nl_msg *nlmsg;
	void *msg_head = NULL;
	int ret = 0;

	if (!cnss_nl.sock) {
		wsvc_printf_err("%s: nl sock is invalid \n", __func__);
		return -EINVAL;
	}

	nlmsg = nlmsg_alloc();
	if (!nlmsg) {
		wsvc_printf_err("%s nl msg alloc failed\n", __func__);
		return -ENOMEM;
	}

	msg_head = genlmsg_put(nlmsg, 0, 0,
			       cnss_nl.family_id, 0, 0,
			       CNSS_GENL_CMD_MSG, 0);

	if (!msg_head) {
		ret = -ENOMEM;
		goto fail;
	}

	wsvc_printf_dbg("%s: type:%u, instance_id:%d value:%d\n",
			__func__, type, instance_id, value);

	ret = nla_put_u8(nlmsg, CNSS_GENL_ATTR_MSG_TYPE, type);
	if (ret < 0)
		goto fail;
	ret = nla_put_u32(nlmsg, CNSS_GENL_ATTR_MSG_INSTANCE_ID, instance_id);
	if (ret < 0)
		goto fail;
	ret = nla_put_u32(nlmsg, CNSS_GENL_ATTR_MSG_VALUE, value);
	if (ret < 0)
		goto fail;

	ret = nl_send_auto_complete(cnss_nl.sock, nlmsg);
	if (ret == 0)
		goto exit;

fail:
	wsvc_printf_err("genl msg send fail: %d\n", ret);
exit:
	if (nlmsg)
		nlmsg_free(nlmsg);
	return ret;
}

int cnss_fw_data_init(unsigned int max_file_count, unsigned long max_file_size)
{
	UNUSED(max_file_count);
	UNUSED(max_file_size);

	return cnss_genl_init();
}

void cnss_fw_data_deinit(void)
{
	cnss_genl_exit();
}
#else
int cnss_genl_send_data(uint8_t type, uint32_t instance_id, uint32_t value)
{
	UNUSED(type);
	UNUSED(instance_id);
	UNUSED(value);
	return 0;
}

int cnss_fw_data_init(unsigned int max_file_count, unsigned long max_file_size)
{
	int ret;

	g_fw_data.max_file_size = max_file_size;
	g_fw_data.free_size = max_file_size;
	g_fw_data.max_file_count = max_file_count;
	g_fw_data.next = NULL;

	ret = cnss_genl_init();

	return ret;
}

void cnss_fw_data_deinit(void)
{
	struct fw_data *temp, *p;

	cnss_genl_exit();
	p = g_fw_data.next;
	while (p) {
		temp = p->next;
		if (p->fp)
			fclose(p->fp);
		free(p);
		p = temp;
	}
}
#endif

int cnss_genl_init(void)
{
	struct nl_sock *sock;
	int ret = 0;

	sock = nl_socket_alloc();
	if (!sock) {
		wsvc_printf_err("NL socket alloc fail");
		return -EINVAL;
	}

	ret = genl_connect(sock);
	if (ret < 0) {
		wsvc_printf_err("GENL socket connect fail");
		goto free_socket;
	}

	ret = nl_socket_set_buffer_size(sock, CNSS_GENL_BUF_SIZE, 0);
	if (ret < 0)
		wsvc_printf_err("Could not set NL RX buffer size %d",
				ret);

	cnss_nl.family_id = genl_ctrl_resolve(sock, CNSS_GENL_FAMILY_NAME);
	if (cnss_nl.family_id < 0) {
		ret = cnss_nl.family_id;
		wsvc_printf_err("Couldn't resolve family id");
		goto close_socket;
	}

	cnss_nl.mcgrp_id = genl_ctrl_resolve_grp(sock, CNSS_GENL_FAMILY_NAME,
					 CNSS_GENL_MCAST_GROUP_NAME);

	wsvc_printf_err("NL group_id %d, cnss_nl.family_id %d",
			cnss_nl.mcgrp_id, cnss_nl.family_id);

	nl_socket_disable_seq_check(sock);
	ret = nl_socket_modify_cb(sock, NL_CB_MSG_IN,
				  NL_CB_CUSTOM, cnss_genl_recv_msg, NULL);
	if (ret < 0) {
		wsvc_printf_err("Couldn't modify NL cb, ret %d", ret);
		goto close_socket;
	}

	ret = nl_socket_add_membership(sock, cnss_nl.mcgrp_id);
	if (ret < 0) {
		wsvc_printf_err("Couldn't add membership to group %d, ret %d",
				cnss_nl.mcgrp_id, ret);
		goto close_socket;
	}
	cnss_nl.sock = sock;

	return ret;

close_socket:
	nl_close(sock);
free_socket:
	nl_socket_free(sock);
	return ret;
}

int cnss_genl_get_fd(void)
{
	if (!cnss_nl.sock)
		return -1;

	return nl_socket_get_fd(cnss_nl.sock);
}

void cnss_genl_exit(void)
{
	if (!cnss_nl.sock)
		return;

	nl_close(cnss_nl.sock);
	nl_socket_free(cnss_nl.sock);
	cnss_nl.sock = NULL;
}
