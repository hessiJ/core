/*
 * Nordicstandard.h
 *
 *  Created on: 08.02.2019
 *      Author: arthur.buchta
 */

#ifndef CORE_NORDICSTANDARD_H_
#define CORE_NORDICSTANDARD_H_

extern "C"{
#include "sdk_errors.h"
#include "nrf_pwr_mgmt.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_delay.h"
}

namespace core {

class Nordic_standard {
private:

public:
	Nordic_standard();

	/** @brief Method for initializing the log module.
	 */
	void Nordic_log_init(void);

	/**@brief Function for handling the idle state (main loop).
	 *
	 * @details This function handles any pending log operations, then sleeps until the next event occurs.
	 */
	void Nordic_idle_state_handle(void);

	/**@brief Function for initializing power management.
	 */
	void Nordic_power_management_init(void);
};

} /* namespace core */

#endif /* CORE_NORDICSTANDARD_H_ */
