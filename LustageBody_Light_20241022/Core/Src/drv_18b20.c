#include "drv_18b20.h"

#define CPU_FREQUENCY_MHZ 168				/* CPU主频，根据实际进行修改 */


/**
 * 此延时函数代码适用于HAL库
 */
static void delay_us(uint32_t delay)
{
    int last, curr, val;
    int temp;

    while (delay != 0)
    {
        temp = delay > 900 ? 900 : delay;
        last = SysTick->VAL;
        curr = last - CPU_FREQUENCY_MHZ * temp;
        if (curr >= 0)
        {
            do
            {
                val = SysTick->VAL;
            }
            while ((val < last) && (val >= curr));
        }
        else
        {
            curr += CPU_FREQUENCY_MHZ * 1000;
            do
            {
                val = SysTick->VAL;
            }
            while ((val <= last) || (val > curr));
        }
        delay -= temp;
    }
}


static void _18B20_Pin_Set(_18B20_Config_TypeDef *_18B20_Config_Struct)
{
	HAL_GPIO_WritePin(_18B20_Config_Struct->GPIOx,_18B20_Config_Struct->GPIO_Pin,GPIO_PIN_SET);
}

static void _18B20_Pin_Reset(_18B20_Config_TypeDef *_18B20_Config_Struct)
{
	HAL_GPIO_WritePin(_18B20_Config_Struct->GPIOx,_18B20_Config_Struct->GPIO_Pin,GPIO_PIN_RESET);
}

static GPIO_PinState _18B20_Pin_Read(_18B20_Config_TypeDef *_18B20_Config_Struct)
{
	return HAL_GPIO_ReadPin(_18B20_Config_Struct->GPIOx,_18B20_Config_Struct->GPIO_Pin);
}

static void _18B20_Pin_Init(_18B20_Config_TypeDef *_18B20_Config_Struct)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	/* 总线空闲为高电平 */
	HAL_GPIO_WritePin(_18B20_Config_Struct->GPIOx, _18B20_Config_Struct->GPIO_Pin, GPIO_PIN_SET);

	GPIO_InitStruct.Pin = _18B20_Config_Struct->GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(_18B20_Config_Struct->GPIOx, &GPIO_InitStruct);
}

static void _18B20_Pin_Output(_18B20_Config_TypeDef *_18B20_Config_Struct)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	
  GPIO_InitStruct.Pin = _18B20_Config_Struct->GPIO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(_18B20_Config_Struct->GPIOx, &GPIO_InitStruct);
}

static void _18B20_Pin_Input(_18B20_Config_TypeDef *_18B20_Config_Struct)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
  GPIO_InitStruct.Pin = _18B20_Config_Struct->GPIO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(_18B20_Config_Struct->GPIOx, &GPIO_InitStruct);
}


static void _18B20_Reset_Pulse(_18B20_Config_TypeDef *_18B20_Config_Struct)
{
	_18B20_Pin_Output(_18B20_Config_Struct);
	
	_18B20_Pin_Reset(_18B20_Config_Struct);
	delay_us(750);
	_18B20_Pin_Set(_18B20_Config_Struct);
	delay_us(15);
}

static uint8_t _18B20_PresenceDetect(_18B20_Config_TypeDef *_18B20_Config_Struct)
{
	uint16_t count = 0;

	_18B20_Pin_Input(_18B20_Config_Struct);

	while( _18B20_Pin_Read(_18B20_Config_Struct) && (count < 200) )
	{
		delay_us(1);
		count++;
	}
	
	if(count >= 200) return RESET; 
	else count = 0;
	
	while (!_18B20_Pin_Read(_18B20_Config_Struct) && count < 240)
	{
		delay_us(1);
		count++;
	}
	if(count>=240) return RESET;
	return SET;
}

