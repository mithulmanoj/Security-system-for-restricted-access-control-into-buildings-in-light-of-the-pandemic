/*
 * mqttTask.h
 *
 *  Created on: 11-Aug-2021
 *      Author: mithu
 */

#ifndef MQTTTASK_H_
#define MQTTTASK_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "iot_mqtt.h"

/*******************************************************************************
* Macros
********************************************************************************/
/* Task parameters for MQTT Client Task. */
#define MQTT_CLIENT_TASK_PRIORITY       (2)
#define MQTT_CLIENT_TASK_STACK_SIZE     (1024 * 2)

/*******************************************************************************
* Global Variables
********************************************************************************/
/* Data-type of various MQTT operation results. */
typedef enum
{
    MQTT_SUBSCRIBE_SUCCESS,
    MQTT_SUBSCRIBE_FAILURE,
    MQTT_PUBLISH_SUCCESS,
    MQTT_PUBLISH_FAILURE,
    MQTT_DISCONNECT
} mqtt_result_t;

/*******************************************************************************
 * Extern variables
 ******************************************************************************/
extern IotMqttConnection_t mqttConnection;
extern QueueHandle_t mqtt_status_q;

/*******************************************************************************
* Function Prototypes
********************************************************************************/
void mqtt_client_task(void *pvParameters);

#endif /* MQTTTASK_H_ */

/* [] END OF FILE */
