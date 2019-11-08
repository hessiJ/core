/*
 * UserFunctions.c
 *
 *  Created on: 13.12.2018
 *      Author: arthur.buchta
 */

#ifndef USERFUNCTIONS_C_
#define USERFUNCTIONS_C_

#include "UserFunctions.h"
#include <stdint.h>

int ble_handle_received_data(ble_nus_c_evt_t const * p_ble_nus_evt){
	uint8_t * data = p_ble_nus_evt->p_data;

	int dataADC = *(data+4);
	dataADC = (dataADC<<8) | *(data+3);
	dataADC = (dataADC<<8) | *(data+2);
	dataADC = (dataADC<<8) | *(data+1);

	return dataADC;
}

/*
int32_t convert_data_adc_temp(uint8_t *data){
	long int bit24;
	int j = 0;

		bit24 = *(data+2);
		bit24 = (bit24 << 8) | *(data+1);
		bit24 = (bit24 << 8) | *(data);                                 // Converting 3 bytes to a 24 bit int

		bit24 = ( bit24 << 8 );
		adc_Data[j] = ( bit24 >> 8 );

}
*/

void convert_data_adc(uint8_t *data, int32_t adc_Data[7]){
	long int bit24;
	int j = 0;

	for(int i=1; i<18; i=i+3){
		bit24 = *(data+i);
		bit24 = (bit24 << 8) | *(data+i+1);
		bit24 = (bit24 << 8) | *(data+i+2);                                 // Converting 3 bytes to a 24 bit int

		bit24 = ( bit24 << 8 );
		adc_Data[j] = ( bit24 >> 8 );
		j++;
	}

	//counter
	adc_Data[7] = *data;

}


bool data_buffer_handling_legacy(uint8_t data_buffer[2][30], uint8_t device, uint8_t data, int *buffer_size_0, int *buffer_size_1){
	static int counter_device_0 = 0;
	static int counter_device_1 = 0;


	if(device==0){
		data_buffer[device][counter_device_0] = data;
		counter_device_0++;
		*buffer_size_0 = counter_device_0;
	}else if(device==1){
		data_buffer[device][counter_device_1] = data;
		counter_device_1++;
		*buffer_size_1 = counter_device_1;
	}

	if((counter_device_0 >=1 ) && (counter_device_1 >=1)){
		counter_device_0--;
		counter_device_1--;
		return true;
	}

	return false;
}


bool data_buffer_handling(int data_buffer[3][30][6], uint8_t device, int32_t adc_Data[6], int buffer_current_size[3]){
	static int counter_device[3];

	for(int i=0; i<6; i++){
		data_buffer[device][counter_device[device]][i] = adc_Data[i];
	}

	//data_buffer[device][counter_device[device]][0] = adc_val;
	counter_device[device]++;
	buffer_current_size[device] = counter_device[device];

	for(int i=0; i<3; i++){
		if(counter_device[i]==0){
			return false;
		}
	}

	for(int i=0; i<3; i++){
		counter_device[i]--;
		while(counter_device[i]>20){
			counter_device[i]--;
		}
	}
	return true;
}




void memcpy_core(int array[30][6], int from, int to){
	int j = 0;
	for(int i = from; i<=to; i++){
		for(int k=0; k<6; k++){
			array[j][k] = array[i][k];
		}
		j++;
	}

	for(int i=to+1; i<30; i++){
		for(int j=0; j<6; j++){
			array[i][j] = 0;
		}
	}

}

#endif /* USERFUNCTIONS_C_ */
