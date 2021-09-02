/*
 * global.h
 * author: Mithul Manoj
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "FreeRTOS.h"
#include "semphr.h"

extern QueueHandle_t servoQueue;
extern SemaphoreHandle_t switchSemaphore;

#endif /* GLOBAL_H_ */
