#include "key.h"
#include "Ticker.h"
hw_timer_s *KeyTickTimer = NULL; // 硬件定时器
Ticker KeyTick; // 软件定时器
#define DEBOUNCE_DELAY 15 // 消抖时间（毫秒）
// 管理输入引脚
DebounceInfo pins[] = {
    {ENTER_PIN, HIGH, HIGH, HIGH, 0, 0}, // 初始化为未按下状态
    {UP_PIN, HIGH, HIGH, HIGH, 0, 0},
    {DOWN_PIN, HIGH, HIGH, HIGH, 0, 0}};

// 消抖函数
void debounce(DebounceInfo *info)
{
    uint8_t current = digitalRead(info->pin); // 当前按键状态
    uint32_t now = millis();                  // 当前时间

    if (current != info->last_state) // 如果状态发生变化
    {
        info->last_state = current;  // 更新上一次状态
        info->last_check_time = now; // 记录变化时间
        info->debouncing = 1;        // 标记为正在消抖
        return;
    }

    if (info->debouncing && (now - info->last_check_time) >= DEBOUNCE_DELAY)
    {
        info->stable_state = current; // 更新稳定状态
        info->debouncing = 0;         // 消抖完成
    }
}
void Key_Tick(void);
// 初始化按键
void Key_Init(void)
{
    for (int i = 0; i < sizeof(pins) / sizeof(pins[0]); i++)
    {
        pinMode(pins[i].pin, INPUT_PULLUP); // 设置为输入模式，启用内部上拉电阻
    }
    // 用硬件定时器还是软件定时器自己看
    KeyTickTimer = timerBegin(0, 80, true);
    timerAttachInterrupt(KeyTickTimer, Key_Tick, true);
    timerAlarmWrite(KeyTickTimer, 0.01*1000000, true);// 0.01 * 1000000us = 0.01s = 10ms
    timerAlarmEnable(KeyTickTimer);
    //KeyTick.attach(0.02, Key_Tick); // 设置定时器，每20毫秒执行一次Key_Tick()
}

// 更新按键状态
void Key_Tick(void)
{
    for (int i = 0; i < sizeof(pins) / sizeof(pins[0]); i++)
        debounce(&pins[i]); // 对每个按键进行消抖处理
}

// 获取按键状态（支持三种状态：松开、按下、长按）
KEY_STATE getKeyState(uint8_t pin)
{
    for (int i = 0; i < sizeof(pins) / sizeof(pins[0]); i++)
    {
        if (pins[i].pin == pin) // 找到对应的按键
        {
            // 检测按键是否从松开变为按下
            if (pins[i].stable_state == LOW && pins[i].last_stable_state == HIGH)
            {
                pins[i].last_stable_state = pins[i].stable_state; // 更新状态
                return KEY_PRESS; // 返回按键按下的事件
            }
            // 检测按键是否从按下变为松开
            if (pins[i].stable_state == HIGH && pins[i].last_stable_state == LOW)
            {
                pins[i].last_stable_state = pins[i].stable_state;
                return KEY_RELEASE;
            }
            // 检测按键是否处于长按状态
            if (pins[i].stable_state == LOW && (millis() - pins[i].last_check_time > 500))
            {
                return KEY_LONG_PRESS;
            }
            // 默认返回松开状态
            return KEY_RELEASE;
        }
    }
    return KEY_RELEASE; // 如果未找到对应按键，默认返回松开状态
}