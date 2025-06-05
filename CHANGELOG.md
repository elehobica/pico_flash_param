# Change Log
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

## [Unreleased]
### Fixed
* Revised get functions to return const reference

## [1.0.2] - 2025-04-20
### Added
* Add support for Raspberry Pi Pico 2
* Support pico-sdk 2.1.1
* Add GitHub Actions for build and release
### Changed
* Remove setDefault() from Parameter<> class -> use loadDefault()

## [1.0.1] - 2024-08-22
### Added
* Add multicore_test project
* Add setDefault() and getDefault() on Parameter<> class
### Changed
* Revised PrintInfo() format of UserFlash
* Support pico-sdk 2.0.0 (previously 1.5.1)
### Fixed
* Fix to store over 256 bytes (up to 1024 bytes)

## [1.0.0] - 2024-06-01
* Initial release