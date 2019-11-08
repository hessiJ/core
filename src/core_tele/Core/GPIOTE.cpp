/*
 * GPIOTE.cpp
 *
 *  Created on: 22.01.2019
 *      Author: arthur.buchta
 */

#include "GPIOTE.h"

GPIOTE::GPIOTE() {
	// TODO Auto-generated constructor stub

}

int GPIOTE::begin(){
	ret_code_t err_code;
	err_code = nrf_drv_gpiote_init();
	return err_code;
}

/**
 *
 * @param pin
 * @param initial_value Initial value LOW=false or HIGH=true
 * @return
 */
int GPIOTE::gpio_out_init(uint8_t pin, bool initial_value){
	nrf_drv_gpiote_out_config_t config;

	config.action = NRF_GPIOTE_POLARITY_LOTOHI;
	config.task_pin = true;

	if(initial_value){
		config.init_state = NRF_GPIOTE_INITIAL_VALUE_HIGH;
	}else{
		config.init_state = NRF_GPIOTE_INITIAL_VALUE_LOW;
	}

	return nrf_drv_gpiote_out_init(pin, &config);
}


void GPIOTE::gpio_set(uint8_t pin, bool high_low){
	if(high_low){
		nrf_drv_gpiote_out_set(pin);
	}else{
		nrf_drv_gpiote_out_clear(pin);
	}

}

/**
 *
 * @param pin
 * @param initial_value Initial value LOW=false or HIGH=true
 * @return
 */
int GPIOTE::gpio_in_init(uint8_t pin, bool initial_value){
	return 0;
}


void GPIOTE::gpio_saadc_init(unsigned long channel, nrf_drv_saadc_event_handler_t  event_handler){
    ret_code_t err_code;
    nrf_saadc_channel_config_t channel_config =
        NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(channel);

    err_code = nrf_drv_saadc_init(NULL, event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(0, &channel_config);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0], SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[1], SAMPLES_IN_BUFFER);
    APP_ERROR_CHECK(err_code);
}

void GPIOTE::gpio_saadc_read(){
	nrfx_saadc_sample();
}

