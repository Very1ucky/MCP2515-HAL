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
#define QUEUE_ITEM_SIZE sizeof(Message)

#define DEBUG_NEEDED 0

#define LED_ON() GPIOA->BSRR = 1 << 5
#define LED_OFF() GPIOA->BSRR = 1 << (5 + 16)
#define LED_BLINK()               \
  LED_OFF();                      \
  vTaskDelay(pdMS_TO_TICKS(100)); \
  LED_ON();                       \
  vTaskDelay(pdMS_TO_TICKS(100))

typedef enum Message_type_ptr
{
  SENSOR_DATA,

} Message_type;

typedef struct Message_ptr
{
  Message_type type;
  char data[128];
} Message;

static char tstr[128];

QueueHandle_t qUartHandle;
QueueHandle_t qCanHandle;

extern UART_HandleTypeDef huart2;

void mesGenTask(void *pvParameters);
void acelReadTask(void *pvParameters);
void infty(void *pvParameters);

void freertos_init()
{
  TaskHandle_t xHandle;
  TaskHandle_t xHandle1;
  // xTaskCreate(mesGenTask, "Uart tx task", TASK_BUFFER_SIZE_IN_WORDS, 0, configMAX_PRIORITIES - 2, &xHandle);
  // xTaskCreate(infty, "", TASK_BUFFER_SIZE_IN_WORDS, 0, configMAX_PRIORITIES - 1, &xHandle);
  xTaskCreate(acelReadTask, "Uart rx task", TASK_BUFFER_SIZE_IN_WORDS, 0, configMAX_PRIORITIES - 1, &xHandle1);

  qUartHandle = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
  qCanHandle = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);

  vTaskStartScheduler();
}


void mesSendTask(void *pvParameters)
{
  Message mes;

  while (true)
  {
    xQueueReceive(qUartHandle, &mes, portMAX_DELAY);

    switch (mes.type)
    {
    case SENSOR_DATA:
      sprintf(mes.data, "Sensor: %s", mes.data);
      break;
    default:
      break;
    }

    HAL_UART_Transmit_DMA(&huart2, mes.data, strlen(mes.data));

    vTaskDelay(pdMS_TO_TICKS(50));
  }

  vTaskDelete(NULL);
}

void acelReadTask(void *pvParameters)
{
  static acel_data_t acel_data;
  static process_status_t status;
  static Message mes;

  mes.type = SENSOR_DATA;
  while (true)
  {
    status = read_velocity(&acel_data);
    process_status(status, "read velocity");
    if (!status)
    {
      sprintf(mes.data, "(acelerometr)X=%d, Y=%d, Z=%d\r\n", acel_data.X, acel_data.Y, acel_data.Z);
      xQueueSend(qUartHandle, &mes, portMAX_DELAY);
    }

    vTaskDelay(pdMS_TO_TICKS(200));
  }

  vTaskDelete(NULL);
}

void can_read_task(void *pvParameters)
{

}

void can_send_task(void *pvParameters)
{
  
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_13)
  {
    LIS331DLN_calibrate();
  }

  if (GPIO_Pin == INT_PIN)
  {
    uint8_t canintf_status;
    mcp2515_read_byte(MCP2515_CANINTF_ADDR, &canintf_status, RX_CAN_SLAVE);

    if (canintf_status&0x3)
    {

    }

    if (canintf_status&0x80)
    {

    }
  }
}
