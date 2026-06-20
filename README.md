# AD9833 DDS 信号发生器 (STM32F103C8T6)

基于 STM32F103C8T6 微控制器和 AD9833 波形发生器芯片的 DDS（直接数字频率合成）信号发生器，配有 SPI 液晶屏和按键交互界面。

## 功能特性

- **波形输出**：通过 AD9833 生成正弦波、三角波和方波
- **频率可调**：通过板载按键实时调节输出频率
- **液晶显示**：240×280 SPI 液晶屏，显示频率、波形等信息
- **中文支持**：LCD 驱动支持显示汉字（12×12、16×16、24×24、32×32）
- **CMake 构建**：基于 CMake 的现代化项目结构，由 STM32CubeMX 生成
- **DMA 驱动 SPI**：SPI1 配合 DMA 实现高效的液晶屏通信

## 硬件配置

| 组件             | 说明                                |
|------------------|-------------------------------------|
| **主控**         | STM32F103C8T6（LQFP48，72 MHz）     |
| **DDS 芯片**     | AD9833（25 MHz 外部晶振）           |
| **显示屏**       | 240×280 SPI TFT 液晶屏              |
| **按键**         | 4 个轻触按键（KEY1–KEY4）           |
| **晶振**         | 8 MHz HSE，PLL 倍频 → 72 MHz SYSCLK |

## 引脚连接

### AD9833（GPIOB 模拟 SPI）

| 信号   | 引脚  | 说明              |
|--------|------|-------------------|
| FSYNC  | PB15 | 帧同步 / 片选     |
| SCLK   | PB14 | 串行时钟          |
| SDATA  | PB13 | 串行数据          |

### LCD（SPI1 + GPIO 控制）

| 信号   | 引脚  | 说明              |
|--------|------|-------------------|
| SCK    | PB3  | SPI1 时钟         |
| MOSI   | PB5  | SPI1 数据         |
| CS     | PA11 | 片选              |
| DC     | PA12 | 数据/命令         |
| RES    | PA15 | 复位              |
| BLK    | PA10 | 背光控制          |

### 按键（低电平有效，内部上拉）

| 按键  | 引脚  | 功能              |
|-------|------|-------------------|
| KEY1  | PA5  | 预留              |
| KEY2  | PA6  | 频率减小          |
| KEY3  | PA7  | 频率增大          |
| KEY4  | PB0  | 预留              |

## 编译

### 环境要求

- **ARM GCC 工具链**（`arm-none-eabi-gcc`）
- **CMake** ≥ 3.22
- **STM32CubeMX**（可选，用于重新生成 HAL 代码）
- **OpenOCD**（用于烧录/调试）

### 编译步骤

```bash
# 配置
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# 编译
cmake --build build
```

### 烧录

```bash
# 使用 OpenOCD（已提供配置文件）
openocd -f openocd.cfg -c "program build/AD9833.elf verify reset exit"
```

## 项目结构

```
├── CMakeLists.txt              # 顶层 CMake 构建文件
├── CMakePresets.json           # CMake 预设配置
├── AD9833.ioc                  # STM32CubeMX 项目文件
├── openocd.cfg                 # OpenOCD 调试器配置
├── startup_stm32f103xb.s       # 启动汇编文件
├── STM32F103XX_FLASH.ld        # 链接脚本
├── Core/                       # STM32CubeMX 生成的核心代码
│   ├── Inc/                    #   主程序、GPIO、SPI、DMA 头文件
│   └── Src/                    #   main.c、HAL MSP、中断服务、系统调用
├── Drivers/                    # STM32 HAL 和 CMSIS 库
│   ├── CMSIS/
│   └── STM32F1xx_HAL_Driver/
├── Headerwares/                # 自定义外设驱动
│   ├── ad9833.c/.h             #   AD9833 DDS 驱动
│   ├── key.c/.h                #   按键扫描和界面逻辑
│   └── LCD/                    #   SPI 液晶屏驱动及字库
│       ├── lcd.c/.h            #     绘图、字符串、汉字显示
│       ├── lcd_init.c/.h       #     液晶初始化、寄存器级控制
│       ├── lcdfont.h           #     字符字模数据
│       └── pic.h               #     嵌入式图片数据
├── cmake/                      # CMake 子目录（STM32CubeMX）
└── build/                      # 编译输出目录
```

## 使用说明

1. 上电后，液晶屏自动初始化并显示界面
2. AD9833 默认输出 10 kHz 正弦波
3. **KEY3**（PA7）：增大频率（步进 +1 kHz）
4. **KEY2**（PA6）：减小频率（步进 -1 kHz）
5. 液晶屏实时刷新当前频率值

### 波形选择

AD9833 驱动支持三种波形模式（定义在 `ad9833.h` 中）：

- `SIN_WAVE`（1）— 正弦波输出
- `TRI_WAVE`（0）— 三角波输出
- `SQU_WAVE`（2）— 方波输出

## AD9833 驱动 API

```c
// 初始化 AD9833，默认设置（FREQ0 输出 1 kHz 正弦波）
void ad9833_init(void);

// 配置波形输出
//  Freq      — 输出频率（单位 Hz）
//  Freq_SFR  — 频率寄存器选择（0 或 1）
//  WaveMode  — 波形模式：SIN_WAVE / TRI_WAVE / SQU_WAVE
//  Phase     — 相位偏移值
void AD9833_WaveSeting(double Freq, unsigned int Freq_SFR,
                       unsigned int WaveMode, unsigned int Phase);

// 快捷波形切换函数
void wave_out(void);           // FREQ1 → 默认 2 kHz
void triangle_wave_out(void);  // FREQ0 → 三角波
void square_wave_out(void);    // FREQ0 → 方波
```

## 作者

- **朱一凡（damowuhe）**
- 邮箱：odin1415@outlook.com

## 许可证

本项目基于 STM32 HAL 库构建（版权归 STMicroelectronics 所有）。自定义驱动和应用代码按原样提供。
