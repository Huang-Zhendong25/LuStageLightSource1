#ifndef __DRV_18B20_H
#define __DRV_18B20_H

#include "main.h"

typedef struct
{
	GPIO_TypeDef  *GPIOx;
	uint16_t      GPIO_Pin;
}_18B20_Config_TypeDef;

typedef struct
{
	uint8_t LS_Byte     ;
	uint8_t MS_Byte     ;
	uint8_t TH_Byte     ;
	uint8_t TL_Byte     ;
	uint8_t Config_Byte ;
	uint8_t RES_1_Byte  ;
	uint8_t RES_2_Byte  ;
	uint8_t RES_3_Byte  ;
	uint8_t CRC_Byte    ;
	
	int16_t Temp_short  ;
	float	  Temp_float  ;
	uint8_t Flag_OC     ;  /* Over Convert */
//	uint8_t Flag_PD     ;  /* Presence Detect */
	uint8_t step;
	uint8_t state;
}_18B20_Convert_Typedef;

typedef enum
{
	_18B20_State_OK = 0x00        ,   /* 温度转换完成 */
	_18B20_State_ResetErr  ,   /* 复位出现错误 */
	_18B20_State_ResetOK   ,   /* 复位完成 */
	_18B20_State_Start     ,   /* 开始转换 */
	_18B20_State_Wait      ,   /* 等待转换 */
	_18B20_State_Covert    ,   /* 转换完成 */
	_18B20_State_Err       ,   /* 步骤异常 */

}_18B20_StateTypeDef;

uint8_t _18B20_Init(_18B20_Config_TypeDef *_18B20_Config_Struct);
_18B20_StateTypeDef _18B20_Get_Temp(_18B20_Config_TypeDef *_18B20_Config_Struct,_18B20_Convert_Typedef *_18B20_Convert_Struct);


#endif


