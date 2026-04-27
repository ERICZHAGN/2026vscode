#ifndef __LIGHT_SENSOR_PA3_H
#define __LIGHT_SENSOR_PA3_H

#include "main.h"

void LightSensor_PA3_Init(void);
uint8_t LightSensor_PA3_IsReady(void);
uint8_t LightSensor_PA3_Read(uint32_t *value);
uint8_t LightSensor_PA3_GetStage(void);
uint32_t LightSensor_PA3_GetAdcError(void);

#endif /* __LIGHT_SENSOR_PA3_H */
