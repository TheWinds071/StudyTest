#ifndef BSP_ENCODER_H
#define BSP_ENCODER_H

#include "stm32h7xx_hal.h"

/**
 * @brief 编码器数据结构体
 */
typedef struct {
    int32_t speed;       // 当前速度 (脉冲数/10ms)
    int64_t position;    // 累计位移 (总脉冲数)
    int32_t last_count;  // 上一次的原始定时器计数值
    uint8_t is_reversed; // 1: 反转编码器极性, 0: 正常
} Encoder_HandleTypeDef;

// 左右轮编码器全局句柄
extern Encoder_HandleTypeDef g_encoderL;
extern Encoder_HandleTypeDef g_encoderR;

/**
 * @brief 编码器硬件和中断初始化 (在 main.c 中调用)
 */
void Encoder_Init(void);

/**
 * @brief 编码器周期更新计算 (必须在 10ms 的定时中断中调用)
 */
void Encoder_Update(void);

/**
 * @brief 获取左轮当前速度 (脉冲数/10ms)
 * @return 速度值
 */
int32_t Encoder_GetSpeedL(void);

/**
 * @brief 获取右轮当前速度 (脉冲数/10ms)
 * @return 速度值
 */
int32_t Encoder_GetSpeedR(void);

/**
 * @brief 获取左轮总累计位移
 * @return 累计脉冲数
 */
int64_t Encoder_GetPositionL(void);

/**
 * @brief 获取右轮总累计位移
 * @return 累计脉冲数
 */
int64_t Encoder_GetPositionR(void);

#endif /* BSP_ENCODER_H */
