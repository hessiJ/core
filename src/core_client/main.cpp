
extern "C"{
#include "ble_client_shim.h"
#include "app_timer.h"
#include "UserFunctions.h"
#include "nrf_delay.h"
}

#include "Nordicstandard.h"
#include "Blecorec.h"
#include "Coretimer.h"
#include "Datahandling.h"

core::Nordic_standard nst;
core::Ble_core_c ble;
core::Core_timer core_timer;
core::Data_handling data_handling;


APP_TIMER_DEF(m_timertest);


uint8_t identifier[3];
int core_data_buffer[3][30][8];
int buffer_current_size[3];
int32_t adc_data[8];
uint32_t time_ms;

uint64_t global_ms = 0;

static void timer_handler(void * p_context)
{
	global_ms++;
}


/**@snippet [Handling events from the ble_nus_c module] */
static void ble_core_c_evt_handler(ble_nus_c_t * p_ble_nus_c, ble_nus_c_evt_t const * p_ble_nus_evt)
{
    ret_code_t err_code;

    switch (p_ble_nus_evt->evt_type)
    {
        case BLE_NUS_C_EVT_DISCOVERY_COMPLETE:
        	/*
        	NRF_LOG_INFO("");
        	NRF_LOG_INFO("---BLE_NUS_C_EVT_DISCOVERY_COMPLETE---");
			*/

            err_code = ble_nus_c_handles_assign(p_ble_nus_c, p_ble_nus_evt->conn_handle, &p_ble_nus_evt->handles);
            APP_ERROR_CHECK(err_code);

            err_code = ble_nus_c_tx_notif_enable(p_ble_nus_c, true);

            //NRF_LOG_INFO("Error Code ble_nus_c_tx_notif_enable: %i", err_code);
            APP_ERROR_CHECK(err_code);

            NRF_LOG_INFO("Connected to device with Core Service.");
            break;

        case BLE_NUS_C_EVT_NUS_TX_EVT:

        	//printf("\r\ndata: %i", ble_handle_received_data(p_ble_nus_evt));
        	//NRF_LOG_INFO("RECEIVED NOTIFICATION from %i",);
        	//NRF_LOG_RAW_INFO("\r\nFrom: %i,%i Data: %i", p_ble_nus_c->conn_handle, p_ble_nus_evt->p_data[19], ble_handle_received_data(p_ble_nus_evt));
        	identifier[p_ble_nus_c->conn_handle] = p_ble_nus_evt->p_data[19];

        	//convert_data_adc(p_ble_nus_evt->p_data, adc_data);

        	data_handling.convert_data_adc(p_ble_nus_evt->p_data, adc_data);

        	if(identifier[p_ble_nus_c->conn_handle] == 29){
        		//NRF_LOG_RAW_INFO("\r\nData: %x %x %x", p_ble_nus_evt->p_data[1], p_ble_nus_evt->p_data[2], p_ble_nus_evt->p_data[3]); //adc_data[0]);


        		/*

        		NRF_LOG_RAW_INFO("\r\nData: %i", adc_data[1]);
        		NRF_LOG_RAW_INFO("\r\nData: %i", adc_data[2]);
        		NRF_LOG_RAW_INFO("\r\nData: %i", adc_data[3]);
        		NRF_LOG_RAW_INFO("\r\nData: %i", adc_data[4]);
        		NRF_LOG_RAW_INFO("\r\nData: %i", adc_data[5]);
        		*/
        	}


        	if(data_handling.data_buffer_handling(core_data_buffer,
        										  p_ble_nus_c->conn_handle,
												  &time_ms,
												  adc_data,
												  buffer_current_size)){

        		data_handling.print_data(core_data_buffer, time_ms);


            	data_handling.memcpy_core(core_data_buffer[0], 1, 30);
            	data_handling.memcpy_core(core_data_buffer[1], 1, 30);
            	data_handling.memcpy_core(core_data_buffer[2], 1, 30);
        	}

            break;


        case BLE_NUS_C_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected.");
            ble.core_scan_start();
            break;
    }
}


int main(void)
{
	nrf_delay_ms(5000);
    core_timer.core_timer_init(timer_handler);
    core_timer.core_timer_start();

	nst.Nordic_log_init();
	ble.ble_client_init(ble_core_c_evt_handler);
	nst.Nordic_power_management_init();

    for (;;)
    {
    	nst.Nordic_idle_state_handle();
    }
}


