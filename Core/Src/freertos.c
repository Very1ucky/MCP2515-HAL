#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

#include "queue.h"
#include "freertos_init.h"
#include <string.h>
#include <stdio.h>

#define TASK_BUFFER_SIZE_IN_WORDS configMINIMAL_STACK_SIZE
#define TASK_BUFFER_SIZE_IN_BYTES configMINIMAL_STACK_SIZE * 4

#define QUEUE_LENGTH 10
#define QUEUE_ITEM_SIZE sizeof(Message_t)

QueueHandle_t qUartHandle;

TaskHandle_t acelReadHandle;
TaskHandle_t uartTxHandle;
TaskHandle_t canRxHandle;

extern UART_HandleTypeDef huart2;

void acelReadTask(void *pvParameters);
void mesSendTask(void *pvParameters);
void canReadTask(void *pvParameters);
void gyroReadTask(void *pvParameters);

void freertos_init()
{

  // xTaskCreate(mesGenTask, "Uart tx task", TASK_BUFFER_SIZE_IN_WORDS, 0, configMAX_PRIORITIES - 2, &xHandle);
  // xTaskCreate(infty, "", TASK_BUFFER_SIZE_IN_WORDS, 0, configMAX_PRIORITIES - 1, &xHandle);
  xTaskCreate(gyroReadTask, "Uart rx task", TASK_BUFFER_SIZE_IN_WORDS*4, 0, configMAX_PRIORITIES - 1, &acelReadHandle);
  xTaskCreate(acelReadTask, "Uart rx task", TASK_BUFFER_SIZE_IN_WORDS*4, 0, configMAX_PRIORITIES - 1, &acelReadHandle);
  xTaskCreate(mesSendTask, "Uart tx task", TASK_BUFFER_SIZE_IN_WORDS * 4, 0, configMAX_PRIORITIES - 1, &uartTxHandle);
  xTaskCreate(canReadTask, "Uart rx task", TASK_BUFFER_SIZE_IN_WORDS*4, 0, configMAX_PRIORITIES - 1, &canRxHandle);

  qUartHandle = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
  // qCanHandle = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);

  vTaskStartScheduler();
}

void mesSendTask(void *pvParameters)
{
  Message_t mes;
  Message_t err_mes;

  process_status_t status;

  while (true)
  {
    xQueueReceive(qUartHandle, &mes, portMAX_DELAY);

    switch (mes.type)
    {
    case CAN_SEND:
      status = send_can_frame((can_frame_t *)mes.data);

      process_status(status, &err_mes, "send can frame");
      if (status)
      {
        HAL_UART_Transmit_DMA(&huart2, err_mes.data, strlen((char *)err_mes.data));
      }
      break;
    case UART_SEND:
      HAL_UART_Transmit_DMA(&huart2, mes.data, strlen((char *)mes.data));
      break;
    default:
      break;
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }

  vTaskDelete(NULL);
}

void acelReadTask(void *pvParameters)
{
  static axes_sensor_data_t axes_data;
  static process_status_t status;
  static Message_t mes;
  static can_frame_t *frame = (void *)mes.data;

  frame->type = ST_DATA;
  frame->st_id = 0x10;
  frame->dlc = 7;
  frame->data[6] = LIS331DLN_ADDR;

  while (true)
  {
    status = read_axes_sensor_data(LIS331DLN_ADDR,&axes_data);
    process_status(status, &mes, "read velocity");
    if (!status)
    {
      mes.type = CAN_SEND;
      for (uint8_t pos = 0; pos < sizeof(axes_data); ++pos)
      {
        frame->data[pos] = ((uint8_t *)&axes_data)[pos];
      }
    }

    xQueueSend(qUartHandle, &mes, 10);

    vTaskDelay(ACCEL_READ_PERIOD);
  }

  vTaskDelete(NULL);
}

void gyroReadTask(void *pvParameters)
{
  static axes_sensor_data_t axes_data;
  static process_status_t status;
  static Message_t mes;
  static can_frame_t *frame = (void *)mes.data;

  frame->type = ST_DATA;
  frame->st_id = 0x10;
  frame->dlc = 7;
  frame->data[6] = L3G4200D_ADDR;

  while (true)
  {
    status = read_axes_sensor_data(L3G4200D_ADDR,&axes_data);
    process_status(status, &mes, "read velocity");
    if (!status)
    {
      mes.type = CAN_SEND;
      for (uint8_t pos = 0; pos < sizeof(axes_data); ++pos)
      {
        frame->data[pos] = ((uint8_t *)&axes_data)[pos];
      }
    }

    xQueueSend(qUartHandle, &mes, 10);

    vTaskDelay(GYRO_READ_PERIOD);
  }

  vTaskDelete(NULL);
}

uint8_t full_buf_num = 0;
void canReadTask(void *pvParameters)
{
  static Message_t mes;
  static can_frame_t frame;
  static axes_sensor_data_t *axes_data = (void *)frame.data;

  mes.type = UART_SEND;
  while (true)
  {
    if (!ulTaskNotifyTake(pdTRUE, CAN_MAX_WAIT_TIME))
    {
      uint8_t err_flag = 0;
      mcp2515_read_byte(MCP2515_ERROR_FLG, &err_flag, TX_CAN_SLAVE);
      sprintf((char *)mes.data, "Can read timeout (err flag - %d)\r\n", err_flag);
    }
    else
    {
      recieve_can_frame(&frame, full_buf_num);
      switch (frame.data[6])
      {
      case LIS331DLN_ADDR:
        float xg = (float)axes_data->X*2/16384;
        float yg = (float)axes_data->Y*2/16384;
        float zg = (float)axes_data->Z*2/16384;
        sprintf((char *)mes.data, "(accelerometr)X=%.2fg, Y=%.2fg, Z=%.2fg\r\n", xg, yg, zg);
        break;
      case L3G4200D_ADDR:
        float xdps = (float)axes_data->X*250/16384;
        float ydps = (float)axes_data->Y*250/16384;
        float zdps = (float)axes_data->Z*250/16384;
        sprintf((char *)mes.data, "(gyroscope)X=%.2fdps, Y=%.2fdps, Z=%.2fdps\r\n", xdps, ydps, zdps);
        break;
      
      default:
        break;
      }
      
    }

    xQueueSend(qUartHandle, &mes, 10);

    vTaskDelay(pdMS_TO_TICKS(0));
  }

  vTaskDelete(NULL);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  static Message_t mes;

  if (GPIO_Pin == INT_PIN)
  {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    uint8_t canintf_status = 1;
    mcp2515_read_byte(MCP2515_CANINTF_ADDR, &canintf_status, RX_CAN_SLAVE);

    // one of rx buffers full
    if (canintf_status & 0x03)
    {
      if (canintf_status & 0x01)
      {
        full_buf_num = 1;
      }
      else
      {
        full_buf_num = 2;
      }

      mcp2515_bit_modify(MCP2515_CANINTF_ADDR, 0x03, 0x00, RX_CAN_SLAVE);
      vTaskNotifyGiveFromISR(canRxHandle, &xHigherPriorityTaskWoken);
    }

    // message error
    if (canintf_status & 0x80)
    {
      mes.type = UART_SEND;
      sprintf((char *)mes.data, "Can message error\r\n");
      mcp2515_bit_modify(MCP2515_CANINTF_ADDR, 0x80, 0x00, RX_CAN_SLAVE);
      xQueueSendFromISR(qUartHandle, &mes, &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}
