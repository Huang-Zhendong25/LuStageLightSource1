#ifndef RGBWAC_H
#define RGBWAC_H

#include <stdbool.h>
#include <stdint.h>

#define CMD_RGBWAC_PREPARE 0x02ACu /* 准备外部 Flash 或选择已存序列。 */
#define CMD_RGBWAC_RECORDS 0x02ADu /* 下发某个播放帧的一个光源分包。 */

void Rgbwac_Prepare(uint16_t frame_count, uint16_t device_id);
void Rgbwac_Receive(const uint8_t *data, uint16_t size);
bool Rgbwac_IsSelected(void);
bool Rgbwac_IsArmed(void);
void Rgbwac_SetLoopLength(uint16_t length);
void Rgbwac_Start(void);
void Rgbwac_Stop(void);
void Rgbwac_Leave(void);
/* 中断入口只记录事件，Flash、DAC 和输出更新由主循环处理。 */
void Rgbwac_Trigger(void);
void Rgbwac_TimerIRQ(uint32_t flag);
void Rgbwac_Service(void);

#endif
