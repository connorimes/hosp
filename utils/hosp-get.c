/**
 * Get status and data from an ODROID Smart Power.
 *
 * @author Connor Imes
 * @date 2018-05-22
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <hosp.h>
#include "util.h"

int main(void) {
  hosp_device* hosp;
  int ret = 0;
  char version[17];
  int is_on;
  int is_started;
  unsigned int mV;
  unsigned int mA;
  unsigned int mW;
  unsigned int mWh;

  if ((hosp = hosp_open()) == NULL) {
    perror("Failed to open ODROID Smart Power connection");
    return errno;
  }

  if (hosp_util_get_version(hosp, version, sizeof(version)) ||
      hosp_util_get_status(hosp, &is_on, &is_started) ||
      hosp_util_get_data(hosp, &mV, &mA, &mW, &mWh)) {
    ret = errno;
    perror("Failed to get status and data from ODROID Smart Power");
  } else {
    printf("Version: %s\n", version);
    printf("On: %d\nStarted: %d\n", is_on, is_started);
    printf("Millivolts: %u\nMilliamps: %u\nMilliwatts: %u\nMilliwatt-hours: %u\n", mV, mA, mW, mWh);
  }

  if (hosp_close(hosp)) {
    ret = errno;
    perror("Failed to close ODROID Smart Power connection");
  }

  return ret;
}
