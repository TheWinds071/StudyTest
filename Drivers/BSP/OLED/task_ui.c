#include "task_ui.h"

#define TOTAL_TASKS 6
#define VISIBLE_ITEMS 3 // 屏幕单屏最多显示的菜单项数量

// 初始化 UI 句柄
void UI_Init(UI_HandleTypeDef *hui) {
    hui->state = UI_STATE_MENU;
    hui->select_index = 0;
    hui->top_index = 0;
    hui->start_tick = 0;
    hui->elapsed_ms = 0;
    hui->is_timing = 0;
}

// 按键事件响应
void UI_ProcessKey(UI_HandleTypeDef *hui, UI_KeyEvent_e key) {
    if (key == KEY_NONE) return;

    if (hui->state == UI_STATE_MENU) {
        switch (key) {
            case KEY_UP:
                if (hui->select_index > 0) {
                    hui->select_index--;
                    if (hui->select_index < hui->top_index) {
                        hui->top_index = hui->select_index;
                    }
                } else {
                    // 从 TASK1 向上循环跳转到 TASK6
                    hui->select_index = TOTAL_TASKS - 1;
                    hui->top_index = TOTAL_TASKS - VISIBLE_ITEMS;
                }
                break;
            case KEY_DOWN:
                if (hui->select_index < TOTAL_TASKS - 1) {
                    hui->select_index++;
                    if (hui->select_index >= hui->top_index + VISIBLE_ITEMS) {
                        hui->top_index = hui->select_index - VISIBLE_ITEMS + 1;
                    }
                } else {
                    // 从 TASK6 向下循环跳转到 TASK1
                    hui->select_index = 0;
                    hui->top_index = 0;
                }
                break;
            case KEY_IN:
                // 处于菜单状态时，按 IN 键选择进入任务，立刻开始秒表计时！
                hui->state = UI_STATE_TASK;
                hui->start_tick = HAL_GetTick();
                hui->elapsed_ms = 0;
                hui->is_timing = 1;
                break;
            default:
                break;
        }
    } 
    else if (hui->state == UI_STATE_TASK) {
        switch (key) {
            case KEY_IN:
                // 处于任务状态时，按 IN 键退出任务，返回主菜单！
                hui->state = UI_STATE_MENU;
                hui->is_timing = 0;
                break;
            default:
                break;
        }
    }
}

// 绘制主菜单 (原生 U8G2 函数)
static void UI_DrawMenu(u8g2_t *u8g2, UI_HandleTypeDef *hui) {
    char buf[16];

    // 1. 标题栏
    u8g2_SetFont(u8g2, u8g2_font_6x10_tf);
    u8g2_DrawStr(u8g2, 24, 10, "SELECT TASK");
    u8g2_DrawHLine(u8g2, 0, 13, 128); // 顶部分割线

    // 2. 动态菜单项
    uint8_t y_start = 16;
    uint8_t item_height = 15;

    for (uint8_t i = 0; i < VISIBLE_ITEMS; i++) {
        uint8_t item_idx = hui->top_index + i;
        if (item_idx >= TOTAL_TASKS) break;

        uint8_t current_y = y_start + i * item_height;
        snprintf(buf, sizeof(buf), "  TASK %d", item_idx + 1);

        // 如果被选中，绘制高亮圆角选择框（反显）
        if (item_idx == hui->select_index) {
            u8g2_SetDrawColor(u8g2, 1);
            u8g2_DrawRBox(u8g2, 4, current_y, 114, 13, 2);
            
            u8g2_SetDrawColor(u8g2, 0); // 黑色文字
            u8g2_DrawStr(u8g2, 8, current_y + 10, ">");
            u8g2_DrawStr(u8g2, 16, current_y + 10, buf);
            u8g2_SetDrawColor(u8g2, 1); // 恢复颜色
        } else {
            u8g2_SetDrawColor(u8g2, 1);
            u8g2_DrawStr(u8g2, 16, current_y + 10, buf);
        }
    }

    // 3. 右侧滚动条
    u8g2_DrawFrame(u8g2, 122, 15, 5, 48);
    uint8_t bar_h = 48 / TOTAL_TASKS;
    uint8_t bar_y = 15 + hui->select_index * bar_h;
    u8g2_DrawBox(u8g2, 123, bar_y, 3, bar_h);
}

// 绘制任务与秒表界面 (原生 U8G2 函数)
static void UI_DrawTaskStopwatch(u8g2_t *u8g2, UI_HandleTypeDef *hui) {
    char str_buf[32];

    // 1. 页眉标题框
    u8g2_SetDrawColor(u8g2, 1);
    u8g2_DrawBox(u8g2, 0, 0, 128, 14);
    
    u8g2_SetDrawColor(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_6x10_tf);
    snprintf(str_buf, sizeof(str_buf), "[ TASK %d ]", hui->select_index + 1);
    u8g2_DrawStr(u8g2, 34, 10, str_buf);
    
    u8g2_SetDrawColor(u8g2, 1);

    // 2. 更新秒表数据
    if (hui->is_timing) {
        hui->elapsed_ms = HAL_GetTick() - hui->start_tick;
    }
    
    uint32_t total_sec = hui->elapsed_ms / 1000;
    uint32_t min = (total_sec / 60) % 100;
    uint32_t sec = total_sec % 60;
    uint32_t ms10 = (hui->elapsed_ms % 1000) / 10;

    // 3. 大字号显示秒表 (格式: 00:00.00)
    snprintf(str_buf, sizeof(str_buf), "%02lu:%02lu.%02lu", (unsigned long)min, (unsigned long)sec, (unsigned long)ms10);
    u8g2_SetFont(u8g2, u8g2_font_ncenB14_tr);
    u8g2_DrawStr(u8g2, 15, 38, str_buf);

    // 4. 页脚状态与提示
    u8g2_DrawHLine(u8g2, 0, 48, 128);
    u8g2_SetFont(u8g2, u8g2_font_6x10_tf);
    if (hui->is_timing) {
        if (hui->select_index == 2) {
            extern volatile float Angle;
            int32_t heading = (int32_t)Angle;
            if (heading > 180) heading -= 360;
            snprintf(str_buf, sizeof(str_buf), "YAW:%ld deg", (long)heading);
        } else {
            extern uint8_t g_gray_raw;
            extern int32_t g_gray_pos;
            snprintf(str_buf, sizeof(str_buf), "POS:%ld R:%02X", g_gray_pos, g_gray_raw);
        }
        u8g2_DrawStr(u8g2, 4, 60, str_buf);
    } else {
        u8g2_DrawStr(u8g2, 4, 60, "# STOP");
    }

    u8g2_DrawStr(u8g2, 90, 60, "[ESC]");
}

// 刷新 UI 入口
void UI_Update(u8g2_t *u8g2, UI_HandleTypeDef *hui) {
    u8g2_ClearBuffer(u8g2);

    switch (hui->state) {
        case UI_STATE_MENU:
            UI_DrawMenu(u8g2, hui);
            break;
        case UI_STATE_TASK:
            UI_DrawTaskStopwatch(u8g2, hui);
            break;
    }

    u8g2_SendBuffer(u8g2);
}
