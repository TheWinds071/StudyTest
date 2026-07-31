#include "gray.h"

// 将硬件引脚映射到物理上的从左到右，并赋予位置权重
static const GraySensor_TypeDef gray_sensors[GRAY_SENSOR_COUNT] =
{
    {GRAY_L2_PORT, GRAY_L2_PIN,  2000},  // 最左 (R3)
    {GRAY_L1_PORT, GRAY_L1_PIN,  1000},  // 左中 (R2)
    {GRAY_R1_PORT, GRAY_R1_PIN, -1000},  // 右中 (R1)
    {GRAY_R2_PORT, GRAY_R2_PIN, -2000}   // 最右 (M)
};

// 静态变量，用于在完全丢线（全白）时保持最后一次的误差状态，引导小车找回黑线
static int32_t last_position = 0;

uint8_t Gray_ReadRaw(void)
{
    uint8_t mask = 0;
    for (int i = 0; i < GRAY_SENSOR_COUNT; i++) {
        if (HAL_GPIO_ReadPin(gray_sensors[i].port, gray_sensors[i].pin) == GRAY_ACTIVE_LEVEL) {
            // 位拼接：最左侧存放在 bit3，最右侧存放在 bit0
            mask |= (1 << (3 - i));
        }
    }
    return mask;
}

int32_t Gray_ReadPosition(void)
{
    int32_t total_weight = 0;
    int active_count = 0;

    for (int i = 0; i < GRAY_SENSOR_COUNT; i++) {
        if (HAL_GPIO_ReadPin(gray_sensors[i].port, gray_sensors[i].pin) == GRAY_ACTIVE_LEVEL) {
            total_weight += gray_sensors[i].weight;
            active_count++;
        }
    }

    if (active_count > 0) {
        // 计算质心平均值
        last_position = total_weight / active_count;
    }
    // 如果 active_count == 0 (说明四个探头都没压到黑线，脱轨了)
    // 则原样返回 last_position。这样 PID 控制器会继续向之前偏离的方向强力打死方向盘，直到找回黑线。
    
    return last_position;
}
