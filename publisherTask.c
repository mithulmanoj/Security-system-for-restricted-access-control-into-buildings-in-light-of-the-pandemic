/*
 * publisherTask.c
 *
 *  Created on: 11-Aug-2021
 *      Author: mithu
 */

#include "cyhal.h"
#include "cybsp.h"
#include "FreeRTOS.h"

/* Task header files */
#include "publisherTask.h"
#include "mqttTask.h"

/* Configuration file for MQTT client */
#include "mqtt_client_config.h"

/* Middleware libraries */
#include "cy_retarget_io.h"
#include "iot_mqtt.h"

/******************************************************************************
* Macros
******************************************************************************/
/* Interrupt priority for User Button Input. */
#define USER_BTN_INTR_PRIORITY          (5)

/* The maximum number of times each PUBLISH in this example will be retried. */
#define PUBLISH_RETRY_LIMIT             (10)

/* A PUBLISH message is retried if no response is received within this
 * time (in milliseconds).
 */
#define PUBLISH_RETRY_MS                (1000)

/******************************************************************************
* Global Variables
*******************************************************************************/
/* FreeRTOS task handle for this task. */
TaskHandle_t publisher_task_handle;

/* Structure to store publish message information. */
IotMqttPublishInfo_t publishInfo =
{
    .qos = (IotMqttQos_t) MQTT_MESSAGES_QOS,
    .pTopicName = MQTT_TOPIC,
    .topicNameLength = (sizeof(MQTT_TOPIC) - 1),
    .retryMs = PUBLISH_RETRY_MS,
    .retryLimit = PUBLISH_RETRY_LIMIT
};

/******************************************************************************
* Function Prototypes
*******************************************************************************/
void isr_button_press(void *callback_arg, cyhal_gpio_event_t event);

/******************************************************************************
 * Function Name: publisher_task
 ******************************************************************************
 * Summary:
 *  Task that handles initialization of the user button GPIO, configuration of
 *  ISR, and publishing of MQTT messages to control the device that is actuated
 *  by the subscriber task.
 *
 * Parameters:
 *  void *pvParameters : Task parameter defined during task creation (unused)
 *
 * Return:
 *  void
 *
 ******************************************************************************/
void publisherTask(void *pvParameters)
{
    /* Status variable */
    int result;

    /* Variable to receive new device state from the user button ISR. */
    uint32_t publish_device_state;

    /* Status of MQTT publish operation that will be communicated to MQTT
     * client task using a message queue in case of failure during publish.
     */
    mqtt_result_t mqtt_publish_status = MQTT_PUBLISH_FAILURE;

    /* To avoid compiler warnings */
    (void)pvParameters;

    /* Initialize the user button GPIO and register interrupt on falling edge. */
    cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT,
                    CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);
    cyhal_gpio_register_callback(CYBSP_USER_BTN, isr_button_press, NULL);
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL,
                            USER_BTN_INTR_PRIORITY, true);

    printf("Press the user button (SW2) to publish \"%s\"/\"%s\" on the topic '%s'...\n\n",
           MQTT_DEVICE_ON_MESSAGE, MQTT_DEVICE_OFF_MESSAGE, publishInfo.pTopicName);

    while (true)
    {
        /* Wait for notification from the User Button ISR. */
        xTaskNotifyWait(0, 0, &publish_device_state, portMAX_DELAY);

        /* Assign the publish message payload according to received device state. */
        publishInfo.pPayload = MQTT_DEVICE_OFF_MESSAGE;

        if (publish_device_state == DEVICE_ON_STATE)
        {
            publishInfo.pPayload = MQTT_DEVICE_ON_MESSAGE;
        }

        publishInfo.payloadLength = strlen(publishInfo.pPayload);

        printf("Publishing '%s' on the topic '%s'\n\n",
               (char *)publishInfo.pPayload,
               publishInfo.pTopicName);

        /* Publish the MQTT message with the configured settings. */
        result = IotMqtt_PublishSync(mqttConnection,
                                     &publishInfo,
                                     0,
                                     MQTT_TIMEOUT_MS);

        if (result != IOT_MQTT_SUCCESS)
        {
            /* Inform the MQTT client task about the publish failure and suspend
             * the task for it to be killed by the MQTT client task later.
             */
            printf("MQTT Publish failed with error '%s'.\n\n",
                   IotMqtt_strerror((IotMqttError_t) result));
            xQueueOverwrite(mqtt_status_q, &mqtt_publish_status);
            vTaskSuspend( NULL );
        }
    }
}

/******************************************************************************
 * Function Name: isr_button_press
 ******************************************************************************
 * Summary:
 *  GPIO interrupt service routine. This function detects button
 *  presses and sends task notifications to the publisher task about the new
 *  device state that needs to be published.
 *
 * Parameters:
 *  void *callback_arg : pointer to variable passed to the ISR (unused)
 *  cyhal_gpio_event_t event : GPIO event type (unused)
 *
 * Return:
 *  void
 *
 ******************************************************************************/
void isr_button_press(void *callback_arg, cyhal_gpio_event_t event)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t new_device_state;

    /* To avoid compiler warnings */
    (void) callback_arg;
    (void) event;

    /* Toggle the device state. */
    if (current_device_state == DEVICE_ON_STATE)
    {
        new_device_state = DEVICE_OFF_STATE;
    }
    else
    {
        new_device_state = DEVICE_ON_STATE;
    }

    /* Notify the publisher task about the new state to be published. */
    xTaskNotifyFromISR(publisher_task_handle, new_device_state, eSetValueWithoutOverwrite,
                       &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/******************************************************************************
 * Function Name: publisher_cleanup
 ******************************************************************************
 * Summary:
 *  Cleanup function for the publisher task that disables the user button
 *  interrupt. This operation needs to be necessarily performed before deleting
 *  the publisher task.
 *
 * Parameters:
 *  void
 *
 * Return:
 *  void
 *
 ******************************************************************************/
void publisher_cleanup(void)
{
    /* Deregister the ISR and disable the interrupt on the user button. */
    cyhal_gpio_register_callback(CYBSP_USER_BTN, NULL, NULL);
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL,
                            USER_BTN_INTR_PRIORITY, false);
}

/* [] END OF FILE */

