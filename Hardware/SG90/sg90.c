#include "sg90.h"
// #include "stm32u5xx_hal.h"

/**
 * @brief  初始化 SG90 舵机
 *         必须在 CubeMX 生成的 MX_TIM2_Init() 之后调用
 */
void SG90_Init(void)
{
    // 启动 PWM 输出
    HAL_TIM_PWM_Start(SG90_TIM_HANDLE, SG90_TIM_CHANNEL);
    
    // 初始化到中位（90°）
    SG90_SetAngle(90);
}

/**
 * @brief  设置舵机角度（0~180度）
 * @param  angle  角度值
 */
void SG90_SetAngle(uint8_t angle)
{
    uint16_t pulse;
    
    if (angle > 180) angle = 180;
    
    // 线性映射：0°→500us，180°→2500us
    pulse = SG90_PULSE_MIN + (uint16_t)((SG90_PULSE_MAX - SG90_PULSE_MIN) * angle / 180.0f);
    
    SG90_SetPulse(pulse);
}

/**
 * @brief  直接设置脉宽（微秒）
 * @param  pulse_us  脉宽时间
 */
void SG90_SetPulse(uint16_t pulse_us)
{
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(SG90_TIM_HANDLE);
    
    // 计算 CCR 值（防止除零）
    if (arr == 0) return;
    
    uint32_t ccr = (pulse_us * (arr + 1)) / 20000;   // 20ms = 20000us
    
    // 限制范围
    if (ccr < (SG90_PULSE_MIN * (arr + 1) / 20000)) 
        ccr = (SG90_PULSE_MIN * (arr + 1) / 20000);
    if (ccr > (SG90_PULSE_MAX * (arr + 1) / 20000)) 
        ccr = (SG90_PULSE_MAX * (arr + 1) / 20000);
    
    __HAL_TIM_SET_COMPARE(SG90_TIM_HANDLE, SG90_TIM_CHANNEL, ccr);
}

