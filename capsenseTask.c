/*
 * capsenseTask.c
 *
 *  Created on: 29-Jul-2021
 *      Author: mithul
 */

#include "cybsp.h"
#include "cyhal.h"
#include "cycfg.h"
#include "cycfg_capsense.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "global.h"

/* Function prototypes */
static void capsense_init(void);
static void process_touch(void);
static void capsense_isr(void);
static void capsense_end_of_scan_callback(cy_stc_active_scan_sns_t* active_scan_sns_ptr);
static void capsense_register_deepsleep(void);

/* Capsense task definition */
void capsenseTask(void* params)
{
    (void)params;

    //capsense_done = xQueueCreateCountingSemaphore(1,0);

    capsense_init();

    Cy_CapSense_ScanAllWidgets(&cy_capsense_context);

    for(;;)
    {
        xSemaphoreTake(switchSemaphore,portMAX_DELAY);
        Cy_CapSense_ProcessAllWidgets(&cy_capsense_context);
        process_touch();
        Cy_CapSense_ScanAllWidgets(&cy_capsense_context);
        vTaskDelay(50); // ~20hz update
        xSemaphoreGive(switchSemaphore);
    }
}

/* Function to process button 0 */
static void process_touch(void)
{
    /* Variables used to store previous touch information */
    static uint32_t button0_status_prev = 0;

    uint32_t button0_status = 0;
    //cy_stc_capsense_touch_t *slider_touch;

    button0_status = Cy_CapSense_IsWidgetActive(CY_CAPSENSE_BUTTON0_WDGT_ID, &cy_capsense_context);

    if((0u != button0_status) && (0u == button0_status_prev))
    {
        printf("Button 0 pressed\r\n");
    }

    button0_status_prev = button0_status;
}


static void capsense_init(void)
{
    Cy_CapSense_Init(&cy_capsense_context);

    static const cy_stc_sysint_t capSense_intr_config =
    {
        .intrSrc = csd_interrupt_IRQn,
        .intrPriority = 7,
    };

    /* Initialize CapSense interrupt */
    Cy_SysInt_Init(&capSense_intr_config, &capsense_isr);
    NVIC_ClearPendingIRQ(capSense_intr_config.intrSrc);
    NVIC_EnableIRQ(capSense_intr_config.intrSrc);

    capsense_register_deepsleep();
    Cy_CapSense_RegisterCallback(CY_CAPSENSE_END_OF_SCAN_E, capsense_end_of_scan_callback, &cy_capsense_context);
    Cy_CapSense_Enable(&cy_capsense_context);
}



/* This function sends a command to CapSense task to process scan */
static void capsense_end_of_scan_callback(cy_stc_active_scan_sns_t* active_scan_sns_ptr)
{
    BaseType_t xYieldRequired;

    (void)active_scan_sns_ptr;
    xYieldRequired = xSemaphoreGiveFromISR(switchSemaphore,&xYieldRequired);

    portYIELD_FROM_ISR(xYieldRequired);
}


/* Wrapper function for handling interrupts from CSD block */
static void capsense_isr(void)
{
    Cy_CapSense_InterruptHandler(CYBSP_CSD_HW, &cy_capsense_context);
}

/* Wrapper function for setting up the Deep Sleep callback for CapSense */
static void capsense_register_deepsleep(void)
{
    static cy_stc_syspm_callback_params_t callback_params =
    {
        .base       = CYBSP_CSD_HW,
        .context    = &cy_capsense_context
    };

    static cy_stc_syspm_callback_t capsense_deep_sleep_cb =
    {
        Cy_CapSense_DeepSleepCallback,
        CY_SYSPM_DEEPSLEEP,
        0,
        &callback_params,
        NULL,
        NULL
    };

    Cy_SysPm_RegisterCallback(&capsense_deep_sleep_cb);
}
