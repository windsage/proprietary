/*
 * qdss cmd handler and hw accel cmd handler and legacy stm cmd handler:
 * A module to handle commands(diag) that can be processed in native code.
 *
 * Copyright (c) 2013-2014, 2017-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * qdss_config.c : Main implementation of qdss_config
 */

#include "qdss_config.h"
#include "diag_mux.h"
#include "diag.h"
#include "diag_cntl.h"

#define DIAG_TRANSPORT_USB     2

static char qdss_sink = 0, qdss_hwevent = 0, qdss_stm = 0, qdss_atb = 0;

static int qdss_file_write_str(const char *qdss_file_path, const char *str)
{
	int qdss_fd, ret;

	if (!qdss_file_path || !str) {
		return QDSS_RSP_FAIL;
	}

	qdss_fd = open(qdss_file_path, O_WRONLY);
	if (qdss_fd < 0) {
		ALOGE("qdss open file: %s error: %s", qdss_file_path, strerror(errno));
		return QDSS_RSP_FAIL;
	}

	ret = write(qdss_fd, str, strlen(str));
	if (ret < 0) {
		ALOGE("qdss write file: %s error: %s", qdss_file_path, strerror(errno));
		close(qdss_fd);
		return QDSS_RSP_FAIL;
	}

	close(qdss_fd);

	return QDSS_RSP_SUCCESS;
}

static int qdss_file_write_byte(const char *qdss_file_path, unsigned char val)
{
	int qdss_fd, ret;

	if (!qdss_file_path) {
		return QDSS_RSP_FAIL;
	}

	qdss_fd = open(qdss_file_path, O_WRONLY);
	if (qdss_fd < 0) {
		ALOGE("qdss open file: %s error: %s", qdss_file_path, strerror(errno));
		return QDSS_RSP_FAIL;
	}

	ret = write(qdss_fd, &val, 1);
	if (ret < 0) {
		ALOGE("%s: qdss write file: %s error: %s", __func__, qdss_file_path, strerror(errno));
		close(qdss_fd);
		return QDSS_RSP_FAIL;
	}

	close(qdss_fd);

	return QDSS_RSP_SUCCESS;
}

