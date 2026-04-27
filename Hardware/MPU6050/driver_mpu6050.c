#include "driver_mpu6050.h"
#include <string.h>
#include "driver_oled.h"  // 可选：用于测试显示

#define USE_OLED_TEST


#define MPU6050_TIMEOUT_MS    100

static uint8_t mpu6050_addr = 0;

// 写寄存器
static int MPU6050_WriteReg(uint8_t reg, uint8_t data)
{
    uint8_t buf[2] = {reg, data};
    return HAL_I2C_Master_Transmit(&hi2c1, mpu6050_addr, buf, 2, MPU6050_TIMEOUT_MS);
}

// 读寄存器
static int MPU6050_ReadReg(uint8_t reg, uint8_t *data)
{
    return HAL_I2C_Mem_Read(&hi2c1, mpu6050_addr, reg, 1, data, 1, MPU6050_TIMEOUT_MS);
}

// 从指定地址连续读取 len 字节
static int MPU6050_ReadRegs(uint8_t reg, uint8_t *buf, uint8_t len)
{
    return HAL_I2C_Mem_Read(&hi2c1, mpu6050_addr, reg, 1, buf, len, MPU6050_TIMEOUT_MS);
}

// 初始化 MPU6050
int MPU6050_Init(void)
{
    uint8_t id;

    // 尝试地址 0x68
    mpu6050_addr = MPU6050_ADDR_0;
    if (HAL_I2C_IsDeviceReady(&hi2c1, mpu6050_addr, 1, 10) == HAL_OK) {
        if (MPU6050_ReadReg(MPU6050_WHO_AM_I, &id) == HAL_OK && id == 0x68) {
            goto init_ok;
        }
    }

    // 尝试地址 0x69
    mpu6050_addr = MPU6050_ADDR_1;
    if (HAL_I2C_IsDeviceReady(&hi2c1, mpu6050_addr, 1, 10) == HAL_OK) {
        if (MPU6050_ReadReg(MPU6050_WHO_AM_I, &id) == HAL_OK && id == 0x68) {
            goto init_ok;
        }
    }

    return -1; // 设备未找到

init_ok:
    // 退出睡眠模式
    if (MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x00) != HAL_OK) return -2;
    HAL_Delay(10);

    // 可选：配置采样率、滤波等（这里用默认值）
    // MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);   // 100Hz
    // MPU6050_WriteReg(MPU6050_CONFIG, 0x06);       // 5Hz DLPF
    // MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x00);  // ±250dps
    // MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x00); // ±2g

    return 0;
}

// 获取 ID
int MPU6050_GetID(void)
{
    uint8_t id;
    if (mpu6050_addr == 0) return -1;
    if (MPU6050_ReadReg(MPU6050_WHO_AM_I, &id) == HAL_OK) {
        return id;
    }
    return -1;
}

// 读取原始数据（14字节：6轴+温度）
int MPU6050_ReadRawData(
    int16_t *ax, int16_t *ay, int16_t *az,
    int16_t *gx, int16_t *gy, int16_t *gz
)
{
    uint8_t buf[14];
    if (MPU6050_ReadRegs(MPU6050_ACCEL_XOUT_H, buf, 14) != HAL_OK) {
        return -1;
    }

    if (ax) *ax = (int16_t)((buf[0] << 8) | buf[1]);
    if (ay) *ay = (int16_t)((buf[2] << 8) | buf[3]);
    if (az) *az = (int16_t)((buf[4] << 8) | buf[5]);

    // 温度：buf[6], buf[7]（可忽略）

    if (gx) *gx = (int16_t)((buf[8] << 8) | buf[9]);
    if (gy) *gy = (int16_t)((buf[10] << 8) | buf[11]);
    if (gz) *gz = (int16_t)((buf[12] << 8) | buf[13]);

    return 0;
}

// ========================
// 测试函数（使用 OLED 打印）
// ========================
#ifdef USE_OLED_TEST
#include "driver_oled.h"  // 请确保你有这个头文件

void MPU6050_TestLoop(void)
{
    int16_t ax, ay, az, gx, gy, gz;

    //OLED_PrintString(0, 0, "MPU6050 OK");
    //OLED_Clear();
    //HAL_Delay(1000);
  
        if (MPU6050_ReadRawData(&ax, &ay, &az, &gx, &gy, &gz) == 0) {
            OLED_PrintString(0, 0, "AX:");
            OLED_PrintSignedVal(4, 0, ax);

            OLED_PrintString(0, 2, "AY:");
            OLED_PrintSignedVal(4, 2, ay);

            OLED_PrintString(0, 4, "AZ:");
            OLED_PrintSignedVal(4, 4, az);

            // 可选：打印陀螺仪（需更多行）
            // OLED_PrintString(0, 6, "GX:");
            // OLED_PrintSignedVal(4, 6, gx);
        } else {
            OLED_PrintString(0, 6, "READ ERR");
        }
        //HAL_Delay(200);
   
}
#endif