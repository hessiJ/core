/*
 * nrf52spi.cpp
 *
 *  Created on: 26.11.2018
 *      Author: arthur.buchta
 */

#include "nrf52spi.h"



volatile bool spi_xfer_done;

#define UNUSED(x) (void)(x)

#define SPI_INSTANCE  0 /**< SPI instance index. */
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);


const nrfx_spi_t * p_instance = &(spi.u.spi);

Nrf52_spi::Nrf52_spi(){}

void Nrf52_spi::begin(uint8_t spiInstance,
		uint8_t misoPin, uint8_t mosiPin, uint8_t sckPin,
		nrf_drv_spi_mode_t mode, nrf_drv_spi_frequency_t frequency){

	spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
	spi_config.miso_pin = misoPin;
	spi_config.mosi_pin = mosiPin;
	spi_config.sck_pin = sckPin;
	spi_config.mode = mode;
	spi_config.frequency = frequency;

	APP_ERROR_CHECK(
			nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL));
}


void Nrf52_spi::set_spi_settings(nrf_spi_frequency_t frequency, nrf_spi_mode_t spi_mode, nrf_spi_bit_order_t spi_bit_order) {

	// see: modules/nrfx/drivcers/src/nrfx_spi.c

    NRF_SPI_Type * p_spi = p_instance->p_reg;
    //nrf_spi_pins_set(p_spi, p_config->sck_pin, mosi_pin, miso_pin);
    nrf_spi_frequency_set(p_spi, frequency);
    nrf_spi_configure(p_spi, spi_mode, spi_bit_order);
}

void Nrf52_spi::nrf52_transfer(uint8_t const * p_tx_buffer,
		uint8_t tx_buffer_length, uint8_t * p_rx_buffer,
		uint8_t rx_buffer_length) {

	spi_xfer_done = false;

	APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, p_tx_buffer, tx_buffer_length, p_rx_buffer, rx_buffer_length));


	while (!spi_xfer_done) {
		__WFE();
	}
	spi_xfer_done = false;

}



void Nrf52_spi::spi_event_handler(nrf_drv_spi_evt_t const * p_event,
		void * p_context) {
	spi_xfer_done = true;
}

const nrf_drv_spi_t Nrf52_spi::get_instance(){
	return spi;
}

