#include <menu.h>
#include <WiFi.h>
#include "math.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite bf = TFT_eSprite(&tft);
TFT_eSprite func_bf = TFT_eSprite(&tft);
uint8_t led = 8; // 板载LED

typedef struct MenuItem MenuItem;
pid_type_def highlightPid = {0};

void exampleFunction(void);
void Return(void);
void about(void);
void BLK_config();
void Date_Time();

struct MenuItem
{
    const char *name;   // 菜单项名称
    void (*function)(); // 菜单项对应的函数
    MenuItem *subMenu;  // 子菜单数组
    int subMenuCount;   // 子菜单数量
};

// 定义子菜单项
MenuItem settingsSubMenu[] = {
    {"Brightness", BLK_config, NULL, 0},
    {"Volume", exampleFunction, NULL, 0},
    {"Language", exampleFunction, NULL, 0},
    {"Time", Date_Time, NULL, 0},
    {"Counter", exampleFunction, NULL, 0},
    {"Return", Return, NULL, 0},
};
MenuItem HardWareSubMenu[] = {
    {"UART1", exampleFunction, NULL, 0},
    {"Wire0", exampleFunction, NULL, 0},
    {"Wire1", exampleFunction, NULL, 0},
    {"GPIO", exampleFunction, NULL, 0},
    {"USB-device", exampleFunction, NULL, 0},
    {"Return", Return, NULL, 0},
};
MenuItem SoftWareSubMenu[] = {
    {"MQTT", exampleFunction, NULL, 0},
    {"TCP", exampleFunction, NULL, 0},
    {"Return", Return, NULL, 0},
};
MenuItem DeviceSubMenu[] = {
    {"Lora-Module", exampleFunction, NULL, 0},
    {"BY5002-Module", exampleFunction, NULL, 0},
    {"Wit-IWT603", exampleFunction, NULL, 0},
    {"Return", Return, NULL, 0},
};
// 主菜单项(屏幕最多放只11个菜单项目)
MenuItem menuItems[] = {
    {"Main Menu", NULL, NULL, 0},
    {"Settings", NULL, settingsSubMenu, sizeof(settingsSubMenu) / sizeof(settingsSubMenu[0])},
    {"HardWare", NULL, HardWareSubMenu, sizeof(HardWareSubMenu) / sizeof(HardWareSubMenu[0])},
    {"SoftWare", NULL, SoftWareSubMenu, sizeof(SoftWareSubMenu) / sizeof(SoftWareSubMenu[0])},
    {"Device & Module", NULL, DeviceSubMenu, sizeof(DeviceSubMenu) / sizeof(DeviceSubMenu[0])},
    {"Game", NULL, NULL, 0},
    {"NONE", NULL, NULL, 0},
    {"NONE", NULL, NULL, 0},
    {"NONE", NULL, NULL, 0},
    {"NONE", NULL, NULL, 0},
    {"NONE", NULL, NULL, 0},
    {"NONE", NULL, NULL, 0},
    {"NONE", NULL, NULL, 0},
    {"NONE", NULL, NULL, 0},
    {"About", about, NULL, 0},
};
// ================================
// 这些个变量比较重要
// ================================
int currentMenuItem = 0;            // 当前选中的菜单项索引
int targetMenuItem = 0;             // 目标菜单项索引
int currentMenuLevel = 0;           // 当前菜单层级（0: 一级菜单，1: 二级菜单）
MenuItem *currentMenu = menuItems;  // 当前菜单指针
float highlightY = 0;               // 高亮框的 Y 坐标（支持浮点数用于平滑移动）
uint32_t lastUpdateTime = 0;        // 上一次更新的时间戳
const uint32_t updateInterval = 10; // 更新间隔（毫秒）
int startIndex = 0;                 // 当前显示的第一个菜单项索引
uint8_t maxVisibleItems = 11;       // 屏幕最多显示的菜单项数量
// ===============================
//       复选框动画PID参数
// ===============================
float currentWidth = 0;                  // 当前宽度
float targetWidth = 0;                   // 目标宽度
pid_type_def widthPid = {0};             // 宽度的 PID 控制器
float widthPid_pid[] = {0.1, 0.0, 0.02}; // 宽度的 PID 参数

