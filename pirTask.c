/*
 * pirTask.c
 * author: Mithul Manoj
 */

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "global.h"

#define GPIO_INTERRUPT_PRIORITY    1

/* Function prototypes */
static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_irq_event_t event);

/* Task definition */
void pirTask(void *params)
{
	(void)params;

	cy_rslt_t result;

	/* Initialize the user LED */
	result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

	/* Initialize pin P12_0 as an input */
	result = cyhal_gpio_init(P12_0, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_NONE, false);

	/* Configure GPIO interrupt for PIR sensor*/
	cyhal_gpio_register_callback(P12_0, gpio_interrupt_handler, NULL);

	cyhal_gpio_enable_event(P12_0, CYHAL_GPIO_IRQ_RISE, GPIO_INTERRUPT_PRIORITY, true);

	for(;;)
	{
		xSemaphoreTake(switchSemaphore,portMAX_DELAY);

		printf("Motion detected!\r\n");

		/* LED glows to indicate that motion has been detected */
	    cyhal_gpio_write(CYBSP_USER_LED,true);
	    cyhal_system_delay_ms(3000);
	    cyhal_gpio_write(CYBSP_USER_LED,false);

	    /* Release semaphore */
	    xSemaphoreGive(switchSemaphore);
	}
}

static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_irq_event_t event)
{

	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(switchSemaphore,&xHigherPriorityTaskWoken); // Tell the UART thread to process the RX FIFO
	if(xHigherPriorityTaskWoken != pdFALSE)
	{
	    portYIELD_FROM_ISR( xHigherPriorityTaskWoken);
	}
}



