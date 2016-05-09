#include "FreeRTOS.h"
void prvHITTestTask(void *pvParameters)
{
	int i = 0;
	for(;;)
	{
		i++;
		vTaskDelay( 250 / portTICK_RATE_MS );	
	}
		
}
