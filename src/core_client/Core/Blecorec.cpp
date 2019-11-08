/*
 * Blecorec.cpp
 *
 *  Created on: 11.02.2019
 *      Author: arthur.buchta
 */

#include "Blecorec.h"

namespace core {

Ble_core_c::Ble_core_c() {
	// TODO Auto-generated constructor stub

}

void Ble_core_c::core_scan_start(){
	scan_start();
}

void Ble_core_c::ble_client_init(ble_nus_c_evt_handler_t _ble_core_c_evt_handler){
	ble_client_init_shim(_ble_core_c_evt_handler);
}

} /* namespace core */
