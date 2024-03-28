/**
 * Manage an ODROID Smart Power.
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
// -1 if unset, 0 for OFF/STOP, 1 for ON/START
static int action_onoff = -1;
static int action_startstop = -1;

static const char short_options[] = "hp:o:s:";
static const struct option long_options[] = {
  {"help",      no_argument,       NULL, 'h'},
  {"path",      required_argument, NULL, 'p'},
  {"onoff",     required_argument, NULL, 'o'},
  {"startstop", required_argument, NULL, 's'},
  {0, 0, 0, 0}
};

__attribute__ ((noreturn))
static void print_usage(int exit_code) {
  fprintf(exit_code ? stderr : stdout,
          "Put an ODROID Smart Power into the desired ON/OFF and START/STOP state.\n\n"
          "Usage: hosp-set OPTION [OPTION]...\n"
          "Options:\n"
          "  -h, --help               Print this message and exit\n"
          "  -p, --path               Device path (defaults to the first Smart Power found)\n"
          "  -o, --onoff=1|0          Turn the device ON (1) or OFF (0)\n"
          "  -s, --startstop=1|0      START (1) or STOP (0) the device\n");
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
      case 'o':
        action_onoff = atoi(optarg) ? 1 : 0;
        break;
      case 's':
        action_startstop = atoi(optarg) ? 1 : 0;
        break;
      case '?':
      default:
        print_usage(EINVAL);
        break;
    }
  }
  if (action_onoff < 0 && action_startstop < 0) {
    fprintf(stderr, "Must specify at least one of the -o or -s options.\n");
    print_usage(EINVAL);
  }
}

static int hosp_set_status(hosp_device* hosp) {
  int is_on;
  int is_started;
  int ret = 0;
  // first get the current status
  if (hosp_util_get_status(hosp, &is_on, &is_started)) {
    ret = errno;
    perror("Failed to get status from ODROID Smart Power");
    return ret;
  }
  if (action_onoff >= 0 && action_onoff != is_on) {
    // toggle ON/OFF
    hosp_util_msleep(HOSP_WRITE_READ_DELAY_MS);
    if (hosp_request_onoff_write(hosp)) {
      ret = errno;
      perror("Failed to toggle ON/OFF");
      return ret;
    }
  }
  if (action_startstop >= 0 && action_startstop != is_started) {
    // toggle START/STOP
    hosp_util_msleep(HOSP_WRITE_READ_DELAY_MS);
    if (hosp_request_startstop_write(hosp)) {
      ret = errno;
      perror("Failed to toggle START/STOP");
      return ret;
    }
  }
  return ret;
}

int main(int argc, char** argv) {
  hid_device* hdev = NULL;
  hosp_device* hosp;
  int ret;

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

  ret = hosp_set_status(hosp);

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