/* Sets the Trace Sink */
static int qdss_trace_sink_handler(qdss_trace_sink_req *pReq, int req_len, qdss_trace_sink_rsp *pRsp, int rsp_len)
{
	int ret = 0;

	if (!pReq || !pRsp) {
		return QDSS_RSP_FAIL;
	}

	pRsp->result = QDSS_RSP_FAIL;
	if (pReq->trace_sink == TMC_TRACESINK_ETB) {
		/* For enabling writing ASCII value of 1 i.e. 0x31 */
		ret = qdss_file_write_byte(QDSS_ETB_SINK_FILE, 0x31);
		if (ret) {
			ret = qdss_file_write_byte(QDSS_ETB_SINK_FILE_2, 0x31);
			if (ret)
				return QDSS_RSP_FAIL;
		}
	} else if (pReq->trace_sink == TMC_TRACESINK_RAM) {
		ret = qdss_file_write_str(QDSS_ETR_OUTMODE_FILE, "mem");
		if (ret) {
			return QDSS_RSP_FAIL;
		}

		ret = qdss_file_write_byte(QDSS_ETR_SINK_FILE, 0x31);
		if (ret) {
			ret = qdss_file_write_byte(QDSS_ETR_SINK_FILE_2, 0x31);
			if (ret)
				return QDSS_RSP_FAIL;
		}
	} else if (pReq->trace_sink == TMC_TRACESINK_USB) {
		ret = qdss_file_write_str(QDSS_ETR_OUTMODE_FILE, "usb");
		if (ret) {
			return QDSS_RSP_FAIL;
		}

		ret = qdss_file_write_byte(QDSS_ETR_SINK_FILE, 0x31);
		if (ret) {
			ret = qdss_file_write_byte(QDSS_ETR_SINK_FILE_2, 0x31);
			if (ret)
				return QDSS_RSP_FAIL;
		}
	} else if (pReq->trace_sink == TMC_TRACESINK_PCIE) {
		ret = qdss_file_write_str(QDSS_ETR_OUTMODE_FILE, "pcie");
		if (ret) {
			return QDSS_RSP_FAIL;
		}

		ret = qdss_file_write_byte(QDSS_ETR_SINK_FILE, 0x31);
		if (ret) {
			ret = qdss_file_write_byte(QDSS_ETR_SINK_FILE_2, 0x31);
			if (ret)
				return QDSS_RSP_FAIL;
		}
	} else if (pReq->trace_sink == TMC_TRACESINK_TPIU) {
		ret = qdss_file_write_byte(QDSS_TPIU_SINK_FILE, 0x31);
		if (ret) {
			return QDSS_RSP_FAIL;
		}

		ret = qdss_file_write_str(QDSS_TPIU_OUTMODE_FILE, "mictor");
		if (ret) {
			return QDSS_RSP_FAIL;
		}
	} else if (pReq->trace_sink == TMC_TRACESINK_SD) {
		ret = qdss_file_write_byte(QDSS_TPIU_SINK_FILE, 0x31);
		if (ret) {
			return QDSS_RSP_FAIL;
		}

		ret = qdss_file_write_str(QDSS_TPIU_OUTMODE_FILE, "sdc");
		if (ret) {
			return QDSS_RSP_FAIL;
		}
	} else {
		qdss_sink = 0;
		return QDSS_RSP_FAIL;
	}

	qdss_sink = pReq->trace_sink;
	pRsp->result = QDSS_RSP_SUCCESS;

	return QDSS_RSP_SUCCESS;
}
static int qdss_etr1_trace_sink_handler(qdss_trace_sink_req *pReq, int req_len, qdss_trace_sink_rsp *pRsp, int rsp_len)
{
	int ret = 0;

	if (!pReq || !pRsp) {
		return QDSS_RSP_FAIL;
	}
	pRsp->result = QDSS_RSP_FAIL;
	if (pReq->trace_sink == TMC_TRACESINK_RAM) {
		ret = qdss_file_write_str(QDSS_ETR1_OUTMODE_FILE, "mem");
		if (ret)
                        return QDSS_RSP_FAIL;

		ret = qdss_file_write_byte(QDSS_ETR1_SINK_FILE_2, 0x31);
		if (ret)
			return QDSS_RSP_FAIL;
	}
	qdss_sink = pReq->trace_sink;
	pRsp->result = QDSS_RSP_SUCCESS;

	return QDSS_RSP_SUCCESS;
}


/* Enable/Disable STM */
static int qdss_filter_stm_handler(qdss_filter_stm_req *pReq, int req_len, qdss_filter_stm_rsp *pRsp, int rsp_len)
{
	char ret = 0, stm_state = 0;

	ALOGE("qdss_filter_stm_handler");

	if (!pReq || !pRsp) {
		return QDSS_RSP_FAIL;
	}

	pRsp->result = QDSS_RSP_FAIL;

	if (pReq->state) {
		stm_state = 1;
	} else {
		stm_state = 0;
	}

	ret = qdss_file_write_byte(QDSS_STM_FILE, stm_state + 0x30);
	if (ret) {
		ret = qdss_file_write_byte(QDSS_STM_FILE_2, stm_state + 0x30);
		if (ret)
			return QDSS_RSP_FAIL;
	}

	qdss_stm = stm_state;
	pRsp->result = QDSS_RSP_SUCCESS;

	return QDSS_RSP_SUCCESS;
}

