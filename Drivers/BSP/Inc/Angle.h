//
// Created by chen2 on 2025/10/23.
//

#ifndef STUDYTEST_ANGLE_H
#define STUDYTEST_ANGLE_H

#include "main.h"
#include "Button.h"

// 移动平均滤波器配置
#define ANGLE_FILTER_SIZE 30

// 角度传感器配置结构体
typedef struct {
    ADC_HandleTypeDef* hadc;        // ADC句柄
    Button_HandleTypeDef button;    // 按钮句柄
    float zero_offset;             // 零点偏移量
    uint8_t is_calibrated;         // 校准状态标志
    uint16_t raw_data;             // 原始ADC数据
    float current_angle;           // 当前角度值

    // 滤波相关变量
    float filter_buffer[ANGLE_FILTER_SIZE];  // 滤波缓冲区
    uint8_t filter_index;           // 缓冲区索引
    uint8_t filter_count;           // 有效数据计数
    float filtered_angle;          // 滤波后角度值
} Angle_HandleTypeDef;

// 函数声明
void Angle_Init(Angle_HandleTypeDef* hangle, ADC_HandleTypeDef* hadc);
void Angle_Process(Angle_HandleTypeDef* hangle);
float Angle_GetCurrentAngle(Angle_HandleTypeDef* hangle);
float Angle_GetFilteredAngle(Angle_HandleTypeDef* hangle);
void Angle_SetZeroPoint(Angle_HandleTypeDef* hangle);
uint8_t Angle_IsCalibrated(Angle_HandleTypeDef* hangle);
static float Angle_ApplyFilter(Angle_HandleTypeDef* hangle, float angle);


#endif //STUDYTEST_ANGLE_H
