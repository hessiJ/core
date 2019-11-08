/*
 * Coretimer.cpp
 *
 *  Created on: 11.02.2019
 *      Author: arthur.buchta
 */

#include "Coretimer.h"

extern "C"{
#include "app_error.h"
}

APP_TIMER_DEF(m_timer_01);

namespace core {

Core_timer::Core_timer() {
	// TODO Auto-generated constructor stub

}

void Core_timer::core_timer_init(app_timer_timeout_handler_t timeout_handler_01){
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_timer_01, APP_TIMER_MODE_REPEATED, timeout_handler_01);
    APP_ERROR_CHECK(err_code);

}

void Core_timer::core_timer_start(){
	ret_code_t err_code = app_timer_start(m_timer_01, APP_TIMER_TICKS(1), NULL); // APP_TIMER_TICKS(100), NULL)
    APP_ERROR_CHECK(err_code);
}

} /* namespace core */
