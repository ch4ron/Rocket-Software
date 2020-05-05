#include "FreeRTOS.h"
#include "stm32f4xx_hal.h"

/* External Idle and Timer task static memory allocation functions */
extern void vApplicationGetIdleTaskMemory  (StaticTask_t **ppxIdleTaskTCBBuffer,  StackType_t **ppxIdleTaskStackBuffer,  uint32_t *pulIdleTaskStackSize);
extern void vApplicationGetTimerTaskMemory (StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize);

/* Idle task control block and stack */
static StaticTask_t Idle_TCB;
static StackType_t  Idle_Stack[configMINIMAL_STACK_SIZE];

/* Timer task control block and stack */
static StaticTask_t Timer_TCB;
static StackType_t  Timer_Stack[configTIMER_TASK_STACK_DEPTH];

/*
  vApplicationGetIdleTaskMemory gets called when configSUPPORT_STATIC_ALLOCATION
  equals to 1 and is required for static memory allocation support.
*/
void vApplicationGetIdleTaskMemory (StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
  *ppxIdleTaskTCBBuffer   = &Idle_TCB;
  *ppxIdleTaskStackBuffer = &Idle_Stack[0];
  *pulIdleTaskStackSize   = (uint32_t)configMINIMAL_STACK_SIZE;
}

/*
  vApplicationGetTimerTaskMemory gets called when configSUPPORT_STATIC_ALLOCATION
  equals to 1 and is required for static memory allocation support.
*/
void vApplicationGetTimerTaskMemory (StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize) {
  *ppxTimerTaskTCBBuffer   = &Timer_TCB;
  *ppxTimerTaskStackBuffer = &Timer_Stack[0];
  *pulTimerTaskStackSize   = (uint32_t)configTIMER_TASK_STACK_DEPTH;
}

#ifndef SS_USE_MOCK

extern void Error_Handler(void);

extern TIM_HandleTypeDef htim14;

void __attribute__((weak)) vConfigureTimerForRunTimeStats(void) {
 RCC_ClkInitTypeDef    clkconfig;
  uint32_t              uwTimclock = 0;
  uint32_t              uwPrescalerValue = 0;
  uint32_t              pFLatency;
  
  /*Configure the TIM14 IRQ priority */
  HAL_NVIC_SetPriority(TIM8_TRG_COM_TIM14_IRQn, 5 ,0); 
  
  /* Enable the TIM14 global Interrupt */
  HAL_NVIC_EnableIRQ(TIM8_TRG_COM_TIM14_IRQn); 
  
  /* Enable TIM14 clock */
  __HAL_RCC_TIM14_CLK_ENABLE();
  
  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
  
  /* Compute TIM14 clock */
  uwTimclock = 2*HAL_RCC_GetPCLK1Freq();
   
  /* Compute the prescaler value to have TIM14 counter clock equal to 1MHz */
  uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000) - 1);
  
  /* Initialize TIM14 */
  htim14.Instance = TIM14;
  
  /* Initialize TIMx peripheral as follow:
  + Period = [(TIM14CLK/25000) - 1]. to have a (1/25000) s time base.
  + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
  + ClockDivision = 0
  + Counter direction = Up
  */
  htim14.Init.Period = (1000000 / 25000) - 1;
  htim14.Init.Prescaler = uwPrescalerValue;
  htim14.Init.ClockDivision = 0;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  if(HAL_TIM_Base_Init(&htim14) == HAL_OK)
  {
    /* Start the TIM time Base generation in interrupt mode */
    HAL_TIM_Base_Start_IT(&htim14);
  }
}

volatile uint32_t counter25khz;

void __attribute__((weak)) SS_FreeRTOS_25khz_timer_callback(TIM_HandleTypeDef *htim) {
    if(htim == &htim14) {
        counter25khz++;
    }
}

#endif
