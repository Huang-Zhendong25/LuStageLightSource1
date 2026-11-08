#ifndef __USER_H
#define __USER_H

#include "main.h"
#include "drv_18b20.h"

#define LED_State_ON                  0x01
#define LED_State_OFF                 0x00

#define Switch_ON                  0x01
#define Switch_OFF                 0x00

#define TIME_TempProtection_ms     1000          /* 温度传感器调用周期，采集5次做滤波，1S出一次滤波数据 */

#define TEMPT_HILG_LIMIT           85            //最高温度不超过85℃
#define TEMPT_LOW_LIMIT            -40           //最低温度不低于-40℃

#define Default_Fan_High     50  
#define Default_Fan_Low      35

#define  Comm_Header_1       0x4C
#define  Comm_Header_2       0x55


#define Install_adjust_MODE     0x00     /*装调模式*/
#define Material_solve_MODE     0x01     /*材质解算模式*/
#define Relighting_MODE         0x02     /*重光照模式*/
#define OLAT_MODE               0x03     /*OLAT模式*/
#define Adj_Relighting_MODE     0x10     /*装调重光照模式*/

/*LED光学参数*/
#define Xr                  0.7068        /*R_LED的X坐标*/
#define Yr                  0.2916        /*R_LED的Y坐标*/
//#define Lr                  1434.16         /*R_LED的光通量14个LED总和，102.44*14*/  


#define Xg                  0.1846        /*G_LED的X坐标*/
#define Yg                  0.7257        /*G_LED的Y坐标*/
//#define Lg                  2918.4         /*G_LED的光通量12个LED总和，243.2*12*/

#define Xb                  0.1564        /*B_LED的X坐标*/
#define Yb                  0.0210         /*B_LED的Y坐标*/
//#define Lb                  651.96          /*B_LED的光通量18个LED总和，36.22*18*/

#define Xw                  0.3476        /*W_LED的X坐标*/
#define Yw                  0.3596        /*W_LED的Y坐标*/
//#define Lw                  2219.8        /*W_LED的光通量11个LED总和，201.8*11*/

#define Xa                  0.5672        /*A_LED的X坐标*/
#define Ya                  0.4201        /*A_LED的Y坐标*/
#define La                  527.1         /*A_LED的光通量3个LED总和，175.7*3*/
#define Xc                  0.0834         /*C_LED的X坐标*/
#define Yc                  0.2732        /*C_LED的Y坐标*/
#define Lc                  283.05          /*C_LED的光通量3个LED总和，94.35*3*/
//#define Lm                  500.00          /*目标光通量*/


/* 设备默认参数 */
#define Default_ID           9999
#define Default_Power        1000
#define Default_Switch       0            //off
#define ALL_ID               0xFFFF

#define Default_Mode         0            //0:装调模式
										       																				       
																					       
#define CMD_Power_Max              2500           /* 最大功率100%（2.5A） */
#define CMD_Power_Min              0            /* 最小功率 */

#define	CMD_Power_Max_Install_adjust	    1500   /*装调模式下最大功率*/   
																					       


#define LIGHT_OFF                 0x00     
#define LIGHT_ON                  0x01

typedef struct
{
	uint16_t           Power_A;
	uint16_t           Power_B;
	uint16_t           Power_C;
	uint16_t           Power_D;
}Power_Data_TypeDef;



#define FlashAddr_Base              ADDR_FLASH_SECTOR_3

////typedef enum
////{
//#define	Addr_DeviceID               0x10     /* 设备ID */
//#define	Addr_Power                  0x20     /* 光源功率 */
//#define	Addr_Mode                   0x30    /* 模式*/
////}FlashAddr_TypeDef;


#define FlashAddr_DeviceID             ADDR_FLASH_SECTOR_1
#define FlashAddr_Power                ADDR_FLASH_SECTOR_2           
#define FlashAddr_Mode                 ADDR_FLASH_SECTOR_3

