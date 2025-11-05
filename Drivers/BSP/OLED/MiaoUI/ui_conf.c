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
#include "ui_conf.h"

#include "Angle.h"
#include "ui.h"
#include "dispDriver.h"
#include "image.h"
#include "custom.h"
#include "version.h"

/*Page*/
ui_page_t Home_Page, System_Page, Information_Page;
/*item */
ui_item_t HomeHead_Item, SystemHead_Item, System_Item,Github_Item, InformationHead_Item,  Information_Item;
ui_item_t Contrast_Item, USER_Angle_Item, Question1_Item, Question2_Item, Question3_Item;

/**
 * 在此建立所需显示或更改的数据
 * 无参数
 * 无返回值
 */
void Create_Parameter(ui_t *ui)
{
    static int Contrast = 255;
    static ui_data_t Contrast_data;
    Contrast_data.name = "Contrast";
    Contrast_data.ptr = &Contrast;
    Contrast_data.function = Disp_SetContrast;
    Contrast_data.functionType = UI_DATA_FUNCTION_STEP_EXECUTE;
    Contrast_data.dataType = UI_DATA_INT;
    Contrast_data.actionType = UI_DATA_ACTION_RW;
    Contrast_data.max = 255;
    Contrast_data.min = 0;
    Contrast_data.step = 5;
    static ui_element_t Contrast_element;
    Contrast_element.data = &Contrast_data;
    Create_element(&Contrast_Item, &Contrast_element);

    extern float Angle;
    static ui_data_t USER_Angle_data;
    USER_Angle_data.name = "Angle";
    USER_Angle_data.ptr = &Angle;
    USER_Angle_data.dataType = UI_DATA_FLOAT;
    USER_Angle_data.actionType = UI_DATA_ACTION_RO;
    USER_Angle_data.max = 360;
    USER_Angle_data.min = -360;
    static ui_element_t USER_Angle_element;
    USER_Angle_element.data = &USER_Angle_data;
    Create_element(&USER_Angle_Item, &USER_Angle_element);

    static uint8_t question1Flag=false;
    static ui_data_t Question1_data;
    Question1_data.name = "Question1";
    Question1_data.ptr = &question1Flag;
    Question1_data.dataType = UI_DATA_SWITCH;
    Question1_data.actionType = UI_DATA_ACTION_RW;
    static ui_element_t Question1_element;
    Question1_element.data = &Question1_data;
    Create_element(&Question1_Item, &Question1_element);

    static uint8_t question2Flag=false;
    static ui_data_t Question2_data;
    Question2_data.name = "Question2";
    Question2_data.ptr = &question2Flag;
    Question2_data.dataType = UI_DATA_SWITCH;
    Question2_data.actionType = UI_DATA_ACTION_RW;
    static ui_element_t Question2_element;
    Question2_element.data = &Question2_data;
    Create_element(&Question2_Item, &Question2_element);

    static uint8_t question3Flag=false;
    static ui_data_t Question3_data;
    Question3_data.name = "Question3";
    Question3_data.ptr = &question3Flag;
    Question3_data.dataType = UI_DATA_SWITCH;
    Question3_data.actionType = UI_DATA_ACTION_RW;
    static ui_element_t Question3_element;
    Question3_element.data = &Question3_data;
    Create_element(&Question3_Item, &Question3_element);

}

// 添加获取Question1 Flag的函数
uint8_t getUIQuestion1Flag(void) {
    // extern ui_item_t Question1_Item;
    if (Question1_Item.element && Question1_Item.element->data && Question1_Item.element->data->ptr) {
        return *((uint8_t*)Question1_Item.element->data->ptr);
    }
    return 0;
}

// 添加获取Question2 Flag的函数
uint8_t getUIQuestion2Flag(void) {
    // extern ui_item_t Question2_Item;
    if (Question2_Item.element && Question2_Item.element->data && Question2_Item.element->data->ptr) {
        return *((uint8_t*)Question2_Item.element->data->ptr);
    }
    return 0;
}

// 添加获取Question3 Flag的函数
uint8_t getUIQuestion3Flag(void) {
    // extern ui_item_t Question3_Item;
    if (Question3_Item.element && Question3_Item.element->data && Question3_Item.element->data->ptr) {
        return *((uint8_t*)Question3_Item.element->data->ptr);
    }
    return 0;
}

/**
 * 在此建立所需显示或更改的文本
 * 无参数
 * 无返回值
 */
void Create_Text(ui_t *ui)
{
    static ui_text_t github_text;
    github_text.font = UI_FONT;
    github_text.fontHight = UI_FONT_HIGHT;
    github_text.fontWidth = UI_FONT_WIDTH;
    github_text.ptr = VERSION_PROJECT_LINK;
    static ui_element_t github_element;
    github_element.text = &github_text;
    Create_element(&Github_Item, &github_element);
}

/*
 * 菜单构建函数
 * 该函数不接受参数，也不返回任何值。
 * 功能：静态地构建一个菜单系统。
 */
void Create_MenuTree(ui_t *ui)
{
    AddPage("[HomePage]", &Home_Page, UI_PAGE_ICON);
        AddItem("[HomePage]", UI_ITEM_ONCE_FUNCTION, logo_allArray[0], &HomeHead_Item, &Home_Page, NULL, Draw_Home);
        AddItem(" +System", UI_ITEM_PARENTS, logo_allArray[1], &System_Item, &Home_Page, &System_Page, NULL);
            AddPage("[System]", &System_Page, UI_PAGE_TEXT);
                AddItem("[System]", UI_ITEM_RETURN, NULL, &SystemHead_Item, &System_Page, &Home_Page, NULL);
                AddItem(" -Contrast", UI_ITEM_DATA, NULL, &Contrast_Item, &System_Page, NULL, NULL);
                AddItem(" -Question1", UI_ITEM_DATA, NULL, &Question1_Item, &System_Page, NULL, NULL);
                AddItem(" -Question2", UI_ITEM_DATA, NULL, &Question2_Item, &System_Page, NULL, NULL);
                AddItem(" -Question3", UI_ITEM_DATA, NULL, &Question3_Item, &System_Page, NULL, NULL);

        AddItem(" +Information", UI_ITEM_PARENTS, logo_allArray[2], &Information_Item, &Home_Page, &Information_Page, NULL);
            AddPage("[Information]", &Information_Page, UI_PAGE_TEXT);
                AddItem("[Information]", UI_ITEM_RETURN, NULL, &InformationHead_Item, &Information_Page, &Home_Page, NULL);
                AddItem(" -Angle", UI_ITEM_DATA, NULL, &USER_Angle_Item, &Information_Page, NULL, NULL);

}

void MiaoUi_Setup(ui_t *ui)
{
    Create_UI(ui, &HomeHead_Item); // 创建UI, 必须给定一个头项目
    Draw_Home(ui);
}
