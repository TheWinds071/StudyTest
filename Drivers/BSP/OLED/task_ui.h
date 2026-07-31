#ifndef __TASK_UI_H
#define __TASK_UI_H

#include "main.h"
#include "u8g2.h"
#include <stdio.h>

// 按键事件定义（仅保留 UP, DOWN, IN 三个按键）
typedef enum {
    KEY_NONE = 0,
    KEY_UP,       // 上移
    KEY_DOWN,     // 下移
    KEY_IN        // 菜单下：进入任务；任务下：退出到菜单
} UI_KeyEvent_e;

// 界面状态定义
typedef enum {
    UI_STATE_MENU = 0,
    UI_STATE_TASK
} UI_State_e;

// UI 全局控制句柄
typedef struct {
    UI_State_e state;        // 当前 UI 状态
    uint8_t select_index;    // 选中的任务索引 (0 ~ 5 对应 TASK1 ~ TASK6)
    uint8_t top_index;       // 菜单视图顶端索引（滚动菜单）
    
    // 秒表计时数据
    uint32_t start_tick;     // 任务开始系统 Tick(ms)
    uint32_t elapsed_ms;     // 已经历的毫秒数
    uint8_t  is_timing;      // 计时状态：1-运行，0-暂停
} UI_HandleTypeDef;

// 函数声明
void UI_Init(UI_HandleTypeDef *hui);
void UI_ProcessKey(UI_HandleTypeDef *hui, UI_KeyEvent_e key);
void UI_Update(u8g2_t *u8g2, UI_HandleTypeDef *hui);

#endif /* __TASK_UI_H */
