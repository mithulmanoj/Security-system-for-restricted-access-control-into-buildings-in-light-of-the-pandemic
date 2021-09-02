/*
 * servoTask.c
 * author: Mithul Manoj
 */

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "global.h"
#include "servoTask.h"

#define PWM_FREQUENCY (50u)
#define PWM_DUTY_LEFT (2.4f)
#define PWM_DUTY_IDLE (4.5f)
#define PWM_DUTY_RIGHT (7.8f)

/* Function prototypes */
void set_servo_position(float value);

/* PWM object */
cyhal_pwm_t pwm_servo_control;

void servoTask(void *params)
{
	(void)params;
	cy_rslt_t result;
	servo_command_t servoComm;

    /* Initialize the PWM */
	result = cyhal_pwm_init(&pwm_servo_control, P12_0, NULL);
	if(CY_RSLT_SUCCESS != result)
	{
	  CY_ASSERT(false);
	}
	set_servo_position(PWM_DUTY_IDLE);
    /* Start the PWM */
    result = cyhal_pwm_start(&pwm_servo_control);

    for(;;)
    {
    	xSemaphoreTake(switchSemaphore,portMAX_DELAY);

    	xQueueReceive(servoQueue, &servoComm, 0 );

        switch(servoComm)
        {
            /* Gate not opened */
            case SERVO_IDLE:
            {
        	    set_servo_position(PWM_DUTY_IDLE);
        	    break;
        	}

        	/* Open gate to the right */
        	case SERVO_ARM_RIGHT:
        	{
        		set_servo_position(PWM_DUTY_RIGHT);
        		cyhal_system_delay_ms(2000);
        		set_servo_position(PWM_DUTY_IDLE);
        		break;
        	}
        	/* Invalid command */
        	default:
        	{
        		/* Handle invalid command here */
        		printf("System error");
        	    break;
        	}
        }

    }
}

void set_servo_position(float value){

    float previous_value = 0;
    float i =0;
    /* API return code */
    cy_rslt_t result;

    if(value > previous_value)
    {

        for(i=previous_value;i<=value;i=i+0.1)
        {

        /* Set the PWM output frequency and duty cycle */
          result = cyhal_pwm_set_duty_cycle(&pwm_servo_control,  i, PWM_FREQUENCY);
          if(CY_RSLT_SUCCESS != result)
          {
              CY_ASSERT(false);
          }

          cyhal_system_delay_ms(10);
    	}

    }
    else
    {
        for(i=previous_value;i>=value;i=i-0.1){

    	    /* Set the PWM output frequency and duty cycle */
    	    result = cyhal_pwm_set_duty_cycle(&pwm_servo_control,  i, PWM_FREQUENCY);
    	    if(CY_RSLT_SUCCESS != result)
    	    {
    	        CY_ASSERT(false);
    	    }

    	      cyhal_system_delay_ms(10);
    		}
    	}
          previous_value = value;
    }

