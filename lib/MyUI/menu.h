#pragma once
#ifndef MENU_H
#define MENU_H

#include <TFT_eSPI.h>
#include <key.h>
#include "pid.h"
#include <menutask.h>
#include <esp_heap_caps.h>

#define BF_BG_COLOR TFT_BLACK      // 主菜单背景颜色
#define BF_FG_COLOR TFT_GOLD   // 主菜单字体颜色
#define FUNC_BF_BG_COLOR TFT_DARKGREY // 小缓冲区背景颜色
#define FUNC_BF_FG_COLOR TFT_BLACK // 小缓冲区字体颜色

extern pid_type_def highlightPid;
extern uint32_t MenuTick;

extern int startX;       // 目标 x 坐标
extern int startY; // 初始 y 坐标
extern int targetY;      // 目标 y 坐标
extern int currentY; // 当前 y 坐标

void Menu_Init(void);
void Draw_Menu(void);
void Menu_Handle_Input();

// 下面的函数不希望用户直接调用
void func_bf_SwitchAnimate();

#endif


