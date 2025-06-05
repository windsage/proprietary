/*
 * Copyright (c) 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
 * UBWCPLib Interface version 1.0
 */

#ifndef UBWCPLIB_H
#define UBWCPLIB_H

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef enum {
	UBWCPLib_LINEAR = 0,
	UBWCPLib_RGBA8888,
	UBWCPLib_NV12,
	UBWCPLib_NV12_Y,
	UBWCPLib_NV12_UV,
	UBWCPLib_NV124R,
	UBWCPLib_NV124R_Y,
	UBWCPLib_NV124R_UV,
	UBWCPLib_TP10,
	UBWCPLib_TP10_Y,
	UBWCPLib_TP10_UV,
	UBWCPLib_P010,
	UBWCPLib_P010_Y,
	UBWCPLib_P010_UV,
	UBWCPLib_P016,
	UBWCPLib_P016_Y,
	UBWCPLib_P016_UV,
	UBWCPLib_NUM_FORMATS
} UBWCPLib_Image_Format;

/**
 * Get the UBWCPLib library version
 * Will be incremented when the library interface is changed.
 * Can be used by clients using dlopen to determine
 * which functions are supported.
 * @major_ver: [out] Will be set to UBWC-P library major version
 * @minor_ver: [out] Will be set to UBWC-P library minor version
 */
void UBWCPLib_get_lib_version(unsigned int *major_ver,
                  unsigned int *minor_ver);

/**
 * Creates a new UBWCPLib session
 * A UBWCPLib session is required when calling many of the UBWCPLib functions.
 * A session can be used across the process.
 *
 * On success returns a pointer to a new UBWCPLib session and NULL otherwise.
 * Will return NULL if UBWC-P is not supported on the target.
 */
void * UBWCPLib_create_session(void);

/**
 * Destroy a UBWCPLib session
 * Must be called when a session will no longer be used.
 */
void UBWCPLib_destroy_session(void *session);


/**
 * Get UBWC-P HW Version
 * If UBWC-P is supported on the target this function returns the major and
 * minor version of the UBWC-P HW.
 *
 * @session: UBWCPLib session pointer
 * @major_ver: [out] On success will be set to UBWC-P HW major version
 * @minor_ver: [out] On success will be set to UBWC-P HW minor version
 *
 * Returns 0 on success and an error code
 * otherwise.
 */
int UBWCPLib_get_hw_version(void *session,
		  unsigned int *major_ver,
		  unsigned int *minor_ver);

/**
 * Get image's supported stride alignment
 * @session: UBWCPLib session pointer
 * @image_format: Target image's format
 * @stride_align: [out] Stride alignment (in bytes)
 * Returns 0 on success and an error code
 * otherwise.
 */
int UBWCPLib_get_stride_alignment(void *session,
	UBWCPLib_Image_Format image_format,
	size_t *stride_align);

/**
 * Validate that stride conforms to image's stride alignment constraints
 * @session: UBWCPLib session pointer
 * @stride: Stride (in bytes) to validate
 * @image_format: Target image's format
 * @width: Target image's width (in pixels)
 * Returns 0 if the stride meets the image's stride alignment requrirements
 * and an error code otherwise.
 */
int UBWCPLib_validate_stride(void *session, unsigned int stride,
	UBWCPLib_Image_Format image_format, unsigned int width);

/**
 * UBWCLib buffer attributes
 * @image_format: UBWC Image format to set
 * @width: Width to set (in pixels)
 * @height: Height to set (in pixels)
 * @stride: Stride to set (in bytes), stride must meet image alignment
 *          requirements
 * @scanlines: Scanlines to set
 * @planar_padding: Planar padding to set (in bytes)
 */
typedef struct {
	UBWCPLib_Image_Format image_format;
	unsigned int width;
	unsigned int height;
	unsigned int stride;
	unsigned int scanlines;
	size_t planar_padding;
} UBWCPLib_buf_attrs;

/**
 * Set a UBWC-P buffer's attributes.
 * This function must be called at least once before the buffer can be accessed.
 * This function can also be used to change a buffer's UBWC-P attributes.
 * To succeed this function may require that there be no oustanding buffer
 * mmap(ings), such as cases where the buffer's linear address range will
 * change. Also the buffer must not be locked for CPU access at the time of the
 * call.
 *
 * @session: UBWCPLib session pointer
 * @dmabuf_fd: The buffer's dmabuf fd, must be a UBWC-P buffer
 * @attrs: The attributes to set on the UBWC-P buffer
  *
 * Returns 0 on success and an error code otherwise.
 */
int UBWCPLib_set_buf_attrs(void *session, unsigned int dmabuf_fd,
	UBWCPLib_buf_attrs *attrs);

/**
 * Function prototypes
 * Usefull for dlopen access
 */

typedef void (*PFN_UBWCPLib_get_lib_version)(unsigned int *major_ver,
               unsigned int *minor_ver);

typedef void * (*PFN_UBWCPLib_create_session)(void);

typedef void (*PFN_UBWCPLib_destroy_session)(void *session);

typedef int (*PFN_UBWCPLib_get_hw_version)(void *session,
	      unsigned int * major_ver,
	      unsigned int * minor_ver);

typedef int (*PFN_UBWCPLib_get_stride_alignment)(void *session,
	      UBWCPLib_Image_Format image_format,
	      size_t *stride_align);

typedef int (*PFN_UBWCPLib_validate_stride)(void *session, unsigned int stride,
	      UBWCPLib_Image_Format image_format, unsigned int width);

typedef int (*PFN_UBWCPLib_set_buf_attrs)(void *session, unsigned int dmabuf_fd,
	      UBWCPLib_buf_attrs *attrs);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // UBWCPLIB_H

