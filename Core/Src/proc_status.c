#include "proc_status.h"

extern UART_HandleTypeDef huart2;

void process_status(process_status_t status, Message_t *message, char *error_mes)
{
    char *data = (char *)message->data;

    switch (status)
    {
    case TRANSFER_ERROR:
        sprintf(data, "Transfer error (%s)\r\n", error_mes);
        break;
    case INCORRECT_INPUT:
        sprintf(data, "Incorrect input error (%s)\r\n", error_mes);
        break;
    case NO_FREE_TX_BUF:
    case NO_FULL_RX_BUF:
    case FAILED:
        sprintf(data, "Failed to perform operation (%s)\r\n", error_mes);
    default:
        return;
    }

    message->type = UART_SEND;
}