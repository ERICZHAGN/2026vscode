#include "driver_w25q64.h"
#include "string.h"
#include "cmsis_os2.h"

// ==================== CS 控制 ====================
void W25Q64_CS_Select(void) {
    HAL_GPIO_WritePin(W25Q64_CS_GPIO_Port, W25Q64_CS_Pin, GPIO_PIN_RESET);
}

void W25Q64_CS_Unselect(void) {
    HAL_GPIO_WritePin(W25Q64_CS_GPIO_Port, W25Q64_CS_Pin, GPIO_PIN_SET);
}

// ==================== 写使能 & 状态读取 ====================
void W25Q64_WriteEnable(void) {
    uint8_t cmd = W25Q64_CMD_WRITE_ENABLE;
    W25Q64_CS_Select();
    HAL_SPI_Transmit(&W25Q64_SPI_HANDLE, &cmd, 1, 100);
    W25Q64_CS_Unselect();
}

uint8_t W25Q64_ReadStatusReg(void) {
    uint8_t cmd = W25Q64_CMD_READ_STATUS_REG;
    uint8_t status = 0;
    W25Q64_CS_Select();
    HAL_SPI_Transmit(&W25Q64_SPI_HANDLE, &cmd, 1, 100);
    HAL_SPI_Receive(&W25Q64_SPI_HANDLE, &status, 1, 100);
    W25Q64_CS_Unselect();
    return status;
}

// ==================== 初始化 ====================
void W25Q64_Init(void) {
    W25Q64_CS_Unselect();
    HAL_Delay(20);
}

// ==================== 读取 ID（Polling） ====================
uint8_t W25Q64_ReadID(void) {
    uint8_t cmd[4] = {W25Q64_CMD_READ_ID, 0x00, 0x00, 0x00};
    uint8_t rx_buf[4] = {0};
    W25Q64_CS_Select();
    HAL_SPI_TransmitReceive(&W25Q64_SPI_HANDLE, cmd, rx_buf, 4, 100);
    W25Q64_CS_Unselect();
    return rx_buf[3];
}

// ==================== 扇区擦除 ====================
void W25Q64_EraseSector(uint32_t addr) {
    while (W25Q64_ReadStatusReg() & 0x01);
    W25Q64_WriteEnable();

    uint8_t cmd[4] = {W25Q64_CMD_SECTOR_ERASE, (addr>>16)&0xFF, (addr>>8)&0xFF, addr&0xFF};

    W25Q64_CS_Select();
    HAL_SPI_Transmit(&W25Q64_SPI_HANDLE, cmd, 4, 200);
    W25Q64_CS_Unselect();

    while (W25Q64_ReadStatusReg() & 0x01);
}

// ==================== 写页面（Polling 方式） ====================
void W25Q64_WritePage(uint8_t* data, uint32_t addr, uint16_t len) {
    if (len > 256 || len == 0 || data == NULL) return;

    while (W25Q64_ReadStatusReg() & 0x01);
    W25Q64_WriteEnable();

    // 将命令+地址+数据合并为一次连续传输，避免SPI被中途关闭
    uint8_t tx_buf[4 + 256];
    tx_buf[0] = W25Q64_CMD_PAGE_PROGRAM;
    tx_buf[1] = (addr>>16)&0xFF;
    tx_buf[2] = (addr>>8)&0xFF;
    tx_buf[3] = addr&0xFF;
    memcpy(tx_buf + 4, data, len);

    W25Q64_CS_Select();
    HAL_SPI_Transmit(&W25Q64_SPI_HANDLE, tx_buf, 4 + len, 1000);
    W25Q64_CS_Unselect();

    while (W25Q64_ReadStatusReg() & 0x01);
}

// ==================== 读数据（Polling 方式） ====================
void W25Q64_ReadData(uint8_t* buffer, uint32_t addr, uint16_t len) {
    if (len == 0 || buffer == NULL) return;

    // 方法：一个连续的 TransmitReceive 事务
    // 先发 4 字节命令+地址，再发 len 字节 0xFF 产生时钟读取数据
    uint8_t cmd[4] = {W25Q64_CMD_READ_DATA, (addr>>16)&0xFF, (addr>>8)&0xFF, addr&0xFF};
    uint8_t rx_cmd[4] = {0};

    W25Q64_CS_Select();

    // 第1步：发送命令+地址（使用全双工，避免 RX FIFO 残留）
    HAL_SPI_TransmitReceive(&W25Q64_SPI_HANDLE, cmd, rx_cmd, 4, 200);

    // 第2步：发送 0xFF dummy 字节产生时钟，同时接收数据
    uint8_t dummy[256];
    memset(dummy, 0xFF, len);
    HAL_SPI_TransmitReceive(&W25Q64_SPI_HANDLE, dummy, buffer, len, 1000);

    W25Q64_CS_Unselect();
}