/* Enable/Disable HW Events */
static int qdss_filter_hwevents_handler(qdss_filter_hwevents_req *pReq, int req_len, qdss_filter_hwevents_rsp *pRsp, int rsp_len)
{
	int ret = 0;

	if (!pReq || !pRsp) {
		return QDSS_RSP_FAIL;
	}

	pRsp->result = QDSS_RSP_FAIL;

	if (pReq->state) {

		qdss_hwevent = 1;
		/* For disabling writing ASCII value of 0 i.e. 0x30 */
		ret = qdss_file_write_byte(QDSS_HWEVENT_FILE, 0x30);
		if (ret) {
			return QDSS_RSP_FAIL;
		}

		ret = qdss_file_write_byte(QDSS_STM_HWEVENT_FILE, 0x30);
		if (ret) {
			return QDSS_RSP_FAIL;
		}

		ret = qdss_file_write_byte(QDSS_HWEVENT_FILE, 0x31);
		if (ret) {
			return QDSS_RSP_FAIL;
		}

		ret = qdss_file_write_byte(QDSS_STM_HWEVENT_FILE, 0x31);
		if (ret) {
			return QDSS_RSP_FAIL;
		}

	} else {

		qdss_hwevent = 0;

		ret = qdss_file_write_byte(QDSS_HWEVENT_FILE, 0x30);
		if (ret) {
			return QDSS_RSP_FAIL;
		}

		ret = qdss_file_write_byte(QDSS_STM_HWEVENT_FILE, 0x30);
		if (ret) {
			return QDSS_RSP_FAIL;
		}
	}

	pRsp->result = QDSS_RSP_SUCCESS;
	return QDSS_RSP_SUCCESS;
}

/* Programming registers to generate HW events */
static int qdss_filter_hwevents_configure_handler(qdss_filter_hwevents_configure_req *pReq, int req_len, qdss_filter_hwevents_configure_rsp *pRsp, int rsp_len)
{
	char reg_buf[100];
	int ret = 0, qdss_fd;

	ALOGE("qdss_filter_hwevents_configure_handler");

	if (!pReq || !pRsp) {
		return QDSS_RSP_FAIL;
	}

	pRsp->result = QDSS_RSP_FAIL;

	snprintf(reg_buf, sizeof(reg_buf), "%x %x", pReq->register_addr, pReq->register_value);

	qdss_fd = open(QDSS_HWEVENT_SET_REG_FILE, O_WRONLY);
	if (qdss_fd < 0) {
		ALOGE("qdss open file: %s error: %s", QDSS_HWEVENT_SET_REG_FILE, strerror(errno));
		return QDSS_RSP_FAIL;
	}

	ret = write(qdss_fd, reg_buf, strlen(reg_buf));
	if (ret < 0) {
		ALOGE("qdss write file: %s error: %s", QDSS_HWEVENT_SET_REG_FILE, strerror(errno));
		close(qdss_fd);
		return QDSS_RSP_FAIL;
	}

	close(qdss_fd);

	pRsp->result = QDSS_RSP_SUCCESS;
	return QDSS_RSP_SUCCESS;
}

/* Get the status of sink, stm and HW events */
static int qdss_query_status_handler(qdss_query_status_req *pReq, int req_len, qdss_query_status_rsp *pRsp, int rsp_len)
{

	if (!pReq || !pRsp) {
		return QDSS_RSP_FAIL;
	}

	pRsp->trace_sink = qdss_sink;
	pRsp->stm_enabled = qdss_stm;
	pRsp->hw_events_enabled = qdss_hwevent;
	pRsp->atb_enabled = qdss_atb;

	return QDSS_RSP_SUCCESS;
}

