/*
 * thermistorTask.h
 *
 *  Created on: 31-Jul-2021
 *      Author: mithu
 */

#ifndef THERMISTORTASK_H_
#define THERMISTORTASK_H_
#pragma once

extern QueueHandle_t servoQueue;

void thermistorTask(void *params);

#endif /* THERMISTORTASK_H_ */