#define Bootloader_Address_Base        ADDR_FLASH_SECTOR_4
#define APP_Run_Address			           ADDR_FLASH_SECTOR_5
#define APP_Backup1_ADDR		           ADDR_FLASH_SECTOR_6
#define APP_Backup2_ADDR		           ADDR_FLASH_SECTOR_7

#define FlashAddr_Material_Solve       ADDR_FLASH_SECTOR_8
#define FlashAddr_OLAT                 ADDR_FLASH_SECTOR_9
#define FlashAddr_Relighting           ADDR_FLASH_SECTOR_10

#define APP_Sectors_NUM			1


#define APP_Backup1					0
#define APP_Backup2					1


#define RS485_Mode_Rx()      HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_RESET)
#define RS485_Mode_Tx()      HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_SET)

#define AR_LED_OFF()         HAL_GPIO_WritePin(AR_PWM_GPIO_Port, AR_PWM_Pin, GPIO_PIN_RESET)
#define AR_LED_ON()          HAL_GPIO_WritePin(AR_PWM_GPIO_Port, AR_PWM_Pin, GPIO_PIN_SET)

#define AG_LED_OFF()         HAL_GPIO_WritePin(AG_PWM_GPIO_Port, AG_PWM_Pin, GPIO_PIN_RESET)
#define AG_LED_ON()          HAL_GPIO_WritePin(AG_PWM_GPIO_Port, AG_PWM_Pin, GPIO_PIN_SET)

#define AB_LED_OFF()         HAL_GPIO_WritePin(AB_PWM_GPIO_Port, AB_PWM_Pin, GPIO_PIN_RESET)
#define AB_LED_ON()          HAL_GPIO_WritePin(AB_PWM_GPIO_Port, AB_PWM_Pin, GPIO_PIN_SET)

#define AW_LED_OFF()         HAL_GPIO_WritePin(AW_PWM_GPIO_Port, AW_PWM_Pin, GPIO_PIN_RESET)
#define AW_LED_ON()          HAL_GPIO_WritePin(AW_PWM_GPIO_Port, AW_PWM_Pin, GPIO_PIN_SET)

#define MR_LED_OFF()         HAL_GPIO_WritePin(MR_PWM_GPIO_Port, MR_PWM_Pin, GPIO_PIN_RESET)
#define MR_LED_ON()          HAL_GPIO_WritePin(MR_PWM_GPIO_Port, MR_PWM_Pin, GPIO_PIN_SET)

#define MG_LED_OFF()         HAL_GPIO_WritePin(MG_PWM_GPIO_Port, MG_PWM_Pin, GPIO_PIN_RESET)
#define MG_LED_ON()          HAL_GPIO_WritePin(MG_PWM_GPIO_Port, MG_PWM_Pin, GPIO_PIN_SET)

#define MB_LED_OFF()         HAL_GPIO_WritePin(MB_PWM_GPIO_Port, MB_PWM_Pin, GPIO_PIN_RESET)
#define MB_LED_ON()          HAL_GPIO_WritePin(MB_PWM_GPIO_Port, MB_PWM_Pin, GPIO_PIN_SET)

#define MW_LED_OFF()         HAL_GPIO_WritePin(MW1_PWM_GPIO_Port, MW1_PWM_Pin|MW2_PWM_Pin, GPIO_PIN_RESET)                     
#define MW_LED_ON()          HAL_GPIO_WritePin(MW1_PWM_GPIO_Port, MW1_PWM_Pin|MW2_PWM_Pin, GPIO_PIN_SET)

												
#define CR_LED_OFF()         HAL_GPIO_WritePin(CR_PWM_GPIO_Port, CR_PWM_Pin, GPIO_PIN_RESET)
#define CR_LED_ON()          HAL_GPIO_WritePin(CR_PWM_GPIO_Port, CR_PWM_Pin, GPIO_PIN_SET)

