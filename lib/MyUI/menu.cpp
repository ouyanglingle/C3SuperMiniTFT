#include <menu.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite bf = TFT_eSprite(&tft);
TFT_eSprite func_bf = TFT_eSprite(&tft);
uint8_t led = 8;

typedef struct MenuItem MenuItem;
pid_type_def highlightPid = {0};

struct MenuItem
{
    const char *name;   // 菜单项名称
    void (*function)(); // 菜单项对应的函数
    MenuItem *subMenu;  // 子菜单数组
    int subMenuCount;   // 子菜单数量
};

// 定义菜单项
MenuItem settingsSubMenu[] = {
    {"Brightness", exampleFunction, NULL, 0},
    {"Volume", exampleFunction, NULL, 0},
    {"Language", exampleFunction, NULL, 0},
    {"Time", exampleFunction, NULL, 0},
    {"Counter", exampleFunction, NULL, 0},
    {"Return", Return, NULL, 0}, // 返回上一级菜单
};

MenuItem menuItems[] = {
    {"Main Menu", NULL, NULL, 0},
    {"Settings", NULL, settingsSubMenu, sizeof(settingsSubMenu) / sizeof(settingsSubMenu[0])},
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
// ===============================
//       复选框动画PID参数
// ===============================
float currentWidth = 0;      // 当前宽度
float targetWidth = 0;       // 目标宽度
pid_type_def widthPid = {0}; // 宽度的 PID 控制器

void Menu_Init(void)
{   
    // 初始化宽度的 PID
    PID_init(&widthPid, PID_POSITION, (const fp32[]){0.5, 0.1, 0.05}, 10.0f, 10.0f);

    pinMode(led, OUTPUT);
    tft.init();
    tft.fillScreen(TFT_BLACK);
    bf.createSprite(240, 240);
    bf.setTextColor(TFT_WHITE, TFT_BLACK);
    bf.setTextSize(2);

    // 创建一个小缓冲区
    func_bf.createSprite(160, 160);             // 小缓冲区大小为 160x80
    func_bf.setTextColor(TFT_BLACK, TFT_WHITE); // 设置字体颜色为白色，背景为黑色
    func_bf.setTextSize(1);                     // 设置字体大小

    // 初始化高亮框位置
    highlightY = 10 + currentMenuItem * 20;

    // 初始化宽度
    currentWidth = 0;
    targetWidth = 0;
}

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
void Draw_Menu(void)
{
    bf.fillSprite(TFT_BLACK); // 清屏

    // 绘制菜单项
    for (int i = 0; i < (currentMenuLevel == 0 ? sizeof(menuItems) / sizeof(menuItems[0]) : currentMenu->subMenuCount); i++)
    {
        int yPos = 10 + i * 20; // 每个菜单项垂直间距为 20 像素
        if (i == currentMenuItem)
        {
            bf.setTextColor(TFT_WHITE, TFT_RED); // 高亮当前选中的菜单项
        }
        else
        {
            bf.setTextColor(TFT_WHITE, TFT_BLACK); // 普通菜单项
        }

        // 根据当前菜单层级绘制菜单项
        if (currentMenuLevel == 0)
        {
            bf.drawString(menuItems[i].name, 10, yPos);
        }
        else
        {
            bf.drawString(currentMenu->subMenu[i].name, 10, yPos);
        }
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
    float step = PID_calc(&widthPid, currentWidth, targetWidth);
    currentWidth += step;

    // 如果接近目标宽度，则直接设置为目标宽度
    if (abs(currentWidth - targetWidth) < 1)
    {
        currentWidth = targetWidth;
    }
    // 动画更新高亮框位置
    float targetY = 10 + targetMenuItem * 20;
    if (highlightY != targetY)
    {
        float step = (targetY - highlightY) * 0.1; // 平滑移动步长
        highlightY += step;

        if (abs(highlightY - targetY) < 1)
        {
            highlightY = targetY;
            currentMenuItem = targetMenuItem; // 更新当前菜单项索引
        }
    }
    // 绘制高亮框
    bf.drawRoundRect(9, highlightY, currentWidth, 17, 3, TFT_RED);
    bf.drawRoundRect(8, highlightY - 1, currentWidth, 19, 3, TFT_RED);
}

// 处理按键输入
void Handle_Input()
{
    Menu_Tick();

    // 检测 DOWN_PIN 是否被按下
    if (getKeyState(DOWN_PIN) == 1)
    {
        // 移动到下一个菜单项
        targetMenuItem++;
        if (currentMenuLevel == 0 && targetMenuItem >= sizeof(menuItems) / sizeof(menuItems[0]))
        {
            targetMenuItem = 0; // 循环回到第一个菜单项
        }
        else if (currentMenuLevel == 1 && targetMenuItem >= currentMenu->subMenuCount)
        {
            targetMenuItem = 0; // 循环回到第一个子菜单项
        }
    }
    // 检测 UP_PIN 是否被按下
    if (getKeyState(UP_PIN) == 1)
    {
        // 移动到上一个菜单项
        targetMenuItem--;
        if (currentMenuLevel == 0 && targetMenuItem < 0)
        {
            targetMenuItem = sizeof(menuItems) / sizeof(menuItems[0]) - 1; // 循环回到最后一个菜单项
        }
        else if (currentMenuLevel == 1 && targetMenuItem < 0)
        {
            targetMenuItem = currentMenu->subMenuCount - 1; // 循环回到最后一个子菜单项
        }
    }
    // 检测 ENTER_PIN 是否被按下
    if (getKeyState(ENTER_PIN) == 1)
    {
        if (currentMenuLevel == 0 && menuItems[currentMenuItem].subMenu != NULL)
        {
            // 进入子菜单
            currentMenu = &menuItems[currentMenuItem];
            currentMenuLevel = 1;
            currentMenuItem = 0;
            targetMenuItem = 0;
            highlightY = 10; // 重置高亮框位置
        }
        else
        {
            // 调用当前选中菜单项的功能函数
            if (currentMenuLevel == 0)
            {
                if (menuItems[currentMenuItem].function != NULL)
                {
                    menuItems[currentMenuItem].function();
                }
            }
            else
            {
                if (currentMenu->subMenu[currentMenuItem].function != NULL)
                {
                    currentMenu->subMenu[currentMenuItem].function();
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
        func_bf.fillSprite(TFT_BLACK);       // 清空小缓冲区
        if (getKeyState(ENTER_PIN) == 1)
        {
            break;
        }
    }
    func_bf.fillSprite(TFT_BLACK); // 清空小缓冲区
}