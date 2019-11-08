/*
 * Fstorage_core.h
 *
 *  Created on: 04.02.2019
 *      Author: arthur.buchta
 */

#ifndef FSTORAGE_CORE_H_
#define FSTORAGE_CORE_H_

void nrf_fstorage_core_init_shim();
void power_manage();
uint8_t nrf_fstorage_read_shim(uint32_t addr, void * p_dest, uint32_t len);
uint8_t nrf_fstorage_write_page_shim(uint32_t dest, void const * p_src, uint32_t bytes);
uint8_t nrf_fstorage_erase_page_shim(uint32_t dest, uint32_t pages);
void convert_page_to_8bit_array(uint32_t  *data, uint8_t array[4]);

#endif /* FSTORAGE_CORE_H_ */
