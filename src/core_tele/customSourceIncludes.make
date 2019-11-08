#Custom sourceFiles
SRC_FILES += \
$(PROJ_DIR)/Core/ble_nus.c \
$(PROJ_DIR)/Core/MPU9250.cpp \
$(PROJ_DIR)/Core/nrf52spi.cpp \
$(PROJ_DIR)/Core/GPIOTE.cpp \
$(PROJ_DIR)/Core/Ads1220.cpp \
$(PROJ_DIR)/Core/Ble_core.c \
$(PROJ_DIR)/Core/Blecore.cpp \
$(PROJ_DIR)/Core/Uart_core.c \
$(PROJ_DIR)/Core/Fstorage_core_shim.c \
$(PROJ_DIR)/Core/Fstoragecore.cpp\
$(PROJ_DIR)/Core/Nordicstandard.cpp\
$(PROJ_DIR)/Core/AttributeStorage.cpp\

#Custom Includefiles
INC_FOLDERS += \
$(PROJ_DIR)/ \
$(PROJ_DIR)/Core \