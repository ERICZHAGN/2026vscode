
#include "sg90_task.h"
#include "sg90.h"          // ← 需要包含硬件驱动头文件
#include "cmsis_os2.h"
#include <stdio.h>       // 用于 printf 调试

void sg90_task(void *argument)
{
  /* USER CODE BEGIN sg90_task */
  SG90_Init();  // 初始化舵机
      // 示例：在 0°、90°、180° 之间循环
    SG90_SetAngle(0);
    osDelay(1000);
   
    SG90_SetAngle(90);
    osDelay(1000);
    
    SG90_SetAngle(180);
    osDelay(1000);
  /* Infinite loop */
  for(;;)
  {
    osDelay(1000);

  }
  /* USER CODE END sg90_task */
}