#ifndef __DRV8870_H
#define __DRV8870_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * @brief DRV8870 衰减模式选择
 */
typedef enum {
    DRV8870_DECAY_SLOW = 0, // 慢衰减模式 (Slow Decay / Brake Low) - 转速平滑，低速扭矩优异（推荐）
    DRV8870_DECAY_FAST      // 快衰减模式 (Fast Decay / Coast) - 惯性滑行
} DRV8870_DecayMode_e;

/**
 * @brief DRV8870 控制模式配置
 */
typedef enum {
    DRV8870_MODE_DUAL_PWM = 0, // 双 PWM 通道模式 (IN1、IN2 均连接 TIM PWM 通道，推荐)
    DRV8870_MODE_GPIO_PWM      // GPIO + 单 PWM 模式 (一个为普通 GPIO，另一个为 PWM)
} DRV8870_ControlMode_e;

/**
 * @brief DRV8870 电机对象句柄结构体
 */
typedef struct {
    DRV8870_ControlMode_e control_mode; // 硬件控制接口模式
    DRV8870_DecayMode_e   decay_mode;   // 衰减模式选择

    TIM_HandleTypeDef* htim;            // 定时器句柄指针
    uint32_t           in1_channel;     // IN1 对应 TIM Channel (如 TIM_CHANNEL_1)
    uint32_t           in2_channel;     // IN2 对应 TIM Channel (如 TIM_CHANNEL_2)

    GPIO_TypeDef*      in1_gpio_port;   // IN1 GPIO 端口 (仅在 GPIO_PWM 模式下有效)
    uint16_t           in1_gpio_pin;    // IN1 GPIO 引脚
    GPIO_TypeDef*      in2_gpio_port;   // IN2 GPIO 端口 (仅在 GPIO_PWM 模式下有效)
    uint16_t           in2_gpio_pin;    // IN2 GPIO 引脚

    uint32_t           pwm_period;      // 定时器 ARR 周期值 (如 12000)
    int32_t            current_speed;   // 当前速度离散值 (-pwm_period ~ +pwm_period)
    uint8_t            is_reversed;     // 1: 极性反转, 0: 正常
} DRV8870_HandleTypeDef;

/* API 函数声明 */

extern DRV8870_HandleTypeDef g_motorL;
extern DRV8870_HandleTypeDef g_motorR;

/**
 * @brief DRV8870 模块化初始化 (内部定义并配置句柄)
 */
void DRV8870_Init(void);

/**
 * @brief 设置 DRV8870 电机速度 (离散步数模式)
 * @param hdrv DRV8870 句柄指针
 * @param speed_step 速度步数 (-pwm_period ~ +pwm_period，即 -12000 到 12000)
 */
void DRV8870_SetSpeed(DRV8870_HandleTypeDef* hdrv, int32_t speed_step);

/**
 * @brief 设置 DRV8870 原始 PWM 寄存器值
 * @param hdrv DRV8870 句柄指针
 * @param raw_pwm 原始 PWM 比较值 (-pwm_period ~ +pwm_period)
 */
void DRV8870_SetRawPWM(DRV8870_HandleTypeDef* hdrv, int32_t raw_pwm);

/**
 * @brief 强制刹车 (慢衰减制动: IN1=1, IN2=1)
 * @param hdrv DRV8870 句柄指针
 */
void DRV8870_Brake(DRV8870_HandleTypeDef* hdrv);

/**
 * @brief 惯性滑行 (快衰减自由停止: IN1=0, IN2=0)
 * @param hdrv DRV8870 句柄指针
 */
void DRV8870_Coast(DRV8870_HandleTypeDef* hdrv);

/**
 * @brief 动态设置衰减模式 (慢衰减 / 快衰减)
 * @param hdrv DRV8870 句柄指针
 * @param decay_mode DRV8870_DECAY_SLOW 或 DRV8870_DECAY_FAST
 */
void DRV8870_SetDecayMode(DRV8870_HandleTypeDef* hdrv, DRV8870_DecayMode_e decay_mode);

#ifdef __cplusplus
}
#endif

#endif /* __DRV8870_H */
