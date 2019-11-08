/*
 * Ble_core.h
 *
 *  Created on: 01.02.2019
 *      Author: arthur.buchta
 */

#ifndef CORE_BLE_CORE_H_
#define CORE_BLE_CORE_H_

void ble_init_core(ble_nus_data_handler_t data_handler);

/**
 * Change devicename on the fly
 * @param deviceName
 * @param deviceNameSize
 */
void setDeviceNameShim(uint8_t *deviceName, uint16_t deviceNameSize);

/**
 * @brief writes the current advertismentData into the committed ble_data_t (pointer) structure
 * @param advertismentData
 */
void getAdvertismentDataShim(ble_data_t *advertismentData);

/**@brief Function for changing the Advertising functionality.
 * @param new_data_t uint8_t array with new data
 * @param new_data_t_size size of new_data_t max val is 12
 */
void advertisementChangeManufactureSpecificDataShim(uint8_t * new_data_t, uint16_t new_data_t_size);

/**@brief   Function for sending a data to the peer.
 *
 * @details This function sends the input string as an RX characteristic notification to the
 *          peer.
 *
 * @param[in]     p_nus       Pointer to the Nordic UART Service structure.
 * @param[in]     p_data      String to be sent.
 * @param[in,out] p_length    Pointer Length of the string. Amount of sent bytes.
 * @param[in]     conn_handle Connection Handle of the destination client.
 *
 * @retval NRF_SUCCESS If the string was sent successfully. Otherwise, an error code is returned.
 */
void ble_nus_data_send_shim(uint8_t   * p_data, uint16_t  * p_length);

/**
 *
 * @param value you want to set to the rx characteristic
 * @param sizeOfValueInBytes
 * @return NRF_SUCCESS If the string was sent successfully. Otherwise, an error code is returned.
 * @todo FUNCTION IS NOT COMPLETE YET. RETURNS ERROR CODE 5
 */
uint32_t bleCoreDataSetRxShim(uint8_t *value, uint8_t sizeOfValueInBytes);




void set_tx_power_role_adv_shim(int8_t tx_power);
void set_tx_power_role_conn_shim(int8_t tx_power);
void set_tx_power_conn_shim();

void advertising_start_shim(void);
void advertising_init_shim(void);
void gatt_init_shim(void);
void ble_stack_init_shim(void);
void conn_params_init_shim(void);
void services_init_shim(ble_nus_data_handler_t data_handler);
void gap_params_init_shim(uint8_t *deviceName, uint16_t sizeOfDeviceName);
void assert_nrf_callback_shim(uint16_t line_num, const uint8_t * p_file_name);






#endif /* CORE_BLE_CORE_H_ */
