#include "proc_status.h"

extern UART_HandleTypeDef huart2;

static char tstr[128];

void process_status(process_status_t status, char *message)
{
    switch (status)
    {
    case TRANSFER_ERROR:
        sprintf(tstr, "Transfer error (%s)\r\n", message);
        break;
    case INCORRECT_INPUT:
        sprintf(tstr, "Incorrect input error (%s)\r\n", message);
        break;
    case NO_FREE_TX_BUF:
    case NO_FULL_RX_BUF:
    case FAILED:
        sprintf(tstr, "Failed to perform operation (%s)\r\n", message);
    default:
        return;
    }

    HAL_UART_Transmit_DMA(&huart2, tstr, strlen(tstr));
}