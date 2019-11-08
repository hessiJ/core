/*
 * ble_client_shim.c
 *
 *  Created on: 08.02.2019
 *      Author: arthur.buchta
 */

#include "ble_client_shim.h"

/**
 * Copyright (c) 2014 - 2018, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/**
 * @brief BLE LED Button Service central and client application main file.
 *
 * This example can be a central for up to 8 peripherals.
 * The peripheral is called ble_app_blinky and can be found in the ble_peripheral
 * folder.
 */






#define APP_BLE_CONN_CFG_TAG      1                                     /**< Tag that refers to the BLE stack configuration that is set with @ref sd_ble_cfg_set. The default tag is @ref APP_BLE_CONN_CFG_TAG. */
#define APP_BLE_OBSERVER_PRIO     3                                     /**< BLE observer priority of the application. There is no need to modify this value. */

#define CENTRAL_SCANNING_LED      BSP_BOARD_LED_0
#define CENTRAL_CONNECTED_LED     BSP_BOARD_LED_1
#define LEDBUTTON_LED             BSP_BOARD_LED_2                       /**< LED to indicate a change of state of the Button characteristic on the peer. */

#define LEDBUTTON_BUTTON          BSP_BUTTON_0                          /**< Button that writes to the LED characteristic of the peer. */
#define BUTTON_DETECTION_DELAY    APP_TIMER_TICKS(50)                   /**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define UNUSED(x) (void)(x)

NRF_BLE_GATT_DEF(m_gatt);                                               /**< GATT module instance. */
BLE_NUS_C_ARRAY_DEF(m_ble_nus_c, NRF_SDH_BLE_CENTRAL_LINK_COUNT);           /**< LED button client instances. */
BLE_DB_DISCOVERY_ARRAY_DEF(m_db_disc, NRF_SDH_BLE_CENTRAL_LINK_COUNT);  /**< Database discovery module instances. */
NRF_BLE_SCAN_DEF(m_scan);                                               /**< Scanning Module instance. */

#define SERVICE_UUID {{0x14, 0x12, 0x8a, 0x76, 0x04, 0xd1, 0x6c, 0x4f, 0x7e, 0x53, 0xf2, 0xe8, 0x00, 0x00, 0xb1, 0x19}}

static char const m_target_periph_name[] = "coreTel";             /**< Name of the device to try to connect to. This name is searched for in the scanning report data. */

uint16_t                    conn_handle;

ble_nus_c_t ble_nus_c_global;

const int CORE_SIZE = 1000;
const int CORE_DEVICES = 3;
//uint8_t core_data_buffer[3][1000];

//int buffer_current_size[3];
//int32_t adc_data[6];
//uint8_t identifier[3];

/**@brief Function for handling asserts in the SoftDevice.
 *
 * @details This function is called in case of an assert in the SoftDevice.
 *
 * @warning This handler is only an example and is not meant for the final product. You need to analyze
 *          how your product is supposed to react in case of an assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num     Line number of the failing assert call.
 * @param[in] p_file_name  File name of the failing assert call.
 */


void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(0xDEADBEEF, line_num, p_file_name);
}


/**@brief Function for initializing the LEDs.
 *
 * @details Initializes all LEDs used by the application.
 */
static void leds_init(void)
{
    bsp_board_init(BSP_INIT_LEDS);
}


static void scan_evt_handler(scan_evt_t const * p_scan_evt)
{
    ret_code_t err_code;

    switch(p_scan_evt->scan_evt_id)
    {
        case NRF_BLE_SCAN_EVT_CONNECTING_ERROR:
        {
            err_code = p_scan_evt->params.connecting_err.err_code;
            APP_ERROR_CHECK(err_code);
        } break;

        default:
            break;
    }
}


/**@brief Function for initializing the scanning and setting the filters.
 */
