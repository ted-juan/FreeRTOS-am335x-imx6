
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

/*-----------------------------------------------------------*/

