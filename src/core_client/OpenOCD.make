# openOCD flags

openOCDFlags += \
  -f \
  ${OPENOCD}scripts/interface/stlink.cfg \
  -f \
  ${OPENOCD}scripts/target/nrf52.cfg \
  -c \
  init \
  -c \
  'reset init' \
  -c \
  halt \
  -c \
  

openOCDFlagsSoftDevice += \
	${openOCDFlags} \
	'program ${SDK_ROOT}/components/softdevice/s132/hex/s132_nrf52_6.1.0_softdevice.hex verify' \
	-c \
	reset\
	-c \
	exit \



openOCDFlagsTarget += \
	${openOCDFlags} \
	'program _build/nrf52832_xxaa.hex verify' \
	-c \
	reset\
	-c \
	exit \

openOCDFlagsBootloader += \
	${openOCDFlags} \
	'program bootloader/feather_nrf52832_bootloader_s132_6.1.1r0.hex verify' \
	-c \
	reset\
	-c \
	exit \

openOCDFlagsExample += \
	${openOCDFlags} \
	'program hex/ble_app_blinky_pca10040_s132.hex verify' \
	-c \
	reset\
	-c \
	exit \


flashSoftDevice:
	@echo ${OPENOCD}bin/openocd.exe ${openOCDFlagsSoftDevice}
	${OPENOCD}bin/openocd.exe ${openOCDFlagsSoftDevice}
	
# Flash the program
flashTarget:
	@echo ${OPENOCD}bin/openocd.exe ${openOCDFlagsTarget}
	${OPENOCD}bin/openocd.exe ${openOCDFlagsTarget}

flashExample:
	@echo ${OPENOCD}bin/openocd.exe ${openOCDFlagsExample}
	${OPENOCD}bin/openocd.exe ${openOCDFlagsExample}

flashBootloader:
	@echo ${OPENOCD}bin/openocd.exe ${openOCDFlagsBootloader}
	${OPENOCD}bin/openocd.exe ${openOCDFlagsBootloader}
