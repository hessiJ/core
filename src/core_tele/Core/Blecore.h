/*
 * Blecore.h
 *
 *  Created on: 08.02.2019
 *      Author: arthur.buchta
 */

#ifndef CORE_BLECORE_H_
#define CORE_BLECORE_H_

extern "C"{
#include <stdint.h>
#include <string.h>
#include "ble_nus.h"
#include "ble_core.h"
}
namespace core {

class Ble_core {
private:
	//two bytes battery two bytes id, thre bytes sensor data
	uint8_t const ADVERT_UNIT_INDEX = 0;
	uint8_t const ADVERT_INDEX_VALUE_0 = 1;
	uint8_t const ADVERT_INDEX_VALUE_1 = 2;
	uint8_t const ADVERT_INDEX_VALUE_2 = 3;
	uint8_t const ADVERT_INDEX_VALUE_3 = 4;
	uint8_t const ADVERT_INDEX_BATTERY_0 = 5;
	uint8_t const ADVERT_INDEX_BATTERY_1 = 6;
	uint8_t const ADVERT_INDEX_ID_0 = 7;
	uint8_t const ADVERT_INDEX_ID_1 = 8;
	uint8_t const ADVERT_INDEX_ADC_0 = 9;
	uint8_t const ADVERT_INDEX_ADC_1 = 10;
	uint8_t const ADVERT_INDEX_ADC_2 = 11;
	uint8_t const ADVERT_INDEX_ADC_MIN_0 = 12;
	uint8_t const ADVERT_INDEX_ADC_MIN_1 = 13;
	uint8_t const ADVERT_INDEX_ADC_MAX_0 = 14;
	uint8_t const ADVERT_INDEX_ADC_MAX_1 = 15;
	uint8_t const ADVERT_INDEX_ADC_MIN_MAX_TIME_0 = 15;
	uint8_t const ADVERT_INDEX_ADC_MIN_MAX_TIME_1 = 16;
	static const uint8_t  SIZE_OF_ADV_DATA = 18;
	uint8_t advData[SIZE_OF_ADV_DATA];
public:
	Ble_core();

	/**
	 * @brief Initialize Ble stack, gap and gatt and start advertising
	 * @param data_handler ble core Datahandler, that handles received ble data
	 * @param deviceName
	 * @param sizeOfDeviceName
	 */
	void ble_init_core(ble_nus_data_handler_t data_handler, uint8_t *deviceName, uint16_t sizeOfDeviceName);

	/**
	 * @brief writes the current advertismentData into the committed ble_data_t (pointer) structure
	 * @param advertismentData
	 */
	void getAdvertisementData(ble_data_t *advertismentData);

	/**
	 * Change devicename on the fly
	 * @param deviceName
	 * @param deviceNameSize
	 */
	void setDeviceName(uint8_t *deviceName, uint16_t deviceNameSize);

	/**
	 * @brief change advertisement (manufacturing) data on the fly
	 * @param new_data_t	byte array with new advertisement data
	 * @param new_data_t_size	size of byte new_data_t byte array
	 * @todo Till now, only 12 bytes of advertisement data can be changed, increase size.
	 */
	void advertisementChangeManufactureSpecificData(uint8_t * new_data_t, uint16_t new_data_t_size);

	/**
	 * @brief changes in manufacture data the calibration value unit
	 * @param calibrationValueUnit
	 */
	void advertisementChangeCalibrationUnit(uint8_t calibrationValueUnit);

	/**
	 * @brief changes in manufacture data the calibration value
	 * @param calibrationValue
	 */
	void advertisementChangeCalibrationValue(uint8_t calibrationValue[4]);

	/**
	* @brief changes in manufacture data the Battery Value
	* @param calibrationValueUnit
	*/
	void advertisementChangeBatteryValue(uint8_t batteryValue[2]);


	/**
	* @brief changes in manufacture data the UNIQE identifier
	* @param calibrationValueUnit
	*/
	void advertisementChangeIdentifier(const uint8_t identifier[2]);

	/**
	* @brief changes in manufacture data the adc value
	* @param calibrationValueUnit
	*/
	void advertisementChangeAdcValue(uint8_t adcValue[3]);

	void advertisementChangeAdcMinValue(uint8_t adcValueMin[2]);

	void advertisementChangeAdcMaxValue(uint8_t adcValueMan[2]);

	void advertisementChangeAdcMinMaxTime(uint8_t adcMinMaxTime[2]);


	/**
	 * @brief function to send notification
	 * @param p_data byte array (max 20 byte)
	 * @param p_length length of byte array
	 */
	void ble_core_data_send(uint8_t   * p_data, uint16_t  * p_length);

	/**
	 * @param value Value to set in Characteristic
	 * @param sizeOfValueInBytes Length of value in bytes
	 * @return error code
	 */
	uint32_t bleCoreDataSetRxVal(uint8_t *value, uint8_t sizeOfValueInBytes);

	/**
	 * @brief Set tx power while advertising
	 * @param tx_power Supported tx_power values: -40dBm, -20dBm, -16dBm, -12dBm, -8dBm, -4dBm, 0dBm, +3dBm and +4dBm.
	 */
	void set_tx_power_role_adv(int8_t tx_power);

	/**
	 * @brief set tx power while connected
	 * @param tx_power Supported tx_power values: -40dBm, -20dBm, -16dBm, -12dBm, -8dBm, -4dBm, 0dBm, +3dBm and +4dBm.
	 */
	void set_tx_power_role_conn(int8_t tx_power);
};

} /* namespace core */

#endif /* CORE_BLECORE_H_ */
