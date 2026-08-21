#include "encoder.h"
#include "tim.h"

Encoder_HandleTypeDef g_encoderL = {0};
Encoder_HandleTypeDef g_encoderR = {0};

void Encoder_Init(void) {
    // ----------------------------------------------------
    // 左轮编码器 (TIM2 - 32位)
    // ----------------------------------------------------
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
    g_encoderL.last_count = (int32_t)__HAL_TIM_GET_COUNTER(&htim2);
    g_encoderL.is_reversed = 1; // 左轮需要反转
    
    // ----------------------------------------------------
    // 右轮编码器 (TIM3 - 16位)
    // ----------------------------------------------------
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    g_encoderR.last_count = (int32_t)__HAL_TIM_GET_COUNTER(&htim3);
    g_encoderR.is_reversed = 0; // 右轮不需要反转
    
    // ----------------------------------------------------
    // 开启测速定时中断 (TIM7 1ms)
    // ----------------------------------------------------
    HAL_TIM_Base_Start_IT(&htim7);
}

void Encoder_Update(void) {
    // ----------------------------------------------------
    // 1. 处理左轮编码器 (TIM2, 32位)
    // ----------------------------------------------------
    uint32_t currL = __HAL_TIM_GET_COUNTER(&htim2);
    // 32位定时器直接相减强转为 int32_t 即可完美处理正负溢出
    int32_t deltaL = (int32_t)(currL - (uint32_t)g_encoderL.last_count);
    
    if (g_encoderL.is_reversed) deltaL = -deltaL;

    g_encoderL.speed = deltaL;
    g_encoderL.position += deltaL;
    g_encoderL.last_count = (int32_t)currL;

    // ----------------------------------------------------
    // 2. 处理右轮编码器 (TIM3, 16位)
    // ----------------------------------------------------
    uint32_t currR = __HAL_TIM_GET_COUNTER(&htim3);
    // 16位定时器必须先强制转换为 uint16_t 相减，再转为 int16_t 处理方向，最后扩展为 int32_t
    int16_t deltaR = (int16_t)((uint16_t)currR - (uint16_t)g_encoderR.last_count);
    
    int32_t deltaR_32 = (int32_t)deltaR;
    if (g_encoderR.is_reversed) deltaR_32 = -deltaR_32;

    g_encoderR.speed = deltaR_32;
    g_encoderR.position += deltaR_32;
    g_encoderR.last_count = (int32_t)currR;
}

int32_t Encoder_GetSpeedL(void) {
    return g_encoderL.speed;
}

int32_t Encoder_GetSpeedR(void) {
    return g_encoderR.speed;
}

int64_t Encoder_GetPositionL(void) {
    return g_encoderL.position;
}

int64_t Encoder_GetPositionR(void) {
    return g_encoderR.position;
}
