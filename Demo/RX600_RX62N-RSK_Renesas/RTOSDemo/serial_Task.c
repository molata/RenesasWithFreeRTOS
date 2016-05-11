/* serial include */
#include "HITSerial/Serial.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* MCU include */
#include "iodefine.h"

/*** for test ****/
int siTestCount = 0;    // 用来测试计数的
int uiRestoreCount = 0;   // 用来计数
unsigned char data = 0;

/**** struct for serial receove *****/
typedef struct st_Serial_Buffer
{
	unsigned char ucSerial_buffers[50];     // memory for receive buffer
	unsigned char ucMax_bufferSize;         // custom define buffer size, maybe it was useless 
	unsigned char ucCur_bufferSize;	        // the buffer size of current array
}ST_SERIAL_BUFFER;
ST_SERIAL_BUFFER stSCI_Buffer = {0};
/*  使用信号量进行ISR和Task进行切换   */
xSemaphoreHandle xBinarySemaphore;

/******* 与中断并行的任务 ****/
void vHandlerTask( void *pvParameters )
{
	sci6_init();
	sci6_allowSCIRXI();
	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/* 使用信号量等待一个事件。信号量在调度器启动之前，也即此任务执行之前就已被创建。任务被无超
		时阻塞，所以此函数调用也只会在成功获取信号量之后才会返回。此处也没有必要检测返回值 */
		uiRestoreCount++;
		if(uiRestoreCount > 10000)
		{
			siTestCount = 0;	
		}
		xSemaphoreTake( xBinarySemaphore, portMAX_DELAY );     // 采用信号量来阻塞程序运行，跳转到低优先级的任务上，（对任务进行阻塞态）
		/* 程序运行到这里时，事件必然已经发生。本例的事件处理只是简单地打印输出一个信息 */
		//vPrintString( "Handler task - Processing event.\r\n" );
	}
}

/* The 'enable' in the following line causes the compiler to generate code that
re-enables interrupts on function entry.  This will allow interrupts to nest. */
#pragma interrupt ( vT2_1InterruptHandler( vect = _VECT( _CMT2_CMI2 ), enable ) )
void vT2_1InterruptHandler( void )
{
long lHigherPriorityTaskWoken;

	siTestCount++;
	xSemaphoreGiveFromISR( xBinarySemaphore, &lHigherPriorityTaskWoken );     // 任务接触阻塞
    //lHigherPriorityTaskWoken = xFirstTimerHandler();
	
	if( lHigherPriorityTaskWoken == pdTRUE )
	{
		/* 给出信号量以使得等待此信号量的任务解除阻塞。如果解出阻塞的任务的优先级高于当前任务的优先
		级 – 强制进行一次任务切换，以确保中断直接返回到解出阻塞的任务(优选级更高)。
		说明：在实际使用中， ISR中强制上下文切换的宏依赖于具体移植。此处调用的是基于Open Watcom DOS
		移植的宏。其它平台下的移植可能有不同的语法要求。对于实际使用的平台，请参如数对应移植自带的示
		例程序，以决定正确的语法和符号。
		*/
		portYIELD_FROM_ISR( lHigherPriorityTaskWoken );
	}
    
}

void vSetupTimer2Interrupt( void )
{
	/* Enable compare match timer 0. */
	MSTP( CMT2 ) = 0;

	/* Interrupt on compare match. */
	CMT2.CMCR.BIT.CMIE = 1;

	/* Set the compare match value. */
	CMT2.CMCOR = ( unsigned short ) (53999);

	/* Divide the PCLK by 8. */
	CMT2.CMCR.BIT.CKS = 0;

	/* Enable the interrupt... */
	ICU.IER[0x03].BIT.IEN6 = 1;          //允许向CPU请求中断
    //ICU.IPR[0x06].BYTE =0x0d;            //中断优先级设定
	/* ...and set its priority to the application defined kernel priority. */
	_IPR( _CMT2_CMI2 ) = configKERNEL_INTERRUPT_PRIORITY;

	/* Start the timer. */
	CMT.CMSTR1.BIT.STR2 = 1;
}


/* The 'enable' in the following line causes the compiler to generate code that
re-enables interrupts on function entry.  This will allow interrupts to nest. */
#pragma interrupt ( vSCI6_1InterruptHandler( vect = _VECT( _SCI6_RXI6 ), enable ) )
void vSCI6_1InterruptHandler( void )
{
long vHigherPriorityTaskWoken;

	unsigned int uiBuffer_size;
	uiBuffer_size = stSCI_Buffer.ucCur_BufferSize + 1;
	stSCI_Buffer.ucSerial_buffers[uiBuffer_size] = SCI6.RDR;
	
	
	xSemaphoreGiveFromISR( xBinarySemaphore, &vHigherPriorityTaskWoken );

	if( vHigherPriorityTaskWoken == pdTRUE )
	{
		/* 给出信号量以使得等待此信号量的任务解除阻塞。如果解出阻塞的任务的优先级高于当前任务的优先
		级 – 强制进行一次任务切换，以确保中断直接返回到解出阻塞的任务(优选级更高)。
		说明：在实际使用中， ISR中强制上下文切换的宏依赖于具体移植。此处调用的是基于Open Watcom DOS
		移植的宏。其它平台下的移植可能有不同的语法要求。对于实际使用的平台，请参如数对应移植自带的示
		例程序，以决定正确的语法和符号。
		*/
		portYIELD_FROM_ISR( vHigherPriorityTaskWoken );
	}
    
}