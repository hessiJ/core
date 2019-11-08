/*
 * Datahandling.h
 *
 *  Created on: 14.02.2019
 *      Author: arthur.buchta
 */

#ifndef CORE_DATAHANDLING_H_
#define CORE_DATAHANDLING_H_

extern "C"{
#include <stdint.h>
}

namespace core {

class Data_handling {
public:
	Data_handling();

	bool data_buffer_handling(int data_buffer[3][30][8], uint8_t device, uint32_t *time, int32_t adc_Data[8], int buffer_current_size[3]);

	void print_data(int core_data_buffer[3][30][8], uint32_t time);
	void convert_data_adc(uint8_t *data, int32_t adc_Data[8]);
	void memcpy_core(int array[30][8], int from, int to);
	uint32_t check_increment(uint8_t increment[3]);
};

} /* namespace core */

#endif /* CORE_DATAHANDLING_H_ */
