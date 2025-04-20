/*
    @brief  存放用户TFT自定义功能函数的文件
*/
#pragma once
#ifndef MENUTASK_H
#define MENUTASK_H

#include <Arduino.h>

PROGMEM void avoidObstaclesGame();

/// @brief 控制BY5002的音量
void Volume();

void GPIO_config();

void RobotARM();

#endif


