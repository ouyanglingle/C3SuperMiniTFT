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
PROGMEM void avoidObstaclesGame()
{
    func_bf_SwitchAnimate();

    // 游戏状态变量
    float playerY = 60;
    float playerVelocity = 0;
    int obstacleX = 160;
    int obstacleGap = 60;
    int obstacleHeight = random(20, 100);
    int score = 0;
    bool gameOver = false;
    uint32_t lastSpawn = millis();

    while (1)
    {
        // 处理输入
        KEY_STATE upState = getKeyState(UP_PIN);
        KEY_STATE enterState = getKeyState(ENTER_PIN);

        // 物理模拟
        if (!gameOver)
        {
            // 跳跃控制
            if (upState == KEY_PRESS)
            {
                playerVelocity = JUMP_FORCE;
            }
            // 应用重力
            playerVelocity += GRAVITY;
            playerY += playerVelocity;
            // 边界限制
            playerY = constrain(playerY, 0, 160 - PLAYER_SIZE);
            // 障碍物移动
            obstacleX -= 3;
            // 生成新障碍物
            if (millis() - lastSpawn > 2000)
            {
                obstacleX = 160;
                obstacleHeight = random(20, 100);
                lastSpawn = millis();
                score++;
            }
            // 碰撞检测
            if (obstacleX < PLAYER_SIZE &&
                (playerY < obstacleHeight ||
                 playerY + PLAYER_SIZE > obstacleHeight + obstacleGap))
            {
                gameOver = true;
            }
        }
        // 清空缓冲区
        func_bf.fillSprite(TFT_WHITE);
        func_bf.drawRect(0, 0, 160, 160, TFT_GREEN);
        // 绘制玩家（圆形）
        func_bf.fillCircle(30, playerY + PLAYER_SIZE / 2, PLAYER_SIZE / 2, TFT_BLUE);
        // 绘制障碍物
        if (!gameOver)
        {
            func_bf.fillRect(obstacleX, 0, OBSTACLE_WIDTH, obstacleHeight, TFT_RED);
            func_bf.fillRect(obstacleX, obstacleHeight + obstacleGap,
                             OBSTACLE_WIDTH, 160 - obstacleHeight - obstacleGap, TFT_RED);
        }

        // 显示分数
        func_bf.setTextColor(TFT_BLACK);
        func_bf.drawNumber(score, 140, 5);
        // 游戏结束界面
        if (gameOver)
        {
            func_bf.setTextColor(TFT_RED, TFT_WHITE);
            func_bf.drawString("GAME OVER", 50, 70);
            func_bf.drawString("Score:", 60, 90);
            func_bf.drawNumber(score, 90, 105);
            func_bf.drawString("Press ENTER", 40, 130);

            if (enterState == KEY_PRESS)
                break;
        }

        func_bf.pushSprite(startX, targetY);
        delay(20);
    }
    func_bf.setTextColor(FUNC_BF_FG_COLOR, FUNC_BF_BG_COLOR);
    func_bf.fillSprite(TFT_BLACK);
}
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

        // 处理 UP 按键事件
        if (upState == KEY_PRESS || upState == KEY_LONG_PRESS)
        {
            float step = (upState == KEY_PRESS) ? 3 : 1; // 按下为 0.1，长按为 0.01
            volume += step;
        }

        // 处理 DOWN 按键事件
        if (downState == KEY_PRESS || downState == KEY_LONG_PRESS)
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
    PROGMEM String gpio_cfg[] = {
        "   GPIO_config    ",
        "5V      | * 5:IN_PU",
        "GND     | * 6:TX1",
        "3v3     | * 7:OUT_PP",
        "*4:SCK  | * 8:RX1",
        "*3:MOSI | * 9:IN_PU",
        "*2:RST  | *10:OUT_PP",
        "*1:DC   |  20",
        "*0:BLK  | *21:IN_PU",
        "SPI  Serial1  Digital I/O",
    };
    func_bf_SwitchAnimate();
    while (1)
    {
        func_bf.fillSprite(FUNC_BF_BG_COLOR); // 清屏
        func_bf.drawRect(0, 0, 160, 160, TFT_GREEN);
        func_bf.drawRect(1, 1, 158, 158, TFT_GREEN);
        /*USER CODE BEGIN*/
        for (int i = 0; i < sizeof(gpio_cfg) / sizeof(gpio_cfg[0]); i++)
        {
            func_bf.drawString(gpio_cfg[i], 5, 5 + i * 15);
        }
        /*USER CODE END*/
        func_bf.pushSprite(startX, targetY); // 固定在目标位置
        if (getKeyState(ENTER_PIN) == KEY_PRESS)
            break;
    }
    func_bf.fillSprite(TFT_BLACK); // 清空小缓冲区
}
//
#define L1 120
#define L2 120
#define L 120
#define END_LEN 100 // 夹爪到末端的距离，夹爪是需要额外安装的

