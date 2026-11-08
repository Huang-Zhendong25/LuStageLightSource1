
#include "stdlib.h"
#include "board_i2c.h"
#include "tm1650.h" 


/*********************************************************************
 * LOCAL VARIABLES
 */                                     
static uint8_t s_7number[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};   // 7段显示方式0~9
//static uint8_t s_7number[10] = {0x3F,0x30,0x5B,0x79,0x74,0x6D,0x6F,0x38,0x7F,0x7D};   // 7段显示方式0~9
static uint8_t s_8number[10] = {0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF};   // 8段显示方式0~9

//static uint8_t s_7number[10] = {0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};   // 7段显示方式0~9 共阳极
////static uint8_t s_7number[10] = {0x3F,0x30,0x5B,0x79,0x74,0x6D,0x6F,0x38,0x7F,0x7D};   // 7段显示方式0~9 共阳极
//static uint8_t s_8number[10] = {0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF};   // 8段显示方式0~9   共阳极

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 @brief 写显存
 @param addr -[in] 写入地址
 @param data -[in] 写入数据
 @return 无
*/
void TM1650_Write(uint8_t addr, uint8_t data)
{		   
    IIC_Start();                                                       // 发送起始信号
    IIC_SendByte(addr);                                                // 写地址
    IIC_WaitAck();                                                     // 等待应答
    IIC_SendByte(data);                                                // 写入数据
    IIC_Ack();                                                         // 发送应答
    IIC_Stop();                                                       // 发送停止信号   
}

/**
 @brief 显示设置
 @param brightness -[in] 亮度1~8级
 @param mode -[in] 7/8段显示方式
 @param state -[in] 开启/关闭显示
 @return 无
*/
void TM1650_SetDisplay(uint8_t brightness, uint8_t mode, uint8_t state)
{		
    if(state)
    {
        if(mode == 7)                                                   // 7段显示方式
        {
            TM1650_Write(0x48, brightness*16 + 1*8 + 1);
        }
        else if(mode == 8)												// 8段显示方式
        {
            TM1650_Write(0x48, brightness*16 + 1);                      
        }
    }
    else
    {
        TM1650_Write(0x48, 0x00);                                       // 关闭显示
    }
}

/**
 @brief 显示数字
 @param index -[in] 第几个数
 @param mode -[in] 7/8段显示方式
 @param num -[in] 显示数
 @return 无
*/
void TM1650_SetNumber(uint8_t index, uint8_t mode, uint8_t num)
{
    uint8_t indexAddr = 0;
    uint8_t numValue = 0;
	
    if(index == 1)
    {
        indexAddr = 0x68;
    }
    else if(index == 2)
    {
        indexAddr = 0x6A;
    }
    else if(index == 3)
    {
        indexAddr = 0x6C;
    }
    else if(index == 4)
    {
        indexAddr = 0x6E;
    }
		
//		 if(index == 1)
//    {
//        indexAddr = 0x6C;
//    }
//    else if(index == 2)
//    {
//        indexAddr = 0x6A;
//    }
//    else if(index == 3)
//    {
//        indexAddr = 0x68;
//    }
//    else if(index == 4)
//    {
//        indexAddr = 0x6E;
//    }
	
    if(mode == 7)                                                   	// 7段显示方式
    {
        numValue = s_7number[num];
    }
    else if(mode == 8)													// 8段显示方式
    {
        numValue = s_8number[num];                      
    }
	
//    printf("index:%02x, num:%02x\n", indexAddr, numValue);
    TM1650_Write(indexAddr, numValue);
}
void Display_update(uint8_t mode,uint16_t ID_data,uint8_t state_display)
{
 	uint8_t hum_kilo = 0;          
	uint8_t hum_hundred = 0;   
	uint8_t hum_ten = 0;       
	uint8_t hum_unit = 0;      
	hum_kilo = ID_data/1000;           //千位 
	hum_hundred = (ID_data/100)%10;    //百位
	hum_ten = (ID_data/10)%10;         //十位
	hum_unit = ID_data%10;             //个位
	TM1650_SetNumber(4,mode,hum_unit);
	TM1650_SetNumber(3,mode,hum_ten);
	TM1650_SetNumber(2,mode,hum_hundred);
	TM1650_SetNumber(1,mode,hum_kilo);
	TM1650_SetDisplay(brightness_8,mode,state_display);
}



/****************************************************END OF FILE****************************************************/




