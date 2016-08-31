/***************************************************************************/
/* Module Name: am335x_timer.c 		                                       */
/*                                                                         */
/* Description: This module implement the main C entry function for        */
/*          timer interrupt service routing                                */
/*                                                       void vTickISR( void )                  */
/*                                                                         */
/* Public Function:                                                        */
/*        SYS_Install_Handler:  function handle the IRQ                    */
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
    volatile int i;
    volatile unsigned long ul;
    unsigned long ulCompareMatch;
    extern void ( vPortYieldProcessor ) ( void );

    /*install IRQ */
    SYS_Install_Handler(NULL, INTC_DMTIMER2, (void(*)(SYS_DEVICE *))vTickISR);

    /* Calculate the match value required for our wanted tick rate */
    ulCompareMatch = configDMTIMER2_CLOCK_HZ / configTICK_RATE_HZ;

	OS_ENTER_CRITICAL();
    //READ CM_PER offset 0, bit 14 to see if timer2 is active
    /* Set and enable DMTIMER2 */
    (*(REG32(CM_DPLL_REG + DMTIMER2_CLKSEL))) =0x1;
     (*(REG32(CM_PER + CM_PER_TIMER2))) =0x2;
     udelay(2000);

    //(*(REG32(GPTI1 + GPTI_TIOCP_CFG))) = 0x2; // reset interface
    (*(REG32(DMTIMER2 + 0x10))) = 0x1; // reset interface

    udelay(100);
    //(*(REG32(GPTI1 + GPTI_TCRR))) = 0; // initialize counter
    (*(REG32(DMTIMER2 + 0x3C))) = 0; // initialize counter
    udelay(100);
    //(*(REG32(GPTI1 + GPTI_TMAR))) = ulCompareMatch; // load match value
    (*(REG32(DMTIMER2 + 0x4C))) = ulCompareMatch;
    udelay(100);
    /* Clear pending matching interrupt (if any) */
    //(*(REG32(GPTI1 + GPTI_TISR))) = 0x1;
    (*(REG32(DMTIMER2 + 0x28))) = 0x1;
    udelay(100);
    /* Enable matching interrupts */
    //(*(REG32(GPTI1 + GPTI_TIER))) = 0x1;
    (*(REG32(DMTIMER2 + 0x2C))) = 0x1;
    (*(REG32(DMTIMER2 + 0x40))) = 0x0; //reload value
    udelay(100);
    /* Timer Control Register
     * bit 0 -> start
     * bit 1 -> autoreload
     * bit 6 -> compare enabled
     */
    (*(REG32(DMTIMER2 + 0x38))) = 0x43;
    udelay(100);
    /* Reset the timer */
    //(*(REG32(GPTI1 + GPTI_TTGR))) = 0xFF;

    (*(REG32(DMTIMER2 + 0x44))) = 0xFF;
    udelay(100);

	OS_EXIT_CRITICAL();

}