/// @brief   简单的逆运动学计算
/// @param X
/// @param Y
/// @param Z
/// @param ss  建议0.95
/// @param speed 速度，建议给30
/// @param acc   加速度，建议给60
void ARM_Calc(float X, float Y, float Z, float ss, float speed, float acc)
{
    float theta, phi, alpha, beta;
    if (Y == 0)
        theta = 0;
    else
        theta = atan2(X, Y) * 180 / M_PI;

    float Length_Temp = sqrt(X * X + Y * Y);
    if (Length_Temp > 0)
        phi = atan2(Z, Length_Temp);
    else
        phi = M_PI / 2;
    alpha = (acos((sqrt(Z * Z + Length_Temp * Length_Temp)) / (2 * L)) - phi) * 180 / M_PI;
    beta = asin((Length_Temp / L) - cos(alpha * M_PI / 180)) * 180 / M_PI;
    func_bf.setCursor(5, 5);
    func_bf.printf("[INFO] α=%.2f, β=%.2f, θ=%.2f\n", alpha, beta, theta);

    if (Z * Z + Y * Y > (ss * 2 * L) * (ss * 2 * L))
        printf("[E1]\n");
    if (Z < -110 || Z > 110)
    {
        printf("[E2]\n");
        return;
    }
    if (Z * Z + Y * Y < 50 * 50)
    {
        printf("[E3]\n");
        return;
    }
    if ((alpha < -10 || alpha > 90) && (beta < -10 || beta > 90))
    {
        printf("[E4]\n");
        return;
    }
}
/// @brief 使用Serial1控制机械臂
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
        static int i = 0;
        ARM_Calc(0, 100, i, 0.95, 50, 25);
        func_bf.setCursor(5, 35);
        func_bf.printf("Move to (0, 100, %d)", i);
        delay(250);
        i++;
        if (i > 40)
            i = 0;
        /*USER CODE END*/
        func_bf.pushSprite(startX, targetY); // 固定在目标位置
        if (getKeyState(ENTER_PIN) == KEY_PRESS)
            break;
    }
    func_bf.fillSprite(TFT_BLACK); // 清空小缓冲区
}

USBHIDMouse mouse;             // 实例化
const int8_t move_d = 3;       // 单次鼠标移动量
const int total_count = 200;   // 一个方向移动总次数
void USB_Mouse()
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
        static int i = 0;
        ARM_Calc(0, 100, i, 0.95, 50, 25);
        func_bf.setCursor(5, 35);
        func_bf.printf("Move to (0, 100, %d)", i);
        delay(250);
        i++;
        if (i > 40)
            i = 0;
        /*USER CODE END*/
        func_bf.pushSprite(startX, targetY); // 固定在目标位置
        if (getKeyState(ENTER_PIN) == KEY_PRESS)
            break;
    }
    func_bf.fillSprite(TFT_BLACK); // 清空小缓冲区
}