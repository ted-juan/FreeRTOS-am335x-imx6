/***************************************************************************/
/* Module Name: am335x_timer.c 		                                       */
/*                                                                         */
/* Description: This module implement the main C entry function for        */
/*          timer interrupt service routing                                */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
#include "FreeRTOS.h"
#include "intc.h"

extern void vTickISR( void );
extern INT32S SYS_Install_Handler(SYS_DEVICE *dev, INT32U location, void (*handler)(SYS_DEVICE *dev));

/*
*============================================================================
*   Public Function: OS_TimerInit
*
*      The ISR used for the scheduler tick.
*
*   Interface parameter:
*		Input:
*			none
*
*		Output:
*			none
*
*============================================================================
*/

/*
 * The ISR used for the scheduler tick.
 */
void vTickISR( void )
{
	/* Save LR. Make sure we will be able to go back to the IRQ handler */
	__asm volatile("push {lr}	\n\t");

    (*(REG32(DMTIMER2 + 0x28))) = 0x01; //Clear pending
    (*(REG32(DMTIMER2 + 0x44))) = 0xFF;

    /* Increment the RTOS tick count, then look for the highest priority
	task that is ready to run. */
	__asm volatile("bl xTaskIncrementTick");

	#if configUSE_PREEMPTION == 1
		__asm volatile("bl vTaskSwitchContext");
	#endif

 	__asm volatile("pop {lr}	\n\t");

}

/*
*============================================================================
*   Public Function: OS_TimerInit
*
*       System Timer init function
*
*   Interface parameter:
*		Input:
*			none
*
*		Output:
*			SYSOK	- successful
*			other	- fail
*
*============================================================================
*/


INT16S OS_TimerInit( void )
{
    /*install IRQ */
    SYS_Install_Handler(NULL, INTC_DMTIMER2, (void(*)(SYS_DEVICE *))vTickISR);

	OS_ENTER_CRITICAL();

	OS_EXIT_CRITICAL();

	return SYSOK;
}
