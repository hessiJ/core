/*
 * AttributeStorage.h
 *
 *  Created on: 27.02.2019
 *      Author: arthur.buchta
 */

#ifndef CORE_ATTRIBUTESTORAGE_H_
#define CORE_ATTRIBUTESTORAGE_H_

#include "Fstoragecore.h"

namespace core {

class AttributeStorage: public Fstorage_core {
private:
	const uint32_t FSTORAGE_PAGE = 0x3e000;

	uint8_t dataRaw[20];
	void refreshDataRaw();
	void writeDataRaw2Page();

public:

	const uint8_t CALIBRATION_UNIT_ELEMENT = 0;
	const uint8_t CALIBRATION_VALUE_ELEMENT_0 = 1;
	const uint8_t CALIBRATION_VALUE_ELEMENT_1 = 2;
	const uint8_t CALIBRATION_VALUE_ELEMENT_2 = 3;
	const uint8_t CALIBRATION_VALUE_ELEMENT_3 = 4;

	const uint8_t IDENTIFIER_ELEMENT_0 = 5;
	const uint8_t IDENTIFIER_ELEMENT_1 = 6;

	const uint8_t BLE_DEVICE_NAME_ELEMENT_0 = 7;
	const uint8_t BLE_DEVICE_NAME_ELEMENT_1 = 8;
	const uint8_t BLE_DEVICE_NAME_ELEMENT_2 = 9;
	const uint8_t BLE_DEVICE_NAME_ELEMENT_3 = 10;
	const uint8_t BLE_DEVICE_NAME_ELEMENT_4 = 11;
	const uint8_t BLE_DEVICE_NAME_ELEMENT_5 = 13;
	const uint8_t BLE_DEVICE_NAME_ELEMENT_6 = 13;
	const uint8_t BLE_DEVICE_NAME_ELEMENT_7 = 14;
	const uint8_t BLE_DEVICE_NAME_ELEMENT_8 = 15;
	const uint8_t BLE_DEVICE_NAME_ELEMENT_9 = 16;
	const uint8_t BLE_DEVICE_NAME_ELEMENT_10 = 17;
	const uint8_t BLE_DEVICE_NAME_SIZE = 18;

	const uint8_t CALIBRATION_UNIT_FORCE = 0x01;
	const uint8_t CALIBRATION_UNIT_TORQUE = 0x02;
	const uint8_t CALIBRATION_UNIT_MILLIVOLT = 0x03;
	const uint8_t CALIBRATION_UNIT_STRAIN = 0x04;

	AttributeStorage();

	void writeCalibrationUnit(uint8_t calibrationUnit);
	uint8_t readCalibrationUnit();

	void writeCalibrationValue(float calibrationValue);
	float readCalibrationValue();
	void float2Bytes(float input, uint8_t bytes[4]);
	float bytes2Float(uint8_t bytes[4]);

	void writeIdentifier(uint16_t identifier);
	uint16_t readIdentifier();

	void writeDeviceName(uint8_t deviceName[8]);
	void readDeviceName(uint8_t deviceName[8]);

	void writeDeviceNameSize(uint8_t deviceNameSize);
	uint8_t readDeviceNameSize();

};

} /* namespace core */

#endif /* CORE_ATTRIBUTESTORAGE_H_ */
