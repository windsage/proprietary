/**
 * Copyright (c) 2020, 2022 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#define LOG_TAG "SECURE-PROCESSOR"

#include "utils.h"
#include <sys/stat.h>
#include <sstream>
#include <vector>

#define HFI_ALIGNMENT_4096 (4096)
#define HFI_ALIGNMENT_64 (64)
#define HFI_ALIGN(a, b) (((a) + (b)-1) & (~((b)-1)))
#define HFI_VENUS_IL_CALC_Y_BUF_SIZE(yBufSize, yStride, yBufHeight) \
    yBufSize = HFI_ALIGN(yStride * yBufHeight, HFI_ALIGNMENT_4096)

static const std::vector<std::string> tALoadLocations = {
    "/vendor/firmware_mnt/image", "/vendor/firmware", "/vendor/etc/firmware",
    "/firmware/image", "/system/etc/firmware"};

static uint32_t getBitsPerPixel(uint32_t format)
{
    uint32_t bits_per_pixel = 0;

    switch (format) {
        case HAL_PIXEL_FORMAT_RAW_OPAQUE:  // 0x24, ImageFormat.RAW_PRIVATE
        case HAL_PIXEL_FORMAT_BLOB:        // 0x21
            return 8;
        case HAL_PIXEL_FORMAT_RAW10:  // 0x25, ImageFormat.RAW10;
            return 10;
        case HAL_PIXEL_FORMAT_RAW12:          // 0x26, ImageFormat.RAW12;
        case HAL_PIXEL_FORMAT_YCbCr_420_888:  // 0x23,
                                              // ImageFormat.YUV_420_888
        case HAL_PIXEL_FORMAT_YCrCb_420_SP:   // 0x11, ImageFormat.NV21
        case HAL_PIXEL_FORMAT_YCbCr_420_SP:   // 0x109, NV12
        case HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED:  // 0x22,
                                                       // ImageFormat.PRIVATE
            return 12;
        case HAL_PIXEL_FORMAT_RAW16:  // 0x20, ImageFormat.RAW_SENSOR
            return 16;
        default:
            ALOGE("%s: Unsupported pixel format %d", __func__,
                  (uint32_t)format);
            break;
    }

    return bits_per_pixel;
}

uint32_t calcBufferSize(uint32_t format, uint32_t width, uint32_t stride,
                        uint32_t height)
{
    uint64_t buffer_size = 0;

    switch (format) {
        case HAL_PIXEL_FORMAT_YCbCr_420_888:
            buffer_size = stride * HFI_ALIGN(height, HFI_ALIGNMENT_64) +
                          stride * height / 2;
            break;
        case HAL_PIXEL_FORMAT_YCrCb_420_SP:
        case HAL_PIXEL_FORMAT_YCbCr_420_SP:
            buffer_size = stride * height * getBitsPerPixel(format) / 8;
            break;
        case HAL_PIXEL_FORMAT_RAW_OPAQUE:
        default:
            buffer_size = stride * height * getBitsPerPixel(format) / 8;
            break;
    }

    if (buffer_size > 0xFFFFFFFF) {
        ALOGE("%s: invalid parameters, potential integer overflow (%u,%u)",
              __func__, width, height);
        return 0;
    }

    return (uint32_t)buffer_size;
}

int64_t getTimeUsec()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (int64_t)now.tv_sec * 1000000LL + now.tv_nsec / 1000LL;
}

bool isYUVSPFormat(uint32_t format)
{
    bool yuvSP = false;

    switch (format) {
        case HAL_PIXEL_FORMAT_YCbCr_420_888:
        case HAL_PIXEL_FORMAT_YCbCr_420_SP:
            yuvSP = true;
            break;
    }
    return yuvSP;
}

uint32_t calcYStride(uint32_t format, uint32_t stride)
{
    uint32_t yStride = 0;

    switch (format) {
        case HAL_PIXEL_FORMAT_YCbCr_420_888:
        case HAL_PIXEL_FORMAT_YCbCr_420_SP:
            yStride = stride;
            break;
    }

    return yStride;
}

uint32_t calcUVStride(uint32_t format, uint32_t stride)
{
    uint32_t uvStride = 0;

    switch (format) {
        case HAL_PIXEL_FORMAT_YCbCr_420_888:
        case HAL_PIXEL_FORMAT_YCbCr_420_SP:
            uvStride = stride;
            break;
    }

    return uvStride;
}

uint32_t calcUVOffset(uint32_t format, uint32_t stride, uint32_t height)
{
    uint32_t uvOffset = 0;

    switch (format) {
        case HAL_PIXEL_FORMAT_YCbCr_420_888:
            uvOffset = stride * HFI_ALIGN(height, HFI_ALIGNMENT_64);
            break;
        case HAL_PIXEL_FORMAT_YCbCr_420_SP:
            uvOffset = stride * height;
            break;
    }

    return uvOffset;
}

int32_t getSecCamAppPath(std::string &fileName, std::string &AppPath)
{
    std::string secCamAppPath;
    struct stat info;

    for (std::string path : tALoadLocations) {
        std::stringstream ss;
        ss << path << "/" << fileName;
        secCamAppPath = ss.str();
        ALOGD("%s: Loading TrustedApp from %s.\n", __func__,
              secCamAppPath.c_str());
        if (stat(secCamAppPath.c_str(), &info) == 0) {
            ALOGD("SecCam app path found, break");
            AppPath = secCamAppPath;
            return 0;
        }
    }

    return -1;
}

int32_t getFileSize(std::string const &filename)
{
    FILE *file = NULL;
    int32_t size = -1;
    int32_t ret = 0;

    do {
        file = fopen(filename.c_str(), "r");
        if (file == NULL) {
            ALOGE("Failed to open file %s: %s (%d)", filename.c_str(),
                  strerror(errno), errno);
            return size;
        }

        ret = fseek(file, 0L, SEEK_END);
        if (ret) {
            ALOGE("Error seeking in file %s: %s (%d)", filename.c_str(),
                  strerror(errno), errno);
            break;
        }

        size = ftell(file);
        if (size == -1) {
            ALOGE("Error telling size of file %s: %s (%d)", filename.c_str(),
                  strerror(errno), errno);
            break;
        }
    } while (0);

    if (file) {
        fclose(file);
    }

    return size;
}

int32_t readFile(std::string const &filename, size_t &size, uint8_t *buffer)
{
    FILE *file = NULL;
    size_t readBytes = 0;
    int32_t ret = 0;

    do {
        file = fopen(filename.c_str(), "r");

        if (file == NULL) {
            ALOGE("Failed to open file %s: %s (%d)", filename.c_str(),
                  strerror(errno), errno);
            ret = -1;
            return ret;
        }

        readBytes = fread(buffer, 1, size, file);

        if (readBytes != size) {
            ALOGE("Error reading the file %s: %zu vs %zu bytes: %s (%d)",
                  filename.c_str(), readBytes, size, strerror(errno), errno);
            ret = -1;
            break;
        }

        ret = size;
    } while (0);

    if (file) {
        fclose(file);
    }

    return ret;
}
