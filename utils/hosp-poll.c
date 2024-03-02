/**
 * Poll an ODROID Smart Power at regular intervals.
 *
 * @author Connor Imes
 * @date 2018-05-22
 */
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <hosp.h>
#include "util.h"

#define HOSP_DEFAULT_INTERVAL_MS 100

#ifndef HOSP_MAX_FAILURES
  #define HOSP_MAX_FAILURES 10
#endif

static volatile int running = 1;
static int restart = 0;
static int count = 0;
static unsigned long interval_ms = HOSP_DEFAULT_INTERVAL_MS;

static const char short_options[] = "hrc:i:";
static const struct option long_options[] = {
  {"help",      no_argument,       NULL, 'h'},
  {"restart",   no_argument,       NULL, 'r'},
  {"count",     required_argument, NULL, 'c'},
  {"interval",  required_argument, NULL, 'i'},
  {0, 0, 0, 0}
};

__attribute__ ((noreturn))
static void print_usage(int exit_code) {
  fprintf(exit_code ? stderr : stdout,
          "Usage: hosp-poll [OPTION]...\n"
          "Options:\n"
          "  -h, --help               Print this message and exit\n"
          "  -r, --restart            Restart the Watt-hour counter before polling\n"
          "  -c, --count=N            Stop after N reads\n"
          "  -i, --interval=MS        The polling interval in milliseconds (default=%u)\n",
          HOSP_DEFAULT_INTERVAL_MS);
  exit(exit_code);
}

static void parse_args(int argc, char** argv) {
  int c;
  while ((c = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
    switch (c) {
      case 'h':
        print_usage(0);
        break;
      case 'r':
        restart = 1;
        break;
      case 'c':
        count = 1;
        running = atoi(optarg);
        break;
      case 'i':
        interval_ms = strtoul(optarg, NULL, 0);
        break;
      case '?':
      default:
        print_usage(EINVAL);
        break;
    }
  }
}

static void shandle(int sig) {
  switch (sig) {
    case SIGTERM:
    case SIGINT:
#ifdef SIGQUIT
    case SIGQUIT:
#endif
#ifdef SIGKILL
    case SIGKILL:
#endif
#ifdef SIGHUP
    case SIGHUP:
#endif
      running = 0;
    default:
      break;
  }
}

static int hosp_restart(hosp_device* hosp) {
  int ret = 0;
  int is_on;
  int is_started;
  // get current status
  if (hosp_util_get_status(hosp, &is_on, &is_started)) {
    ret = errno;
    perror("Failed to get status from ODROID Smart Power");
    return ret;
  }
  if (!is_on) {
    // turn on
    if (hosp_request_onoff_write(hosp)) {
      ret = errno;
      perror("Failed to turn on ODROID Smart Power");
      return ret;
    }
  } else if (is_started) {
    // stop
    if (hosp_request_startstop_write(hosp)) {
      ret = errno;
      perror("Failed to stop ODROID Smart Power");
      return ret;
    }
  }
  // start
  if (hosp_request_startstop_write(hosp)) {
    ret = errno;
    perror("Failed to start ODROID Smart Power");
    return ret;
  }
  // allow time for counter to restart
  hosp_util_msleep(HOSP_DEFAULT_INTERVAL_MS);
  return 0;
}

static int hosp_poll(hosp_device* hosp) {
  int ret = 0;
  unsigned int mV;
  unsigned int mA;
  unsigned int mW;
  unsigned int mWh;
  unsigned int failures = 0;
  // print header
  printf("Millivolts,Milliamps,Milliwatts,Milliwatt-hours\n");
  while (running) {
    if (count) {
      running--;
    }
    // get data
    if (hosp_util_get_data(hosp, &mV, &mA, &mW, &mWh)) {
      perror("Failed to get data from ODROID Smart Power");
      failures++;
      if (failures >= HOSP_MAX_FAILURES) {
        ret = errno;
        running = 0;
        fprintf(stderr, "Too many consecutive failures, exiting...\n");
      }
    } else {
      // print data
      printf("%u,%u,%u,%u\n", mV, mA, mW, mWh);
      failures = 0;
    }
    if (running) {
      // sleep for interval
      hosp_util_msleep(interval_ms);
    }
  }
  return ret;
}

int main(int argc, char** argv) {
  hosp_device* hosp;
  int ret;

  signal(SIGINT, shandle);
  parse_args(argc, argv);

  if ((hosp = hosp_open()) == NULL) {
    perror("Failed to open ODROID Smart Power connection");
    return errno;
  }

  if (!restart || !(ret = hosp_restart(hosp))) {
    ret = hosp_poll(hosp);
  }

  if (hosp_close(hosp)) {
    ret = errno;
    perror("Failed to close ODROID Smart Power connection");
  }

  return ret;
}
