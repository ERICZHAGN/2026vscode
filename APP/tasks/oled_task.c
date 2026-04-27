
#include "oled_task.h"
#include "driver_oled.h"         // ← 需要包含硬件驱动头文件
#include "cmsis_os2.h"
#include "driver_mpu6050.h"
#include <stdio.h>             // 用于 sprintf

extern osMutexId_t i2cMutexHandle;
extern osMessageQueueId_t myQueue01Handle;

void oled_task(void *argument)
{
    
Mpu6050_Data_t receivedData;
    osStatus_t status;
    char buf[20];

    if (osMutexAcquire(i2cMutexHandle, osWaitForever) == osOK) {
        OLED_PrintString(0, 0, "OLED OK");
        OLED_PrintString(0, 2, "Waiting MPU...");
        osMutexRelease(i2cMutexHandle);
    }

    while (1) {
        /* 1. 阻塞等待队列消息 (不消耗 CPU) */
        status = osMessageQueueGet(myQueue01Handle, &receivedData, NULL, osWaitForever);

        if (status == osOK) {
            /* 2. 获取互斥锁以使用 I2C 屏幕 */
            if (osMutexAcquire(i2cMutexHandle, 100) == osOK) {
                // 仅更新数值，不要在这里调用 OLED_Clear() 以防闪烁
                sprintf(buf, "AX:%-6d", receivedData.ax);
                OLED_PrintString(0, 4, buf);
                
                sprintf(buf, "AY:%-6d", receivedData.ay);
                OLED_PrintString(0, 6, buf);
                
                osMutexRelease(i2cMutexHandle);
            }
        }
    }
}


