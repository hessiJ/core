/*
 * Datahandling.cpp
 *
 *  Created on: 14.02.2019
 *      Author: arthur.buchta
 */

#include "Datahandling.h"

extern "C"{
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
}

namespace core {

Data_handling::Data_handling() {
	// TODO Auto-generated constructor stub

}

void Data_handling::convert_data_adc(uint8_t *data, int32_t adc_Data[8]){
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
	adc_Data[6] = *data;

	//Identifier
	adc_Data[7] = *(data+19);

}

/**
 *
 * @param increment increments of data packages
 * @return time
 */
uint32_t Data_handling::check_increment(uint8_t increment[3]){
	static uint8_t increment_old[3];
	static uint32_t time = 0;
	static bool counted = false;

	time += 10; // 6 measurements per packet with 2 ms  cycle duration

	for(int i = 0; i<3; i++){
		if(((increment[i]-increment_old[i]) == 1 || (increment[i]-increment_old[i]) == -255) && !counted){
			time += 2;
			counted = true;
		}else if (((increment[i]-increment_old[i]) == 1 || (increment[i]-increment_old[i]) == -255) && counted){

		}else if((increment[i]-increment_old[i]) == 0){

		}else{
			//NRF_LOG_INFO("!DATA LOSS! Old %i %i %i, new %i %i %i", increment_old[0], increment_old[1], increment_old[2], increment[0], increment[1], increment[2]);
		}
	}
	counted = false;

	increment_old[0] = increment[0];
	increment_old[1] = increment[1];
	increment_old[2] = increment[2];

	return time;
}

bool Data_handling::data_buffer_handling(int data_buffer[3][30][8], uint8_t device, uint32_t *time, int32_t adc_Data[8], int buffer_current_size[3]){
	static int counter_device[3];
	static uint8_t increment[3];

	for(int i=0; i<8; i++){
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

	for(int i=0; i<=device; i++){
		increment[i] = data_buffer[i][0][6];
	}


	//NRF_LOG_INFO("Increment: %i", increment[0]);

	*time = check_increment(increment);
	return true;
}




void Data_handling::memcpy_core(int array[30][8], int from, int to){
	int j = 0;
	for(int i = from; i<=to; i++){
		for(int k=0; k<8; k++){
			array[j][k] = array[i][k];
		}
		j++;
	}

	for(int i=to+1; i<30; i++){
		for(int j=0; j<8; j++){
			array[i][j] = 0;
		}
	}
}


void Data_handling::print_data(int core_data_buffer[3][30][8], uint32_t time){
	static bool first_print = true;

	if(first_print){
		NRF_LOG_RAW_INFO("\r\nTime(ms),tele_%i,tele_%i,tele_%i", core_data_buffer[0][0][7],core_data_buffer[1][0][7],core_data_buffer[2][0][7]);
		first_print = false;
	}

	NRF_LOG_RAW_INFO("\r\n%i,%i,%i,%i\r\n%i,%i,",
					time,
	        		core_data_buffer[0][0][0],
	        		core_data_buffer[1][0][0],
	        		core_data_buffer[2][0][0],
					time + 2,
	        		core_data_buffer[0][0][1]
	        		);

	NRF_LOG_RAW_INFO("%i,%i\r\n%i,%i,%i,%i",
	        		core_data_buffer[1][0][1],
	        		core_data_buffer[2][0][1],
					time + 4,
	        		core_data_buffer[0][0][2],
	        		core_data_buffer[1][0][2],
	        		core_data_buffer[2][0][2]
	        		);

	NRF_LOG_RAW_INFO("\r\n%i,%i,%i,%i\r\n%i,%i,",
	        		time + 6,
	        		core_data_buffer[0][0][3],
	        		core_data_buffer[1][0][3],
	        		core_data_buffer[2][0][3],
					time + 8,
	        		core_data_buffer[0][0][4]
	        		);

	NRF_LOG_RAW_INFO("%i,%i\r\n%i,%i,%i,%i",
	        		core_data_buffer[1][0][4],
	        		core_data_buffer[2][0][4],
					time + 10,
	        		core_data_buffer[0][0][5],
	        		core_data_buffer[1][0][5],
	        		core_data_buffer[2][0][5]
	        		);
}

} /* namespace core */
