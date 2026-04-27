#ifndef __DRIVER_MPU6050_H
#define __DRIVER_MPU6050_H

#include "main.h"  // 确保包含 HAL 库和 I2C 句柄

// 寄存器地址
#define MPU6050_WHO_AM_I        0x75
#define MPU6050_PWR_MGMT_1      0x6B
#define MPU6050_SMPLRT_DIV      0x19
#define MPU6050_CONFIG          0x1A
#define MPU6050_GYRO_CONFIG     0x1B
#define MPU6050_ACCEL_CONFIG    0x1C

// 数据寄存器起始地址
#define MPU6050_ACCEL_XOUT_H    0x3B
#define MPU6050_GYRO_XOUT_H     0x43

// I2C 地址（7位左移1位）
#define MPU6050_ADDR_0          (0x68 << 1)  // AD0 = GND
#define MPU6050_ADDR_1          (0x69 << 1)  // AD0 = VCC

extern I2C_HandleTypeDef hi2c1;  // ← 根据你的 CubeMX 修改（如 hi2c2）

/* MPU6050 数据结构体（根据你的实际数据调整） */
typedef struct {
    int16_t ax;
    int16_t ay;
    int16_t az;
} Mpu6050_Data_t;


// 函数声明
int MPU6050_Init(void);
int MPU6050_GetID(void);
int MPU6050_ReadRawData(
    int16_t *ax, int16_t *ay, int16_t *az,
    int16_t *gx, int16_t *gy, int16_t *gz
);

// 测试函数（可选）
void MPU6050_TestLoop(void);  // 需要外部提供 OLED 或串口打印

#endif