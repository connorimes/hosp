# Hardkernel ODROID Smart Power Library and Utilities

A library and tools for managing an [ODROID Smart Power](http://www.hardkernel.com/main/products/prdt_info.php?g_code=G137361754360) device.


## Prerequisites

You need an ODROID Smart Power device with a USB connection.

You will also need the [hidapi](https://github.com/signal11/hidapi/) library.
On Ubuntu 14.04 LTS and newer, just install `libhidapi-dev`.


## Building

This project uses CMake.

To build, run:

```sh
mkdir _build
cd _build
cmake ..
make
```


## Installing

To install, run with proper privileges:

```sh
make install
```

On Linux, installation typically places libraries in `/usr/local/lib` and header files in `/usr/local/include/hosp`.


## Uninstalling

Install must be run before uninstalling in order to have a manifest.
To uninstall, run with proper privileges:

```sh
make uninstall
```


## Linking

To link with libhosp, get linker information (including transitive dependencies) with `pkg-config`:

``` sh
pkg-config --libs --static hosp
```

You may also need additional compile flags, e.g., to get headers:

``` sh
pkg-config --cflags hosp
```


## Linux Privileges

To use an ODROID Smart Power without needing sudo/root at runtime, set appropriate [udev](https://en.wikipedia.org/wiki/Udev) privileges.

You can give access to a specific group, e.g. `plugdev`, by creating/modifying a `udev` config file like `/etc/udev/rules.d/10-local.rules`.
Depending on whether you are using the `libusb` or `hidraw` implementations of `hidapi`, use one of the following rules (having both doesn't hurt):

```sh
# ODROID Smart Power - HIDAPI/libusb
SUBSYSTEM=="usb", ATTRS{idVendor}=="04d8", ATTRS{idProduct}=="003f", GROUP="plugdev"

# ODROID Smart Power - HIDAPI/hidraw
KERNEL=="hidraw*", ATTRS{busnum}=="1", ATTRS{idVendor}=="04d8", ATTRS{idProduct}=="003f", GROUP="plugdev"
```

For the new permissions to take effect, the device must be remounted by the kernel - either disconnect and reconnect the device or reboot the system.


## Usage

```C
  // get the handle
  hosp_device* hosp = hosp_open();
  if (hosp == NULL) {
    perror("hosp_open");
    return -errno;
  }
  // maybe ensure the OSP is started using hosp_request_status_write(...)/hosp_request_status_read(...)...
  // ...now read data from it
  unsigned int mV, mA, mW, mWh;
  // we must inform the device of the kind of request we want to read
  if (hosp_request_data_write(hosp)) {
    perror("hosp_request_data_write");
  } else {
    for (i = 0; i < HOSP_READ_RETRIES; i++) {
      // give the device a moment
      usleep(10);
      // now try to read
      if ((ret = hosp_request_data_read(hosp, &mV, &mA, &mW, &mWh)) < 0) {
        // some type of I/O failure
        perror("hosp_request_data_read");
        break;
      } else if (!ret) {
        // read was successful
        printf("Millivolts: %u\nMilliamps: %u\nmilliwatts: %u\nMilliwatt-hours: %u\n", mV, mA, mW, mWh);
        break;
      }
      // if we get here, the device responded but data was not ready yet, so maybe try again
    }
    if (i == HOSP_READ_RETRIES) {
      // we exhausted our retries
      errno = ENODATA;
      perror("hosp_request_data_read");
    }
  }
  // close the handle
  if (hosp_close(hosp)) {
    perror("hosp_close");
    return -errno;
  }
```


## Utilities

The following command-line utilities are also included.
See their man pages for further usage instructions and examples.

* `hosp-get`: Get information from an ODROID Smart Power
* `hosp-poll`: Poll an ODROID Smart Power at regular intervals
* `hosp-set`: Set an ODROID Smart Power ON/OFF and START/STOP status


## Project Source

Find this and related project sources at the [energymon organization on GitHub](https://github.com/energymon).  
This project originates at: https://github.com/energymon/hosp

Bug reports and pull requests for bug fixes and enhancements are welcome.
