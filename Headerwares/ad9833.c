#include "ad9833.h"

/*Author:   @朱一凡(damo)(shui)
 *time:     2025.12.12
 *version:  V1.0
 *library:  ad9833_moudle        */
 
//向AD9833写16位数据
//---------------------------------------------------//
void write_ad9833_d16(unsigned int data)
{
    uint8_t i;
    uint16_t write_data = (uint16_t)data;
    HAL_GPIO_WritePin(GPIOB, SCLK_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, FSYNC_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOB, FSYNC_Pin, GPIO_PIN_RESET);
    //写16位数据
    for(i=0;i<16;i++)
    {
    // 设置SDATA引脚电平（取当前最高位）
        if(write_data & 0x8000)  //0x8000是16位数据的最高位掩码
        {
            HAL_GPIO_WritePin(GPIOB, SDATA_Pin, GPIO_PIN_SET);
        }
        else
        {
            HAL_GPIO_WritePin(GPIOB, SDATA_Pin, GPIO_PIN_RESET);
        }
        // SCLK产生下降沿+上升沿，锁存当前位数据
        HAL_GPIO_WritePin(GPIOB, SCLK_Pin, GPIO_PIN_RESET);
        HAL_Delay(1);  // 可选：延时保证时钟宽度（根据AD9833时钟频率调整，如无需可删）
        HAL_GPIO_WritePin(GPIOB, SCLK_Pin, GPIO_PIN_SET);
        HAL_Delay(1);  // 可选：延时保证时钟宽度

        // 数据左移，准备下一位
        write_data <<= 1;
    }
     HAL_GPIO_WritePin(GPIOB, FSYNC_Pin, GPIO_PIN_SET);
     HAL_GPIO_WritePin(GPIOB, SCLK_Pin, GPIO_PIN_RESET);
}

void ad9833_init(void)
{
    HAL_Delay(50);
    //寄存器频率0输出1KHz波形
    write_ad9833_d16(0x0100); //复位AD9833
    write_ad9833_d16(0x2100); //选择数据一次写入
    write_ad9833_d16(0x69F1); //L14
    write_ad9833_d16(0x4000); //H14 频率寄存器0为1KHz
    write_ad9833_d16(0x93E3); //L14
    write_ad9833_d16(0x8001); //H14 频率寄存器1为2KHz
    
    write_ad9833_d16(0x2000); //芯片进入工作状态,寄存器频率0输出1KHz波形
}

void AD9833_WaveSeting(double Freq,unsigned int Freq_SFR,unsigned int WaveMode,unsigned int Phase )
{
		int frequence_LSB,frequence_MSB,Phs_data;
		double   frequence_mid,frequence_DATA;
		long int frequence_hex;

		/*********************************计算频率的16进制值***********************************/
		frequence_mid=268435456/25;//适合25M晶振
		//如果时钟频率不为25MHZ，修改该处的频率值，单位MHz ，AD9833最大支持25MHz
		frequence_DATA=Freq;
		frequence_DATA=frequence_DATA/1000000;
		frequence_DATA=frequence_DATA*frequence_mid;
		frequence_hex=frequence_DATA;  //这个frequence_hex的值是32位的一个很大的数字，需要拆分成两个14位进行处理；
		frequence_LSB=frequence_hex; //frequence_hex低16位送给frequence_LSB
		frequence_LSB=frequence_LSB&0x3fff;//去除最高两位，16位数换去掉高位后变成了14位
		frequence_MSB=frequence_hex>>14; //frequence_hex高16位送给frequence_HSB
		frequence_MSB=frequence_MSB&0x3fff;//去除最高两位，16位数换去掉高位后变成了14位

		Phs_data=Phase|0xC000;	//相位值
		write_ad9833_d16(0x0100); //复位AD9833,即RESET位为1
		write_ad9833_d16(0x2100); //选择数据一次写入，B28位和RESET位为1

		if(Freq_SFR==0)				  //把数据设置到设置频率寄存器0
		{
		 	frequence_LSB=frequence_LSB|0x4000;
		 	frequence_MSB=frequence_MSB|0x4000;
			 //使用频率寄存器0输出波形
			write_ad9833_d16(frequence_LSB); //L14，选择频率寄存器0的低14位数据输入
			write_ad9833_d16(frequence_MSB); //H14 频率寄存器的高14位数据输入
			write_ad9833_d16(Phs_data);	//设置相位
			//AD9833_Write(0x2000); /**设置FSELECT位为0，芯片进入工作状态,频率寄存器0输出波形**/
	    }
		if(Freq_SFR==1)				//把数据设置到设置频率寄存器1
		{
			 frequence_LSB=frequence_LSB|0x8000;
			 frequence_MSB=frequence_MSB|0x8000;
			//使用频率寄存器1输出波形
			write_ad9833_d16(frequence_LSB); //L14，选择频率寄存器1的低14位输入
			write_ad9833_d16(frequence_MSB); //H14 频率寄存器1为
			write_ad9833_d16(Phs_data);	//设置相位
			//AD9833_Write(0x2800); /**设置FSELECT位为0，设置FSELECT位为1，即使用频率寄存器1的值，芯片进入工作状态,频率寄存器1输出波形**/
		}

		if(WaveMode==TRI_WAVE) //输出三角波波形
		 	write_ad9833_d16(0x2002); 
		if(WaveMode==SQU_WAVE)	//输出方波波形
			write_ad9833_d16(0x2028); 
		if(WaveMode==SIN_WAVE)	//输出正弦波形
			write_ad9833_d16(0x2000); 

}

//寄存器频率1输出2KHz波形
void wave_out(void)
{
    HAL_Delay(50);
    write_ad9833_d16(0x2800);
}

//寄存器频率0输出1KHz三角波波形
void triangle_wave_out(void)
{
    HAL_Delay(50);
    write_ad9833_d16(0x2002);
}
//寄存器频率0输出1KHz方波波形
void square_wave_out(void)
{
    HAL_Delay(50);
    write_ad9833_d16(0x2020);
}


