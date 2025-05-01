# ESP32C3 SuperMini TFT Menu

一个基于Arduino固件的嵌入式菜单系统（测试性，随心情开发中），目前只支持二级菜单导航、平滑动画和交互式控制，适用于ESP32C3等微控制器平台。
（要是我有ESP32S3 Mini的话，我会选用至少2.8寸的LCD屏，使用8MB的PSRAM）


## 特性

- 🖥️ TFT显示屏驱动（支持240x240分辨率）:
  - 主界面使用全缓冲区
  - 子界面单独使用160x160的缓冲区
- 🎮 三按键输入控制（上/下/确认）：
  - 非阻塞性按键消抖处理(15ms消抖时间)
- 📜 多级菜单系统（支持返回功能）
- 🌀 平滑动画效果：
  - 菜单高亮框PID控制移动
  - 子菜单切换滑动动画
  - 动态宽度调整效果
- ⚙️ 可扩展功能框架：
  - 计数器示例
  - 系统信息显示
  - 亮度/音量等多设置项占位

## 缺点

- 目前只有二级菜单，不过一级菜单和二级菜单都支持添加很多个项目

## 文件结构
```txt
├── lib
│   ├── MyUI        # 菜单核心逻辑
│   │   ├── menu.cpp
│   │   └── menu.h
│   ├── PID         # PID控制器实现
│   └── Key         # 按键处理模块
├── src
│   └── main.cpp    # 主程序入口
└── platformio.ini
```
## 硬件要求

- 主控板：ESP32C3 SuperMini 或兼容板
- 显示屏：1.3寸 TFT LCD（ST7789驱动）
- 输入设备：3个轻触开关

## 软件依赖

- PlatformIO (推荐) 或 Arduino IDE

## 安装指南

1. 克隆仓库(或者下载.zip文件):
```bash
git clone https://github.com/yourusername/arduino-tft-menu.git
```
2. 用PlatformIO或Arduino IDE打开项目。

## 特别致谢
- TFT_eSPI库作者Bodmer
- PlatformIO开发团队
- Arduino社区支持