/* Handling of qdss qtimer ts sync */
static int qdss_qtimer_ts_sync_handler(qdss_qtimer_ts_sync_req *pReq, int req_len, qdss_qtimer_ts_sync_rsp *pRsp, int rsp_len)
{
	int ret = 0, qdss_ts_fd = 0;
	uint64_t qdss_ticks = 0, qtimer_ticks = 0;
	char qdss_ts_val[17];


	if (!pReq || !pRsp) {
		return QDSS_RSP_FAIL;
	}

	memset(qdss_ts_val, '\0', sizeof(qdss_ts_val));

	qdss_ts_fd = open(QDSS_SWAO_CSR_TIMESTAMP, O_RDONLY);
	if (qdss_ts_fd < 0) {
		ALOGE("qdss open file: %s error: %s", QDSS_SWAO_CSR_TIMESTAMP, strerror(errno));
		goto fail;
	}

	ret = read(qdss_ts_fd, qdss_ts_val, sizeof(qdss_ts_val)-1);
	if (ret < 0) {
		ALOGE("qdss read file: %s error: %s", QDSS_SWAO_CSR_TIMESTAMP, strerror(errno));
		close(qdss_ts_fd);
		goto fail;
	}

	qdss_ticks = atoll(qdss_ts_val);

	close(qdss_ts_fd);

#if defined __aarch64__ && __aarch64__ == 1
	asm volatile("mrs %0, cntvct_el0" : "=r" (qtimer_ticks));
#else
	asm volatile("mrrc p15, 1, %Q0, %R0, c14" : "=r" (qtimer_ticks));
#endif

	ALOGD("In %s: qdss_ts_val: %s qdss_ticks: %ld qtimer_ticks: %ld\n",
		__func__, qdss_ts_val, qdss_ticks, qtimer_ticks);

	pRsp->status = 0;
	pRsp->qdss_ticks = qdss_ticks;
	pRsp->qtimer_ticks = qtimer_ticks;
	pRsp->qdss_freq = QDSS_CLK_FREQ_HZ;
	pRsp->qtimer_freq = QTIMER_CLK_FREQ_HZ;
	return QDSS_RSP_SUCCESS;
fail:
	pRsp->status = 1;
	pRsp->qdss_ticks = 0;
	pRsp->qtimer_ticks = 0;
	pRsp->qdss_freq = 0;
	pRsp->qtimer_freq = 0;
	return QDSS_RSP_FAIL;
}

/* QDSS commands handler */
int qdss_diag_pkt_handler(struct diag_client *client, const void *pReq, size_t pkt_len, int pid)

