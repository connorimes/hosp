/**
 * A library for managing a Hardkernel ODROID Smart Power (HOSP) device.
 * These devices refresh at 10 Hz (every 100 ms).
 *
 * HOSP devices use a commercial off-the-shelf Microchip Technology Inc. PIC18F45K50 USB Flash MCU.
 * The library uses this MCU's Vendor ID and Product ID (defined below) to optimistically identify HOSP devices.
 * Unfortunately, HOSP devices don't report serial numbers to enable unique device identification.
 * If multiple HID devices with this MCU are connected to the system, the user is responsible for differentiating
 * between them, e.g., by opening the HID device themselves using hid_open_path() rather than relying on hosp_open().
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
#include <hidapi.h>

#define HOSP_VENDOR_ID 0x04d8
#define HOSP_PRODUCT_ID 0x003f

/**
 * Opaque HOSP handle.
 */
typedef struct hosp_device hosp_device;

/**
 * A wrapper around hid_enumerate() to get only HOSP HID devices.
 * This is likely only needed if the user must disambiguate between multiple HOSP devices connected to the system.
 * The user is responsible for calling hid_free_enumeration() when finished with the result.
 *
 * @return The struct hid_device_info* result from hid_enumerate(), or NULL on failure (sets errno)
 */
struct hid_device_info* hosp_enumerate(void);

/**
 * Open a HOSP handle.
 * If more than one device is connected to the system, the first one discovered will be used.
 *
 * @return A hosp_device handle, or NULL on failure (sets errno)
 */
hosp_device* hosp_open(void);

/**
 * Open a HOSP handle, optionally using an open HID device.
 *
 * If a HID device is provided, it must be open for the lifetime of the HOSP handle.
 *
 * @param dev An optional HID device returned from hid_open(); if NULL, the first HOSP device discovered will be used
 * @return A hosp_device handle, or NULL on failure (sets errno)
 */
hosp_device* hosp_open_device(hid_device* dev);

/**
 * Close a HOSP handle.
 *
 * If the user provided the HID device to hosp_open_device(), they are responsible for closing it after hosp_close().
 *
 * @param hosp An open device handle, not NULL
 * @return 0 on success, a negative value on failure (sets errno)
 */
int hosp_close(hosp_device* hosp);

/**
 * Get the underlying HID device.
 *
 * @param hosp An open device handle, not NULL
 * @return The hid_device pointer
 */
hid_device* hosp_get_device(hosp_device* hosp);

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
