
#ifndef __SG90_H
#define __SG90_H

#include "stm32u5xx_hal.h"
#include "cmsis_os2.h"   // 如果你在 FreeRTOS 任务中使用可保留，否则可删除

// ====================== 用户根据实际 TIM 配置修改 ======================
extern TIM_HandleTypeDef htim2;          // CubeMX 生成的 TIM2 句柄

#define SG90_TIM_HANDLE         &htim2
#define SG90_TIM_CHANNEL        TIM_CHANNEL_2     // PA1 对应 TIM2_CH2

// SG90 典型脉宽范围（单位：微秒）
// 0°   ≈ 500us
// 90°  ≈ 1500us
// 180° ≈ 2500us
#define SG90_PULSE_MIN          500
#define SG90_PULSE_MAX          2500

// =====================================================================

/**
 * @brief  初始化 SG90 舵机（启动 PWM 输出）
 */
void SG90_Init(void);

/**
 * @brief  设置舵机角度（0~180度）
 * @param  angle  角度值，范围 0~180
 */
void SG90_SetAngle(uint8_t angle);

/**
 * @brief  直接设置 PWM 脉宽（微秒），用于精细控制或调试
 * @param  pulse_us  脉宽时间，推荐范围 500~2500
 */
void SG90_SetPulse(uint16_t pulse_us);

#endif /* __SG90_H */