/// @brief 初始化菜单
/// @param void
void Menu_Init(void)
{
    // 初始化宽度的 PID
    PID_init(&widthPid, PID_POSITION, widthPid_pid, 5.0f, 0.1f);

    pinMode(led, OUTPUT);
    pinMode(0, OUTPUT); // 0号引脚控制背光
    analogWrite(0, (uint8_t)255);
    tft.init();
    tft.fillScreen(TFT_BLACK);
    bf.createSprite(240, 240);
    bf.setTextColor(TFT_WHITE, TFT_BLACK);
    bf.setTextSize(2);

    // 创建一个小缓冲区
    func_bf.createSprite(160, 160);             // 小缓冲区大小为 160x80
    func_bf.setTextColor(TFT_BLACK, TFT_WHITE); // 设置字体颜色为白色，背景为黑色
    func_bf.setTextSize(1);                     // 设置字体大小
    func_bf.setSwapBytes(true);                 // 启用颜色混合

    // 初始化高亮框位置
    highlightY = 10 + currentMenuItem * 20;

    // 初始化宽度
    currentWidth = 0;
    targetWidth = 0;
}

/// @brief 菜单滴答
/// @param void
void Menu_Tick(void)
{
    uint32_t currentTime = millis();
    digitalWrite(led, !digitalRead(led));

    // 检查是否需要更新
    if (currentTime - lastUpdateTime >= updateInterval)
    {
        lastUpdateTime = currentTime;
        Draw_Menu();
        bf.pushSprite(0, 0); // 更新屏幕
    }
}
// ====================================
//             上拉过场动画
// ====================================
// 初始位置设置为屏幕上方
int startX = 40;       // 目标 x 坐标
int startY = 240 + 80; // 初始 y 坐标
int targetY = 60;      // 目标 y 坐标
int currentY = startY; // 当前 y 坐标
void func_bf_SwitchAnimate(void)
{
    // 动画逻辑
    while (currentY > targetY)
    {
        func_bf.fillSprite(TFT_WHITE); // 清屏
        func_bf.setCursor(1, 1);
        func_bf.printf("Example Function");   // 显示初始内容
        func_bf.pushSprite(startX, currentY); // 更新小缓冲区位置
        // 更新 y 坐标
        currentY -= 2; // 每次移动 1 像素
        delay(1);      // 控制动画速度
    }
    startX = 40;       // 目标 x 坐标
    startY = 240 + 80; // 初始 y 坐标
    targetY = 60;      // 目标 y 坐标
    currentY = startY; // 当前 y 坐标
    bf.fillRect(40, 200, 160, 40, TFT_BLACK);
    bf.pushSprite(0, 0);
}
/// @brief 绘制菜单
/// @param void
void Draw_Menu(void)
{
    bf.fillSprite(TFT_BLACK); // 清屏
    // 获取当前菜单信息
    int totalItems = (currentMenuLevel == 0) ? sizeof(menuItems) / sizeof(menuItems[0]) : currentMenu->subMenuCount;
    // 确保起始索引有效
    startIndex = constrain(startIndex, 0, max(totalItems - maxVisibleItems, 0));
    // 绘制可见的菜单项
    for (int i = 0; i < maxVisibleItems; i++)
    {
        int itemIndex = startIndex + i;
        if (itemIndex >= totalItems)
            break;

        int yPos = 10 + i * 20; // 根据可见位置计算Y坐标

        // // 设置文本颜色
        // bf.setTextColor((itemIndex == targetMenuItem) ? TFT_WHITE : TFT_SILVER,
        //                 (itemIndex == targetMenuItem) ? TFT_RED : TFT_BLACK);

        // 绘制菜单文本
        const char *name = (currentMenuLevel == 0) ? menuItems[itemIndex].name : currentMenu->subMenu[itemIndex].name;
        bf.drawString(name, 10, yPos);
    }
    // 计算目标宽度
    int RoundRectWidth = 0;
    if (currentMenuLevel == 0) // 一级菜单
    {
        RoundRectWidth = bf.textWidth(menuItems[targetMenuItem].name);
    }
    else // 二级菜单
    {
        RoundRectWidth = bf.textWidth(currentMenu->subMenu[targetMenuItem].name);
    }
    targetWidth = RoundRectWidth + 3; // 添加左右边距
    // 使用 PID 控制器更新当前宽度
    float width_step = PID_calc(&widthPid, currentWidth, targetWidth);
    currentWidth += width_step;
    // 如果接近目标宽度，则直接设置为目标宽度
    if (abs(currentWidth - targetWidth) < 1)
    {
        currentWidth = targetWidth;
    }
    // 动画更新高亮框位置
    // float targetY = 10 + targetMenuItem * 20;
    int relativePos = targetMenuItem - startIndex;
    float targetHighlightY = 10 + relativePos * 20;
    if (highlightY != targetHighlightY)
    {
        float step = (targetHighlightY - highlightY) * 0.12; // 平滑移动步长
        highlightY += step;

        if (abs(highlightY - targetHighlightY) < 1)
        {
            highlightY = targetHighlightY;
            currentMenuItem = targetMenuItem; // 更新当前菜单项索引
        }
    }
    // 绘制高亮框
    bf.drawRoundRect(9, highlightY, currentWidth, 17, 3, TFT_RED);
    bf.drawRoundRect(8, highlightY - 1, currentWidth, 19, 3, TFT_RED);

    if (totalItems > maxVisibleItems)
    {
        float visibleRatio = (float)maxVisibleItems / totalItems;
        int scrollbarHeight = 240 * visibleRatio;
        scrollbarHeight = constrain(scrollbarHeight, 20, 240); // 最小高度20像素
        int scrollbarPos = map(startIndex, 0, totalItems - maxVisibleItems, 0, 240 - scrollbarHeight);
        bf.fillRoundRect(220, 0, 5, 240, 2, TFT_DARKGREY);
        bf.fillRoundRect(220, scrollbarPos, 5, scrollbarHeight, 2, TFT_WHITE);
    }
}

