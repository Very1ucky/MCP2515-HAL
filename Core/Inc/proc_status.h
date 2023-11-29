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

typedef enum Message_type
{
  CAN_SEND,
  UART_SEND
} Message_type_t;

typedef struct Message
{
  Message_type_t type;
  uint8_t data[128];
} Message_t;

void process_status(process_status_t status, Message_t *message, char *error_mes);
void process_status_and_transmite(process_status_t status, char *error_mes);

#endif // PROC_STATUS_H_