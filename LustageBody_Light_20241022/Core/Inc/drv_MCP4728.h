#ifndef __drv_MCP4728_H
#define __drv_MCP4728_H

#include "main.h"
#include "user.h"
/* MCP4728 configer struct */
typedef struct
{
	I2C_HandleTypeDef  *MCP4728_I2C;
	uint8_t            MCP4728_DevAddress;
}MCP4728_Config_TypeDef;




 //MCP4728写地址 0XC0   读地址  0XC1
 #define MCP4728I2C_W_addres 0xC0 
 #define MCP4728I2C_R_addres 0xC1 

 //MCP4728各通道工作模式
 #define normal_model  0x80    // 1000000   VREF=1(内部基准2.048V),PD1=0,PD0=0(正常模式),GX=0(GAIN=1)

 //MCP4728寄存器写入模式
 #define Sequential_Write  0x50    // 01010000   C2=0,C1=1,C0=0,W1=1,W0=0,DAC1=0,DAC0=0,U/D/A/C/=0 连续顺序写入模式
void MCP4728_Set_Value(MCP4728_Config_TypeDef *pHandler_Config, Power_Data_TypeDef *pHandler_Data);
void LED_Power_Updata(void);
#endif
