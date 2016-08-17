/*
    FreeRTOS V7.0.1 - Copyright (C) 2011 Real Time Engineers Ltd.

    ***************************************************************************
    *                                                                         *
    * If you are:                                                             *
    *                                                                         *
    *    + New to FreeRTOS,                                                   *
    *    + Wanting to learn FreeRTOS or multitasking in general quickly       *
    *    + Looking for basic training,                                        *
    *    + Wanting to improve your FreeRTOS skills and productivity           *
    *                                                                         *
    * then take a look at the FreeRTOS books - available as PDF or paperback  *
    *                                                                         *
    *        "Using the FreeRTOS Real Time Kernel - a Practical Guide"        *
    *                  http://www.FreeRTOS.org/Documentation                  *
    *                                                                         *
    * A pdf reference manual is also available.  Both are usually delivered   *
    * to your inbox within 20 minutes to two hours when purchased between 8am *
    * and 8pm GMT (although please allow up to 24 hours in case of            *
    * exceptional circumstances).  Thank you for your support!                *
    *                                                                         *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/*
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/

/* Standard includes. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "serial.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "am335.h"

int checkInt = 0;
short channel1_flag = FALSE;
short channel2_flag = FALSE;
short channel3_flag = FALSE;

static void prvSetupHardware( void );
extern int test_printf(void);

void DATA_ABORT ( void ) __attribute__((naked));

/*-----------------------------------------------------------*/

static void vRespTask1(void *pvParameters)
{

	uint32_t xLastWakeTime;
	const uint32_t xFrequency = 1;

	pvParameters= pvParameters; /* avoid compile warning */

	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();
	serial_puts(SERIAL_BASE, "task1\n");

	while(1)
	{
		serial_puts(SERIAL_BASE,"TASK1\n");
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		//vTaskDelay(1);
	}

}


static void vRespTask2(void *pvParameters)
{
	pvParameters= pvParameters; /* avoid compile warning */
	serial_puts(SERIAL_BASE,"task2\n");

	while(1)
	{
		serial_puts(SERIAL_BASE,"TASK2\n");
	}
}


void DATA_ABORT()	{
	serial_puts(SERIAL_BASE,"omg dataabort...\n");
	while(1){}
}

/*
 * Starts all the other tasks, then starts the scheduler.
 */
int main( void )
{

	/* Initialise the LED outputs */
	prvSetupHardware();

	//INIT SERIAL
	init_serial(SERIAL_BASE);
	serial_puts(SERIAL_BASE,"Starting FreeRTOS\n");

	test_printf();

	xTaskCreate(vRespTask1, "resp1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, ( xTaskHandle * ) NULL);

	xTaskCreate(vRespTask2, "resp2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, ( xTaskHandle * ) NULL);

	vTaskStartScheduler();

	return 0;
}

/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{

	/* Initialize GPIOs */

	/* Enabling the UART clocks */
	(*(REG32(SERIAL_CLK_CTRL))) =0x2;

        /* Setup GPIO mux */
	// ref: uboot board/ti/am335x/mux.c
        (*(REG32(CONTROL_MODULE + 0x860))) = 0x3f; /* Buzzer,  gpmc_a8 pin, GPIO1_24 */

        /* Setup GPIO direction */
        (*(REG32(GPIO1_BASE+GPIO_OE))) = ~(PIN24);

        /* Switch off the leds */
        (*(REG32(GPIO1_BASE+GPIO_CLEARDATAOUT))) = PIN24;

}