static uint8_t _18B20_Read_Bit(_18B20_Config_TypeDef *_18B20_Config_Struct)
{
//    uint8_t data;
//    DS18B20_IO_OUT();//SET PA0 OUTPUT
//    DS18B20_DQ_OUT(0);
//    delay_us(2);
//    DS18B20_DQ_OUT(1);
//    DS18B20_IO_IN();//SET PA0 INPUT
//    delay_us(12);
//    if(DS18B20_DQ_IN())data=1;
//    else data=0;
//    delay_us(50);
//    return data;
	
	
    uint8_t data;
	_18B20_Pin_Output(_18B20_Config_Struct);
	_18B20_Pin_Reset(_18B20_Config_Struct);// Write 1
	delay_us(2);
	_18B20_Pin_Set(_18B20_Config_Struct);
	_18B20_Pin_Input(_18B20_Config_Struct);
	delay_us(12);
	if(_18B20_Pin_Read(_18B20_Config_Struct) == GPIO_PIN_SET)data=1;
	    else data=0;
    delay_us(50);
    return data;
	
	
}

static uint8_t _18B20_Read_Byte(_18B20_Config_TypeDef *_18B20_Config_Struct)
{
	uint8_t i;
	
	uint8_t data = 0;

	for (i = 0; i < 8; i++)
	{
		data = data >> 1;
		
		if(_18B20_Read_Bit(_18B20_Config_Struct) == GPIO_PIN_SET)
		{
			data |= 0x80;
		}
	}
	return data;
}


static void _18B20_Write_Byte(_18B20_Config_TypeDef *_18B20_Config_Struct,uint8_t Data)
{
	_18B20_Pin_Output(_18B20_Config_Struct);
	
	for (uint8_t i = 0 ; i < 8; i++)
	{
		if ( Data & 0x01 )
		{
			_18B20_Pin_Reset(_18B20_Config_Struct);// Write 1
			delay_us(2);
			
			_18B20_Pin_Set(_18B20_Config_Struct);
			delay_us(60);
		}
		else
		{
			_18B20_Pin_Reset(_18B20_Config_Struct);// Write 0
			delay_us(60);
			
			_18B20_Pin_Set(_18B20_Config_Struct);
			delay_us(2);
		}
		Data = Data>>1;
	}
}


static void _18B20_Start(_18B20_Config_TypeDef *_18B20_Config_Struct)// ds1820 start convert
{
//	_18B20_Reset_Pulse(_18B20_Config_Struct);
//	_18B20_PresenceDetect(_18B20_Config_Struct);
	_18B20_Write_Byte(_18B20_Config_Struct,0xcc);// skip rom
	_18B20_Write_Byte(_18B20_Config_Struct,0x44);// convert
}


uint8_t _18B20_Init(_18B20_Config_TypeDef *_18B20_Config_Struct)
{
	_18B20_Pin_Init(_18B20_Config_Struct);
	_18B20_Reset_Pulse(_18B20_Config_Struct);
	return _18B20_PresenceDetect(_18B20_Config_Struct);
}




