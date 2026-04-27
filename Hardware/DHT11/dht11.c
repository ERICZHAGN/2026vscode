#include "dht11.h"
#include "main.h"
#include "gpio.h"

// DWT 微秒延时
static void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000);
    while ((DWT->CYCCNT - start) < ticks);
}

// 初始化 DWT 周期计数器
static void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
}

// 拉低数据线
static void DHT11_Low(void)
{
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
}

// 释放数据线（高阻，上拉至高电平）
static void DHT11_High(void)
{
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
}

// 读取数据线电平
static uint8_t DHT11_ReadPin(void)
{
    return HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN);
}

// 等待引脚变为指定电平，超时返回 -1
static int DHT11_WaitLevel(uint8_t level, uint32_t timeout_us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t timeout_ticks = timeout_us * (SystemCoreClock / 1000000);

    while ((DWT->CYCCNT - start) < timeout_ticks) {
        if (DHT11_ReadPin() == level)
            return 0;
    }
    return -1;
}

// 读取一个 bit（先低 50us，高电平时长决定 0/1）
static int DHT11_ReadBit(uint8_t *bit)
{
    // 等待低电平结束
    if (DHT11_WaitLevel(GPIO_PIN_SET, 100) != 0)
        return -1;

    delay_us(40); // 在 40us 处采样：短高=0，长高=1

    if (DHT11_ReadPin() == GPIO_PIN_SET)
        *bit = 1;
    else
        *bit = 0;

    // 等待当前 bit 周期结束（低电平到来）
    if (DHT11_WaitLevel(GPIO_PIN_RESET, 100) != 0)
        return -1;

    return 0;
}

// 读取一个字节
static int DHT11_ReadByte(uint8_t *byte)
{
    uint8_t b = 0;
    for (int i = 0; i < 8; i++) {
        uint8_t bit;
        if (DHT11_ReadBit(&bit) != 0)
            return -1;
        b = (b << 1) | bit;
    }
    *byte = b;
    return 0;
}

int DHT11_Read(float *temperature, float *humidity)
{
    uint8_t data[5] = {0};
    uint32_t check;

    // 单次初始化（首次调用时使能 DWT）
    static uint8_t dwt_inited = 0;
    if (!dwt_inited) {
        DWT_Init();
        dwt_inited = 1;
    }

    // 1. 发送起始信号：拉低 ≥18ms，然后拉高
    DHT11_Low();
    HAL_Delay(20);
    DHT11_High();

    // 等待 20-40us 后 DHT11 响应
    delay_us(30);

    // 2. 检测 DHT11 响应：先低 80us，再高 80us
    if (DHT11_WaitLevel(GPIO_PIN_RESET, 200) != 0)
        return -1;  // DHT11 没拉低

    if (DHT11_WaitLevel(GPIO_PIN_SET, 200) != 0)
        return -1;   // DHT11 没拉高

    // 3. 读取 40 位数据（5 字节）
    for (int i = 0; i < 5; i++) {
        if (DHT11_ReadByte(&data[i]) != 0)
            return -1;
    }

    // 4. 等待 DHT11 释放总线
    DHT11_WaitLevel(GPIO_PIN_SET, 200);

    // 5. 校验：前 4 字节之和等于第 5 字节
    check = data[0] + data[1] + data[2] + data[3];
    if ((check & 0xFF) != data[4])
        return -1;

    // 6. 转换数据
    // DHT11: data[0]=湿度整数, data[1]=湿度小数(始终0), data[2]=温度整数, data[3]=温度小数(始终0)
    *humidity    = (float)data[0] + (float)data[1] / 10.0f;
    *temperature = (float)data[2] + (float)data[3] / 10.0f;

    return 0;
}
