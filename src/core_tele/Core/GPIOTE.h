/*
 * GPIOTE.h
 *
 *  Created on: 22.01.2019
 *      Author: arthur.buchta
 */

#ifndef CORE_GPIOTE_H_
#define CORE_GPIOTE_H_


#include <stdint.h>
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "nrf_drv_saadc.h"

class GPIOTE {
private:
	uint8_t const SAMPLES_IN_BUFFER = 1;
	nrf_saadc_value_t m_buffer_pool[2][1];

public:
	GPIOTE();
	int begin();
	int gpio_out_init(uint8_t pin, bool initial_value);
	int gpio_in_init(uint8_t pin, bool initial_value);
	static void gpio_set(uint8_t pin, bool high_low);
	void gpio_saadc_init(unsigned long channel, nrf_drv_saadc_event_handler_t  event_handler);
	static void gpio_saadc_read();

};

#endif /* CORE_GPIOTE_H_ */