{
/*
 * 1) Checks the request command size. If it fails send error response.
 * 2) If request command size is valid then allocates response packet
 *    based on request.
 * 3) Invokes the respective command handler
 */

#define QDSS_HANDLE_DIAG_CMD(cmd)                              \
   if (pkt_len < sizeof(cmd##_req)) {                          \
      return -EINVAL;                                   \
   }                                                           \
   else {                                                      \
      resp_size = sizeof(cmd##_rsp);                           \
      cmd##_rsp *tempRsp = alloca(resp_size);                   \
      pRsp = (void *)tempRsp;                                \
      if (NULL != pRsp) {                                      \
         tempRsp->hdr.subsysId = DIAG_SUBSYS_QDSS;  \
         tempRsp->hdr.subsysCmdCode = pHdr->subsysCmdCode; \
         tempRsp->hdr.cmdCode = DIAG_SUBSYS_CMD_F;   \
         cmd##_handler((cmd##_req *)pReq,                      \
                       pkt_len,                                \
                       (cmd##_rsp *)pRsp,                      \
                       sizeof(cmd##_rsp));                     \
      }                                                        \
   }

	qdss_diag_pkt_hdr *pHdr;
	PACK(void *)pRsp = NULL;
	int resp_size = 0;

	if (NULL != pReq) {
		pHdr = (qdss_diag_pkt_hdr *)pReq;

		switch (pHdr->subsysCmdCode & 0x0FF) {
		case QDSS_QUERY_STATUS:
			QDSS_HANDLE_DIAG_CMD(qdss_query_status);
			break;
		case QDSS_TRACE_SINK:
			 QDSS_HANDLE_DIAG_CMD(qdss_trace_sink);
			 break;
		case QDSS_FILTER_STM:
			 QDSS_HANDLE_DIAG_CMD(qdss_filter_stm);
			 break;
		case QDSS_FILTER_HWEVENT_ENABLE:
			 QDSS_HANDLE_DIAG_CMD(qdss_filter_hwevents);
			 break;
		case QDSS_FILTER_HWEVENT_CONFIGURE:
			 QDSS_HANDLE_DIAG_CMD(qdss_filter_hwevents_configure);
			 break;
		case QDSS_QTIMER_TS_SYNC:
			 QDSS_HANDLE_DIAG_CMD(qdss_qtimer_ts_sync);
			 break;
		case QDSS_ETR1_TRACE_SINK:
			QDSS_HANDLE_DIAG_CMD(qdss_etr1_trace_sink);
			break;
		default:
			 return -EINVAL;;
		}

		if (NULL != pRsp) {
			diag_apps_rsp_send(pid, pRsp, resp_size);
		}
	}
	return 0;
}

static int qdss_update_etr_sink_file(char state, int pid)
{
	int ret;

	if (etr1_support && pid) {
		ret = qdss_file_write_byte(QDSS_ETR1_SINK_FILE, state + 0x30);
		if (ret)
			ret = qdss_file_write_byte(QDSS_ETR1_SINK_FILE_2, state + 0x30);
	} else {
		ret = qdss_file_write_byte(QDSS_ETR_SINK_FILE, state + 0x30);
		if (ret)
			ret = qdss_file_write_byte(QDSS_ETR_SINK_FILE_2, state + 0x30);
	}

	if (ret)
		ret = DIAG_HW_ACCEL_FAIL;

	return ret;
}

/* Diag hw acceleration commands handler */
int hw_accel_diag_pkt_handler(struct diag_client *client, const void *pReq, size_t pkt_len, int pid)
{
	struct diag_pkt_header_t *pHdr;
	struct diag_hw_accel_cmd_req_t *local_Req = NULL;
	struct diag_hw_accel_cmd_query_resp_t *local_pRsp = NULL;
	struct diag_hw_accel_query_rsp_payload_t query_rsp;
	struct diag_hw_accel_cmd_op_resp_t *local_op_pRsp;
	void * pRsp = NULL;
	int pRsp_len = 0;
	char ret = 0, stm_state = 0, atb_state = 0;
	int err = 0, rsp_len = 0;
	uint8 i = 1, j = 1, rsp_status, num_rsp = 1, buf_size = 0;
	uint8 hw_accel_type = 0, hw_accel_ver = 0, operation = 0;

	if (!pReq || pkt_len < (sizeof(struct diag_hw_accel_cmd_req_t) - sizeof(uint32))) {
		ALOGE("diag: %s: Invalid command length to parse the request\n", __func__);
		return -EINVAL;
	} else {
		pHdr = (struct diag_pkt_header_t *)pReq;
		if (pHdr->subsys_cmd_code == DIAG_HW_ACCEL_CMD) {
			local_Req = (struct diag_hw_accel_cmd_req_t *)pReq;

			rsp_status = DIAG_HW_ACCEL_STATUS_SUCCESS;
			operation = local_Req->operation;

			if (operation > DIAG_HW_ACCEL_OP_QUERY) {
				rsp_status = DIAG_HW_ACCEL_FAIL;
				operation = DIAG_HW_ACCEL_OP_QUERY;
				goto fill_resp;
			}
			hw_accel_type = local_Req->op_req.hw_accel_type;
			hw_accel_ver = local_Req->op_req.hw_accel_ver;

			if (hw_accel_type > DIAG_HW_ACCEL_TYPE_MAX) {
				rsp_status = DIAG_HW_ACCEL_INVALID_TYPE;
				goto fill_resp;
			}
			if (hw_accel_ver > DIAG_HW_ACCEL_VER_MAX)
				rsp_status = DIAG_HW_ACCEL_INVALID_VER;

fill_resp:
			if (operation == DIAG_HW_ACCEL_OP_QUERY) {
				/*
				 * HW ACCELERATION command response formulation
				 * QUERY operation
				 */
				if (!rsp_status) {
					if (!hw_accel_type && !hw_accel_ver) {
						num_rsp = DIAG_HW_ACCEL_TYPE_MAX * DIAG_HW_ACCEL_VER_MAX;
					} else if (!hw_accel_type && hw_accel_ver) {
						num_rsp = DIAG_HW_ACCEL_TYPE_MAX;
					} else if (hw_accel_type && !hw_accel_ver) {
						num_rsp = DIAG_HW_ACCEL_VER_MAX;
					} else {
						num_rsp = 1;
					}
				}
				buf_size = num_rsp * sizeof(struct diag_hw_accel_query_sub_payload_rsp_t);

				rsp_len = sizeof(local_Req->header) + sizeof(local_Req->version) + sizeof(local_Req->operation) + sizeof(local_Req->reserved) +
					sizeof(query_rsp.status) + sizeof(query_rsp.diag_transport) + sizeof(query_rsp.num_accel_rsp) + buf_size;

				pRsp = alloca(rsp_len);
				if (!pRsp) {
					ALOGE("diag: %s: Unable to allocate query packet response\n", __func__);
					goto err_rsp;
				}
				pRsp_len = rsp_len;
				local_pRsp = (struct diag_hw_accel_cmd_query_resp_t *)pRsp;
				local_pRsp->header.cmd_code = DIAG_SUBSYS_CMD_F;
				local_pRsp->header.subsys_cmd_code = local_Req->header.subsys_cmd_code;
				local_pRsp->header.subsys_id = DIAG_SUBSYS_DIAG_SERV;
				local_pRsp->version = local_Req->version;
				local_pRsp->operation = local_Req->operation;
				local_pRsp->reserved = 0;
				local_pRsp->query_rsp.diag_transport = diagmem->transport_set;
				local_pRsp->query_rsp.num_accel_rsp = num_rsp;

				for (i = DIAG_HW_ACCEL_TYPE_STM;
						i <= DIAG_HW_ACCEL_TYPE_MAX; i++) {
					if (hw_accel_type && i != hw_accel_type)
						continue;
					for (j = DIAG_HW_ACCEL_VER_MIN;
							j <= DIAG_HW_ACCEL_VER_MAX; j++) {
						if (hw_accel_ver && j != hw_accel_ver)
							continue;
						local_pRsp->query_rsp.sub_query_rsp[i-1][j-1].hw_accel_type = i;
						local_pRsp->query_rsp.sub_query_rsp[i-1][j-1].hw_accel_ver = j;
						local_pRsp->query_rsp.sub_query_rsp[i-1][j-1].diagid_mask_supported = 0;
						local_pRsp->query_rsp.sub_query_rsp[i-1][j-1].diagid_mask_enabled = 0;
						if (operation == DIAG_HW_ACCEL_OP_QUERY)
							err = diag_cntl_query_pd_featuremask_local(&local_pRsp->query_rsp.sub_query_rsp[i-1][j-1]);
						else
							err = diag_cntl_send_passthru_control_pkt(local_Req);
						if (err < 0) {
							ALOGE("diag: %s: Failure in handling query operation for type: %d, ver: %d, err: %d, errno: %d\n",
								__func__, i, j, err, errno);
							pRsp = NULL;
							rsp_status = DIAG_HW_ACCEL_FAIL;
							break;
						}
					}
				}
				local_pRsp->query_rsp.status = rsp_status;
			} else {
				/*
				 * HW ACCELERATION command response formulation
				 * DISABLE/ENABLE operation
				 */
				if (hw_accel_type == DIAG_HW_ACCEL_TYPE_ALL)
					rsp_status = DIAG_HW_ACCEL_INVALID_TYPE;

				pRsp_len = sizeof(struct diag_hw_accel_cmd_op_resp_t);
				pRsp = alloca(pRsp_len);
				if (!pRsp) {
					ALOGE("diag: %s: Unable to allocate op packet response\n", __func__);
					goto err_rsp;
				}
				local_op_pRsp = (struct diag_hw_accel_cmd_op_resp_t *)pRsp;
				local_op_pRsp->header.cmd_code = DIAG_SUBSYS_CMD_F;
				local_op_pRsp->header.subsys_cmd_code = local_Req->header.subsys_cmd_code;
				local_op_pRsp->header.subsys_id = DIAG_SUBSYS_DIAG_SERV;
				local_op_pRsp->version = local_Req->version;
				local_op_pRsp->operation = local_Req->operation;
				local_op_pRsp->reserved = 0;
				local_op_pRsp->op_rsp.hw_accel_type = hw_accel_type;
				local_op_pRsp->op_rsp.hw_accel_ver = hw_accel_ver;
				i = hw_accel_type;
				j = hw_accel_ver;

				if (operation == DIAG_HW_ACCEL_OP_QUERY)
					err = diag_cntl_query_pd_featuremask_local(&local_pRsp->query_rsp.sub_query_rsp[i-1][j-1]);
				else
					err = diag_cntl_send_passthru_control_pkt(local_Req);
				if (err < 0) {
					ALOGE("diag: %s: Failure in hw accelartion operation, err: %d, errno: %d\n",
						 __func__, err, errno);
					pRsp = NULL;
				}
				/*
				 *	diag_id_mask returned from kernel is reused to
				 *	to return the status of the features updated
				 *	as part of the command.
				 */
				local_op_pRsp->op_rsp.diagid_status = local_Req->op_req.diagid_mask;
				/*
				 *	After successful handling of command to fill the response
				 *	based on the type of the hw acceleration type
				 *	configure coresight node.
				 */
				if (local_Req->op_req.hw_accel_type == DIAG_HW_ACCEL_TYPE_STM) {
					/*
					 * configure coresight node for STM.
					 */
					if (local_Req->operation)
						stm_state = 1;
					else
						stm_state = 0;

					if (stm_state)
						rsp_status = qdss_update_etr_sink_file(stm_state, pid);

					ret = qdss_file_write_byte(QDSS_STM_FILE, stm_state + 0x30);
					if (ret) {
						ret = qdss_file_write_byte(QDSS_STM_FILE_2, stm_state + 0x30);
						if (ret) {
							ALOGE("diag: %s: Failure to write operation on STM node\n", __func__);
							rsp_status = DIAG_HW_ACCEL_FAIL;
						}
					}
					qdss_stm = stm_state;

				} else if (local_Req->op_req.hw_accel_type == DIAG_HW_ACCEL_TYPE_ATB) {
					/*
					 * configure coresight node for ATB.
					 */
					if (local_Req->operation)
						atb_state = 1;
					else
						atb_state = 0;

					if (atb_state)
						rsp_status = qdss_update_etr_sink_file(atb_state, pid);

					ret = qdss_file_write_byte(QDSS_ATB_FILE, atb_state + 0x30);
					if (ret) {
						ALOGE("diag: %s: Failure to write operation on ATB node\n", __func__);
						rsp_status = DIAG_HW_ACCEL_FAIL;
					}
					qdss_atb = atb_state;
				}
				local_op_pRsp->op_rsp.status = rsp_status;
				if (rsp_status) {
					ALOGE("diag: %s: Invalid response status: %d\n", __func__, rsp_status);
					local_op_pRsp->op_rsp.diagid_status = 0;
				}
			}
		}
	}

err_rsp:
	ALOGD("diag: %s: operation %hhu and resp is %s\n", __func__, operation,
		pRsp? "succ": "fail");

	if (pRsp) {
		diag_apps_rsp_send(pid, pRsp, pRsp_len);
		pRsp = NULL;
		return 0;
	} else {
		return -EINVAL;
	}
}


void qdss_diag_pkt_hdlr_init(void)
{
	register_fallback_subsys_cmd_range(DIAG_SUBSYS_QDSS, QDSS_DIAG_PROC_ID | QDSS_QUERY_STATUS, QDSS_DIAG_PROC_ID | QDSS_ETR1_TRACE_SINK, qdss_diag_pkt_handler);
	return;
}

void hw_accel_diag_pkt_hdlr_init(void)
{
	register_fallback_subsys_cmd(DIAG_SUBSYS_DIAG_SERV, DIAG_HW_ACCEL_CMD, hw_accel_diag_pkt_handler);
	return;
}

