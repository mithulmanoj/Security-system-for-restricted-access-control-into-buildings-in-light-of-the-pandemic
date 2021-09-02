/*
 * thermistorTask.c
 * Author: mithul
 */

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "global.h"
#include "mtb_thermistor_ntc_gpio.h"
#include <string.h>
#include "servoTask.h"
#include "thermistorTask.h"

/*
 * THERMISTOR PIN CONFIG MACROS - CY8CPROTO kit
 */
#define THERM_VDD   P10_3
#define THERM_GND   P10_0
#define THERM_OUT   P10_2

/* Initialization of thermistor global variables */
mtb_thermistor_ntc_gpio_t mtb_thermistor_obj;
cyhal_adc_t adc_obj;

//QueueHandle_t servoQueue;

/* Function prototypes */
float temperature(void);

/* Thermistor task definition */
void thermistorTask(void *params)
{
    (void)params;
    cy_rslt_t result ;

    float value;
    char message_buffer[20];

    /* Refer thermistor datasheet for the values */
    mtb_thermistor_ntc_gpio_cfg_t therm_gpio_cfg=
        {
        		.b_const = (float)(3380),         //refers to beta constant

    			/* Resistance of the thermistor is 10K at 25 degrees C (from datasheet)
    			 * Therefore R0 = 10000 Ohm, and T0 = 298.15 Kelvin, which gives
    			 * R_INFINITY = R0 e^(-B_CONSTANT / T0) = 0.1192855
    			 */
    			.r_infinity = (float)(0.1192855), //refers to resistance at infinity, ideally 0
    			.r_ref = (float)(10000)           //refres to reference resistance
        };


    /*
     * ADC and Thermistor initialization
     */
    result = cyhal_adc_init(&adc_obj, THERM_OUT, NULL);

    if (result != CY_RSLT_SUCCESS)
    {
        	CY_ASSERT(0);
    }

    result = mtb_thermistor_ntc_gpio_init(&mtb_thermistor_obj, &adc_obj, THERM_GND, THERM_VDD, THERM_OUT, &therm_gpio_cfg,MTB_THERMISTOR_NTC_WIRING_VIN_NTC_R_GND);

    if (result != CY_RSLT_SUCCESS)
    {
        	CY_ASSERT(0);
    }

    while(1)
    {
        xSemaphoreTake(switchSemaphore,portMAX_DELAY);
        value = temperature();
        servo_command_t servoComm = SERVO_ARM_RIGHT;

            if (value > 0u)
            {
                sprintf(message_buffer,"%2.2f",value);
                xQueueSend(servoQueue, &servoComm, portMAX_DELAY );
                xSemaphoreGive(switchSemaphore);
            }
            else
            {
                printf("Invalid values, Please check the thermistor\r\n\n");
                mtb_thermistor_ntc_gpio_free(&mtb_thermistor_obj);
                servoComm = SERVO_IDLE;
                xQueueSend(servoQueue, &servoComm, portMAX_DELAY );
                sprintf(message_buffer,"Invalid values");
                printf("%s", message_buffer);
            }
    }
}

float temperature(void)
{
	float value;
	float sum=0;
	uint8_t loop;
	for (loop = 0; loop < 10; loop++)
	{
		value = mtb_thermistor_ntc_gpio_get_temp(&mtb_thermistor_obj);
		sum=sum+value;
	}
	sum = sum/10;
	printf("Temperature is %2.2fC\r\n\n",sum);
	return sum;
}
