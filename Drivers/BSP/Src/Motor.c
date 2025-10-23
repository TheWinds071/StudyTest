#include "Motor.h"

#include "tim.h"

// 声明handler
TB6612_Handler tb6612_handler;

// 在运行时初始化handler的函数
void TB6612_Handler_Init(void) {
    // 电机0配置
    tb6612_handler.motor[0].ain1_port = GPIOA;
    tb6612_handler.motor[0].ain1_pin = GPIO_PIN_0;
    tb6612_handler.motor[0].ain2_port = GPIOA;
    tb6612_handler.motor[0].ain2_pin = GPIO_PIN_1;
    tb6612_handler.motor[0].htim = &htim1;
    tb6612_handler.motor[0].channel = TIM_CHANNEL_1;
    tb6612_handler.motor[0].max_pwm = 1000;

    // 电机1配置
    tb6612_handler.motor[1].ain1_port = GPIOA;
    tb6612_handler.motor[1].ain1_pin = GPIO_PIN_2;
    tb6612_handler.motor[1].ain2_port = GPIOA;
    tb6612_handler.motor[1].ain2_pin = GPIO_PIN_3;
    tb6612_handler.motor[1].htim = &htim1;
    tb6612_handler.motor[1].channel = TIM_CHANNEL_2;
    tb6612_handler.motor[1].max_pwm = 1000;

    // STBY引脚配置
    tb6612_handler.stby_port = GPIOB;
    tb6612_handler.stby_pin = GPIO_PIN_12;
}

/**
 * @brief 初始化TB6612驱动器
 * @param handler TB6612句柄指针
 * @return 无
 */
void TB6612_Init(TB6612_Handler* handler) {
    // 启用所有引脚的时钟（假设已在HAL初始化中完成）
    
    // 初始化STBY引脚为高电平（启用驱动器）
    HAL_GPIO_WritePin(handler->stby_port, handler->stby_pin, GPIO_PIN_SET);
    
    // 初始化电机控制引脚为低电平
    for (int i = 0; i < TB6612_MOTOR_COUNT; i++) {
        HAL_GPIO_WritePin(handler->motor[i].ain1_port, handler->motor[i].ain1_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(handler->motor[i].ain2_port, handler->motor[i].ain2_pin, GPIO_PIN_RESET);
        
        // 初始化PWM为0
        __HAL_TIM_SET_COMPARE(handler->motor[i].htim, handler->motor[i].channel, 0);
    }
}

/**
 * @brief 电机模块初始化（包含Handler初始化和外设启动）
 * @return 无
 */
void Motor_Init(void) {
    // 初始化TB6612 Handler配置
    TB6612_Handler_Init();
    
    // 初始化TB6612驱动器
    TB6612_Init(&tb6612_handler);
    
    // 启动PWM通道
    HAL_TIM_PWM_Start(tb6612_handler.motor[0].htim, tb6612_handler.motor[0].channel);
    HAL_TIM_PWM_Start(tb6612_handler.motor[1].htim, tb6612_handler.motor[1].channel);
}

/**
 * @brief 设置电机方向和速度
 * @param handler TB6612句柄指针
 * @param motor_index 电机索引(0或1)
 * @param direction 电机方向
 * @param speed 速度值(0-最大PWM值)
 * @return 无
 */
void TB6612_SetMotor(TB6612_Handler* handler, uint8_t motor_index, MotorDirection direction, uint16_t speed) {
    if (motor_index >= TB6612_MOTOR_COUNT) {
        return; // 电机索引超出范围
    }
    
    // 限制速度在有效范围内
    if (speed > handler->motor[motor_index].max_pwm) {
        speed = handler->motor[motor_index].max_pwm;
    }
    
    switch (direction) {
        case MOTOR_FORWARD:
            // 设置正转
            HAL_GPIO_WritePin(handler->motor[motor_index].ain1_port, handler->motor[motor_index].ain1_pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(handler->motor[motor_index].ain2_port, handler->motor[motor_index].ain2_pin, GPIO_PIN_RESET);
            break;
            
        case MOTOR_BACKWARD:
            // 设置反转
            HAL_GPIO_WritePin(handler->motor[motor_index].ain1_port, handler->motor[motor_index].ain1_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(handler->motor[motor_index].ain2_port, handler->motor[motor_index].ain2_pin, GPIO_PIN_SET);
            break;
            
        case MOTOR_STOP:
        default:
            // 设置停止
            HAL_GPIO_WritePin(handler->motor[motor_index].ain1_port, handler->motor[motor_index].ain1_pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(handler->motor[motor_index].ain2_port, handler->motor[motor_index].ain2_pin, GPIO_PIN_RESET);
            speed = 0;
            break;
    }
    
    // 设置PWM速度
    __HAL_TIM_SET_COMPARE(handler->motor[motor_index].htim, handler->motor[motor_index].channel, speed);
}

/**
 * @brief 设置电机PWM占空比
 * @param handler TB6612句柄指针
 * @param motor_index 电机索引(0或1)
 * @param duty_cycle 占空比值(0-最大PWM值)
 * @return 无
 */
void TB6612_SetDutyCycle(TB6612_Handler* handler, uint8_t motor_index, uint16_t duty_cycle) {
    if (motor_index >= TB6612_MOTOR_COUNT) {
        return; // 电机索引超出范围
    }
    
    // 限制占空比在有效范围内
    if (duty_cycle > handler->motor[motor_index].max_pwm) {
        duty_cycle = handler->motor[motor_index].max_pwm;
    }
    
    // 设置PWM占空比
    __HAL_TIM_SET_COMPARE(handler->motor[motor_index].htim, handler->motor[motor_index].channel, duty_cycle);
}

/**
 * @brief 停止指定电机
 * @param handler TB6612句柄指针
 * @param motor_index 电机索引(0或1)
 * @return 无
 */
void TB6612_StopMotor(TB6612_Handler* handler, uint8_t motor_index) {
    if (motor_index >= TB6612_MOTOR_COUNT) {
        return; // 电机索引超出范围
    }
    
    // 设置停止
    HAL_GPIO_WritePin(handler->motor[motor_index].ain1_port, handler->motor[motor_index].ain1_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(handler->motor[motor_index].ain2_port, handler->motor[motor_index].ain2_pin, GPIO_PIN_RESET);
    
    // 设置PWM为0
    __HAL_TIM_SET_COMPARE(handler->motor[motor_index].htim, handler->motor[motor_index].channel, 0);
}

/**
 * @brief 停止所有电机
 * @param handler TB6612句柄指针
 * @return 无
 */
void TB6612_StopAllMotors(TB6612_Handler* handler) {
    for (uint8_t i = 0; i < TB6612_MOTOR_COUNT; i++) {
        TB6612_StopMotor(handler, i);
    }
}

/**
 * @brief 启用TB6612驱动器
 * @param handler TB6612句柄指针
 * @return 无
 */
void TB6612_Enable(TB6612_Handler* handler) {
    HAL_GPIO_WritePin(handler->stby_port, handler->stby_pin, GPIO_PIN_SET);
}

/**
 * @brief 禁用TB6612驱动器
 * @param handler TB6612句柄指针
 * @return 无
 */
void TB6612_Disable(TB6612_Handler* handler) {
    HAL_GPIO_WritePin(handler->stby_port, handler->stby_pin, GPIO_PIN_RESET);
}