#define CG_LED_OFF()         HAL_GPIO_WritePin(CG_PWM_GPIO_Port, CG_PWM_Pin, GPIO_PIN_RESET)
#define CG_LED_ON()          HAL_GPIO_WritePin(CG_PWM_GPIO_Port, CG_PWM_Pin, GPIO_PIN_SET)

#define CB_LED_OFF()         HAL_GPIO_WritePin(CB1_PWM_GPIO_Port, CB1_PWM_Pin|CB2_PWM_Pin, GPIO_PIN_RESET)
#define CB_LED_ON()          HAL_GPIO_WritePin(CB1_PWM_GPIO_Port, CB1_PWM_Pin|CB2_PWM_Pin, GPIO_PIN_SET)

#define CW_LED_OFF()         HAL_GPIO_WritePin(CW1_PWM_GPIO_Port, CW1_PWM_Pin|CW2_PWM_Pin, GPIO_PIN_RESET)
#define CW_LED_ON()          HAL_GPIO_WritePin(CW1_PWM_GPIO_Port, CW1_PWM_Pin|CW2_PWM_Pin, GPIO_PIN_SET)
										

#define CA_LED_OFF()         HAL_GPIO_WritePin(CA_PWM_GPIO_Port, CA_PWM_Pin, GPIO_PIN_RESET)
#define CA_LED_ON()          HAL_GPIO_WritePin(CA_PWM_GPIO_Port, CA_PWM_Pin, GPIO_PIN_SET)

#define CC_LED_OFF()         HAL_GPIO_WritePin(CC_PWM_GPIO_Port, CC_PWM_Pin, GPIO_PIN_RESET)
#define CC_LED_ON()          HAL_GPIO_WritePin(CC_PWM_GPIO_Port, CC_PWM_Pin, GPIO_PIN_SET)

#define Reserve_LED_OFF()    HAL_GPIO_WritePin(Reserve_PWM_GPIO_Port, Reserve_PWM_Pin, GPIO_PIN_RESET)
#define Reserve_LED_ON()     HAL_GPIO_WritePin(Reserve_PWM_GPIO_Port, Reserve_PWM_Pin, GPIO_PIN_SET)

#define Camera_Focus_OFF()   HAL_GPIO_WritePin(CAM_FOCUS_GPIO_Port, CAM_FOCUS_Pin, GPIO_PIN_RESET)
#define Camera_Focus_ON()    HAL_GPIO_WritePin(CAM_FOCUS_GPIO_Port, CAM_FOCUS_Pin, GPIO_PIN_SET)

#define Camera_Trig_OFF()    HAL_GPIO_WritePin(CAM_TRIG_OUT_GPIO_Port, CAM_TRIG_OUT_Pin, GPIO_PIN_RESET)
#define Camera_Trig_ON()     HAL_GPIO_WritePin(CAM_TRIG_OUT_GPIO_Port, CAM_TRIG_OUT_Pin, GPIO_PIN_SET)

#define LED_FAN_OFF()        HAL_GPIO_WritePin(LIGHT_FAN_GPIO_Port, LIGHT_FAN_Pin, GPIO_PIN_RESET)
#define LED_FAN_ON()         HAL_GPIO_WritePin(LIGHT_FAN_GPIO_Port, LIGHT_FAN_Pin, GPIO_PIN_SET)


#pragma pack(1)

typedef struct
{
	uint32_t APP_Run_Flag	          ;
	uint32_t APP_Backup_Flag         ;
}Bootloader_Address_Typedef;


typedef struct
{
		uint16_t R_Power[2500];
		uint16_t R_Pulse[2500];
		uint16_t G_Power[2500];
		uint16_t G_Pulse[2500];
		uint16_t B_Power[2500];
		uint16_t B_Pulse[2500];
}Material_solve_parameter_Typedef;


typedef struct
{
		uint16_t W_Power[2500];
		uint16_t W_Pulse[2500];
}OLAT_parameter_Typedef;

