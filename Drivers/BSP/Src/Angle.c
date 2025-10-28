//
// Created by chen2 on 2025/10/23.
//

#include "Angle.h"

#include "fashion_star_uart_servo.h"
#include "main.h"

__attribute__((section(".RAM"))) uint16_t adcRawData[10] = {0};

/**
 * @brief 初始化角度传感器
 * @param hangle 角度传感器句柄
 * @param hadc ADC句柄
 */
void Angle_Init(Angle_HandleTypeDef* hangle, ADC_HandleTypeDef* hadc)
{
    hangle->hadc = hadc;
    hangle->zero_offset = 0.0f;
    hangle->is_calibrated = 0;
    hangle->raw_data = 0;
    hangle->current_angle = 0.0f;
    hangle->filtered_angle = 0.0f;

    // 初始化滤波器
    hangle->filter_index = 0;
    hangle->filter_count = 0;
    for (int i = 0; i < ANGLE_FILTER_SIZE; i++) {
        hangle->filter_buffer[i] = 0.0f;
    }

    // 按钮初始化
    Button_Init(&hangle->button, KEY_GPIO_Port, KEY_Pin, 0);

    HAL_ADC_Start_DMA(hadc, (uint32_t*)adcRawData, 10);
}

/**
 * @brief 应用移动平均滤波
 * @param hangle 角度传感器句柄
 * @param angle 原始角度值
 * @return 滤波后角度值
 */
static float Angle_ApplyFilter(Angle_HandleTypeDef* hangle, float angle)
{
    // 将新角度值存入缓冲区
    hangle->filter_buffer[hangle->filter_index] = angle;
    hangle->filter_index = (hangle->filter_index + 1) % ANGLE_FILTER_SIZE;

    // 更新有效数据计数
    if (hangle->filter_count < ANGLE_FILTER_SIZE) {
        hangle->filter_count++;
    }

    // 计算平均值
    float sum = 0.0f;
    for (int i = 0; i < hangle->filter_count; i++) {
        sum += hangle->filter_buffer[i];
    }

    return sum / hangle->filter_count;
}

/**
 * @brief 处理角度传感器逻辑
 * @param hangle 角度传感器句柄
 */
void Angle_Process(Angle_HandleTypeDef* hangle)
{
    // 读取ADC原始数据
    SCB_CleanInvalidateDCache_by_Addr((uint32_t*)adcRawData , sizeof(adcRawData));
    hangle->raw_data = adcRawData[0];

    // 处理按钮事件
    Button_EventTypeDef event = Button_Process(&hangle->button);

    switch (event) {
        case BUTTON_EVENT_CLICKED:
            // 单击设置零点
            Angle_SetZeroPoint(hangle);
            //FSUS_SetOriginPoint(&FSUS_Usart, 0);
            break;
        case BUTTON_EVENT_DOUBLE_CLICKED:
            // 双击重置校准
            hangle->zero_offset = 0.0f;
            hangle->is_calibrated = 0;
            break;
        default:
            break;
    }

    // 计算当前角度
    // hangle->current_angle = ((float)hangle->raw_data * 360.0f / 65535.0f); // 假设16位ADC
    // hangle->current_angle -= hangle->zero_offset;
    // 计算相对于零点的角度差值
    float raw_angle = (float)hangle->raw_data;
    float angle_diff = raw_angle - hangle->zero_offset;

    // 转换为角度值 (-180到+180度)
    hangle->current_angle = (angle_diff * 360.0f) / 65535.0f;


    // 角度范围处理：限制在-180到+180度之间
    if (hangle->current_angle > 180.0f) {
        hangle->current_angle -= 360.0f;
    }
    if (hangle->current_angle <= -180.0f) {
        hangle->current_angle += 360.0f;
    }

    // 应用滤波
    hangle->filtered_angle = Angle_ApplyFilter(hangle, hangle->current_angle);

    // 保留1位小数
    hangle->filtered_angle = roundf(hangle->filtered_angle * 10.0f) / 10.0f;

}

/**
 * @brief 获取当前角度值
 * @param hangle 角度传感器句柄
 * @return 当前角度值 (0-360度)
 */
float Angle_GetCurrentAngle(Angle_HandleTypeDef* hangle)
{
    return hangle->current_angle;
}

/**
 * @brief 获取滤波后角度值
 * @param hangle 角度传感器句柄
 * @return 滤波后角度值
 */
float Angle_GetFilteredAngle(Angle_HandleTypeDef* hangle)
{
    return hangle->filtered_angle;
}

/**
 * @brief 设置当前角度为零点
 * @param hangle 角度传感器句柄
 */
void Angle_SetZeroPoint(Angle_HandleTypeDef* hangle)
{
    hangle->zero_offset = (float)hangle->raw_data;
    hangle->is_calibrated = 1;
}

/**
 * @brief 检查是否已校准
 * @param hangle 角度传感器句柄
 * @return 1:已校准, 0:未校准
 */
uint8_t Angle_IsCalibrated(Angle_HandleTypeDef* hangle)
{
    return hangle->is_calibrated;
}
