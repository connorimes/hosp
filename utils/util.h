/**
 * Utility functions.
 *
 * @author Connor Imes
 * @date 2018-05-22
 */
#ifndef _HOSP_UTIL_H_
#define _HOSP_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <hosp.h>

#pragma GCC visibility push(hidden)

// Time wait between writing a command the reading the response
#define HOSP_WRITE_READ_DELAY_MS 1

// Try for up to 1/4 second
#define HOSP_READ_RETRIES 250

int hosp_util_msleep(unsigned long ms);

int hosp_util_get_version(hosp_device* hosp, char* version, size_t len);

int hosp_util_get_status(hosp_device* hosp, int* is_on, int* is_started);

int hosp_util_get_data(hosp_device* hosp, unsigned int* mv, unsigned int* ma, unsigned int* mw, unsigned int* mWh);

#pragma GCC visibility pop

#ifdef __cplusplus
}
#endif

#endif
