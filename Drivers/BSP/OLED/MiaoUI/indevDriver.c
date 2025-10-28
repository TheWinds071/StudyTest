/**
<one line to give the program's name and a brief idea of what it does.>
Copyright (C) <2024>  <JianFeng>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "dispDriver.h"
#include "main.h"

// 按键状态枚举
typedef enum {
    KEY_STATE_IDLE,      // 空闲状态
    KEY_STATE_PRESS,     // 按下状态
    KEY_STATE_RELEASE    // 释放状态
} KeyState_TypeDef;

// 按键类型枚举
typedef enum {
    KEY_NONE,
    KEY_UP,
    KEY_DOWN,
    KEY_ENTER
} Key_TypeDef;

UI_ACTION indevScan(void)
{
    /* 放入你的按键扫描代码 */
    static KeyState_TypeDef keyState = KEY_STATE_IDLE;
    static Key_TypeDef lastKey = KEY_NONE;
    static uint32_t pressTime = 0;
    Key_TypeDef currentKey = KEY_NONE;

    /* 检测当前按下的按键 */
    if (HAL_GPIO_ReadPin(KEY_Up_GPIO_Port, KEY_Up_Pin) == GPIO_PIN_RESET) {
        currentKey = KEY_UP;
    } else if (HAL_GPIO_ReadPin(KEY_Down_GPIO_Port, KEY_Down_Pin) == GPIO_PIN_RESET) {
        currentKey = KEY_DOWN;
    } else if (HAL_GPIO_ReadPin(KEY_Enter_GPIO_Port, KEY_Enter_Pin) == GPIO_PIN_RESET) {
        currentKey = KEY_ENTER;
    }

    switch (keyState) {
        case KEY_STATE_IDLE:
            if (currentKey != KEY_NONE) {
                // 按键按下，进入按下状态
                keyState = KEY_STATE_PRESS;
                lastKey = currentKey;
                pressTime = HAL_GetTick();
            }
            break;

        case KEY_STATE_PRESS:
            if (currentKey == lastKey) {
                // 按键保持按下，检查是否超过消抖时间
                if ((HAL_GetTick() - pressTime) >= 20) { // 20ms消抖时间
                    // 确认按键有效，返回对应动作
                    switch (lastKey) {
                        case KEY_UP:
                            keyState = KEY_STATE_RELEASE;
                            return UI_ACTION_UP;
                        case KEY_DOWN:
                            keyState = KEY_STATE_RELEASE;
                            return UI_ACTION_DOWN;
                        case KEY_ENTER:
                            keyState = KEY_STATE_RELEASE;
                            return UI_ACTION_ENTER;
                        default:
                            keyState = KEY_STATE_IDLE;
                            break;
                    }
                }
            } else {
                // 按键释放或按下其他按键，回到空闲状态
                keyState = KEY_STATE_IDLE;
            }
            break;

        case KEY_STATE_RELEASE:
            // 等待按键完全释放
            if (currentKey == KEY_NONE) {
                keyState = KEY_STATE_IDLE;
            }
            break;

        default:
            keyState = KEY_STATE_IDLE;
            break;
    }

    return UI_ACTION_NONE;
}