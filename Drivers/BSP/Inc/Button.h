#ifndef STUDYTEST_BUTTON_H
#define STUDYTEST_BUTTON_H

#include "main.h"

// 按钮状态枚举
typedef enum {
    BUTTON_STATE_IDLE = 0,      // 空闲状态
    BUTTON_STATE_PRESSED,       // 按下状态
    BUTTON_STATE_RELEASED,      // 释放状态
    BUTTON_STATE_DEBOUNCING     // 消抖状态
} Button_StateTypeDef;

// 按钮事件枚举
typedef enum {
    BUTTON_EVENT_NONE = 0,      // 无事件
    BUTTON_EVENT_PRESSED,       // 按下事件
    BUTTON_EVENT_RELEASED,      // 释放事件
    BUTTON_EVENT_CLICKED,       // 单击事件
    BUTTON_EVENT_DOUBLE_CLICKED // 双击事件
} Button_EventTypeDef;

// 按钮配置结构体
typedef struct {
    GPIO_TypeDef* port;         // GPIO端口
    uint16_t pin;              // GPIO引脚
    uint8_t active_level;      // 有效电平 (0:低电平有效, 1:高电平有效)
    Button_StateTypeDef state; // 当前状态
    uint32_t press_time;       // 按下时间
    uint32_t release_time;     // 释放时间
    uint32_t last_debounce_time; // 上次消抖时间
    uint32_t debounce_delay;   // 消抖延迟时间(ms)
    uint8_t click_count;       // 点击计数
    uint32_t last_click_time;  // 上次点击时间
} Button_HandleTypeDef;

// 函数声明
void Button_Init(Button_HandleTypeDef* hbutton, GPIO_TypeDef* port, uint16_t pin, uint8_t active_level);
Button_EventTypeDef Button_Process(Button_HandleTypeDef* hbutton);
uint8_t Button_GetState(Button_HandleTypeDef* hbutton);


#endif //STUDYTEST_BUTTON_H