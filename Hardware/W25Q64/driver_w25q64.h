#ifndef __DRIVER_W25Q64_H
#define __DRIVER_W25Q64_H

#include "main.h"
#include "stm32u575xx.h"
#include "stm32u5xx_hal.h"
#include "spi.h"

#define W25Q64_TIMEOUT_MS        (1000U)
#define W25Q64_ERASE_TIMEOUT_MS  (5000U)

// ==================== W25Q64 命令定义 ====================
#define W25Q64_CMD_READ_DATA        0x03
#define W25Q64_CMD_WRITE_ENABLE     0x06
#define W25Q64_CMD_WRITE_DISABLE    0x04
#define W25Q64_CMD_READ_STATUS_REG  0x05
#define W25Q64_CMD_WRITE_STATUS_REG 0x01
#define W25Q64_CMD_PAGE_PROGRAM     0x02
#define W25Q64_CMD_SECTOR_ERASE     0x20
#define W25Q64_CMD_CHIP_ERASE       0xC7
#define W25Q64_CMD_READ_ID          0x9F

// ==================== 用户配置 ====================
#define W25Q64_SPI_HANDLE   hspi1
#define W25Q64_CS_GPIO_Port GPIOA
#define W25Q64_CS_Pin       GPIO_PIN_4

// ==================== 函数声明（保留原有接口） ====================
void W25Q64_Init(void);
uint8_t W25Q64_ReadID(void);
void W25Q64_EraseSector(uint32_t addr);
void W25Q64_WritePage(uint8_t* data, uint32_t addr, uint16_t len);
void W25Q64_ReadData(uint8_t* buffer, uint32_t addr, uint16_t len);

uint8_t W25Q64_ReadStatusReg(void);
void W25Q64_WriteEnable(void);
void W25Q64_CS_Select(void);
void W25Q64_CS_Unselect(void);


// ==================== DMA 函数（已弃用，改用 Polling） ====================
// uint8_t W25Q64_ReadData_DMA(uint8_t* buffer, uint32_t addr, uint16_t len);
// uint8_t W25Q64_WritePage_DMA(uint8_t* data, uint32_t addr, uint16_t len);

#endif