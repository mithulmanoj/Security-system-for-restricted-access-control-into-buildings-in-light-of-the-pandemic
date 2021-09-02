/*
 * servoTask.h
 * author: Mithul Manoj
 */

#ifndef SERVOTASK_H_
#define SERVOTASK_H_
#pragma once

void servoTask(void *params);

typedef enum
{
    SERVO_IDLE,
    SERVO_ARM_RIGHT,
} servo_command_t;

#endif /* SERVOTASK_H_ */
