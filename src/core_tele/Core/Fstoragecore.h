/*
 * Fstoragecore.h
 *
 *  Created on: 09.02.2019
 *      Author: core_alpha
 */

#ifndef CORE_FSTORAGECORE_H_
#define CORE_FSTORAGECORE_H_

#include <stdint.h>
#include "Fstorage_core_shim.h"

namespace core {

class Fstorage_core {
public:
	Fstorage_core();

	void nrf_fstorage_core_init();

	/**
	 *
	 */
	uint8_t nrf_fstorage_read(uint32_t addr, void * p_dest, uint32_t len);

	uint8_t nrf_fstorage_write_page(uint32_t dest, void const * p_src, uint32_t bytes);

	void convert_page_to_8bit_array(uint32_t  *data, uint8_t array[4]);

	uint8_t nrf_fstorage_erase(uint32_t destination, uint32_t pages);

};

} /* namespace core */

#endif /* CORE_FSTORAGECORE_H_ */
