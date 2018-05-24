/**
 * A library for managing a Hardkernel ODROID Smart Power device.
 * These devices refresh at 10 Hz (every 100 ms).
 *
 * Function hosp_device pointer parameters may never be NULL.
 * Pointers for storing values on read are optional in situations where a user may not care about all fields.
 *
 * A write request of the same type must precede any number of read requests of that type.
 * If not, the read request will never get the desired information.
 * Data is not always available on read requests, in which case clients should wait for a period, then try again.
 * This write/read protocol is left exposed in this API so the library does not have to perform undesirable sleep
 * operations between write and read while also allowing users to implement their own retry algorithms.
 *
 * @author Connor Imes
 * @date 2018-05-22
 */
#ifndef _HOSP_H_
#define _HOSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/**
 * Opaque HOSP handle.
 */
typedef struct hosp_device hosp_device;

/**
 * Open a HOSP handle.
 * If more than one device is connected to the system, the first one discovered will be used.
 *
 * @return A hosp_device handle, or NULL on failure (sets errno)
 */
hosp_device* hosp_open(void);

/**
 * Close a HOSP handle.
 *
 * @param hosp An open device handle, not NULL
 * @return 0 on success, a negative value on failure (sets errno)
 */
int hosp_close(hosp_device* hosp);

/**
 * Write to the device to request the firmware version string.
 *
 * @param hosp An open device handle, not NULL
 * @return 0 on success, a negative value on failure (sets errno)
 */
int hosp_request_version_write(hosp_device* hosp);

/**
 * Try to read from the device to get the firmware version string, e.g., "SMART POWER V3.0".
 *
 * @param hosp An open device handle, not NULL
 * @param buf Version string buffer, not NULL
 * @param bufsize Size of version string buffer, should be >= 17
 * @return 0 on success, a negative value on failure (sets errno), a positive value if result is not yet available
 */
int hosp_request_version_read(hosp_device* hosp, char* buf, size_t bufsize);

/**
 * Write to the device to request its status.
 *
 * @param hosp An open device handle, not NULL
 * @return 0 on success, a negative value on failure (sets errno)
 */
int hosp_request_status_write(hosp_device* hosp);

/**
 * Try to read from the device to get its status.
 * Integer pointers are optional, though presumably at least one of them should not be NULL.
 *
 * @param hosp An open device handle, not NULL
 * @param is_on Optional boolean value to set if device is ON (1) or OFF (0)
 * @param is_started Optional boolean value to set if device is STARTED (1) or STOPPED (0)
 * @return 0 on success, a negative value on failure (sets errno), a positive value if result is not yet available
 */
int hosp_request_status_read(hosp_device* hosp, int* is_on, int* is_started);

/**
 * Write to the device to request to toggle its ON/OFF state.
 *
 * @param hosp An open device handle, not NULL
 * @return 0 on success, a negative value on failure (sets errno)
 */
int hosp_request_onoff_write(hosp_device* hosp);

/**
 * Write to the device to request to toggle its START/STOP state.
 *
 * @param hosp An open device handle, not NULL
 * @return 0 on success, a negative value on failure (sets errno)
 */
int hosp_request_startstop_write(hosp_device* hosp);

/**
 * Write to the device to request data.
 *
 * @param hosp An open device handle, not NULL
 * @return 0 on success, a negative value on failure (sets errno)
 */
int hosp_request_data_write(hosp_device* hosp);

/**
 * Try to read from the device to get data.
 * Integer pointers are optional, though presumably at least one of them should not be NULL.
 *
 * @param hosp An open device handle, not NULL
 * @param mV Optional millivolts value to set
 * @param mA Optional milliamps value to set
 * @param mW Optional milliwatts value to set
 * @param mWh Optional milliwatt-hours value to set
 * @return 0 on success, a negative value on failure (sets errno), a positive value if result is not yet available
 */
int hosp_request_data_read(hosp_device* hosp, unsigned int* mV, unsigned int* mA, unsigned int* mW, unsigned int* mWh);

#ifdef __cplusplus
}
#endif

#endif
