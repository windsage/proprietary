/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <log/log.h>

#include "UBWCPLib.h"
#include "ubwcp_ioctl.h"

static constexpr char kUBWCPDevice[] = "/dev/ubwcp";

void UBWCPLib_get_lib_version(unsigned int *major_ver,
                  unsigned int *minor_ver)
{
	*major_ver = 1;
	*minor_ver = 0;
}

typedef struct {
	int dma_buf_fd;
} UBWCP_Session;

void * UBWCPLib_create_session(void)
{
	UBWCP_Session *session;
	session = (UBWCP_Session *)malloc(sizeof(*session));

	if (!session)
		return NULL;

	session->dma_buf_fd = open(kUBWCPDevice, O_RDONLY| O_CLOEXEC);
	if (session->dma_buf_fd < 0) {
		ALOGE("Could not open %s err:%d", kUBWCPDevice, session->dma_buf_fd);
		goto cleanup_session;
	}

	return session;

cleanup_session:
	free(session);
	
	return NULL;
}

void UBWCPLib_destroy_session(void *session)
{
	UBWCP_Session *ubwcp_session = (UBWCP_Session *)session;

	if (!ubwcp_session)
		return;

	if (ubwcp_session->dma_buf_fd >= 0)
		close(ubwcp_session->dma_buf_fd);

	free(ubwcp_session);
}

int UBWCPLib_get_hw_version(void *session,
		  unsigned int *major_ver,
		  unsigned int *minor_ver)
{
	UBWCP_Session *ubwcp_session = (UBWCP_Session *)session;
	struct ubwcp_ioctl_hw_version hw_version = {0};	

	if (!ubwcp_session || ubwcp_session->dma_buf_fd < 0)
		return -EINVAL;

	int ret = ioctl(ubwcp_session->dma_buf_fd, UBWCP_IOCTL_GET_HW_VER, &hw_version);

	if (ret < 0) {
		ALOGE("UBWCP_IOCTL_GET_HW_VER IOCTL err:%d", ret);
		return ret;
	}

	*major_ver = hw_version.major;
	*minor_ver = hw_version.minor;

	return 0;
}