/* Temperature Conversion Time       */
/*  9-bit resolution 93.75ms         */
/* 10-bit resolution 187.5ms         */
/* 11-bit resolution 375ms           */
/* 12-bit resolution 750ms (default) */
_18B20_StateTypeDef _18B20_Get_Temp(_18B20_Config_TypeDef *_18B20_Config_Struct,_18B20_Convert_Typedef *_18B20_Convert_Struct)
{
//	uint8_t state = _18B20_State_OK;
	
	_18B20_Convert_Struct->state = _18B20_State_OK;
//	static uint8_t step = 0;
	
	//__disable_irq();    /* 中断可能会单总线的时序从而导致读出来的温度值不正确,所以读取之前屏蔽中断 */
	
	switch ( _18B20_Convert_Struct->step )
	{
		case 0:
		{
//			_18B20_Convert_Struct->Flag_PD = _18B20_Init(_18B20_Config_Struct);
//			
//			if(_18B20_Convert_Struct->Flag_PD == RESET) /* Presence Detect */
//			{
//				step++;
//			}
			if(_18B20_Init(_18B20_Config_Struct) != RESET) /* Presence Detect */
			{
				_18B20_Convert_Struct->step++;
				_18B20_Convert_Struct->state = _18B20_State_ResetOK;/* 复位正常 */
			}
			else
			{
				_18B20_Convert_Struct->state = _18B20_State_ResetErr;/* 复位出错 */
				_18B20_Convert_Struct->step = 0;
			}
		}
		break;
		case 1:
		{
			_18B20_Start (_18B20_Config_Struct);                    // ds1820 start convert
			_18B20_Convert_Struct->step++;
			_18B20_Convert_Struct->state = _18B20_State_Start;
		}
		break;
		case 2:
		{
			if(_18B20_Read_Byte(_18B20_Config_Struct) == 0xFF)
			{
				_18B20_Convert_Struct->step++;
			}
			else
			{
				_18B20_Convert_Struct->state = _18B20_State_Wait;
			}
		}
		break;
		case 3:
		{
			_18B20_Reset_Pulse(_18B20_Config_Struct);
			_18B20_PresenceDetect(_18B20_Config_Struct);
			_18B20_Write_Byte(_18B20_Config_Struct,0xcc);// skip rom
			_18B20_Write_Byte(_18B20_Config_Struct,0xbe);// convert
			_18B20_Convert_Struct->LS_Byte      = _18B20_Read_Byte(_18B20_Config_Struct); // LSB
			_18B20_Convert_Struct->MS_Byte      = _18B20_Read_Byte(_18B20_Config_Struct); // MSB
			_18B20_Convert_Struct->TH_Byte      = _18B20_Read_Byte(_18B20_Config_Struct); 
			_18B20_Convert_Struct->TL_Byte      = _18B20_Read_Byte(_18B20_Config_Struct); 
			_18B20_Convert_Struct->Config_Byte  = _18B20_Read_Byte(_18B20_Config_Struct); 
			_18B20_Convert_Struct->RES_1_Byte   = _18B20_Read_Byte(_18B20_Config_Struct); 
			_18B20_Convert_Struct->RES_2_Byte   = _18B20_Read_Byte(_18B20_Config_Struct); 
			_18B20_Convert_Struct->RES_3_Byte   = _18B20_Read_Byte(_18B20_Config_Struct); 
			_18B20_Convert_Struct->CRC_Byte     = _18B20_Read_Byte(_18B20_Config_Struct); 

			_18B20_Convert_Struct->Temp_short = _18B20_Convert_Struct->LS_Byte | (_18B20_Convert_Struct->MS_Byte << 8);
			_18B20_Convert_Struct->Temp_float = _18B20_Convert_Struct->Temp_short * 0.0625;
			_18B20_Convert_Struct->Flag_OC = SET;
			
			_18B20_Convert_Struct->state = _18B20_State_Covert;
			_18B20_Convert_Struct->step = 0;
		}break;
		default: 
			_18B20_Convert_Struct->state = _18B20_State_Err;
			_18B20_Convert_Struct->step = 0;
		break;
	}
	//__enable_irq();    /* 再将全局中断打开 */
	
	return _18B20_Convert_Struct->state;
}


//short DRV_18B20_Get_Temp(_18B20_Config_TypeDef *_18B20_Config_Struct)
//{
//	uint8_t temp,buff;
//	uint8_t TL,TH;
//	short tem;

//	__disable_irq();    /* 中断可能会单总线的时序从而导致读出来的温度值不正确，所以读取之前屏蔽中断 */

//	DRV_18B20_Start (_18B20_Config_Struct);                    // ds1820 start convert
//	
//	do /* wait device converting */
//	{
//		buff = DRV_18B20_Read_Byte(_18B20_Config_Struct);
//	}
//	while(buff != 0xff);
//	
//	DRV_18B20_Reset_Pulse(_18B20_Config_Struct);
//	DRV_18B20_PresenceDetect(_18B20_Config_Struct);
//	DRV_18B20_Write_Byte(_18B20_Config_Struct,0xcc);// skip rom
//	DRV_18B20_Write_Byte(_18B20_Config_Struct,0xbe);// convert
//	TL=DRV_18B20_Read_Byte(_18B20_Config_Struct); // LSB
//	TH=DRV_18B20_Read_Byte(_18B20_Config_Struct); // MSB
//	if(TH>7)
//	{
//			TH=~TH;
//			TL=~TL;
//			temp=0;//温度为负
//	}
//	else temp=1; //温度为正
//	tem=TH; //获得高八位
//	tem<<=8;
//	tem+=TL;//获得底八位

//	__enable_irq();    /* 再将全局中断打开 */

//	tem=(float)tem*0.625;//转换
//	if(temp)return tem; //返回温度值
//	else return -tem;
//}




















