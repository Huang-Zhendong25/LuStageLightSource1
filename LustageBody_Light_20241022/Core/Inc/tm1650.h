#ifndef __TM1650_H
#define __TM1650_H

#include "main.h"


#define Display_ON   0x01
#define Display_OFF  0x00

#define Display_Mode_7                         0x07              /*7段显示*/ 
#define Display_Mode_8                         0x08              /*8段显示*/

#define brightness_8                           0x00              /*8级亮度*/

void TM1650_SetNumber(uint8_t index, uint8_t mode, uint8_t num);
void TM1650_SetDisplay(uint8_t brightness, uint8_t mode, uint8_t state);
void Display_update(uint8_t mode,uint16_t ID_data,uint8_t state_display);
#endif

