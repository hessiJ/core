/*
 * Nordicstandard.cpp
 *
 *  Created on: 08.02.2019
 *      Author: arthur.buchta
 */

#include "Nordicstandard.h"

namespace core {

Nordic_standard::Nordic_standard() {
	// TODO Auto-generated constructor stub
}

/**@brief Function for initializing power management.
 */
void Nordic_standard::Nordic_power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling the idle state (main loop).
 *
 * @details This function handles any pending log operations, then sleeps until the next event occurs.
 */
void Nordic_standard::Nordic_idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}

/** @brief Function for initializing the log module.
 */

void Nordic_standard::Nordic_log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


} /* namespace core */
