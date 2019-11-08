/*
 * Blecore.cpp
 *
 *  Created on: 08.02.2019
 *      Author: arthur.buchta
 */

#include "Blecore.h"

namespace core {

Ble_core::Ble_core() {
	// TODO Auto-generated constructor stub

}

/**
 * @brief Set tx power while advertising
 * @param tx_power Supported tx_power values: -40dBm, -20dBm, -16dBm, -12dBm, -8dBm, -4dBm, 0dBm, +3dBm and +4dBm.
 */
void Ble_core::set_tx_power_role_adv(int8_t tx_power){
	set_tx_power_role_adv_shim(tx_power);
}

/**
 * @brief set tx power while connected
 * @param tx_power Supported tx_power values: -40dBm, -20dBm, -16dBm, -12dBm, -8dBm, -4dBm, 0dBm, +3dBm and +4dBm.
 */
void Ble_core::set_tx_power_role_conn(int8_t tx_power){
	set_tx_power_role_conn_shim( tx_power);
}

/**
 * @brief Initialize Ble stack, gap and gatt and start advertising
 * @param data_handler ble core Datahandler, that handles received ble data
 */
void Ble_core::ble_init_core(ble_nus_data_handler_t data_handler, uint8_t *deviceName, uint16_t sizeOfDeviceName){
	ble_stack_init_shim();
	gap_params_init_shim(deviceName, sizeOfDeviceName);
	gatt_init_shim();
	services_init_shim(data_handler);
	advertising_init_shim();
	conn_params_init_shim();
	advertising_start_shim();
	set_tx_power_role_adv(0);

}

/**
 * @brief writes the current advertismentData into the committed ble_data_t (pointer) structure
 * @param advertismentData
 */
void Ble_core::getAdvertisementData(ble_data_t *advertismentData){
	getAdvertismentDataShim(advertismentData);
}

/**
 * Change devicename on the fly
 * @param deviceName
 * @param deviceNameSize
 */
void Ble_core::setDeviceName(uint8_t *deviceName, uint16_t deviceNameSize){
	setDeviceNameShim(deviceName, deviceNameSize);
}

/**
 * @brief change advertisement (manufacturing) data on the fly
 * @param new_data_t	byte array with new advertisement data
 * @param new_data_t_size	size of byte new_data_t byte array
 * @todo Till now, only 12 bytes of advertisement data can be changed, increase size.
 */
void Ble_core::advertisementChangeManufactureSpecificData(uint8_t * new_data_t, uint16_t new_data_t_size){
	advertisementChangeManufactureSpecificDataShim(new_data_t, new_data_t_size);
}


/**
* @brief changes in manufacture data the calibration value unit
* @param calibrationValueUnit
*/
void Ble_core::advertisementChangeCalibrationUnit(uint8_t calibrationValueUnit){
	advData[ADVERT_UNIT_INDEX] = calibrationValueUnit;
	advertisementChangeManufactureSpecificData(advData, SIZE_OF_ADV_DATA);
}

/**
* @brief changes in manufacture data the Battery Value
* @param calibrationValueUnit
*/
void Ble_core::advertisementChangeBatteryValue(uint8_t batteryValue[2]){
	advData[ADVERT_INDEX_BATTERY_0] = batteryValue[0];
	advData[ADVERT_INDEX_BATTERY_1] = batteryValue[1];
	advertisementChangeManufactureSpecificData(advData, SIZE_OF_ADV_DATA);
}

/**
* @brief changes in manufacture data the UNIQE identifier
* @param calibrationValueUnit
*/
void Ble_core::advertisementChangeIdentifier(const uint8_t identifier[2]){
	advData[ADVERT_INDEX_ID_0] = identifier[0];
	advData[ADVERT_INDEX_ID_1] = identifier[1];
	advertisementChangeManufactureSpecificData(advData, SIZE_OF_ADV_DATA);
}

/**
* @brief changes in manufacture data the adc value
* @param calibrationValueUnit
*/
void Ble_core::advertisementChangeAdcValue(uint8_t adcValue[3]){
	advData[ADVERT_INDEX_ADC_0] = adcValue[0];
	advData[ADVERT_INDEX_ADC_1] = adcValue[1];
	advData[ADVERT_INDEX_ADC_2] = adcValue[2];
	advertisementChangeManufactureSpecificData(advData, SIZE_OF_ADV_DATA);
}

void Ble_core::advertisementChangeAdcMinValue(uint8_t adcValueMin[2]){
	advData[ADVERT_INDEX_ADC_MIN_0] = adcValueMin[0];
	advData[ADVERT_INDEX_ADC_MIN_1] = adcValueMin[1];
	advertisementChangeManufactureSpecificData(advData, SIZE_OF_ADV_DATA);
}

void Ble_core::advertisementChangeAdcMaxValue(uint8_t adcValueMax[2]){
	advData[ADVERT_INDEX_ADC_MAX_0] = adcValueMax[0];
	advData[ADVERT_INDEX_ADC_MAX_1] = adcValueMax[1];
	advertisementChangeManufactureSpecificData(advData, SIZE_OF_ADV_DATA);
}

void Ble_core::advertisementChangeAdcMinMaxTime(uint8_t adcMinMaxTime[2]){
	advData[ADVERT_INDEX_ADC_MIN_MAX_TIME_0] = adcMinMaxTime[0];
	advData[ADVERT_INDEX_ADC_MIN_MAX_TIME_1] = adcMinMaxTime[1];
	advertisementChangeManufactureSpecificData(advData, SIZE_OF_ADV_DATA);
}

/**
* @brief changes in manufacture data the calibration value
* @param calibrationValue
*/
void Ble_core::advertisementChangeCalibrationValue(uint8_t calibrationValue[4]){
	memcpy(&advData[ADVERT_INDEX_VALUE_0], calibrationValue, 4);
	advertisementChangeManufactureSpecificData(advData, SIZE_OF_ADV_DATA);
}

/**
 * @brief function to send notification
 * @param p_data byte array (max 20 byte)
 * @param p_length length of byte array
 */
void Ble_core::ble_core_data_send(uint8_t   * p_data, uint16_t  * p_length){
	ble_nus_data_send_shim(p_data, p_length);
}

uint32_t Ble_core::bleCoreDataSetRxVal(uint8_t *value, uint8_t sizeOfValueInBytes){
	return bleCoreDataSetRxShim(value, sizeOfValueInBytes);
}

} /* namespace core */