typedef struct
{
		uint16_t Color_Coordinate_X[2500];    //颜色坐标X
		uint16_t Color_Coordinate_Y[2500];    //颜色坐标Y
		uint16_t Power[2500];              //功率亮度
		uint16_t Pulse[2500];              //触发脉宽
}Relighting_parameter_Typedef;

typedef struct
{
	struct
	{
		uint8_t Flag;
		uint8_t _0 ;
		uint8_t _1 ;
		uint8_t _2 ;
	}Power;
	struct
	{
		uint8_t Flag;
		uint16_t _0 ;
		uint16_t _1 ;
		uint16_t _2 ;
	}DeviceID;
	struct
	{
		uint8_t Flag;
		uint8_t _0 ;
		uint8_t _1 ;
		uint8_t _2 ;
	}Mode;
	
}FlashParam_TypeDef;


typedef struct
{


	uint16_t Device_ID;
  uint8_t Mode;                          /*模式*/
	uint8_t _18b20_EN;                     /*18b20使能状态*/
	uint8_t _18b20_state;                  /*18b20状态*/
	uint8_t State_Fan;                     /*风扇状态*/
	uint8_t Fan_flag;
  uint8_t State_display;                 /*数码管状态*/
  uint8_t Adj_mode_valid;               /*装调模式有效标志*/
	uint8_t Idle_Flag ;                   /*空闲标志*/
  uint8_t Trig_Level;                   /*触发电平检测*/
  uint8_t State_LED;                    /*LED状态*/
	uint8_t Start_LED_TRIG_Flag;          /*LED触发标志*/
	uint8_t Start_LED_TRIG_ON;          /*LED触发结束标志*/
	uint8_t Start_Camera_TRIG_Flag;       /*相机触发标志*/
	uint16_t Trig_Cnt;                    /*实际触发次数*/
	uint16_t Trig_Frequency;              /*触发频率*/
	uint16_t Trig_Cnt_Set;                /*设置触发次数*/
	uint16_t Data_Cycle_quantity;         /*数据循环量*/
	uint8_t  Camera_Type;                 /*相机类型*/
	uint8_t  Camera_div_Frequency;        /*相机分频参数*/
	uint8_t  Camera_Trig_cnt;             /*相机实际触发次数*/
	uint16_t Camera_Trig_Pulse;           /*相机触发脉宽*/
	uint16_t Camera_Trig_Pulse_cnt; 
	uint16_t Camera_Trig_Delay;           /*相机触发延迟*/
	uint16_t Camera_Trig_Delay_cnt;       /*相机触发延迟计数，基础10us*/
	uint8_t Relighting_State_LED;
//	/*OLAT模式下相关参数*/                                     
//	uint8_t Strob_EN;                      /*频闪使能*/
//	uint8_t Circle_EN;                     /*循环使能*/
//	uint8_t Strob_interval;                /*频闪间隔*/	
//	uint16_t OLAT_cnt ;                     /*频闪次数*/
	_18B20_Convert_Typedef Convert_Temp;   /* LED灯板温度 */

	
	struct
	{
		int8_t Buffer[5];  /* 温度滤波缓冲 */
		uint8_t Flag_Range[5];  /* 超界标记 */
	}TempFilt[2];/* 温度滤波缓冲 */
	
	
	int8_t Temp_Current; /* 滤波后温度值 */
	union
	{
		uint8_t data ;
		struct
		{
			uint8_t ResetCurrent_1           :1;      //当前复位有问题
			uint8_t RangeCurrent_1           :1;      //当前温度超界
			uint8_t ResetOnce_1              :1;      //曾经复位出现过问题
			uint8_t RangeOnce_1              :1;      //曾经温度超界
			uint8_t ResetCurrent_2           :1;      //当前复位有问题
			uint8_t RangeCurrent_2           :1;      //当前温度超界
			uint8_t ResetOnce_2              :1;      //曾经复位出现过问题
			uint8_t RangeOnce_2              :1;      //曾经温度超界

		}bits;
		
	}State_TempErr;/* 温度传感器错误状态 */
                   

	
    uint8_t BroadcastBack;
    uint8_t Upgrade_Device;
    uint8_t Upgrade_Size;
	  uint8_t Upgrade_Sequence;
	  uint8_t Upgrade_status;
	  uint16_t Power;			                   /* LED功率 */
		union
	{
		uint8_t data;
		struct
		{
			uint8_t Center_Area            :1;
			uint8_t Annular_Area           :1;
			uint8_t Edge_Area              :1;
			uint8_t remain                 :5;
		}bits;               
	}Area_LED;
	
			union
	{
		uint8_t data;
		struct
		{
			uint8_t Channel_R              :1;
			uint8_t Channel_G              :1;
			uint8_t Channel_B              :1;
			uint8_t Channel_W              :1;
			uint8_t Channel_A              :1;
			uint8_t Channel_C              :1;
			uint8_t Channel_D              :1;
			uint8_t remain                 :1;
		}bits;               
	}Channel_Color;
	
/*装调模式下LED状态数据结构体*/
struct
	{
		uint8_t CR_LED_State;             //CR状态
		uint8_t CG_LED_State;             //CG状态
		uint8_t CB_LED_State;             //CB状态
		uint8_t CW_LED_State;             //CW状态
		uint8_t CA_LED_State;             //CA状态
		uint8_t CC_LED_State;             //CC状态
		uint8_t MR_LED_State;             //MR状态
		uint8_t MG_LED_State;             //MG状态
		uint8_t MB_LED_State;             //MB状态
		uint8_t MW_LED_State;             //MW状态
		uint8_t Reserve_LED_State;        //预留LED状态
		uint8_t AR_LED_State;             //AR状态
		uint8_t AG_LED_State;             //AG状态
		uint8_t AB_LED_State;             //AB状态
		uint8_t AW_LED_State;             //AW状态
	}Adj_mode_parameter;	
	
		uint8_t CR_LED_State;             //CR状态
		uint8_t CG_LED_State;             //CG状态
		uint8_t CB_LED_State;             //CB状态
		uint8_t CW_LED_State;             //CW状态
		uint8_t CA_LED_State;             //CA状态
		uint8_t CC_LED_State;             //CC状态
		uint8_t MR_LED_State;             //MR状态
		uint8_t MG_LED_State;             //MG状态
		uint8_t MB_LED_State;             //MB状态
		uint8_t MW_LED_State;             //MW状态
		uint8_t Reserve_LED_State;        //预留LED状态
		uint8_t AR_LED_State;             //AR状态
		uint8_t AG_LED_State;             //AG状态
		uint8_t AB_LED_State;             //AB状态
		uint8_t AW_LED_State;             //AW状态
	
/*材质解算数据结构体*/	
struct
	{
		uint16_t data_pake_num;            //数据包序号
		uint8_t sub_pake_num;             //分包号
		uint16_t R_Power[2500];
		uint16_t R_Pulse[2500];
		uint16_t G_Power[2500];
		uint16_t G_Pulse[2500];
		uint16_t B_Power[2500];
		uint16_t B_Pulse[2500];
		uint16_t Switch_Pulse_R;
		uint16_t Switch_Pulse_G;
		uint16_t Switch_Pulse_B;
		uint8_t  State_LED_R;
		uint8_t  State_LED_G;
		uint8_t  State_LED_B;
		uint16_t  R_Pulse_cnt;
		uint16_t  G_Pulse_cnt;
		uint16_t  B_Pulse_cnt;
		uint8_t  R_Pulse_Flag;
		uint8_t  G_Pulse_Flag;
		uint8_t  B_Pulse_Flag;
	}Material_solve_parameter;
	
/*OLAT数据结构体*/		
	struct
	{
		uint16_t data_pake_num;            //数据包序号
		uint8_t sub_pake_num;             //分包号
		uint16_t W_Power[2500];
		uint16_t W_Pulse[2500];
    uint16_t Switch_Pulse_W;
		uint8_t  State_LED_W;
		uint16_t  W_Pulse_cnt;
	}OLAT_parameter;

/*重光照数据结构体*/	
		struct
	{
		uint16_t data_pake_num;            //数据包序号
		uint8_t sub_pake_num;             //分包号
		uint16_t Color_Coordinate_X[3000];    //颜色坐标X
		uint16_t Color_Coordinate_Y[3000];    //颜色坐标Y
		uint16_t Power[3000];              //功率亮度
		uint16_t Pulse[3000];              //触发脉宽
		uint16_t Switch_Pulse;            //实际脉宽
		uint16_t Coordinate_X;
		uint16_t Coordinate_Y;
		uint8_t  State_LED;              //重光照模式下LED状态
		uint8_t  Dr;
		uint8_t  Dg;
		uint8_t  Db;
		uint8_t  State_LED_R;
		uint8_t  State_LED_G;
		uint8_t  State_LED_B;
		uint8_t  State_LED_W;
		uint8_t  duty_cnt_R ;
		uint8_t  duty_cnt_G ;
		uint8_t  duty_cnt_B ;
		uint16_t  Pulse_cnt;
	}Relighting_parameter;

}LED_Para_TypeDef;

