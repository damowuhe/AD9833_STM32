#include "key.h"

int k = 1000;
int fre = 10;


void lcd_final(void)
{
    char freq_str[20]; // 定义一个缓冲区存放转换后的字符串
    // 将数字转换为字符串
    // %d 表示十进制整数
    sprintf(freq_str, "%d   ", fre); 
    
    // 在 "fre:" 后面显示具体的数值
    // 坐标 x=160 (在 "fre:" 之后), y=180 (与 "fre:" 同一行)
    LCD_ShowString(160, 180, freq_str, BROWN , WHITE, 16, 0);
}

void key_scan(void)
{
    
    if (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET)
    {
        HAL_Delay(1); // 软件消抖
        if (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET)
        {
            fre += 1; // k 加 10

            // 等待按键松开，防止按一次加多次
            while (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET);
            
            // 更新波形设置
            AD9833_WaveSeting(fre*k, 0, SIN_WAVE, 0);
        }
    }

    /*--- KEY2: 减少 10 ---*/
    if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)
    {
        HAL_Delay(1); // 软件消抖
        if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)
        {
            if (fre >= 1) {
                fre -= 1; // k 减 10
            } else {
                fre = 1; // 防止减成负数（因为k是uint32_t无符号类型）
            }

            // 等待按键松开
            while (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET);
            
            // 更新波形设置
            AD9833_WaveSeting(fre*k, 0, SIN_WAVE, 0);
        }
    }

}

