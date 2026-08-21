#include "drv8870.h"

#include "tim.h"

DRV8870_HandleTypeDef g_motorL;
DRV8870_HandleTypeDef g_motorR;

/**
 * @brief 限幅辅助宏
 */
#define CLAMP(val, min, max)  ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))

/**
 * @brief DRV8870 模块化初始化 (内部定义并配置句柄)
 */
void DRV8870_Init(void) {
    // ----------------------------------------------------
    // 配置左电机 (Motor L): TIM1_CH1(IN1), TIM1_CH2(IN2)
    // ----------------------------------------------------
    g_motorL.htim = &htim1;
    g_motorL.in1_channel = TIM_CHANNEL_1;
    g_motorL.in2_channel = TIM_CHANNEL_2;
    g_motorL.control_mode = DRV8870_MODE_DUAL_PWM;
    g_motorL.decay_mode = DRV8870_DECAY_SLOW;
    g_motorL.pwm_period = 1000;
    g_motorL.is_reversed = 0;
    
    HAL_TIM_PWM_Start(g_motorL.htim, g_motorL.in1_channel);
    HAL_TIM_PWM_Start(g_motorL.htim, g_motorL.in2_channel);
    __HAL_TIM_MOE_ENABLE(g_motorL.htim);
    DRV8870_Brake(&g_motorL);

    // ----------------------------------------------------
    // 配置右电机 (Motor R): TIM1_CH3(IN1), TIM1_CH4(IN2) (已反转极性)
    // ----------------------------------------------------
    g_motorR.htim = &htim1;
    g_motorR.in1_channel = TIM_CHANNEL_3;
    g_motorR.in2_channel = TIM_CHANNEL_4;
    g_motorR.control_mode = DRV8870_MODE_DUAL_PWM;
    g_motorR.decay_mode = DRV8870_DECAY_SLOW;
    g_motorR.pwm_period = 1000;
    g_motorR.is_reversed = 1;

    HAL_TIM_PWM_Start(g_motorR.htim, g_motorR.in1_channel);
    HAL_TIM_PWM_Start(g_motorR.htim, g_motorR.in2_channel);
    __HAL_TIM_MOE_ENABLE(g_motorR.htim);
    DRV8870_Brake(&g_motorR);
}

/**
 * @brief 动态切换衰减模式
 */
void DRV8870_SetDecayMode(DRV8870_HandleTypeDef* hdrv, DRV8870_DecayMode_e decay_mode) {
    if (hdrv == NULL) return;
    hdrv->decay_mode = decay_mode;
    // 重新应用当前速度
    DRV8870_SetSpeed(hdrv, hdrv->current_speed);
}

/**
 * @brief 刹车制动 (慢衰减刹车: IN1=1, IN2=1)
 */
void DRV8870_Brake(DRV8870_HandleTypeDef* hdrv) {
    if (hdrv == NULL) return;

    hdrv->current_speed = 0;

    if (hdrv->control_mode == DRV8870_MODE_DUAL_PWM) {
        // 两路均输出 100% 占空比高电平
        __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in1_channel, hdrv->pwm_period);
        __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in2_channel, hdrv->pwm_period);
    } else if (hdrv->control_mode == DRV8870_MODE_GPIO_PWM) {
        HAL_GPIO_WritePin(hdrv->in1_gpio_port, hdrv->in1_gpio_pin, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in2_channel, hdrv->pwm_period);
    }
}

/**
 * @brief 惯性滑行 (快衰减自由停止: IN1=0, IN2=0)
 */
void DRV8870_Coast(DRV8870_HandleTypeDef* hdrv) {
    if (hdrv == NULL) return;

    hdrv->current_speed = 0;

    if (hdrv->control_mode == DRV8870_MODE_DUAL_PWM) {
        __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in1_channel, 0);
        __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in2_channel, 0);
    } else if (hdrv->control_mode == DRV8870_MODE_GPIO_PWM) {
        HAL_GPIO_WritePin(hdrv->in1_gpio_port, hdrv->in1_gpio_pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in2_channel, 0);
    }
}

/**
 * @brief 设置 DRV8870 原始 PWM 比较值
 * @param hdrv DRV8870 句柄指针
 * @param raw_pwm 比较值 (-pwm_period ~ +pwm_period)
 */
