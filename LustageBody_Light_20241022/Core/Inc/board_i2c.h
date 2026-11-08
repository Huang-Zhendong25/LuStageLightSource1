#ifndef _BOARD_I2C_H_
#define _BOARD_I2C_H_

/*********************************************************************
 * INCLUDES
 */
#include "stm32f4xx.h"

/*********************************************************************
 * DEFINITIONS
 */
// I2C_SCL时钟
//#define IIC_SCL_CLK         RCC_APB2Periph_GPIOB        // GPIO端口时钟
//#define IIC_SCL_PORT        GPIOB                       // GPIO端口
//#define IIC_SCL_PIN         GPIO_Pin_10                  // GPIO引脚
//// I2C_SDA时钟
//#define IIC_SDA_CLK         RCC_APB2Periph_GPIOB        // GPIO端口时钟
//#define IIC_SDA_PORT        GPIOB                       // GPIO端口
//#define IIC_SDA_PIN         GPIO_Pin_11                  // GPIO引脚

/*********************************************************************
 * MACROS
 */
#define IIC_SCL_0()          HAL_GPIO_WritePin(DISPLAY_SCL_GPIO_Port, DISPLAY_SCL_Pin, GPIO_PIN_RESET)  
#define IIC_SCL_1()          HAL_GPIO_WritePin(DISPLAY_SCL_GPIO_Port, DISPLAY_SCL_Pin, GPIO_PIN_SET) 
#define IIC_SDA_0()          HAL_GPIO_WritePin(DISPLAY_SDA_GPIO_Port, DISPLAY_SDA_Pin, GPIO_PIN_RESET) 
#define IIC_SDA_1()          HAL_GPIO_WritePin(DISPLAY_SDA_GPIO_Port, DISPLAY_SDA_Pin, GPIO_PIN_SET) 
//#define IIC_SDA_READ()       HAL_GPIO_ReadPin(IIC_SDA_GPIO_Port, IIC_SDA_Pin) 

/*********************************************************************
 * API FUNCTIONS
 */
void IIC_Init(void);			 
void IIC_Start(void);
void IIC_Stop(void);
void IIC_SendByte(uint8_t ucByte);
uint8_t IIC_ReadByte(void);
uint8_t IIC_WaitAck(void);
void IIC_Ack(void);
void IIC_NAck(void);
uint8_t IIC_CheckDevice(uint8_t address);
uint8_t IIC_SDA_READ(void);

#endif /* _BOARD_I2C_H_ */
