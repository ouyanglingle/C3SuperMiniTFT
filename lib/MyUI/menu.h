#ifndef MENU_H
#define MENU_H

#include <TFT_eSPI.h>
#include <key.h>
#include "pid.h"
#include <menutask.h>

extern pid_type_def highlightPid;
extern uint32_t MenuTick;
void Menu_Init(void);
void Draw_Menu(void);
void Menu_Handle_Input();

// 下面的函数不希望用户直接调用
void func_bf_SwitchAnimate();

#endif


