#ifndef PROC_STATUS_H_
#define PROC_STATUS_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>

typedef enum
{
    OK,
    INCORRECT_INPUT,
    TRANSFER_ERROR,
    NO_FREE_TX_BUF,
    NO_FULL_RX_BUF,
    FAILED
} process_status_t;

void process_status(process_status_t status, char *message);

#endif // PROC_STATUS_H_