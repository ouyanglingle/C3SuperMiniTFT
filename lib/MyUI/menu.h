#ifndef MENU_H
#define MENU_H

#include <TFT_eSPI.h>
#include <key.h>
#include "pid.h"

extern pid_type_def highlightPid;
extern uint32_t MenuTick;
void Menu_Init(void);

void Menu_Tick(void);

void Draw_Menu(void);

void Handle_Input();

void exampleFunction();

void Return();

void about();

#endif


