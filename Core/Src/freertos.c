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
  LED_BLINK_TWICE,
  ECHO_REQEST,
  ERROR_MESSAGE,
  BUTTON_PRESSED
} Message_type;

typedef struct Message_ptr
{
  Message_type type;
  char data[128];
} Message;

QueueHandle_t qHandle;

extern UART_HandleTypeDef huart2;

void mesGenTask(void *pvParameters);
void mesHandleTask(void *pvParameters);
void infty(void *pvParameters);

void freertos_init()
{
  TaskHandle_t xHandle;
  TaskHandle_t xHandle1;
  xTaskCreate(mesGenTask, "Uart tx task", TASK_BUFFER_SIZE_IN_WORDS, 0, configMAX_PRIORITIES - 2, &xHandle);
  //xTaskCreate(infty, "", TASK_BUFFER_SIZE_IN_WORDS, 0, configMAX_PRIORITIES - 1, &xHandle);
  xTaskCreate(mesHandleTask, "Uart rx task", TASK_BUFFER_SIZE_IN_WORDS, 0, configMAX_PRIORITIES - 1, &xHandle1);

  qHandle = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);

  if (qHandle == NULL)
  {
    HAL_UART_Transmit_DMA(&huart2, "Queue cannot be created", 24);
    
    return;
  }

  vTaskStartScheduler();
}

/*
void infty(void *pvParameters) {

  for (;;) {}
}
*/

void mesGenTask(void *pvParameters)
{
  Message mes;

  int c = 0;

  while (1)
  {
#if DEBUG_NEEDED
    LED_ON();
#endif

    sprintf(mes.data, "Hello\r\n");
    if (c % 2 == 0)
      mes.type = ECHO_REQEST;
    else
      mes.type = LED_BLINK_TWICE;

    xQueueSend(qHandle, &mes, portMAX_DELAY);

    c++;

#if DEBUG_NEEDED
    LED_OFF();
#endif

    vTaskDelay(pdMS_TO_TICKS(500));
  }

  vTaskDelete(NULL);
}

void mesHandleTask(void *pvParameters)
{
  Message mes;

  while (1)
  {
    xQueueReceive(qHandle, &mes, portMAX_DELAY);

    switch (mes.type)
    {
    case LED_BLINK_TWICE:
      LED_BLINK();
      LED_BLINK();
      break;
    case BUTTON_PRESSED:
    case ECHO_REQEST:
    case ERROR_MESSAGE:
      HAL_UART_Transmit_DMA(&huart2, (uint8_t *)mes.data, strlen(mes.data));
      break;
    }

    //vTaskDelay(pdMS_TO_TICKS(0));
  }

  vTaskDelete(NULL);
}



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  static Message glob_mes;
  static int button_pressed_counter = 0;

  // no tasks have yet been unblocked
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  glob_mes.type = BUTTON_PRESSED;
  sprintf(glob_mes.data, "Button pressed %d\r\n", ++button_pressed_counter);
  xQueueSendFromISR(qHandle, &glob_mes, &xHigherPriorityTaskWoken);

  // if any of tasks unblocked after our actions, when we say to switch context before next tick
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