int UBWCPLib_get_image_format(int lib_image_format, int *image_format_out)
{
	int ret = 0;

	switch (lib_image_format) {
	case UBWCPLib_LINEAR:
		*image_format_out = UBWCP_LINEAR;
		break;
	case UBWCPLib_RGBA8888:
		*image_format_out = UBWCP_RGBA8888;
		break;
	case UBWCPLib_NV12:
		*image_format_out = UBWCP_NV12;
		break;
	case UBWCPLib_NV12_Y:
		*image_format_out = UBWCP_NV12_Y;
		break;
	case UBWCPLib_NV12_UV:
		*image_format_out = UBWCP_NV12_UV;
		break;
	case UBWCPLib_NV124R:
		*image_format_out = UBWCP_NV124R;
		break;
	case UBWCPLib_NV124R_Y:
		*image_format_out = UBWCP_NV124R_Y;
		break;
	case UBWCPLib_NV124R_UV:
		*image_format_out = UBWCP_NV124R_UV;
		break;
	case UBWCPLib_TP10:
		*image_format_out = UBWCP_TP10;
		break;
	case UBWCPLib_TP10_Y:
		*image_format_out = UBWCP_TP10_Y;
		break;
	case UBWCPLib_TP10_UV:
		*image_format_out = UBWCP_TP10_UV;
		break;
	case UBWCPLib_P010:
		*image_format_out = UBWCP_P010;
		break;
	case UBWCPLib_P010_Y:
		*image_format_out = UBWCP_P010_Y;
		break;
	case UBWCPLib_P010_UV:
		*image_format_out = UBWCP_P010_UV;
		break;
	case UBWCPLib_P016:
		*image_format_out = UBWCP_P016;
		break;
	case UBWCPLib_P016_Y:
		*image_format_out = UBWCP_P016_Y;
		break;
	case UBWCPLib_P016_UV:
		*image_format_out = UBWCP_P016_UV;
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

int UBWCPLib_get_stride_alignment(void *session,
	UBWCPLib_Image_Format image_format,
	size_t *stride_align)
{
	UBWCP_Session *ubwcp_session = (UBWCP_Session *)session;
	int ioctl_image_format;
	struct ubwcp_ioctl_stride_align stride_align_ioctl;

	if (!ubwcp_session || ubwcp_session->dma_buf_fd < 0 ||
	    !stride_align)
		return -EINVAL;

	int ret = UBWCPLib_get_image_format(image_format, &ioctl_image_format);
	if (ret < 0) {
		ALOGE("Unsupported image format:%d err:%d", image_format, ret);
		return ret;
	}

	memset(&stride_align_ioctl, 0, sizeof(stride_align_ioctl));
	stride_align_ioctl.image_format = ioctl_image_format;
	ret = ioctl(ubwcp_session->dma_buf_fd, UBWCP_IOCTL_GET_STRIDE_ALIGN, &stride_align_ioctl);
	if (ret < 0) {
		ALOGE("UBWCP_IOCTL_GET_STRIDE_ALIGN IOCTL err:%d", ret);
		return ret;
	}

	*stride_align = stride_align_ioctl.stride_align;
	return 0;
}

int UBWCPLib_validate_stride(void *session, unsigned int stride,
	UBWCPLib_Image_Format image_format, unsigned int width)
{
	UBWCP_Session *ubwcp_session = (UBWCP_Session *)session;
	int ret;
	size_t align;
	int image_format_ioctl;
	struct ubwcp_ioctl_validate_stride validate_stride;

	if (!ubwcp_session || ubwcp_session->dma_buf_fd < 0 ||
	    stride == 0 || stride < width)
		return -EINVAL;

	ret = UBWCPLib_get_image_format(image_format, &image_format_ioctl);
	if (ret < 0) {
		ALOGE("Unsupported image format:%d err:%d", image_format, ret);
		return ret;
	}

	memset(&validate_stride, 0, sizeof(validate_stride));
	validate_stride.image_format = image_format_ioctl;
	validate_stride.width = width;
	validate_stride.stride = stride;
	ret = ioctl(ubwcp_session->dma_buf_fd, UBWCP_IOCTL_VALIDATE_STRIDE, &validate_stride);
	if (ret < 0) {
		ALOGE("UBWCP_IOCTL_VALIDATE_STRIDE IOCTL err:%d", ret);
		return ret;
	}

	return (validate_stride.valid ? 0 : -EINVAL);
}

int UBWCPLib_set_buf_attrs(void *session, unsigned int dmabuf_fd,
	UBWCPLib_buf_attrs *attrs)
{
	UBWCP_Session *ubwcp_session = (UBWCP_Session *)session;
	struct ubwcp_ioctl_buffer_attrs ioctl_attrs = {0};
	int image_format, ret;

	if (!ubwcp_session || ubwcp_session->dma_buf_fd < 0 || !attrs)
		return -EINVAL;


	ret = UBWCPLib_get_image_format(attrs->image_format, &image_format);
	if (ret < 0) {
		ALOGE("Unsupported image format:%d err:%d", attrs->image_format, ret);
		return ret;
	}

	ioctl_attrs.attr.image_format = image_format;
	ioctl_attrs.attr.compression_type = UBWCP_COMPRESSION_LOSSLESS;
	ioctl_attrs.attr.width = attrs->width;
	ioctl_attrs.attr.height = attrs->height;
	ioctl_attrs.attr.stride = attrs->stride;
	ioctl_attrs.attr.scanlines = attrs->scanlines;
	ioctl_attrs.attr.planar_padding = attrs->planar_padding;
	ioctl_attrs.attr.subsample = UBWCP_SUBSAMPLE_4_2_0;
	ioctl_attrs.attr.sub_system_target = UBWCP_SUBSYSTEM_TARGET_CPU; 
	ioctl_attrs.attr.batch_size = 1;

	ioctl_attrs.fd = dmabuf_fd;

	ret = ioctl(ubwcp_session->dma_buf_fd, UBWCP_IOCTL_SET_BUF_ATTR, &ioctl_attrs);

	if (ret < 0)
		ALOGE("UBWCP_IOCTL_SET_BUF_ATTR err:%d", ret);

	return ret;
}
