# Release Notes

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [Unreleased]

### Added

- Functions:
  - hosp_enumerate: new function to wrap `hid_enumerate`.
  - hosp_open_device: new function to open using an existing `hid_device`.
  - hosp_get_device: new function to get the underlying `hid_device` handle.
- Utilities:
  - hosp-enumerate: new executable to find and print ODROID Smart Power device paths.
  - hosp-{get,poll,set}: add `-p`/`--path` CLI argument.

### Changed

- HIDAPI is now a public dependency (was private).
- Functions:
  - hosp_open: don't automatically enable nonblocking on the `hid_device` (now the user's responsibility if they want it).
- Utilities:
  - hosp-poll: flush output at line boundaries to improve streaming/piping performance.
- Build:
  - Increase minimum required CMake version to 3.16.
  - Prefer native HIDAPI backends over `libusb` backend.
  - Add `HOSP_HIDAPI_PC_MODULES` CMake option to let user configure the HIDAPI module search order.
- README: update documentation.
- CI: Switch from Travis CI to GitHub workflows for continuous integration.

### Fixed

- Functions:
  - hosp_request_version_read: stringop-truncation warning with gcc >= 8.
- Utilities:
  - hosp-{poll,set}: missing-noreturn warning for `print_usage` functions.
- Build:
  - pkg-config: pc file is broken when CMAKE_INSTALL_{INCLUDE,LIB}DIR is absolute.


## v0.1.0 - 2018-05-31

### Added

- Initial public release.

[Unreleased]: https://github.com/energymon/hosp/compare/v0.1.0...HEAD
