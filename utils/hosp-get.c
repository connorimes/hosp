/**
 * Get status and data from an ODROID Smart Power.
 *
 * @author Connor Imes
 * @date 2018-05-22
 */
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <hidapi.h>
#include <hosp.h>
#include "util.h"

static const char* path = NULL;

static const char short_options[] = "hp:";
static const struct option long_options[] = {
  {"help",      no_argument,       NULL, 'h'},
  {"path",      required_argument, NULL, 'p'},
  {0, 0, 0, 0}
};

__attribute__ ((noreturn))
static void print_usage(int exit_code) {
  fprintf(exit_code ? stderr : stdout,
          "Usage: hosp-get [OPTION]...\n"
          "Options:\n"
          "  -h, --help               Print this message and exit\n"
          "  -p, --path               Device path (defaults to the first Smart Power found)\n");
  exit(exit_code);
}

static void parse_args(int argc, char** argv) {
  int c;
  while ((c = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
    switch (c) {
      case 'h':
        print_usage(0);
        break;
      case 'p':
        path = optarg;
        break;
      case '?':
      default:
        print_usage(EINVAL);
        break;
    }
  }
}

int main(int argc, char** argv) {
  hid_device* hdev = NULL;
  hosp_device* hosp;
  int ret = 0;
  char version[17];
  int is_on;
  int is_started;
  unsigned int mV;
  unsigned int mA;
  unsigned int mW;
  unsigned int mWh;

  parse_args(argc, argv);

  if (hid_init() < 0) {
    fprintf(stderr, "hid_init: %ls\n", hid_error(NULL));
    return 1;
  }

  if (path != NULL) {
    if ((hdev = hid_open_path(path)) == NULL) {
      fprintf(stderr, "%ls\n", hid_error(NULL));
      ret = 1;
      goto exit_hid;
    }
  }

  if ((hosp = hosp_open_device(hdev)) == NULL) {
    perror("Failed to open ODROID Smart Power connection");
    ret = errno;
    goto close_hdev;
  }

  if (hid_set_nonblocking(hosp_get_device(hosp), 1) < 0) {
    // Not a fatal error.
    fprintf(stderr, "hid_set_nonblocking: %ls\n", hid_error(hosp_get_device(hosp)));
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

close_hdev:
  if (hdev != NULL) {
    hid_close(hdev);
  }

exit_hid:
  hid_exit();
  return ret;
}
