**Author:** Arthur Buchta
**Date:**	08.11.2019

# Core Client
This Firmware is part of the core ble central/client project.

## Changelog
All notable changes to this project will be documented below this line.
This project adheres to [Semantic Versioning](http://semver.org/).

### Board
- coreRED cR00100004

## [1.2.0] - 14.02.2019 (Branch global_time)

### Added
- Datahandling.cpp
- Datahandling.h

### Changed
- transformend functions in Userfunctions.h to cpp methods in Datahandling
- added time to data stream
- added 5s delay in startup for server recovering 

## [1.1.0] - 11.02.2019 (Branch Ble_cpp)

### Added
- ble_client_shim.h
- ble_client_shim.c
- Blecorec.h
- Blecorec.cpp
- Nordicstandard.h
- Nordicstandard.cpp
- Coretimer.h
- Coretimer.cpp

### Changed
- Wrapped Nordic standard functions into cpp methods
- Created ble client shim functions and wrapped it in Ble core cpp methods
- Created timer class
- Increased NRF LOG baud rate to 1 000 000 Bd

## [1.0.0] - 08.02.2019 (Initial release) 
	
	