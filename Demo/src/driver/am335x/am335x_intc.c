/***************************************************************************/
/* Module Name: irq.c 		                                               */
/*                                                                         */
/* Description: This module implement the main C entry function for        */
/*          interrupt service routing and install IRQ routing              */
/*                                                                         */
/*                                                                         */
/* Public Function:                                                        */
/*        SYS_Install_Handler:  function handle the IRQ                    */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
#include "FreeRTOS.h"
#include "intc.h"

void SYS_Unhandle_Irq(SYS_DEVICE *dev);
void vApplicationIRQHandler(void);
void DATA_ABORT ( void ) __attribute__((naked));

/*define function pointer array*/
static void (*SYS_Irq[MAX_IRQ_NUM])(SYS_DEVICE *dev);
static SYS_DEVICE *SYS_IrqDev[MAX_IRQ_NUM];


/*
*============================================================================
*   Public Function: Exception_Handler
*
*       Install the interrupt service routing
*
*   Interface parameter:
*
*============================================================================
*/
void SYS_Data_Abort(void)	{
	printf("dataabort...\n");
	while(1){}
}

/*
*============================================================================
*   Public Function: SYS_Install_Handler
*
*       Install the interrupt service routing
*
*   Interface parameter:
*       Input: 	IRQ number
*				new vector entry point
*       Return value:
*				SYSOK	- successful
*				other	- error
*
*============================================================================
*/
INT32S SYS_Install_Handler(SYS_DEVICE *dev, INT32U location, void (*handler)(SYS_DEVICE *dev))
{
	INT32U irq_set, irq_index;

	printf("location=%d\n", location);
	OS_ENTER_CRITICAL();
	if (location<MAX_IRQ_NUM && SYS_Irq[location] == SYS_Unhandle_Irq)
	{
		irq_set = location >> 5;
		irq_index = location & 0x1f;
        (*(REG32(MPU_INTC + INTCPS_ILSR0 + (location<<2)))) = 0x0C;
        (*(REG32(MPU_INTC + INTCPS_MIR_CLEAR0 + irq_set*0x20))) = ~(*(REG32(MPU_INTC + INTCPS_MIR0 + irq_set*0x20)))| (1<< irq_index);
		SYS_Irq[location] = handler;
		SYS_IrqDev[location] = dev;		/*device pointer, NULL for timer*/
		OS_EXIT_CRITICAL();
	}
	else
	{
		OS_EXIT_CRITICAL();
		SYS_DEBUGP(DBG_SysInfo, DBG_MASK1,
	        ("IRQ%d already install for %s! %d",location, SYS_IrqDev[location]->name));
	}
	return SYSOK;
}

/*
*============================================================================
*   Public Function: vApplicationIRQHandler
*
*       Interrupt service routing, call by vSYS_IRQHandler (portISR.c)
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

void vApplicationIRQHandler(void)
{
	INT32U irq_num, irq_set, irq_index, mask;

	while ((irq_num = (*(REG32(MPU_INTC + INTCPS_SIR_IRQ))) & 0x7f ) !=0 )
	{
		irq_set = irq_num >> 5;
		irq_index = irq_num & 0x1f;
		mask = *(REG32(MPU_INTC + INTCPS_MIR0 + irq_set*0x20));

		if (( (1<<irq_index) & (~mask))==0)
			break;

		/*do isr routing*/
		SYS_Irq[irq_num](SYS_IrqDev[irq_num]);
        (*(REG32(MPU_INTC + INTCPS_CONTROL))) = 0x01; //NEWIRQ

		if (irq_num == INTC_DMTIMER2) /* break for context switch */
			break;
	}
}

/*
*============================================================================
*   Public Function: SYS_Irq_Init
*
*       Initial IRQ handler array
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
INT32S SYS_IRQ_Init(void)
{
	INT32S i;

	OS_ENTER_CRITICAL();

    /* Interrupt Controller module reset */
	(*(REG32(MPU_INTC + INTCPS_SYSCONFIG))) = 0x00000002;
	while(((*(REG32(MPU_INTC + INTCPS_SYSSTATUS))) & 0x1) != 0x1); //wait for module to be OK

	udelay(2000);
    /* Functional clock auto-idle mode : FuncFree */
	(*(REG32(MPU_INTC + INTCPS_IDLE))) = 0x00000001;

	udelay(100);

	(*(REG32(MPU_INTC + 0x68))) = 0xFF;

	for (i=0; i<4; i+=0x20)
	{
		(*(REG32(MPU_INTC + INTCPS_MIR_SET0 + i))) = 0xFFFFFFFF;
		(*(REG32(MPU_INTC + INTCPS_ISR_CLEAR0 + i))) = 0xFFFFFFFF;
	}

    for(i=0;i<MAX_IRQ_NUM;i++)
		SYS_Irq[i]=SYS_Unhandle_Irq;

	OS_EXIT_CRITICAL();

	return SYSOK;
}

/*
*============================================================================
*   Public Function: SYS_Unhandle_Irq
*
*       Interrupt service routing for unregister interrupt,
*
*   Interface parameter:
*		Input:
*			IRQ number
*
*		Output:
*			none
*
*============================================================================
*/
VOID SYS_Unhandle_Irq(SYS_DEVICE *pdev)
{
	printf("unhandle interrupt, dev=%s\n", pdev->name);
}


