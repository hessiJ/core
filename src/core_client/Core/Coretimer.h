/*
 * Coretimer.h
 *
 *  Created on: 11.02.2019
 *      Author: arthur.buchta
 */

#ifndef CORE_CORETIMER_H_
#define CORE_CORETIMER_H_


#include "app_timer.h"

namespace core {

class Core_timer {
public:
	Core_timer();
	void core_timer_init(app_timer_timeout_handler_t timeout_handler_01);
	void core_timer_start();
};

} /* namespace core */

#endif /* CORE_CORETIMER_H_ */
