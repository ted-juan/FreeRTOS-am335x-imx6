
/***************************************************************************/
/* Module Name: main.c		                                               */
/*                                                                         */
/* Description: This module implement the main C entry function of the     */
/*          system. System will start by an reset interrupt and execute    */
/*                                                                         */
/*                                                                         */
/* Public Function:                                                        */
/*        main : main entry of the system                                  */
/*        SYS_Init: Initialize function of the system                      */
/*        SYS_Task : Entry function of system task                         */
/*                                                                         */
/***************************************************************************/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "serial.h"

INT8U   DBG_SysInfo = DBG_LEVEL1;
struct SYS_IO_BUF	SYS_OutBuf;
/*-----------------------------------------------------------*/

static void SYS_Task(void *pvParameters)
{
	INT16S ret;

	uint32_t xLastWakeTime;
	const uint32_t xFrequency = 1;

	pvParameters= pvParameters; /* avoid compile warning */

    if ((ret = OS_TimerInit())!=SYSOK)
    {
        SYS_DEBUGP(DBG_SysInfo, DBG_MASK1,
            ("Cannot create timer! %d",ret));
    }
	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();
	printf("start task1\n");

	while(1)
	{
		printf("TASK1\n");
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		//vTaskDelay(1);
	}
}



static void  SYS_HwInit(void)
{

	/* Initialize GPIOs */

	/* Enabling the UART clocks */
	(*(REG32(SERIAL_CLK_CTRL))) =0x2;

    init_serial(SERIAL_BASE);
	//test_printf();
}

/*
*============================================================================
*   Public Function: SYS_Init
*       Create the processes of the system. The function will get the
*       information of process and create them.
*
*   Interface parameter:
*       Input: void pointer
*
*       Return value: SYSOK - successful
*                     other - fail
*
*============================================================================
*/
INT16S SYS_Init(void)
{
    INT8U err;
	extern INT32S SYS_IRQ_Init(void); /* in irq.c */

    /*this function should be done before driver using the none cache region*/
//    SYS_NoneCacheRegionInit();
//    SYS_MEM_Init();
	SYS_IRQ_Init();

    /*Create the system task*/
	err = xTaskCreate(SYS_Task, "sys_task", SYS_TASK_STK_SIZE, NULL, SYS_TASK_PRIO, ( xTaskHandle * ) NULL);

    if (err == pdPASS)
        return SYSOK;
    else
        return SYSERR;
}

/*
 * Starts all the other tasks, then starts the scheduler.
 */
int main( void )
{
    INT16S ret;

    /*clear output buffer memory*/
    memset((CHAR*)&SYS_OutBuf,0,sizeof(struct SYS_IO_BUF));

    /* hardware initialize */
    SYS_HwInit();

    printf("\n\n");
    printf("==========================================\n");
    printf("TI AM335X - FreeRTOS\n");
    printf("==========================================\n");
    printf("System running!....\n");

    /* Create system tasks */
    ret = SYS_Init();
    SYS_ASSERT(("<1> Cannot initial system\n"),ret==SYSOK);

	vTaskStartScheduler();

	return 0;
}

void vApplicationIdleHook( void )
{
#if 0
volatile size_t xFreeHeapSpace, xMinimumEverFreeHeapSpace;

	/* This is just a trivial example of an idle hook.  It is called on each
	cycle of the idle task.  It must *NOT* attempt to block.  In this case the
	idle task just queries the amount of FreeRTOS heap that remains.  See the
	memory management section on the http://www.FreeRTOS.org web site for memory
	management options.  If there is a lot of heap memory free then the
	configTOTAL_HEAP_SIZE value in FreeRTOSConfig.h can be reduced to free up
	RAM. */
	xFreeHeapSpace = xPortGetFreeHeapSize();
	xMinimumEverFreeHeapSpace = xPortGetMinimumEverFreeHeapSize();

	/* Remove compiler warning about xFreeHeapSpace being set but never used. */
	( void ) xFreeHeapSpace;
	( void ) xMinimumEverFreeHeapSpace;
#endif
}

/*-----------------------------------------------------------*/
void vAssertCalled( const char * pcFile, unsigned long ulLine )
{
volatile unsigned long ul = 0;

	( void ) pcFile;
	( void ) ulLine;

	taskENTER_CRITICAL();
	{
		/* Set ul to a non-zero value using the debugger to step out of this
		function. */
		while( ul == 0 )
		{
			portNOP();
		}
	}
	taskEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/
void vApplicationTickHook( void )
{
	#if( mainSELECTED_APPLICATION == 1 )
	{
		/* The full demo includes a software timer demo/test that requires
		prodding periodically from the tick interrupt. */
		vTimerPeriodicISRTests();

		/* Call the periodic queue overwrite from ISR demo. */
		vQueueOverwritePeriodicISRDemo();

		/* Call the periodic event group from ISR demo. */
		vPeriodicEventGroupsProcessing();

		/* Use task notifications from an interrupt. */
		xNotifyTaskFromISR();

		/* Use mutexes from interrupts. */
		vInterruptSemaphorePeriodicTest();
	}
	#endif
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

	/* Pass out a pointer to the StaticTask_t structure in which the Idle task's
	state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

	/* Pass out a pointer to the StaticTask_t structure in which the Timer
	task's state will be stored. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}


/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

void vConfigureTickInterrupt( void )
{
}

void vClearTickInterrupt( void )
{
}