void DRV8870_SetRawPWM(DRV8870_HandleTypeDef* hdrv, int32_t raw_pwm) {
    if (hdrv == NULL || hdrv->pwm_period == 0) return;

    if (hdrv->is_reversed) {
        raw_pwm = -raw_pwm;
    }

    // 限幅防护
    int32_t max_period = (int32_t)hdrv->pwm_period;
    raw_pwm = CLAMP(raw_pwm, -max_period, max_period);
    uint32_t abs_ccr = (uint32_t)(raw_pwm >= 0 ? raw_pwm : -raw_pwm);

    hdrv->current_speed = raw_pwm;

    // ----------------------------------------------------
    // 1. 慢衰减模式 (Slow Decay Mode)
    // ----------------------------------------------------
    if (hdrv->decay_mode == DRV8870_DECAY_SLOW) {
        if (raw_pwm > 0) {
            // 【正转慢衰减】:
            // IN1 保持全高 100% (CCR = ARR)
            // IN2 PWM 占空比 = ARR - abs_ccr (取反 PWM)
            // 当 IN2 为低电平时：IN1=1, IN2=0 -> 驱动导通 (正转)
            // 当 IN2 为高电平时：IN1=1, IN2=1 -> 慢衰减续流 (低端 MOS 制动)
            if (hdrv->control_mode == DRV8870_MODE_DUAL_PWM) {
                __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in1_channel, hdrv->pwm_period);
                __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in2_channel, hdrv->pwm_period - abs_ccr);
            } else if (hdrv->control_mode == DRV8870_MODE_GPIO_PWM) {
                HAL_GPIO_WritePin(hdrv->in1_gpio_port, hdrv->in1_gpio_pin, GPIO_PIN_SET);
                __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in2_channel, hdrv->pwm_period - abs_ccr);
            }
        } 
        else if (raw_pwm < 0) {
            // 【反转慢衰减】:
            // IN2 保持全高 100% (CCR = ARR)
            // IN1 PWM 占空比 = ARR - abs_ccr (取反 PWM)
            // 当 IN1 为低电平时：IN1=0, IN2=1 -> 驱动导通 (反转)
            // 当 IN1 为高电平时：IN1=1, IN2=1 -> 慢衰减续流 (低端 MOS 制动)
            if (hdrv->control_mode == DRV8870_MODE_DUAL_PWM) {
                __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in1_channel, hdrv->pwm_period - abs_ccr);
                __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in2_channel, hdrv->pwm_period);
            } else if (hdrv->control_mode == DRV8870_MODE_GPIO_PWM) {
                HAL_GPIO_WritePin(hdrv->in1_gpio_port, hdrv->in1_gpio_pin, GPIO_PIN_RESET);
                __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in2_channel, abs_ccr);
            }
        } 
        else {
            // raw_pwm == 0: 慢衰减制动停止 (IN1=1, IN2=1)
            DRV8870_Brake(hdrv);
        }
    }
    // ----------------------------------------------------
    // 2. 快衰减模式 (Fast Decay Mode)
    // ----------------------------------------------------
    else {
        if (raw_pwm > 0) {
            // 正转快衰减: IN1=PWM, IN2=0 (关断时滑行 IN1=0, IN2=0)
            if (hdrv->control_mode == DRV8870_MODE_DUAL_PWM) {
                __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in1_channel, abs_ccr);
                __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in2_channel, 0);
            } else if (hdrv->control_mode == DRV8870_MODE_GPIO_PWM) {
                HAL_GPIO_WritePin(hdrv->in1_gpio_port, hdrv->in1_gpio_pin, GPIO_PIN_SET);
                __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in2_channel, 0);
            }
        } else if (raw_pwm < 0) {
            // 反转快衰减: IN1=0, IN2=PWM
            if (hdrv->control_mode == DRV8870_MODE_DUAL_PWM) {
                __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in1_channel, 0);
                __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in2_channel, abs_ccr);
            } else if (hdrv->control_mode == DRV8870_MODE_GPIO_PWM) {
                HAL_GPIO_WritePin(hdrv->in1_gpio_port, hdrv->in1_gpio_pin, GPIO_PIN_RESET);
                __HAL_TIM_SET_COMPARE(hdrv->htim, hdrv->in2_channel, abs_ccr);
            }
        } else {
            DRV8870_Coast(hdrv);
        }
    }
}

/**
 * @brief 设置 DRV8870 电机速度 (离散步数模式)
 * @param hdrv DRV8870 句柄指针
 * @param speed_step 速度步数 (-pwm_period ~ +pwm_period)
 */
void DRV8870_SetSpeed(DRV8870_HandleTypeDef* hdrv, int32_t speed_step) {
    if (hdrv == NULL || hdrv->pwm_period == 0) return;

    DRV8870_SetRawPWM(hdrv, speed_step);
}
