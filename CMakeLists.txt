cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME 06-1_STM32U575_USART_Printf)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# 1. 包含 CubeMX 自动生成的配置（包含 HAL库、FreeRTOS、设备初始化等）
add_subdirectory(cmake/stm32cubemx)

# 2. 添加源文件 (.c 文件)
# 建议：当你添加新的外设文件时，手动在这里列出，这样最清晰且不容易出错
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # 你可以在这里添加你的业务逻辑和驱动文件
    Hardware/SG90/sg90.c
    app/tasks/sg90_task.c
    Hardware/OLED/driver_oled.c
    Hardware/DHT11/dht11.c
    Hardware/LightSensor/light_sensor_pa3.c
    app/tasks/dht11_task.c
    # App/motor_control.c
)

# 3. 添加头文件路径 (.h 路径)
# 这样你就可以在代码里直接 #include "sg90.h" 而不需要写全路径
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    Core/Inc
    Hardware/SG90
    Hardware/OLED
    Hardware/DHT11
    Hardware/LightSensor
    Hardware/mpu6050
    Hardware/W25Q64
    App/tasks
    App
)

# 4. 添加全局宏定义
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    USE_HAL_DRIVER
    STM32U575xx
    # 如果是调试模式，可以保留此宏
    $<$<CONFIG:Debug>:DEBUG>
)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # 如果有外部的 .a 静态库，路径写在这里
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# 5. 链接库 (核心：必须链接 stm32cubemx)
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
)