static void scan_init(void)
{
	//NRF_LOG_INFO("");
	//NRF_LOG_INFO("---scan_init---");
    ret_code_t          err_code;
    nrf_ble_scan_init_t init_scan;

    memset(&init_scan, 0, sizeof(init_scan));

    init_scan.connect_if_match = true;
    init_scan.conn_cfg_tag     = APP_BLE_CONN_CFG_TAG;

    err_code = nrf_ble_scan_init(&m_scan, &init_scan, scan_evt_handler);
    //NRF_LOG_INFO("nrf_ble_scan_init: %i", err_code);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_scan_filter_set(&m_scan, SCAN_NAME_FILTER, m_target_periph_name);
    //NRF_LOG_INFO("nrf_ble_scan_filter_set: %i", err_code);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_scan_filters_enable(&m_scan, NRF_BLE_SCAN_NAME_FILTER, false);
    //NRF_LOG_INFO("nrf_ble_scan_filters_enable: %i", err_code);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting scanning. */
void scan_start(void)
{
    ret_code_t ret;

    NRF_LOG_INFO("Start scanning for device name %s.", (uint32_t)m_target_periph_name);
    ret = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(ret);
    // Turn on the LED to signal scanning.
    bsp_board_led_on(CENTRAL_SCANNING_LED);
}





/**@brief Callback handling Nordic UART Service (NUS) client events.
 *
 * @details This function is called to notify the application of NUS client events.
 *
 * @param[in]   p_ble_nus_c   NUS client handle. This identifies the NUS client.
 * @param[in]   p_ble_nus_evt Pointer to the NUS client event.
 */


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err_code;

    // For readability.
    ble_gap_evt_t const * p_gap_evt = &p_ble_evt->evt.gap_evt;

    switch (p_ble_evt->header.evt_id)
    {
        // Upon connection, check which peripheral is connected, initiate DB
        // discovery, update LEDs status, and resume scanning, if necessary.
        case BLE_GAP_EVT_CONNECTED:
        {
        	//NRF_LOG_INFO("");
        	//NRF_LOG_INFO("---BLE_GAP_EVT_CONNECTED---");
            NRF_LOG_INFO("Connection 0x%x established, starting DB discovery.",
                         p_gap_evt->conn_handle);


            APP_ERROR_CHECK_BOOL(p_gap_evt->conn_handle < NRF_SDH_BLE_CENTRAL_LINK_COUNT);

            err_code = ble_nus_c_handles_assign(&m_ble_nus_c[p_gap_evt->conn_handle],
                    p_gap_evt->conn_handle,
                    NULL);

            APP_ERROR_CHECK(err_code);


            //NRF_LOG_INFO("Trying to Enable Notification ...");
            err_code = ble_nus_c_handles_assign(&m_ble_nus_c[p_gap_evt->conn_handle], p_ble_evt->evt.gap_evt.conn_handle, NULL);
            //NRF_LOG_INFO("ErrorCode ble_nus_c_handles_assign: %i", err_code);
            APP_ERROR_CHECK(err_code);




            /*
            err_code = ble_nus_c_tx_notif_enable(&m_ble_nus_c);


            NRF_LOG_INFO("ErrorCode ble_nus_c_tx_notif_enable: %i", err_code);
            APP_ERROR_CHECK(err_code);

            //sd_ble_gattc_write
             */

            //start a service discovery ...
            NRF_LOG_INFO("start service discovery for connection 0x%x ...", p_gap_evt->conn_handle);
            err_code = ble_db_discovery_start(&m_db_disc[p_gap_evt->conn_handle],
                                              p_gap_evt->conn_handle);




            //d_ble_gatts_sys_attr_set();




            if (err_code != NRF_ERROR_BUSY)
            {
            	//NRF_LOG_INFO("Error Code NRF_ERROR_BUSY %i", err_code);
                APP_ERROR_CHECK(err_code);
            }

/*
            //Check Services
            ble_db_discovery_t services = m_db_disc[p_gap_evt->conn_handle];


            uint8_t serviceCount = services.srv_count;
            NRF_LOG_INFO("Found %i services.", serviceCount);
*/
            // Update LEDs status and check whether it is needed to look for more
            // peripherals to connect to.
            bsp_board_led_on(CENTRAL_CONNECTED_LED);
            if (ble_conn_state_central_conn_count() == NRF_SDH_BLE_CENTRAL_LINK_COUNT)
            {
                bsp_board_led_off(CENTRAL_SCANNING_LED);
            }
            else
            {
                // Resume scanning.
                bsp_board_led_on(CENTRAL_SCANNING_LED);
                scan_start();
            }
        } break; // BLE_GAP_EVT_CONNECTED

        // Upon disconnection, reset the connection handle of the peer that disconnected, update
        // the LEDs status and start scanning again.
        case BLE_GAP_EVT_DISCONNECTED:
        {
        	//NRF_LOG_INFO("");
        	//NRF_LOG_INFO("---BLE_GAP_EVT_DISCONNECTED---");

            NRF_LOG_INFO("LBS central link 0x%x disconnected (reason: 0x%x)",
                         p_gap_evt->conn_handle,
                         p_gap_evt->params.disconnected.reason);

            if (ble_conn_state_central_conn_count() == 0)
            {
                err_code = app_button_disable();
                APP_ERROR_CHECK(err_code);

                // Turn off the LED that indicates the connection.
                bsp_board_led_off(CENTRAL_CONNECTED_LED);
            }

            // Start scanning.
            scan_start();

            // Turn on the LED for indicating scanning.
            bsp_board_led_on(CENTRAL_SCANNING_LED);

        } break;

        case BLE_GAP_EVT_TIMEOUT:
        {
            // Timeout for scanning is not specified, so only the connection requests can time out.
            if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN)
            {
                NRF_LOG_DEBUG("Connection request timed out.");
            }
        } break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST.");
            // Accept parameters requested by peer.
            err_code = sd_ble_gap_conn_param_update(p_gap_evt->conn_handle,
                                        &p_gap_evt->params.conn_param_update_request.conn_params);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys = {
            		.rx_phys = BLE_GAP_PHY_AUTO,
            		.tx_phys = BLE_GAP_PHY_AUTO
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_ADV_REPORT:
        {
        	/*
        	ble_gap_evt_adv_report_t report = p_gap_evt->params.adv_report;
        	ble_gap_addr_t peer_addr = report.peer_addr;
        	uint8_t * addrId = peer_addr.addr;
        	//ble_data_t data;

        	if(addrId[5] == 0xdd){
        		//NRF_LOG_INFO("In ADV Report %x: %x: %x: %x: %x: %x", addrId[5],addrId[4],addrId[3],addrId[2],addrId[1],addrId[0]);
        		//data = report.data;
        		//NRF_LOG_INFO("Data length: %i", data.len);

        	}
*/
        	//uint8_t adress = p_gap_evt->params.adv_report.

        } break;

        case BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP:
        {
        	//NRF_LOG_INFO("");
        	//NRF_LOG_INFO("---BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP---");
        	//uint16_t valueLength = p_ble_evt->evt.gattc_evt.params.char_val_by_uuid_read_rsp.count;
        	//ble_gattc_evt_t gatc_event = p_ble_evt->evt.gattc_evt;
        	//NRF_LOG_INFO("Value Length: %i", valueLength);

        }break;

        case BLE_GATTC_EVT_HVX:
        {
        	//NRF_LOG_INFO("");
        	//NRF_LOG_INFO("---BLE_GATTC_EVT_HVX---");

        }break;

        case BLE_GATTC_EVT_TIMEOUT:
        {
        	//NRF_LOG_INFO("");
        	//NRF_LOG_INFO("---BLE_GATTC_EVT_TIMEOUT---");
            // Disconnect on GATT client timeout event.
            NRF_LOG_DEBUG("GATT client timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTS_EVT_TIMEOUT:
        {
        	//NRF_LOG_INFO("");
        	//NRF_LOG_INFO("---BLE_GATTS_EVT_TIMEOUT---");
            // Disconnect on GATT server timeout event.
            NRF_LOG_DEBUG("GATT server timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
        } break;


        default:
            // No implementation needed.
            break;
    }
}




static void nus_c_init(ble_nus_c_evt_handler_t ble_core_c_evt_handler)
{
    ret_code_t       err_code;
    ble_nus_c_init_t init;

    init.evt_handler = ble_core_c_evt_handler;

    for (uint32_t i = 0; i< NRF_SDH_BLE_CENTRAL_LINK_COUNT; i++) {

    	err_code = ble_nus_c_init(&m_ble_nus_c[i], &init);
    	//NRF_LOG_INFO("");
    	//NRF_LOG_INFO("---NUS_C_INIT---")
    	//NRF_LOG_INFO("Error Code ble_nus_c_init %i", err_code);

    	//NRF_LOG_INFO("UUID Type of nus_c: %i", m_ble_nus_c[i].uuid_type);

    	APP_ERROR_CHECK(err_code);
    }
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupts.
 */
static void ble_stack_init()
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);

}


/**@brief Function for writing to the LED characteristic of all connected clients.
 *
 * @details Based on whether the button is pressed or released, this function writes a high or low
 *          LED status to the server.
 *
 * @param[in] button_action The button action (press or release).
 *            Determines whether the LEDs of the servers are ON or OFF.
 *
 * @return If successful, NRF_SUCCESS is returned. Otherwise, returns the error code from @ref ble_lbs_led_status_send.
 */
static ret_code_t led_status_send_to_all(uint8_t button_action)
{
    //ret_code_t err_code;

    for (uint32_t i = 0; i< NRF_SDH_BLE_CENTRAL_LINK_COUNT; i++)
    {
    	//NRF_LOG_INFO("NRF_SDH_BLE_CENTRAL_LINK_COUNT %i", i);
    }
        return NRF_SUCCESS;
}


/**@brief Function for handling events from the button handler module.
 *
 * @param[in] pin_no        The pin that the event applies to.
 * @param[in] button_action The button action (press or release).
 */
static void button_event_handler(uint8_t pin_no, uint8_t button_action)
{
    ret_code_t err_code;

    switch (pin_no)
    {
        case LEDBUTTON_BUTTON:
            err_code = led_status_send_to_all(button_action);
            if (err_code == NRF_SUCCESS)
            {
                NRF_LOG_INFO("LBS write LED state %d", button_action);
            }
            break;

        default:
            APP_ERROR_HANDLER(pin_no);
            break;
    }
}


/**@brief Function for initializing the button handler module.
 */
static void buttons_init(void)
{
    ret_code_t err_code;

   // The array must be static because a pointer to it is saved in the button handler module.
    static app_button_cfg_t buttons[] =
    {
        {LEDBUTTON_BUTTON, false, BUTTON_PULL, button_event_handler}
    };

    err_code = app_button_init(buttons, ARRAY_SIZE(buttons), BUTTON_DETECTION_DELAY);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling database discovery events.
 *
 * @details This function is a callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function forwards the events
 *          to their respective services.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */
static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
	//NRF_LOG_INFO("");
	//NRF_LOG_INFO("---db_disc_handler---");

	//ble_nus_c_on_db_disc_evt(&m_ble_nus_c, p_evt);


    NRF_LOG_DEBUG("call to ble_lbs_on_db_disc_evt for instance %d and link 0x%x!",
                  p_evt->conn_handle,
                  p_evt->conn_handle);


    if(p_evt->evt_type == BLE_DB_DISCOVERY_COMPLETE){
    	NRF_LOG_INFO("DB DISCOVERY COMPLETE");

    }else if(p_evt->evt_type == BLE_DB_DISCOVERY_ERROR){
    	NRF_LOG_INFO("DB DISCOVERY BLE_DB_DISCOVERY_ERROR");

    }else if(p_evt->evt_type == BLE_DB_DISCOVERY_SRV_NOT_FOUND){
    	NRF_LOG_INFO("DB DISCOVERY BLE_DB_DISCOVERY_SRV_NOT_FOUND");

    }else{
    	NRF_LOG_INFO("DB DISCOVERY BLE_DB_DISCOVERY_AVAILABLE");
    }



    //ble_gatt_db_srv_t dataBase = p_evt->params.discovered_db;
    //ble_gatt_db_char_t * charc =  dataBase.charateristics;
    //ble_uuid_t charac_uuid_t = charc->characteristic.uuid;
    //uint16_t charac_uuid = charc->characteristic.uuid.uuid;


    //uint8_t countOfCharac = dataBase.char_count;

   // ble_uuid_t serviceUUID = dataBase.srv_uuid;

    //uint16_t uuidService = serviceUUID.uuid;
/*
    NRF_LOG_INFO("service UUID %x", uuidService);
    NRF_LOG_INFO("Count of Characteristics %i", countOfCharac);
    NRF_LOG_INFO("Characteristic UUID: %x", charac_uuid);
*/
    /*
    ble_gattc_handle_range_t handleRange;
    handleRange.end_handle = 0xffff;
    handleRange.start_handle = 0x0001;

    sd_ble_gattc_char_value_by_uuid_read(p_evt->conn_handle, &charac_uuid_t, &handleRange);
*/

/*
    uint16_t buffer[4];
    conn_handle = p_evt->conn_handle;
    buffer[0] = 16;
    buffer[1] = 2;
    buffer[2] = 1;
    buffer[3] = 0xCACD;

    uint8_t code = sd_ble_gatts_sys_attr_set(conn_handle, (uint8_t*)(buffer), 8, 0);

    NRF_LOG_INFO("error code sd_ble_gatts_sys_attr_set: %i", code);
*/

/*
    uint8_t buf[2];
    buf[0] = BLE_GATT_HVX_NOTIFICATION;
    buf[1] = 0;

    ble_gattc_write_params_t write_params;

    write_params.write_op = BLE_GATT_OP_WRITE_REQ;
    write_params.handle = p_evt->
    write_params.offset = 0;
    write_params.len = sizeof(buf);
    write_params.p_value = buf;

    sd_ble_gattc_write(p_evt->conn_handle, write_params);
*/

    NRF_LOG_INFO("p_evt->conn_handle] %i", p_evt->conn_handle);
    ble_nus_c_on_db_disc_evt(&m_ble_nus_c[p_evt->conn_handle], p_evt);


}


/** @brief Database discovery initialization.
 */
static void db_discovery_init(void)
{
    ret_code_t err_code = ble_db_discovery_init(db_disc_handler);
    APP_ERROR_CHECK(err_code);

    //nus_c_init();
}






/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}

void ble_client_init_shim(ble_nus_c_evt_handler_t _ble_core_c_evt_handler){
    // Initialize.
	//uart_init_shim();
	//printf("\r\n printed data");
    leds_init();
    buttons_init();

    ble_stack_init();
    gatt_init();
    db_discovery_init();
    nus_c_init(_ble_core_c_evt_handler);
    ble_conn_state_init();
    scan_init();

    // Start execution.
    NRF_LOG_INFO("Multilink core started.");
    scan_start();
}

