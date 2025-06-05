/***********************************************************
 Copyright (c) 2023 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
************************************************************/

#include <iostream>
#include <utils/Log.h>
#include <errno.h>
#include <string.h>
using namespace std;

#include "IAppLoader.h"
#include "object.h"
#include "IClientEnv.h"
#include "CAppLoader.h"

//needed for load_app
int get_file_size(const char* filename) {
  FILE* file = NULL;
  int size = 0;
  int ret = 0;

  do {
    file = fopen(filename, "r");
    if (file == NULL) {
      ALOGE("Failed to open file %s: %s (%d)\n", filename, strerror(errno), errno);
      size = -1;
      break;
    }

    ret = fseek(file, 0L, SEEK_END);
    if (ret) {
      ALOGE("Error seeking in file %s: %s (%d)\n", filename, strerror(errno), errno);
      size = -1;
      break;
    }

    size = ftell(file);
    if (size == -1) {
      ALOGE("Error telling size of file %s: %s (%d)\n", filename, strerror(errno), errno);
      size = -1;
      break;
    }
  } while (0);
  if (file) {
    fclose(file);
  }
  return size;
}

//needed for load_app
int read_file(const char* filename, size_t size, uint8_t* buffer) {
  FILE* file = NULL;
  size_t readBytes = 0;
  int ret = 0;
  do {
    file = fopen(filename, "r");
    if (file == NULL) {
      ALOGE("Failed to open file %s: %s (%d)\n", filename, strerror(errno), errno);
      ret = -1;
      break;
    }
    readBytes = fread(buffer, 1, size, file);
    if (readBytes != size) {
      ALOGE("Error reading the file %s: %zu vs %zu bytes: %s (%d)\n",
                filename, readBytes, size, strerror(errno), errno);
      ret = -1;
      break;
    }
  } while (0);
  if (file) {
    fclose(file);
  }
  return ret;
}

int32_t load_app(char *path, size_t *size, uint8_t** buffer)
{
  int32_t ret = Object_OK;

  do {
    ret = get_file_size(path);
    if (ret <= 0) {
      ret = -1;
      break;
    }
    *size = (size_t)ret;
    *buffer = new uint8_t[*size];
    if (!(*buffer)) {
      ALOGE("Malloc failed while allocating memory to buffer\n");
      ret = Object_ERROR_KMEM;
      break;
    }

    ret = read_file(path, *size, *buffer);

    if (ret < 0) break;
    ALOGI("load %s, size %zu, buffer %p\n", path, size, (uint32_t *)(*buffer));

    } while (0);

    return ret;
}
