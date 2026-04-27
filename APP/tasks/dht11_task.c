#include "dht11_task.h"
#include "dht11.h"
#include "driver_oled.h"
#include "cmsis_os2.h"
#include <stdio.h>

extern osMutexId_t i2cMutexHandle;

void dht11_task(void *argument)
{
    float temp, humi;
    char buf[20];

    // 等待 OLED 初始化完成（main 中已 init）
    osDelay(100);

    if (osMutexAcquire(i2cMutexHandle, osWaitForever) == osOK) {
        OLED_Clear();
        OLED_PrintString(0, 0, "DHT11 Start...");
        osMutexRelease(i2cMutexHandle);
    }

    while (1) {
        if (DHT11_Read(&temp, &humi) == 0) {
            if (osMutexAcquire(i2cMutexHandle, 100) == osOK) {
                sprintf(buf, "TMP: %.1f C", temp);
                OLED_PrintString(0, 2, buf);

                sprintf(buf, "HUM: %.1f %%", humi);
                OLED_PrintString(0, 4, buf);

                osMutexRelease(i2cMutexHandle);
            }
        }
        osDelay(2000);  // DHT11 最大采样率 1Hz，每 2s 读取一次
    }
}
