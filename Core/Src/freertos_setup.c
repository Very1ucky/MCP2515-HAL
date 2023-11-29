#include "freertos_init.h"

extern void SysTick_Handler(void);

extern void xPortSysTickHandler(void);

void SysTick_Handler(void)
{

  SysTick->CTRL;

  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {

    xPortSysTickHandler();
  }
}

/* Callback function prototypes */
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void vApplicationDaemonTaskStartupHook(void);
extern void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName);

/**
  Dummy implementation of the callback function vApplicationIdleHook().
*/
#if (configUSE_IDLE_HOOK == 1)
__WEAK void vApplicationIdleHook(void) {}
#endif

/**
  Dummy implementation of the callback function vApplicationTickHook().
*/
#if (configUSE_TICK_HOOK == 1)
__WEAK void vApplicationTickHook(void) {}
#endif

/**
  Dummy implementation of the callback function vApplicationMallocFailedHook().
*/
#if (configUSE_MALLOC_FAILED_HOOK == 1)
__WEAK void vApplicationMallocFailedHook(void) {}
#endif

/**
  Dummy implementation of the callback function vApplicationDaemonTaskStartupHook().
*/
#if (configUSE_DAEMON_TASK_STARTUP_HOOK == 1)
__WEAK void vApplicationDaemonTaskStartupHook(void) {}
#endif

/**
  Dummy implementation of the callback function vApplicationStackOverflowHook().
*/
#if (configCHECK_FOR_STACK_OVERFLOW > 0)
__WEAK void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
{
  (void)xTask;
  (void)pcTaskName;
  configASSERT(0);
}
#endif

/*---------------------------------------------------------------------------*/
#if (configSUPPORT_STATIC_ALLOCATION == 1)
/* External Idle and Timer task static memory allocation functions */
extern void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);
extern void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize);

/*
  vApplicationGetIdleTaskMemory gets called when configSUPPORT_STATIC_ALLOCATION
  equals to 1 and is required for static memory allocation support.
*/
__WEAK void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
  /* Idle task control block and stack */
  static StaticTask_t Idle_TCB;
  static StackType_t Idle_Stack[configMINIMAL_STACK_SIZE];

  *ppxIdleTaskTCBBuffer = &Idle_TCB;
  *ppxIdleTaskStackBuffer = &Idle_Stack[0];
  *pulIdleTaskStackSize = (uint32_t)configMINIMAL_STACK_SIZE;
}

/*
  vApplicationGetTimerTaskMemory gets called when configSUPPORT_STATIC_ALLOCATION
  equals to 1 and is required for static memory allocation support.
*/
__WEAK void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
  /* Timer task control block and stack */
  static StaticTask_t Timer_TCB;
  static StackType_t Timer_Stack[configTIMER_TASK_STACK_DEPTH];

  *ppxTimerTaskTCBBuffer = &Timer_TCB;
  *ppxTimerTaskStackBuffer = &Timer_Stack[0];
  *pulTimerTaskStackSize = (uint32_t)configTIMER_TASK_STACK_DEPTH;
}
#endif