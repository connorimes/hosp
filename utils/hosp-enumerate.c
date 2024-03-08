/**
 * Enumerate ODROID Smart Power devices.
 *
 * @author Connor Imes
 * @date 2024-03-07
 */
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <hidapi.h>
#include <hosp.h>

// Devices don't appear to have serial numbers.
// As a result, users can only differentiate by path
// They will have to use hid_open_path() rather than hid_open().

// A HIDAPI complete device dump looks like:
//   type: 04d8 003f
//   path: /dev/hidraw0
//   serial_number: 
//   Manufacturer: Microchip Technology Inc.
//   Product:      Simple HID Device Demo
//   Release:      2
//   Interface:    0
//   Usage (page): 0x1 (0xff00)
//   Bus type: 1 (USB)
//

static const char short_options[] = "h";
static const struct option long_options[] = {
  {"help",      no_argument,       NULL, 'h'},
  {0, 0, 0, 0}
};

__attribute__ ((noreturn))
static void print_usage(int exit_code) {
  fprintf(exit_code ? stderr : stdout,
          "Usage: hosp-enumerate [OPTION]...\n"
          "Options:\n"
          "  -h, --help               Print this message and exit\n");
  exit(exit_code);
}

static void parse_args(int argc, char** argv) {
  int c;
  while ((c = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
    switch (c) {
      case 'h':
        print_usage(0);
        break;
      case '?':
      default:
        print_usage(EINVAL);
        break;
    }
  }
}

static void print_hosp_devices(struct hid_device_info* dev) {
  for (; dev != NULL; dev = dev->next) {
    printf("%s\n", dev->path);
  }
}

int main(int argc, char** argv) {
  int ret = 0;

  parse_args(argc, argv);

  if (hid_init() < 0) {
    fprintf(stderr, "hid_init: %ls\n", hid_error(NULL));
    return 1;
  }

  struct hid_device_info* devs = hosp_enumerate();
  if (devs == NULL) {
    ret = errno;
    fprintf(stderr, "hosp_enumerate: %ls\n", hid_error(NULL));
  } else {
    print_hosp_devices(devs);
    hid_free_enumeration(devs);
  }

  hid_exit();
  return ret;
}
