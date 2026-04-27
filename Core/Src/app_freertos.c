/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os2.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sg90_task.h"
#include "dht11_task.h"
#include "driver_oled.h"
#include "light_sensor_pa3.h"
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
osThreadId_t ledTaskHandle;
const osThreadAttr_t ledTask_attributes = {
  .name = "ledTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
osThreadId_t sg90TaskHandle;
const osThreadAttr_t sg90Task_attributes = {
  .name = "sg90Task",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
osThreadId_t dht11TaskHandle;
const osThreadAttr_t dht11Task_attributes = {
  .name = "dht11Task",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 512 * 4
};

osMutexId_t i2cMutexHandle;
const osMutexAttr_t i2cMutex_attributes = {
  .name = "i2cMutex"
};
osSemaphoreId_t spiDmaSemaphoreHandle;
const osSemaphoreAttr_t spiDmaSemaphore_attributes = { .name = "spiDmaSemaphore" };
osMutexId_t spiMutexHandle;
const osMutexAttr_t spiMutex_attributes = { .name = "spiMutex" };

void ledtoggle_task(void *argument);
void sg90_task(void *argument);
void dht11_task(void *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
    i2cMutexHandle = osMutexNew(&i2cMutex_attributes);
  if (i2cMutexHandle == NULL) {
    // 处理互斥锁创建失败的情况
    Error_Handler();
  }

  spiMutexHandle = osMutexNew(&spiMutex_attributes);
if (spiMutexHandle == NULL) {
    Error_Handler();
}
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */

spiDmaSemaphoreHandle = osSemaphoreNew(1, 0, &spiDmaSemaphore_attributes);
if (spiDmaSemaphoreHandle == NULL) {
    Error_Handler();
}
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */

  /* USER CODE END RTOS_QUEUES */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  ledTaskHandle = osThreadNew(ledtoggle_task, NULL, &ledTask_attributes);
  sg90TaskHandle = osThreadNew(sg90_task, NULL, &sg90Task_attributes);
  //dht11TaskHandle = osThreadNew(dht11_task, NULL, &dht11Task_attributes);
  
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief Function implementing the defaultTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN defaultTask */
  uint32_t light_value = 0;
  uint8_t light_ok = 0;
  uint8_t last_ready = 0xFF;
  uint8_t last_stage = 0xFF;
  uint32_t last_value = 0xFFFFFFFFUL;
  uint32_t last_err = 0xFFFFFFFFUL;

  LightSensor_PA3_Init();
  printf("Light sensor task start\r\n");

  /* Infinite loop */
  for(;;)
  {
    light_ok = LightSensor_PA3_Read(&light_value);
    {
      uint8_t ready = LightSensor_PA3_IsReady();
      uint8_t stage = LightSensor_PA3_GetStage();
      uint32_t err = LightSensor_PA3_GetAdcError();

      if ((ready != last_ready) || (stage != last_stage) || (err != last_err) ||
          (light_ok && (light_value != last_value))) {
        printf("PA3 ready=%u stage=%u ok=%u value=%lu err=0x%08lX\r\n",
               ready, stage, light_ok, light_value, err);
        last_ready = ready;
        last_stage = stage;
        last_err = err;
        if (light_ok) {
          last_value = light_value;
        }
      }
    }

    osDelay(300);
  }
  /* USER CODE END defaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void ledtoggle_task(void *argument)
{
  /* USER CODE BEGIN ledtoggle_task */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    osDelay(500);
  }
  /* USER CODE END ledtoggle_task */
}

// ==================== SPI DMA 完成回调 ====================
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1) {
        osSemaphoreRelease(spiDmaSemaphoreHandle);
    }
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1) {
        osSemaphoreRelease(spiDmaSemaphoreHandle);
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1) {
        osSemaphoreRelease(spiDmaSemaphoreHandle);
    }
}

/* USER CODE END Application */

