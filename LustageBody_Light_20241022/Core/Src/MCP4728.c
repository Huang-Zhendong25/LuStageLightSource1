#include "main.h"
#include "user.h"
#include "drv_mcp4728.h"

#define MCP4728_ADDR	0xC0




void MCP4728_Set_Value(MCP4728_Config_TypeDef *pHandler_Config, Power_Data_TypeDef *pHandler_Data)
{
	uint8_t tx_buff[9];
	uint16_t Data_A,Data_B,Data_C,Data_D;
//	Data_A = pHandler_Data->Power_A*2480/255;          //将12位DAC转化为8位，且输出最大为2480对应1.24V
//	Data_B = pHandler_Data->Power_B*2480/255;          //将12位DAC转化为8位，且输出最大为2480对应1.24V
//	Data_C = pHandler_Data->Power_C*2480/255;          //将12位DAC转化为8位，且输出最大为2480对应1.24V
//	Data_D = pHandler_Data->Power_D*2480/255;          //将12位DAC转化为8位，且输出最大为2480对应1.24V
	Data_A = pHandler_Data->Power_A;                     //12位DAC最大为2480对应1.24V
	Data_B = pHandler_Data->Power_B;                     //12位DAC最大为2480对应1.24V
	Data_C = pHandler_Data->Power_C;                     //12位DAC最大为2480对应1.24V
	Data_D = pHandler_Data->Power_D;                     //12位DAC最大为2480对应1.24V
	tx_buff[0] = Sequential_Write;     //顺序写入
	tx_buff[1] = ((Data_A>>8)&0x0F)| normal_model;    //取高4位并与模式位组成一个字节
	tx_buff[2] = Data_A & 0xFF;
	tx_buff[3] = ((Data_B>>8)&0x0F)| normal_model;    //取高4位并与模式位组成一个字节
	tx_buff[4] = Data_B & 0xFF;
	tx_buff[5] = ((Data_C>>8)&0x0F)| normal_model;    //取高4位并与模式位组成一个字节
	tx_buff[6] = Data_C & 0xFF;
	tx_buff[7] = ((Data_D>>8)&0x0F)| normal_model;    //取高4位并与模式位组成一个字节
	tx_buff[8] = Data_D & 0xFF;
	HAL_I2C_Master_Transmit(pHandler_Config->MCP4728_I2C,pHandler_Config->MCP4728_DevAddress,tx_buff,9,10);
}







