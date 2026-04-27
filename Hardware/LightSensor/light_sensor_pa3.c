#include "light_sensor_pa3.h"

#include "stm32u5xx_hal_adc.h"
#include "stm32u5xx_hal_adc_ex.h"
#include "stm32u5xx_ll_rcc.h"

static ADC_HandleTypeDef hadc1;
static uint8_t g_light_sensor_ready = 0U;
static uint8_t g_light_sensor_stage = 0U;

enum
{
    LIGHT_STAGE_NONE = 0,
    LIGHT_STAGE_INIT_OK,
    LIGHT_STAGE_INIT_FAIL,
    LIGHT_STAGE_CFG_FAIL,
    LIGHT_STAGE_CAL_FAIL,
    LIGHT_STAGE_START_FAIL,
    LIGHT_STAGE_POLL_FAIL,
    LIGHT_STAGE_READ_OK
};

void HAL_ADC_MspInit(ADC_HandleTypeDef *adcHandle)
{
    if (adcHandle->Instance == ADC1) {
        __HAL_RCC_ADC12_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        LL_RCC_SetADCDACClockSource(LL_RCC_ADCDAC_CLKSOURCE_SYSCLK);
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef *adcHandle)
{
    if (adcHandle->Instance == ADC1) {
        __HAL_RCC_ADC12_CLK_DISABLE();
    }
}

void LightSensor_PA3_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    g_light_sensor_ready = 0U;
    g_light_sensor_stage = LIGHT_STAGE_NONE;

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.GainCompensation = 0;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1.Init.LowPowerAutoWait = DISABLE;
    hadc1.Init.LowPowerAutoPowerOff = ADC_LOW_POWER_NONE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.NbrOfDiscConversion = 1;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
    hadc1.Init.OversamplingMode = DISABLE;
    hadc1.Init.Oversampling.Ratio = 0;
    hadc1.Init.Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_NONE;
    hadc1.Init.Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
    hadc1.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;
    hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;

    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        g_light_sensor_ready = 0U;
        g_light_sensor_stage = LIGHT_STAGE_INIT_FAIL;
        return;
    }

    g_light_sensor_stage = LIGHT_STAGE_INIT_OK;

    sConfig.Channel = ADC_CHANNEL_8;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_68CYCLES;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    sConfig.OffsetSignedSaturation = DISABLE;
    sConfig.OffsetSign = ADC_OFFSET_SIGN_NEGATIVE;
    sConfig.OffsetRightShift = DISABLE;
    sConfig.OffsetSaturation = DISABLE;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        g_light_sensor_ready = 0U;
        g_light_sensor_stage = LIGHT_STAGE_CFG_FAIL;
        return;
    }

    g_light_sensor_ready = 1U;
}

uint8_t LightSensor_PA3_IsReady(void)
{
    return g_light_sensor_ready;
}

uint8_t LightSensor_PA3_Read(uint32_t *value)
{
    if (value == NULL) {
        return 0U;
    }

    if (g_light_sensor_ready == 0U) {
        return 0U;
    }

    if (HAL_ADC_Start(&hadc1) != HAL_OK) {
        g_light_sensor_stage = LIGHT_STAGE_START_FAIL;
        return 0U;
    }

    if (HAL_ADC_PollForConversion(&hadc1, 10) != HAL_OK) {
        (void)HAL_ADC_Stop(&hadc1);
        g_light_sensor_stage = LIGHT_STAGE_POLL_FAIL;
        return 0U;
    }

    *value = HAL_ADC_GetValue(&hadc1);
    (void)HAL_ADC_Stop(&hadc1);
    g_light_sensor_stage = LIGHT_STAGE_READ_OK;

    return 1U;
}

uint8_t LightSensor_PA3_GetStage(void)
{
    return g_light_sensor_stage;
}

uint32_t LightSensor_PA3_GetAdcError(void)
{
    return HAL_ADC_GetError(&hadc1);
}
