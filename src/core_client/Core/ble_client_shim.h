/*
 * ble_client_shim.h
 *
 *  Created on: 08.02.2019
 *      Author: arthur.buchta
 */

#ifndef CORE_BLE_CLIENT_SHIM_H_
#define CORE_BLE_CLIENT_SHIM_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "app_timer.h"
#include "bsp_btn_ble.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_db_discovery.h"
#include "ble_lbs_c.h"
#include "ble_conn_state.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_scan.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "ble_nus_c.h"
#include "Uart_core.h"

/**
 * @brief initialize ble_stack, gatt db_discovery nus_c ble_conn_stat, scan and start scan
 * @param _ble_core_c_evt_handler event handler for receiving Handling events from the ble_core_c module
 */
void ble_client_init_shim(ble_nus_c_evt_handler_t _ble_core_c_evt_handler);

/**@brief Function for starting scanning. */
void scan_start(void);

#endif /* CORE_BLE_CLIENT_SHIM_H_ */
