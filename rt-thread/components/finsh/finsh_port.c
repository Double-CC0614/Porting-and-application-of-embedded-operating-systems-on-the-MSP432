/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rthw.h>
#include <rtconfig.h>

#ifndef RT_USING_FINSH
#error Please uncomment the line <#include "finsh_config.h"> in the rtconfig.h 
#endif

#ifdef RT_USING_FINSH

RT_WEAK char rt_hw_console_getchar(void)
{
    /* Note: the initial value of ch must < 0 */
    int ch = -1;
	   if (UART_getInterruptStatus(EUSCI_A0_BASE,EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG))
    {
        // Echo the received character back
        ch = UART_receiveData(EUSCI_A0_BASE);
    }
    return ch;
}

#endif /* RT_USING_FINSH */

