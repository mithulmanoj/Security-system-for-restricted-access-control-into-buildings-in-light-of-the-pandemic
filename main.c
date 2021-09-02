/******************************************************************************
* File Name: main.c
*******************************************************************************/

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "global.h"
#include "pirTask.h"
#include "thermistorTask.h"
#include "capsenseTask.h"
#include "uartTask.h"
#include "servoTask.h"

/* Global variable for Semaphore handle creation */
SemaphoreHandle_t switchSemaphore;

/* Global variable for queue handle creation */
QueueHandle_t servoQueue;

void handle_error(void)
{
     /* Disable all interrupts. */
    __disable_irq();

    CY_ASSERT(0);
}


int main(void)
{
	cy_rslt_t result;

	/* Set up the device based on configurator selections */
	result = cybsp_init();
	if (result != CY_RSLT_SUCCESS)
	{
	    handle_error();
	}

	result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,CY_RETARGET_IO_BAUDRATE);
	if (result != CY_RSLT_SUCCESS)
	{
	    handle_error();
	}

	/* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
	printf("\x1b[2J\x1b[;H");

	printf("************************************************************\r\n");
	printf(" Smart gate\r\n");
	printf("************************************************************\r\n\n");

	__enable_irq();

	/* Queue handle creation */
	servoQueue = xQueueCreate( 1, sizeof(unsigned int));

	/* Semaphore handle creation */
	switchSemaphore = xSemaphoreCreateBinary();

	xTaskCreate(pirTask, "PIR" , configMINIMAL_STACK_SIZE*1 ,0 /* args */ ,2 /* priority */, 0 /* handle */);
	xTaskCreate(thermistorTask, "thermistor" ,configMINIMAL_STACK_SIZE*4  , NULL, 1, 0);
	xTaskCreate(uartTask, "UART" ,configMINIMAL_STACK_SIZE*8  , NULL, 1, 0);
	xTaskCreate(capsenseTask, "capsense" ,configMINIMAL_STACK_SIZE*8  , NULL, 1, 0);
	xTaskCreate(servoTask, "servo" ,configMINIMAL_STACK_SIZE*8  , NULL, 1, 0);

	vTaskStartScheduler();

	while(1);
}

/* [] END OF FILE */









