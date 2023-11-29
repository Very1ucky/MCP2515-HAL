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

void freertos_init();

#endif // FREERTOS_INIT_H_