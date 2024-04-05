# Hardkernel ODROID Smart Power Library and Utilities

A library and tools for managing an [ODROID Smart Power](https://wiki.odroid.com/old_product/accessory/odroidsmartpower) device.

> NOTE: If you're using a newer generation [ODROID Smart Power 3](https://wiki.odroid.com/accessory/power_supply_battery/smartpower3) device, see the [hosp](https://github.com/energymon/hosp) project instead.

This project is tested with firmware version "SMART POWER V3.0".


## Prerequisites

You need an ODROID Smart Power device with a USB connection.

You will also need [CMake](https://cmake.org/), [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/), and a [HIDAPI](https://github.com/libusb/hidapi) library.

On Debian-based Linux systems (including Ubuntu):

```sh
sudo apt install cmake libhidapi-dev pkg-config
```

On macOS, using [Homebrew](https://brew.sh/):

```sh
brew install cmake hidapi pkg-config
```


## Building

To build, run:

```sh
mkdir _build
cd _build
cmake ..
cmake --build .
```

To build a shared object library (instead of a static library), add `-DBUILD_SHARED_LIBS=On` to the first cmake command.
Add `-DCMAKE_BUILD_TYPE=Release` for an optimized build.
Refer to CMake documentation for more a complete description of build options.

The build uses pkg-config to search for a supported HIDAPI library.
By default, it first searches for libraries using native OS backends (e.g., `hidraw` for Linux, `IOHIDManager` for macOS), then searches for the `libusb` backend (for Linux, macOS, BSD, and other UNIX-like systems).
To change the default search order, set the `HOSP_HIDAPI_PC_MODULES` variable with a ;-delimited list of pkg-config module name(s), e.g., to prioritize `libusb`:

```sh
cmake .. -DHOSP_HIDAPI_PC_MODULES="hidapi-libusb;hidapi;hidapi-hidraw"
```


## Installing

To install, run with proper privileges:

```sh
cmake --build . --target install
```

On Linux, installation typically places libraries in `/usr/local/lib` and header files in `/usr/local/include/hosp`.


## Uninstalling

Install must be run before uninstalling in order to have a manifest.
To uninstall, run with proper privileges:

```sh
cmake --build . --target uninstall
```


## Linking

To link with libhosp, get linker information (including transitive dependencies) with `pkg-config`:

```sh
pkg-config --libs --static hosp
```

The `--static` flag is unnecessary if you built/installed a shared object library.

You may also need additional compile flags, e.g., to get headers:

```sh
pkg-config --cflags hosp
```


## Linux Privileges

To use an ODROID Smart Power without needing sudo/root at runtime, set appropriate [udev](https://en.wikipedia.org/wiki/Udev) privileges.

You can give access to a specific group, e.g. `plugdev`, by creating/modifying a `udev` config file like `/etc/udev/rules.d/10-local.rules`.
Depending on whether you are using the `libusb` or `hidraw` implementations of `hidapi`, use one of the following rules (having both doesn't hurt):

```
# ODROID Smart Power - HIDAPI/libusb
SUBSYSTEM=="usb", ATTRS{idVendor}=="04d8", ATTRS{idProduct}=="003f", GROUP="plugdev"

# ODROID Smart Power - HIDAPI/hidraw
KERNEL=="hidraw*", ATTRS{idVendor}=="04d8", ATTRS{idProduct}=="003f", GROUP="plugdev"
```

For the new permissions to take effect, the device must be remounted by the kernel - either disconnect and reconnect the device or reboot the system.


## Usage

The user is responsible for initializing and finalizing the HIDAPI library using `hid_init()` and `hid_exit()`.
The following example skips those for brevity.

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

* `hosp-enumerate`: Find and print ODROID Smart Power device paths
* `hosp-get`: Get information from an ODROID Smart Power
* `hosp-poll`: Poll an ODROID Smart Power at regular intervals
* `hosp-set`: Set an ODROID Smart Power ON/OFF and START/STOP status


## Project Source

Find this and related project sources at the [energymon organization on GitHub](https://github.com/energymon).  
This project originates at: https://github.com/energymon/hosp

Bug reports and pull requests for bug fixes and enhancements are welcome.