/// @brief 处理输入
void Menu_Handle_Input()
{
    Menu_Tick();
    // 获取总菜单项数量
    int totalItems = (currentMenuLevel == 0) ? sizeof(menuItems) / sizeof(menuItems[0]) : currentMenu->subMenuCount;
    // 检测 DOWN_PIN 是否被按下
    if (getKeyState(DOWN_PIN) == KEY_PRESS)
    {
        targetMenuItem++;
        if (targetMenuItem >= totalItems)
        {
            targetMenuItem = 0;
            startIndex = 0;
        }
        // 滚动逻辑：当目标项超出可见区域时调整起始索引
        if (targetMenuItem >= startIndex + maxVisibleItems)
        {
            startIndex = targetMenuItem - maxVisibleItems + 1;
        }
    }
    // 检测 UP_PIN 是否被按下
    if (getKeyState(UP_PIN) == KEY_PRESS)
    {
        targetMenuItem--;
        if (targetMenuItem < 0)
        {
            targetMenuItem = totalItems - 1;
            startIndex = max(totalItems - maxVisibleItems, 0); // 确保在项数不足时不会出现负索引
        }

        // 滚动逻辑：当目标项在可见区域上方时调整起始索引
        if (targetMenuItem < startIndex)
        {
            startIndex = targetMenuItem;
        }
    }
    // 检测 ENTER_PIN 是否被按下
    if (getKeyState(ENTER_PIN) == KEY_PRESS)
    {
        if (currentMenuLevel == 0 && menuItems[targetMenuItem].subMenu != NULL)
        {
            // 进入子菜单
            currentMenu = &menuItems[targetMenuItem];
            currentMenuLevel = 1;
            currentMenuItem = 0;
            targetMenuItem = 0;
            startIndex = 0;
            highlightY = 10; // 重置高亮框位置
        }
        else
        {
            // 调用当前选中菜单项的功能函数
            if (currentMenuLevel == 0)
            {
                if (menuItems[targetMenuItem].function != NULL)
                {
                    menuItems[targetMenuItem].function();
                }
            }
            else
            {
                if (currentMenu->subMenu[targetMenuItem].function != NULL)
                {
                    currentMenu->subMenu[targetMenuItem].function();
                }
            }
        }
    }
    Draw_Menu();
}

// 示例功能函数：计数器
void exampleFunction(void)
{
    func_bf_SwitchAnimate();
    // 在目标位置显示计数器
    for (int i = 0; i <= 10; i++)
    {
        func_bf.fillSprite(TFT_WHITE); // 清屏
        func_bf.drawRect(0, 0, 160, 160, TFT_GREEN);
        func_bf.drawRect(1, 1, 158, 158, TFT_GREEN);
        /*USER CODE BEGIN*/
        func_bf.setCursor(5, 5);
        func_bf.printf("Count: %d", i);
        /*USER CODE END*/
        func_bf.pushSprite(startX, targetY); // 固定在目标位置
        delay(200);
    }

    func_bf.fillSprite(TFT_BLACK); // 清空小缓冲区
}

// 返回上一级菜单
void Return(void)
{
    if (currentMenuLevel == 1)
    {
        currentMenu = menuItems; // 返回到一级菜单
        currentMenuLevel = 0;
        currentMenuItem = 0;
        targetMenuItem = 0;
        highlightY = 10; // 重置高亮框位置
    }
    startIndex = 0; // 返回上级时重置滚动位置
}

void about(void)
{
    func_bf_SwitchAnimate();
    while (1)
    {
        func_bf.fillSprite(TFT_WHITE); // 清屏
        func_bf.drawRect(0, 0, 160, 160, TFT_GREEN);
        func_bf.drawRect(1, 1, 158, 158, TFT_GREEN);
        /*USER CODE BEGIN*/
        func_bf.drawString("Version:0.1.0", 3, 5);
        func_bf.drawString("Author:Wang Mingzhao-AUST", 3, 15);
        func_bf.drawString("Date:2025/3/8", 3, 25);
        /*USER CODE END*/
        func_bf.pushSprite(startX, targetY); // 固定在目标位置
        if (getKeyState(ENTER_PIN) == KEY_PRESS)
            break;
    }
    func_bf.fillSprite(TFT_BLACK); // 清空小缓冲区
}

