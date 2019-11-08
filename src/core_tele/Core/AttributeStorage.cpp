/*
 * AttributeStorage.cpp
 *
 *  Created on: 27.02.2019
 *      Author: arthur.buchta
 */

#include "AttributeStorage.h"
#include "String.h"
namespace core {

AttributeStorage::AttributeStorage() {
	// TODO Auto-generated constructor stub

}





void AttributeStorage::writeCalibrationUnit(uint8_t calibrationUnit){
	refreshDataRaw();
	dataRaw[CALIBRATION_UNIT_ELEMENT] = calibrationUnit;
	writeDataRaw2Page();
}


uint8_t AttributeStorage::readCalibrationUnit(){
	refreshDataRaw();
	return dataRaw[CALIBRATION_UNIT_ELEMENT];
}

void AttributeStorage::writeCalibrationValue(float calibrationValue){
	uint8_t doubleValueAsBytes[4];
	float2Bytes(calibrationValue, doubleValueAsBytes);
	refreshDataRaw();
	memcpy(&dataRaw[CALIBRATION_VALUE_ELEMENT_0], doubleValueAsBytes, 4); //copy Doublevalues as bytes into dataRaw array
	writeDataRaw2Page();
}

float AttributeStorage::readCalibrationValue(){
	refreshDataRaw();
	return bytes2Float(&dataRaw[CALIBRATION_VALUE_ELEMENT_0]);
}

void AttributeStorage::writeIdentifier(uint16_t identifier){
	refreshDataRaw();
	dataRaw[IDENTIFIER_ELEMENT_0] = (uint8_t)(identifier>>8);
	dataRaw[IDENTIFIER_ELEMENT_1] = (uint8_t)(identifier & 0xff);
	writeDataRaw2Page();
}

uint16_t AttributeStorage::readIdentifier(){
	refreshDataRaw();
	return (dataRaw[IDENTIFIER_ELEMENT_0]<<8 | dataRaw[IDENTIFIER_ELEMENT_1]);
}

void AttributeStorage::writeDeviceName(uint8_t deviceName[10]){
	refreshDataRaw();
	memcpy(&dataRaw[BLE_DEVICE_NAME_ELEMENT_0], deviceName, 10);
	writeDataRaw2Page();
}

void AttributeStorage::readDeviceName(uint8_t deviceName[10]){
	refreshDataRaw();
	memcpy(deviceName, &dataRaw[BLE_DEVICE_NAME_ELEMENT_0], 10);
}

void AttributeStorage::writeDeviceNameSize(uint8_t deviceNameSize){
	refreshDataRaw();
	dataRaw[BLE_DEVICE_NAME_SIZE] = deviceNameSize;
	writeDataRaw2Page();
}

uint8_t AttributeStorage::readDeviceNameSize(){
	refreshDataRaw();
	return dataRaw[BLE_DEVICE_NAME_SIZE];
}

void AttributeStorage::refreshDataRaw(){
	nrf_fstorage_read(FSTORAGE_PAGE, dataRaw, sizeof(dataRaw));
}

void AttributeStorage::writeDataRaw2Page(){
	nrf_fstorage_erase(FSTORAGE_PAGE, 1);
	nrf_fstorage_write_page(FSTORAGE_PAGE, dataRaw, sizeof(dataRaw));
}

void AttributeStorage::float2Bytes(float input, uint8_t bytes[4]){
	union {
	    float float_variable;
	    uint8_t tempBytes[4];
	  } u;
	  // Overite bytes of union with float variable
	  u.float_variable = input;
	  // Assign bytes to input array
	  memcpy(bytes, u.tempBytes, 4);
}

float AttributeStorage::bytes2Float(uint8_t bytes[4]){
	union {
	    float floatVariable;
	    uint8_t tempBytes[4];
	  } u;
	  memcpy(u.tempBytes, bytes, 4);

	  return u.floatVariable;
}



} /* namespace core */
