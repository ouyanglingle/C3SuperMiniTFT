#include <menutask.h>
#include <menu.h>
#include <USB.h>
#include <USBHIDMouse.h>

#define PLAYER_SIZE 12
#define GRAVITY 0.5       // 重力强度
#define JUMP_FORCE -8     // 跳跃力度
#define OBSTACLE_WIDTH 20 // 障碍物宽度
#define OBSTACLE_SPEED 3  // 障碍物移动速度（X轴递减值）

extern TFT_eSprite func_bf;

/// @brief 控制BY5002的音量
void Volume()
{
    func_bf_SwitchAnimate();
    static int volume = 3; // 初始音量
    while (1)
    {
        func_bf.fillSprite(FUNC_BF_BG_COLOR); // 清屏
        func_bf.drawRect(0, 0, 160, 160, TFT_GREEN);
        func_bf.drawRect(1, 1, 158, 158, TFT_GREEN);
        /*USER CODE BEGIN*/
        KEY_STATE upState = getKeyState(UP_PIN);       // 获取 UP 按键状态
        KEY_STATE downState = getKeyState(DOWN_PIN);   // 获取 DOWN 按键状态
        KEY_STATE enterState = getKeyState(ENTER_PIN); // 获取 ENTER 按键状态
        if (upState == KEY_PRESS || upState == KEY_LONG_PRESS)
        {
            float step = (upState == KEY_PRESS) ? 3 : 1; // 按下为 0.1，长按为 0.01
            volume += step;
        }
        else if (downState == KEY_PRESS || downState == KEY_LONG_PRESS)
        {
            float step = (downState == KEY_PRESS) ? 3 : 1; // 按下为 0.1，长按为 0.01
            volume -= step;
        }
        // 限制 BLK_rates 的范围
        if (volume <= 0)
            volume = 0;
        else if (volume >= 30)
            volume = 30;

        // 更新 PWM 输出和环形亮度圆圈
        func_bf.drawSmoothArc(80, 80, 70, 60, 0, (uint32_t)((volume + 0.1) * 12), TFT_GOLD, FUNC_BF_BG_COLOR, true);
        func_bf.drawString(F("Set BY5002 Volume"), 20, 60);
        func_bf.setCursor(72, 75);
        func_bf.printf("%d", volume);
        // 此处应该有一个控制BY5002音量的代码
        /*USER CODE END*/
        func_bf.pushSprite(startX, targetY); // 固定在目标位置
        if (enterState == KEY_PRESS)
            break;
    }
    func_bf.fillSprite(TFT_BLACK); // 清空小缓冲区
}

void GPIO_config()
{
    /*USER CODE BEGIN*/
    /*USER CODE END*/
    func_bf_SwitchAnimate();
    /*USER CODE BEGIN*/
    /*USER CODE END*/
    while (1)
    {
        func_bf.fillSprite(FUNC_BF_BG_COLOR); // 清屏
        func_bf.drawRect(0, 0, 160, 160, TFT_GREEN);
        func_bf.drawRect(1, 1, 158, 158, TFT_GREEN);
        /*USER CODE BEGIN*/
        /*USER CODE END*/
        func_bf.pushSprite(startX, targetY); // 固定在目标位置
        if (getKeyState(ENTER_PIN) == KEY_PRESS)
            break;
    }
    func_bf.fillSprite(TFT_BLACK); // 清空小缓冲区
}

/// @brief 使用func_bf1控制机械臂
void RobotARM()
{
    /*USER CODE BEGIN*/
    /*USER CODE END*/
    func_bf_SwitchAnimate();
    /*USER CODE BEGIN*/
    /*USER CODE END*/
    while (1)
    {
        func_bf.fillSprite(FUNC_BF_BG_COLOR); // 清屏
        func_bf.drawRect(0, 0, 160, 160, TFT_GREEN);
        func_bf.drawRect(1, 1, 158, 158, TFT_GREEN);
        /*USER CODE BEGIN*/
        /*USER CODE END*/
        func_bf.pushSprite(startX, targetY); // 固定在目标位置
        if (getKeyState(ENTER_PIN) == KEY_PRESS)
            break;
    }
    func_bf.fillSprite(TFT_BLACK); // 清空小缓冲区
}
// ==========================
//
// ==========================
// PROGMEM BleMouse bleMouse("C3 BleMouse", "Esp", 10); // 其中“ESP32蓝牙键盘”为键盘名称；"Espressif"为制造商
// PROGMEM void Ble_Mouse()
// {
//     /*USER CODE BEGIN*/
//     /*USER CODE END*/
//     func_bf_SwitchAnimate();
//     /*USER CODE BEGIN*/
//     /*USER CODE END*/
//     while (1)
//     {
//         func_bf.fillSprite(FUNC_BF_BG_COLOR); // 清屏
//         func_bf.drawRect(0, 0, 160, 160, TFT_GREEN);
//         func_bf.drawRect(1, 1, 158, 158, TFT_GREEN);
//         /*USER CODE BEGIN*/

//         if (bleMouse.isConnected())
//         {
//             PROGMEM uint32_t startTime;
//             func_bf.println(F("向上移动鼠标指针"));
//             startTime = millis();
//             while (millis() < startTime + 2000)
//             {
//                 bleMouse.move(0, -1);
//                 delay(20);
//             }
//             func_bf.println(F("下移鼠标指针"));
//             startTime = millis();
//             while (millis() < startTime + 2000)
//             {
//                 bleMouse.move(0, 1);
//                 delay(20);
//             }
//         }
//         /*USER CODE END*/
//         func_bf.pushSprite(startX, targetY); // 固定在目标位置
//         if (getKeyState(ENTER_PIN) == KEY_PRESS)
//             break;
//     }
//     func_bf.fillSprite(TFT_BLACK); // 清空小缓冲区
//     bleMouse.end();
// }