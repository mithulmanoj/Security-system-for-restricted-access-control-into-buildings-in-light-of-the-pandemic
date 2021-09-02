/*
 * uartTask.c
 * author: Mithul Manoj
 */

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "global.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

uint8_t uart_read_value;

void uartTask(void *params)
{
	for (;;)
	{
		xSemaphoreTake(switchSemaphore,portMAX_DELAY);
		/* Check if passkey was entered */
	    if (cyhal_uart_getc(&cy_retarget_io_uart_obj, &uart_read_value, 1) == CY_RSLT_SUCCESS)
	    {
	        if (uart_read_value == 'M')
	        {
	        	printf("Passkey entered has been successfully matched!\r\n");
	        	xSemaphoreGive(switchSemaphore);

	        }
	        else /* Resume LED blinking by starting the timer */
	        {
	            printf("Wrong passkey entered - Aborting\r\n");
	        }
        }
	}
}

