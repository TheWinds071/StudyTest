#ifndef H7TEMPLATE_MOTOR_H
#define H7TEMPLATE_MOTOR_H

#include "stm32h7xx_hal.h"

/**
 * @brief TB6612电机驱动宏定义
 */
#define TB6612_MOTOR_COUNT 2          // 最大支持2个电机

/* 电机控制引脚定义 */
typedef enum {
    MOTOR_STOP = 0,
    MOTOR_FORWARD,
    MOTOR_BACKWARD
} MotorDirection;

/**
 * @brief 电机控制结构体
 */
typedef struct {
    GPIO_TypeDef* ain1_port;         // AIN1引脚端口
    uint16_t ain1_pin;               // AIN1引脚号
    GPIO_TypeDef* ain2_port;         // AIN2引脚端口
    uint16_t ain2_pin;               // AIN2引脚号
    TIM_HandleTypeDef* htim;         // PWM定时器句柄
    uint32_t channel;                // PWM通道
    uint16_t max_pwm;                // 最大PWM值
} Motor_Handler;

/**
 * @brief TB6612驱动结构体
 */
typedef struct {
    Motor_Handler motor[TB6612_MOTOR_COUNT]; // 两个电机控制结构体
    GPIO_TypeDef* stby_port;         // STBY引脚端口
    uint16_t stby_pin;               // STBY引脚号
} TB6612_Handler;

// 声明全局TB6612句柄
extern TB6612_Handler tb6612_handler;

/**
 * @brief 初始化TB6612 Handler配置
 * @return 无
 */
void TB6612_Handler_Init(void);

/**
 * @brief 初始化TB6612驱动器
 * @param handler TB6612句柄指针
 * @return 无
 */
void TB6612_Init(TB6612_Handler* handler);

/**
 * @brief 电机模块初始化（包含Handler初始化和外设启动）
 * @return 无
 */
void Motor_Init(void);

/**
 * @brief 设置电机方向和速度
 * @param handler TB6612句柄指针
 * @param motor_index 电机索引(0或1)
 * @param direction 电机方向
 * @param speed 速度值(0-最大PWM值)
 * @return 无
 */
void TB6612_SetMotor(TB6612_Handler* handler, uint8_t motor_index, MotorDirection direction, uint16_t speed);

/**
 * @brief 设置电机PWM占空比
 * @param handler TB6612句柄指针
 * @param motor_index 电机索引(0或1)
 * @param duty_cycle 占空比值(0-最大PWM值)
 * @return 无
 */
void TB6612_SetDutyCycle(TB6612_Handler* handler, uint8_t motor_index, uint16_t duty_cycle);

/**
 * @brief 停止指定电机
 * @param handler TB6612句柄指针
 * @param motor_index 电机索引(0或1)
 * @return 无
 */
void TB6612_StopMotor(TB6612_Handler* handler, uint8_t motor_index);

/**
 * @brief 停止所有电机
 * @param handler TB6612句柄指针
 * @return 无
 */
void TB6612_StopAllMotors(TB6612_Handler* handler);

/**
 * @brief 启用TB6612驱动器
 * @param handler TB6612句柄指针
 * @return 无
 */
void TB6612_Enable(TB6612_Handler* handler);

/**
 * @brief 禁用TB6612驱动器
 * @param handler TB6612句柄指针
 * @return 无
 */
void TB6612_Disable(TB6612_Handler* handler);

#endif //H7TEMPLATE_MOTOR_H