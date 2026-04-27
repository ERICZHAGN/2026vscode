#ifndef __DHT11_H
#define __DHT11_H

#include <stdint.h>

#define DHT11_PORT  GPIOA
#define DHT11_PIN   GPIO_PIN_12

// 读取温湿度，成功返回 0，失败返回 -1
int DHT11_Read(float *temperature, float *humidity);

#endif /* __DHT11_H */
