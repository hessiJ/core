/*
 * Blecorec.h
 *
 *  Created on: 11.02.2019
 *      Author: arthur.buchta
 */

#ifndef CORE_BLECOREC_H_
#define CORE_BLECOREC_H_

extern "C"{
#include "ble_client_shim.h"
}

namespace core {

class Ble_core_c {
public:
	Ble_core_c();

	/**
	 * @brief initialize ble_stack, gatt db_discovery nus_c ble_conn_stat, scan and start scan
	 * @param _ble_core_c_evt_handler event handler for receiving Handling events from the ble_core_c module
	 */
	void ble_client_init(ble_nus_c_evt_handler_t _ble_core_c_evt_handler);

	/**@brief Function for starting scanning. */
	void core_scan_start();
};

} /* namespace core */

#endif /* CORE_BLECOREC_H_ */
