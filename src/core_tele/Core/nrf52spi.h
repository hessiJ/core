/*
 * nrf52spi.h
 *
 *  Created on: 26.11.2018
 *      Author: arthur.buchta
 */

#ifndef NRF52SPI_H_
#define NRF52SPI_H_

#include <stdint.h>

extern "C" {
#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include <string.h>
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
}

#define SPI_INSTANCE  0 // 0, 1, 2, 3

class Nrf52_spi {
private:
	nrf_drv_spi_t const spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);;
	nrf_drv_spi_config_t spi_config;
	static void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
			void * p_context);

public:
	Nrf52_spi();

	void begin(uint8_t spiInstance, uint8_t misoPin,
				uint8_t mosiPin, uint8_t sckPin, nrf_drv_spi_mode_t mode,
				nrf_drv_spi_frequency_t frequency);

	void set_spi_settings(nrf_spi_frequency_t frequency, nrf_spi_mode_t spi_mode, nrf_spi_bit_order_t spi_bit_order);

	void nrf52_transfer(uint8_t const * p_tx_buffer,
			uint8_t tx_buffer_length, uint8_t * p_rx_buffer,
			uint8_t rx_buffer_length);

	const nrf_drv_spi_t get_instance();


};

#endif /* NRF52SPI_H_ */
