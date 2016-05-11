/* serial include */
#include "HITSerial/Serial.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* MCU include */
#include "iodefine.h"

/*** for test ****/
int siTestCount = 0;    // �������Լ�����
int uiRestoreCount = 0;   // ��������
unsigned char data = 0;

/**** struct for serial receove *****/
typedef struct st_Serial_Buffer
{
	unsigned char ucSerial_buffers[50];     // memory for receive buffer
	unsigned char ucMax_bufferSize;         // custom define buffer size, maybe it was useless 
	unsigned char ucCur_bufferSize;	        // the buffer size of current array
}ST_SERIAL_BUFFER;
ST_SERIAL_BUFFER stSCI_Buffer = {0};
/*  ʹ���ź�������ISR��Task�����л�   */
xSemaphoreHandle xBinarySemaphore;

/******* ���жϲ��е����� ****/
void vHandlerTask( void *pvParameters )
{
	sci6_init();
	sci6_allowSCIRXI();
	/* As per most tasks, this task is implemented within an infinite loop. */
	for( ;; )
	{
		/* ʹ���ź����ȴ�һ���¼����ź����ڵ���������֮ǰ��Ҳ��������ִ��֮ǰ���ѱ������������޳�
		ʱ���������Դ˺�������Ҳֻ���ڳɹ���ȡ�ź���֮��Ż᷵�ء��˴�Ҳû�б�Ҫ��ⷵ��ֵ */
		uiRestoreCount++;
		if(uiRestoreCount > 10000)
		{
			siTestCount = 0;	
		}
		xSemaphoreTake( xBinarySemaphore, portMAX_DELAY );     // �����ź����������������У���ת�������ȼ��������ϣ����������������̬��
		/* �������е�����ʱ���¼���Ȼ�Ѿ��������������¼�����ֻ�Ǽ򵥵ش�ӡ���һ����Ϣ */
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
	xSemaphoreGiveFromISR( xBinarySemaphore, &lHigherPriorityTaskWoken );     // ����Ӵ�����
    //lHigherPriorityTaskWoken = xFirstTimerHandler();
	
	if( lHigherPriorityTaskWoken == pdTRUE )
	{
		/* �����ź�����ʹ�õȴ����ź�������������������������������������ȼ����ڵ�ǰ���������
		�� �C ǿ�ƽ���һ�������л�����ȷ���ж�ֱ�ӷ��ص��������������(��ѡ������)��
		˵������ʵ��ʹ���У� ISR��ǿ���������л��ĺ������ھ�����ֲ���˴����õ��ǻ���Open Watcom DOS
		��ֲ�ĺꡣ����ƽ̨�µ���ֲ�����в�ͬ���﷨Ҫ�󡣶���ʵ��ʹ�õ�ƽ̨�����������Ӧ��ֲ�Դ���ʾ
		�������Ծ�����ȷ���﷨�ͷ��š�
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
	ICU.IER[0x03].BIT.IEN6 = 1;          //������CPU�����ж�
    //ICU.IPR[0x06].BYTE =0x0d;            //�ж����ȼ��趨
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
		/* �����ź�����ʹ�õȴ����ź�������������������������������������ȼ����ڵ�ǰ���������
		�� �C ǿ�ƽ���һ�������л�����ȷ���ж�ֱ�ӷ��ص��������������(��ѡ������)��
		˵������ʵ��ʹ���У� ISR��ǿ���������л��ĺ������ھ�����ֲ���˴����õ��ǻ���Open Watcom DOS
		��ֲ�ĺꡣ����ƽ̨�µ���ֲ�����в�ͬ���﷨Ҫ�󡣶���ʵ��ʹ�õ�ƽ̨�����������Ӧ��ֲ�Դ���ʾ
		�������Ծ�����ȷ���﷨�ͷ��š�
		*/
		portYIELD_FROM_ISR( vHigherPriorityTaskWoken );
	}
    
}