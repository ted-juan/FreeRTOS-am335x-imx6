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

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "type.h"
#include "am335.h"
#include <string.h>
#include "error.h"
#include "sysdbg.h"
#include "sys.h"

#define ___swab16(x) \
                    ((unsigned int)( \
                                (((unsigned int)(x) & (unsigned int)0x00ffU) << 8) | \
                                (((unsigned int)(x) & (unsigned int)0xff00U) >> 8) ))
#define ___swab32(x) \
                    ((unsigned long)( \
                                (((unsigned long)(x) & (unsigned long)0x000000ffUL) << 24) | \
                                (((unsigned long)(x) & (unsigned long)0x0000ff00UL) <<  8) | \
                                (((unsigned long)(x) & (unsigned long)0x00ff0000UL) >>  8) | \
                                (((unsigned long)(x) & (unsigned long)0xff000000UL) >> 24) ))

#define htonl(x) ___swab32(x)
#define ntohl(x) ___swab32(x)
#define htons(x) ___swab16(x)
#define ntohs(x) ___swab16(x)
            

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION	1		
#define configUSE_IDLE_HOOK	0
#define configUSE_TICK_HOOK	0
#define configUSE_TIME_SLICING	1

//#define configCPU_CLOCK_HZ		( ( unsigned long ) 8000000 )
#define configCPU_CLOCK_HZ		( ( unsigned long ) 1000000000 )
#define configDMTIMER2_CLOCK_HZ		( ( unsigned long ) 24000000 )


/*
 * Task stack size, priority and related configuration
 */

#define SYS_ISR_STK_SIZE 	    512

/*System timer task */
#define TIMER_TASK_PRIO     	4
#define TIMER_TASK_STK_SIZE 	4096

/*System Event task, handle device's event from ISR */
#define EVENT_TASK_PRIO      	6
#define EVENT_TASK_STK_SIZE  	512
#define EVENT_Q_MSG          	5

/*System task */
#define SYS_TASK_PRIO      	    7
#define SYS_TASK_STK_SIZE  	    2048


#define configTICK_RATE_HZ		( ( portTickType ) 1000 ) 
#define configMAX_PRIORITIES		( ( unsigned portBASE_TYPE ) 5 )
#define configMINIMAL_STACK_SIZE       ( ( unsigned short ) 128 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 24 * 1024 ) )
#define configMAX_TASK_NAME_LEN		( 16 )
#define configUSE_TRACE_FACILITY	0
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		1
#define configQUEUE_REGISTRY_SIZE	0

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet	1
#define INCLUDE_uxTaskPriorityGet	1
#define INCLUDE_vTaskDelete		1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend		1
#define INCLUDE_vTaskDelayUntil		1
#define INCLUDE_vTaskDelay		1


#endif /* FREERTOS_CONFIG_H */
