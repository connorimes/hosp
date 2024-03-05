# Release Notes

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [Unreleased]

### Changed

- Build:
  - Increase minimum required cmake version to 3.16.
  - Prefer native HIDAPI backends over `libusb` backend.
- README: update documentation.
- CI: Switch from Travis CI to GitHub workflows for continuous integration.

### Fixed

- hosp_request_version_read: stringop-truncation warning with gcc >= 8.
- hosp-{poll,set}: missing-noreturn warning for `print_usage` functions.
- pkg-config: pc file is broken when CMAKE_INSTALL_{INCLUDE,LIB}DIR is absolute.


## v0.1.0 - 2018-05-31

### Added

- Initial public release.

[Unreleased]: https://github.com/energymon/hosp/compare/v0.1.0...HEAD
