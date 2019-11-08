/*
 * Uart_core.h
 *
 *  Created on: 01.02.2019
 *      Author: arthur.buchta
 */

#ifndef CORE_UART_CORE_H_
#define CORE_UART_CORE_H_


#include "app_uart.h"


#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

void uart_init_shim(void);


#endif /* CORE_UART_CORE_H_ */
