/*
 * publisherTask.h
 *
 *  Created on: 11-Aug-2021
 *      Author: mithu
 */

#ifndef PUBLISHERTASK_H_
#define PUBLISHERTASK_H_

#include "FreeRTOS.h"
#include "task.h"

/*******************************************************************************
* Macros
********************************************************************************/
/* Task parameters for Button Task. */
#define PUBLISHER_TASK_PRIORITY               (2)
#define PUBLISHER_TASK_STACK_SIZE             (1024 * 1)

/*******************************************************************************
* Extern Variables
********************************************************************************/
extern TaskHandle_t publisher_task_handle;

/*******************************************************************************
* Function Prototypes
********************************************************************************/
void publisher_task(void *pvParameters);
void publisher_cleanup(void);

#endif /* PUBLISHERTASK_H_ */

/* [] END OF FILE */
