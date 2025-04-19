// // #include <TFT_eSPI.h>
// // #include <key.h>
// // #include <Ticker.h>

// // // ======================
// // // 3D渲染相关配置
// // // ======================
// // TFT_eSPI tft;
// // TFT_eSprite bf(&tft);
// // int count = 0;
// // Ticker ticker;
// // uint8_t led = 8;
// // void setup()
// // {
// //     pinMode(led, OUTPUT);
// //     tft.init();
// //     tft.fillScreen(TFT_WHITE);
// //     bf.createSprite(240, 240);
// //     bf.setTextColor(TFT_WHITE, TFT_BLACK);
// //     bf.setSwapBytes(true); // 启用颜色混合
// //     ticker.attach_ms(10, [](){ count++; });
// // }

// // void loop()
// // {
// //     // 显示传感器数据
// //     static uint32_t lastTime = 0;
// //     static int i = 0;
// //     bf.drawArc(20, 20, 20, 15, 0, i, TFT_RED, TFT_BLACK);
// //     i++;
// //     if (i > 360)
// //         i = 0;
// //     float deltaTime = (millis() - lastTime) / 1000.0f;
// //     lastTime = millis();
// //     float fps = 1.0f / deltaTime;
// //     bf.setCursor(5, 20);
// //     bf.printf("FPS:%d", (int)fps);
// //     digitalWrite(led, !digitalRead(led));
// //     bf.pushSprite(0, 0);
// //     if (millis() - lastTime > 2)
// //     {
// //         bf.fillScreen(TFT_BLACK);
// //     }
// // }

// #include <TFT_eSPI.h>
// TFT_eSPI tft;
// TFT_eSprite buf1(&tft), buf2(&tft);  // 双缓冲
// TFT_eSprite* activeBuf = &buf1;
// uint32_t lastFrameTime = 0;
// const uint16_t targetFPS = 60;
// // 定义 renderFrame 函数
// void renderFrame(TFT_eSprite& buffer, int angle) {
//     buffer.fillScreen(TFT_BLACK);  // 清屏
//     buffer.setTextColor(TFT_WHITE, TFT_BLACK);
//     buffer.setSwapBytes(true);  // 启用颜色混合

//     // 绘制一个动态弧线
//     buffer.drawArc(120, 120, 100, 80, 0, angle, TFT_RED, TFT_BLACK);

//     // 显示帧率信息
//     float deltaTime = (millis() - lastFrameTime) / 1000.0f;
//     float fps = 1.0f / deltaTime;
//     buffer.setCursor(5, 20);
//     buffer.printf("FPS:%d", (int)fps);
// }
// void setup() {
//     tft.init();
//     buf1.createSprite(240, 240);
//     buf2.createSprite(240, 240);
// }

// void loop() {
//     static int angle = 0;
//     angle = (angle + 1) % 360;

//     // 在非活动缓冲区绘制
//     TFT_eSprite* renderBuf = (activeBuf == &buf1) ? &buf2 : &buf1;
//     renderFrame(*renderBuf, angle);

//     // 推送并切换缓冲区
//     renderBuf->pushSprite(0, 0);
//     activeBuf = renderBuf;

//     // 控制帧率
//     while (millis() - lastFrameTime < (1000 / targetFPS));
//     lastFrameTime = millis();
// }

#include <menu.h>

void setup()
{
    pinMode(7, OUTPUT);
    pinMode(10, OUTPUT);
    digitalWrite(7, LOW);
    digitalWrite(10, LOW);

    Key_Init();
    Menu_Init();
    Draw_Menu();
}

void loop()
{
    Handle_Input();
}