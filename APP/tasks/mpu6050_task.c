
#include "mpu6050_task.h"
#include "driver_mpu6050.h"
#include "cmsis_os2.h"
#include "driver_oled.h"  // 可选：用于测试显示
#include <stdio.h>

extern osMutexId_t i2cMutexHandle;
extern osMessageQueueId_t myQueue01Handle;

void mpu6050_task(void *argument) {
    Mpu6050_Data_t sensorData;
    /* 任务开始前进行一次硬件初始化 */
    if (osMutexAcquire(i2cMutexHandle, osWaitForever) == osOK) {
        MPU6050_Init();
        osMutexRelease(i2cMutexHandle);
    }

    while (1) {
        /* 1. 获取互斥锁保护 I2C 总线 */
        if (osMutexAcquire(i2cMutexHandle, 100) == osOK) {
            // 仅读取数据，不在此处做耗时的 OLED 刷新
            MPU6050_ReadRawData(&sensorData.ax, &sensorData.ay, &sensorData.az, NULL, NULL, NULL);
            osMutexRelease(i2cMutexHandle); // 读完立刻释放

            /* 2. 将数据推送到队列 */
            // 最后一个参数 0U 表示如果不入队不等待
            osMessageQueuePut(myQueue01Handle, &sensorData, 0U, 0U);
        }
        osDelay(100); // 采样频率 10Hz
    }
}













// void mpu6050_task(void *argument) {
//     while (1) {
//         if (osMutexAcquire(i2cMutexHandle, 100) == osOK) { // 建议给个超时，不要死等
//             MPU6050_TestLoop();
//             osMutexRelease(i2cMutexHandle);
//         }
//         // 关键：必须在 if 外面延时，确保放弃 CPU 所有权
//         osDelay(3000); 
//     }
// }

