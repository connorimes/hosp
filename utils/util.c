/**
 * Utility functions.
 *
 * @author Connor Imes
 * @date 2018-05-22
 */
#define _BSD_SOURCE
#include <errno.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "hosp.h"
#include "util.h"

int hosp_util_msleep(unsigned long ms) {
#if defined(_WIN32)
  Sleep(ms);
  return 0;
#else
  return usleep(ms * 1000);
#endif
}

int hosp_util_get_version(hosp_device* hosp, char* version, size_t len) {
  unsigned int i;
  int ret;
  if (hosp_request_version_write(hosp)) {
    return -1;
  }
  for (i = 0; i < HOSP_READ_RETRIES; i++) {
    hosp_util_msleep(HOSP_WRITE_READ_DELAY_MS);
    if ((ret = hosp_request_version_read(hosp, version, len)) < 0) {
      return ret;
    } else if (!ret) {
      return 0;
    }
  }
  errno = ENODATA;
  return -1;
}

int hosp_util_get_status(hosp_device* hosp, int* is_on, int* is_started) {
  unsigned int i;
  int ret;
  if (hosp_request_status_write(hosp)) {
    return -1;
  }
  for (i = 0; i < HOSP_READ_RETRIES; i++) {
    hosp_util_msleep(HOSP_WRITE_READ_DELAY_MS);
    if ((ret = hosp_request_status_read(hosp, is_on, is_started)) < 0) {
      return ret;
    } else if (!ret) {
      return 0;
    }
  }
  errno = ENODATA;
  return -1;
}

int hosp_util_get_data(hosp_device* hosp, unsigned int* mV, unsigned int* mA, unsigned int* mW, unsigned int* mWh) {
  unsigned int i;
  int ret;
  if (hosp_request_data_write(hosp)) {
    return -1;
  }
  for (i = 0; i < HOSP_READ_RETRIES; i++) {
    hosp_util_msleep(HOSP_WRITE_READ_DELAY_MS);
    if ((ret = hosp_request_data_read(hosp, mV, mA, mW, mWh)) < 0) {
      return ret;
    } else if (!ret) {
      return 0;
    }
  }
  errno = ENODATA;
  return -1;
}
