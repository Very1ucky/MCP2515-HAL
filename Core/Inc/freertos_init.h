#ifndef FREERTOS_INIT_H_
#define FREERTOS_INIT_H_

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

#include "queue.h"
#include "freertos_init.h"
#include <string.h>
#include <stdio.h>
#include "accel_amperka.h"
#include "can.h"
#include "proc_status.h"

#define ACCEL_READ_PERIOD pdMS_TO_TICKS(200)
#define GYRO_READ_PERIOD pdMS_TO_TICKS(400)
#define CAN_MAX_WAIT_TIME ACCEL_READ_PERIOD*4

void freertos_init();

#endif // FREERTOS_INIT_H_