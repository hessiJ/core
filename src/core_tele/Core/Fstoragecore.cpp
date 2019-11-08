/*
 * Fstoragecore.cpp
 *
 *  Created on: 09.02.2019
 *      Author: core_alpha
 */
extern "C"{
#include "Fstoragecore.h"
}

namespace core {

Fstorage_core::Fstorage_core() {
	// TODO Auto-generated constructor stub
}
void Fstorage_core::nrf_fstorage_core_init(){
	nrf_fstorage_core_init_shim();
}

uint8_t Fstorage_core::nrf_fstorage_erase(uint32_t destination, uint32_t pages){
	return nrf_fstorage_erase_page_shim(destination, pages);
}

uint8_t Fstorage_core::nrf_fstorage_read(uint32_t addr, void * p_dest, uint32_t len){
	return nrf_fstorage_read_shim(addr, p_dest, len);
}

uint8_t Fstorage_core::nrf_fstorage_write_page(uint32_t dest, void const * p_src, uint32_t bytes){
	return nrf_fstorage_write_page_shim(dest, p_src, bytes);
}

void Fstorage_core::convert_page_to_8bit_array(uint32_t  *data, uint8_t array[4]){
	array[0] = (uint8_t) (0xff & *data>>24);
	array[1] = (uint8_t) (0xff & (*data>>16));
	array[2] = (uint8_t) (0xff & (*data>>8));
	array[3] = (uint8_t) (0xff & (*data));
}

} /* namespace core */
