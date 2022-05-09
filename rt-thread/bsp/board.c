/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-24     Tanek        the first version
 * 2018-11-12     Ernest Chen  modify copyright
 */
 
#include <stdint.h>
#include <rthw.h>
#include <rtthread.h>

#define _SCB_BASE       (0xE000E010UL)
#define _SYSTICK_CTRL   (*(rt_uint32_t *)(_SCB_BASE + 0x0))
#define _SYSTICK_LOAD   (*(rt_uint32_t *)(_SCB_BASE + 0x4))
#define _SYSTICK_VAL    (*(rt_uint32_t *)(_SCB_BASE + 0x8))
#define _SYSTICK_CALIB  (*(rt_uint32_t *)(_SCB_BASE + 0xC))
#define _SYSTICK_PRI    (*(rt_uint8_t  *)(0xE000ED23UL))

// Updates the variable SystemCoreClock and must be called 
// whenever the core clock is changed during program execution.
extern void SystemCoreClockUpdate(void);

// Holds the system core clock, which is the system clock 
// frequency supplied to the SysTick timer and the processor 
// core clock.
extern uint32_t SystemCoreClock;

static uint32_t _SysTick_Config(rt_uint32_t ticks)
{
    if ((ticks - 1) > 0xFFFFFF)
    {
        return 1;
    }
    SysTick_enableModule();
    SysTick_setPeriod(ticks);
    SysTick_enableInterrupt();
    Interrupt_enableMaster(); 
    
    return 0;
}

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE 1024
static uint32_t rt_heap[RT_HEAP_SIZE];     // heap default size: 4K(1024 * 4)
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif


static void SystemClock_init(void)
{
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
    GPIO_PIN3 | GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);  
    CS_setExternalClockSourceFrequency(32000,48000000);                
    PCM_setCoreVoltageLevel(PCM_VCORE1);
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);
    CS_startHFXT(false);
    CS_initClockSignal( CS_MCLK,  CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal( CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_4); 
}

static int uart_init(void)
{
        eUSCI_UART_Config uartConfig =
	{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        6,                                     			// BRDIV = 78
        8,                                      			 // UCxBRF = 2
        0x20,                                       			// UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                 		 // No Parity
        EUSCI_A_UART_LSB_FIRST,                  		// LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               	// One stop bit
        EUSCI_A_UART_MODE,                       		// UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
	};
			GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
	    UART_initModule(EUSCI_A0_BASE, &uartConfig);
	    UART_enableModule(EUSCI_A0_BASE);
			return 0;
}

/**
 * This function will initial your board.
 */
void rt_hw_board_init()
{
		SystemClock_init();
    _SysTick_Config(48000000/ RT_TICK_PER_SECOND);
		uart_init();
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}