#pragma pack()


#define CMD_Set_Display                        0x0800             /*数码管显示  0关  1开*/
#define CMD_Set_Adj_mode_test                  0x0100             /*装调模式测试*/
#define CMD_Set_LED_OFF                        0x0200             /*LED光源  0关  */
#define CMD_Set_start                          0x02FF             /*软起动*/
#define CMD_Set_stop                           0x03FF             /*停止*/

#define CMD_Set_LED_Parameter_Material         0x0955             /*材质解算模式下光源参数配置*/

#define CMD_Set_DeviceID                       0x0400             /*设置光源ID */
#define CMD_Set_MODE                           0x0255             /*系统工作模式配置*/
#define CMD_Set_Frequency                      0x0455             /*系统工作频率配置*/

#define CMD_Set_Adj_mode_parameter             0x1000             /*装调模式下光源参数配置*/
#define CMD_Set_Adj_mode_Relighting_parameter  0x0200             /*装调模式下重光照参数配置*/
#define CMD_Set_Material_solve_mode_parameter  0x0801             /*材质解算模式下光源闪烁次数和顺序配置*/
#define CMD_Set_OLAT_mode_parameter            0x0203             /*OLAT模式下光源参数配置*/
#define CMD_Set_Relighting_mode_parameter      0x0205             /*重光照模式下光源参数配置*/
#define CMD_parameter_Set_Over                 0x0555             /*参数配置完成*/
#define CMD_Set_Loopdata_parameter             0x0155             /*数据循环组配置*/

#define CMD_Set_Firmware_Update_ready          0x04AA             /*固件升级准备*/
#define CMD_Set_Firmware_Update_data           0x05AA             /*固件升级数据配置*/
#define CMD_Set_Firmware_Update_start          0x09AA             /*开始固件升级*/

void Comm_Init(void);


/* 参数初始化 */
void Parameter_Init(void);

void Comm_Function_Handler(void);

void LED_Power_Min(void);

/* LED状态处理 */
void LED_State_Updata(void);
/* LED状态指示 */
void LED_Switch_Updata(void);

void LED_Dimming_Updata(void);

void Comm_Send_Laser_CMD(void);

void Debug_Data_Handler(void);

void TRIG_LIGHT_ON(void);

void TRIG_LIGHT_OFF(void);

void LED_Relighting_state_Updata(void);

/* 温度保护处理 */
void LED_OTP_Handler(void);




#endif