void BLK_config()
{
    func_bf_SwitchAnimate();
    uint8_t BLK = 0;
    static float BLK_rates = 1; // 初始亮度比例
    while (1)
    {
        func_bf.fillSprite(TFT_WHITE); // 清屏
        func_bf.drawRect(0, 0, 160, 160, TFT_GREEN);
        func_bf.drawRect(1, 1, 158, 158, TFT_GREEN);
        /*USER CODE BEGIN*/
        KEY_STATE upState = getKeyState(UP_PIN);       // 获取 UP 按键状态
        KEY_STATE downState = getKeyState(DOWN_PIN);   // 获取 DOWN 按键状态
        KEY_STATE enterState = getKeyState(ENTER_PIN); // 获取 ENTER 按键状态

        // 处理 UP 按键事件
        if (upState == KEY_PRESS || upState == KEY_LONG_PRESS)
        {
            float step = (upState == KEY_PRESS) ? 0.1 : 0.01; // 按下为 0.1，长按为 0.01
            BLK_rates += step;
        }

        // 处理 DOWN 按键事件
        if (downState == KEY_PRESS || downState == KEY_LONG_PRESS)
        {
            float step = (downState == KEY_PRESS) ? 0.1 : 0.01; // 按下为 0.1，长按为 0.01
            BLK_rates -= step;
        }

        // 限制 BLK_rates 的范围
        if (BLK_rates < 0.1)
            BLK_rates = 0.1;
        else if (BLK_rates > 1)
            BLK_rates = 1;

        // 更新 PWM 输出和环形亮度圆圈
        func_bf.drawSmoothArc(80, 80, 70, 60, 0, (uint32_t)360 * BLK_rates, TFT_GOLD, TFT_WHITE, true);
        func_bf.setCursor(72, 75);
        func_bf.printf("%.0f", BLK_rates * 100);
        analogWrite(BLK, (uint8_t)(BLK_rates * 255));
        /*USER CODE END*/
        func_bf.pushSprite(startX, targetY); // 固定在目标位置
        if (enterState == KEY_PRESS)
            break;
    }
    func_bf.fillSprite(TFT_BLACK); // 清空小缓冲区
}

/// @brief 打印开机时间
void Date_Time()
{
    // WiFi 和 NTP 配置
    const char *ssid = "3D-bambu";          // 网络名称
    const char *password = "hua12345";      // 网络密码
    const char *ntpServer = "pool.ntp.org"; // NTP 服务器地址
    const long gmtOffset_sec = 8 * 3600;    // 北京时间偏移量（+8 小时）
    const int daylightOffset_sec = 0;       // 夏令时偏移量

    // 初始化 WiFi 和 NTP
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(10); // 等待 WiFi 连接
    }
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    // 等待 NTP 时间同步完成
    struct tm timeinfo;
    while (!getLocalTime(&timeinfo))
    {
        delay(10); // 等待时间同步
    }
    // 断开 WiFi 连接
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    func_bf_SwitchAnimate();

    while (1)
    {
        func_bf.fillSprite(TFT_WHITE); // 清屏
        func_bf.drawRect(0, 0, 160, 160, TFT_GREEN);
        func_bf.drawRect(1, 1, 158, 158, TFT_GREEN);

        // 获取系统运行时间（毫秒）
        uint32_t uptimeMillis = millis();
        uint32_t seconds = uptimeMillis / 1000;
        uint32_t days = seconds / 86400; // 一天有 86400 秒
        seconds %= 86400;                // 剩余秒数
        uint32_t hours = seconds / 3600; // 一小时有 3600 秒
        seconds %= 3600;                 // 剩余秒数
        uint32_t minutes = seconds / 60; // 一分钟有 60 秒
        seconds %= 60;                   // 剩余秒数
        // 显示开机时间
        func_bf.setCursor(5, 5);
        func_bf.printf("Time: %u d %02u h %02u m %02u s", days, hours, minutes, seconds);
        // 获取当前时间
        if (getLocalTime(&timeinfo))
        {
            func_bf.setCursor(5, 15);
            func_bf.printf("Date: %04d-%02d-%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
            func_bf.setCursor(5, 25);
            func_bf.printf("Time: %02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        }
        else
        {
            func_bf.setCursor(5, 30);
            func_bf.printf("Failed to fetch time");
        }
        func_bf.pushSprite(startX, targetY); // 固定在目标位置
        // 检测按键退出
        if (getKeyState(ENTER_PIN) == KEY_PRESS)
            break;
    }
    func_bf.fillSprite(TFT_BLACK); // 清空小缓冲区
}