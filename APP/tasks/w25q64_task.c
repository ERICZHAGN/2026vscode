#include "w25q64_task.h"
#include "driver_w25q64.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include <string.h>



void W25Q64_TestTask(void *argument)
{
    W25Q64_Init();
    printf("W25Q64 Task Started\r\n");

    uint8_t id = W25Q64_ReadID();
    printf("W25Q64 ID: 0x%02X\r\n", id);

    uint8_t write_buf[16];
    memcpy(write_buf, "Hello W25Q64", 13);

    uint8_t read_buf[16] = {0};

    // 使用带超时的安全调用
    W25Q64_EraseSector(0x000000);
    osDelay(100);

    W25Q64_WritePage(write_buf, 0x000000, 13);
    printf("Write OK\r\n");

    osDelay(100);

    W25Q64_ReadData(read_buf, 0x000000, 13);
    printf("Read Back: %s\r\n", read_buf);
    
    // 调试：逐个打印字节
    printf("Read Bytes: ");
    for (int i = 0; i < 13; i++) {
        printf("%02X ", read_buf[i]);
    }
    printf("\r\n");
    
    // 验证数据是否正确
    printf("Expected: Hello W25Q64\r\n");
    
    if (memcmp(read_buf, "Hello W25Q64", 13) == 0) {
        printf("✓ Data Match - PASS\r\n");
    } else {
        printf("✗ Data Mismatch - FAIL\r\n");
    }

    for (;;) {
        osDelay(1000);
    }
}