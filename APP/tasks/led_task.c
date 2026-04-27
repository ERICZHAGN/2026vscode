
#include "sg90.h"

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
