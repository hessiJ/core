/*
 * UserFunctions.h
 *
 *  Created on: 13.12.2018
 *      Author: arthur.buchta
 */

#ifndef USERFUNCTIONS_H_
#define USERFUNCTIONS_H_

#include "ble_nus_c.h"

int ble_handle_received_data(ble_nus_c_evt_t const * p_ble_nus_evt);
bool data_buffer_handling_legacy(uint8_t data_buffer[2][30], uint8_t device, uint8_t data, int *buffer_size_0, int *buffre_size_1);

bool data_buffer_handling(int data_buffer[3][30][6], uint8_t device, int32_t adc_Data[6], int buffer_current_size[3]);

void memcpy_core(int array[30][6], int from, int to);
void convert_data_adc(uint8_t *data, int32_t adc_Data[7]);


#endif /* USERFUNCTIONS_H_ */
