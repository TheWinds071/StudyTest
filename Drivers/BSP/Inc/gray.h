#ifndef BSP_GRAY_H
#define BSP_GRAY_H

#include "stm32h7xx_hal.h"

#define GRAY_SENSOR_COUNT 4U

// --- 硬件引脚映射 ---
// R3 - 最左
#define GRAY_L2_PORT GPIOA
#define GRAY_L2_PIN  GPIO_PIN_8

// R2 - 左中
#define GRAY_L1_PORT GPIOA
#define GRAY_L1_PIN  GPIO_PIN_10

// R1 - 右中
#define GRAY_R1_PORT GPIOA
#define GRAY_R1_PIN  GPIO_PIN_11

// M - 最右
#define GRAY_R2_PORT GPIOA
#define GRAY_R2_PIN  GPIO_PIN_12

// 定义探头压到黑线时的触发电平（绝大多数模块压到黑线输出低电平）
#define GRAY_ACTIVE_LEVEL GPIO_PIN_RESET

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    int16_t weight;
} GraySensor_TypeDef;

/**
 * @brief 读取灰度传感器融合后的位置偏差 (用于PID)
 * @return 误差值 (-2000 到 +2000), 0为完美居中. 丢线时会保持最后一次的偏离状态.
 */
int32_t Gray_ReadPosition(void);

/**
 * @brief 获取原始传感器触发掩码 (常用于调试或处理十字路口)
 * @return bit3:最左(R3), bit2:左中(R2), bit1:右中(R1), bit0:最右(M)
 */
uint8_t Gray_ReadRaw(void);

#endif /* BSP_GRAY_H */
