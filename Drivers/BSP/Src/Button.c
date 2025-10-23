//
// Created by chen2 on 2025/10/23.
//

#include "Button.h"

// 获取当前系统时间(ms)
//extern uint32_t HAL_GetTick(void);

/**
 * @brief 初始化按钮
 * @param hbutton 按钮句柄
 * @param port GPIO端口
 * @param pin GPIO引脚
 * @param active_level 有效电平
 */
void Button_Init(Button_HandleTypeDef* hbutton, GPIO_TypeDef* port, uint16_t pin, uint8_t active_level)
{
    hbutton->port = port;
    hbutton->pin = pin;
    hbutton->active_level = active_level;
    hbutton->state = BUTTON_STATE_IDLE;
    hbutton->press_time = 0;
    hbutton->release_time = 0;
    hbutton->last_debounce_time = 0;
    hbutton->debounce_delay = 50; // 默认50ms消抖
    hbutton->click_count = 0;
    hbutton->last_click_time = 0;
}

/**
 * @brief 处理按钮状态机
 * @param hbutton 按钮句柄
 * @return 按钮事件类型
 */
Button_EventTypeDef Button_Process(Button_HandleTypeDef* hbutton)
{
    Button_EventTypeDef event = BUTTON_EVENT_NONE;
    uint32_t current_time = HAL_GetTick();
    uint8_t pin_state = HAL_GPIO_ReadPin(hbutton->port, hbutton->pin);
    uint8_t button_state = (pin_state == GPIO_PIN_SET) ? 1 : 0;

    // 如果有效电平为低电平，则取反
    if (hbutton->active_level == 0) {
        button_state = !button_state;
    }

    switch (hbutton->state) {
        case BUTTON_STATE_IDLE:
            if (button_state) {
                hbutton->last_debounce_time = current_time;
                hbutton->state = BUTTON_STATE_DEBOUNCING;
            }
            break;

        case BUTTON_STATE_DEBOUNCING:
            if ((current_time - hbutton->last_debounce_time) >= hbutton->debounce_delay) {
                if (button_state) {
                    hbutton->state = BUTTON_STATE_PRESSED;
                    hbutton->press_time = current_time;
                    event = BUTTON_EVENT_PRESSED;
                } else {
                    hbutton->state = BUTTON_STATE_IDLE;
                }
            }
            break;

        case BUTTON_STATE_PRESSED:
            if (!button_state) {
                hbutton->last_debounce_time = current_time;
                hbutton->state = BUTTON_STATE_RELEASED;
            }
            break;

        case BUTTON_STATE_RELEASED:
            if ((current_time - hbutton->last_debounce_time) >= hbutton->debounce_delay) {
                if (!button_state) {
                    hbutton->state = BUTTON_STATE_IDLE;
                    hbutton->release_time = current_time;
                    event = BUTTON_EVENT_RELEASED;

                    // 处理点击事件
                    if ((current_time - hbutton->press_time) < 1000) { // 按下时间小于1秒认为是点击
                        uint32_t click_interval = current_time - hbutton->last_click_time;
                        hbutton->last_click_time = current_time;

                        if (click_interval < 300 && hbutton->click_count == 1) {
                            // 双击
                            hbutton->click_count = 0;
                            event = BUTTON_EVENT_DOUBLE_CLICKED;
                        } else {
                            // 单击
                            hbutton->click_count = 1;
                            // 延迟发送单击事件，等待可能的双击
                        }
                    }
                } else {
                    hbutton->state = BUTTON_STATE_PRESSED;
                }
            }
            break;
    }

    // 处理单击超时
    if (hbutton->click_count == 1 && (current_time - hbutton->last_click_time) >= 300) {
        event = BUTTON_EVENT_CLICKED;
        hbutton->click_count = 0;
    }

    return event;
}

/**
 * @brief 获取按钮当前状态
 * @param hbutton 按钮句柄
 * @return 当前按钮状态
 */
uint8_t Button_GetState(Button_HandleTypeDef* hbutton)
{
    return hbutton->state;
}
