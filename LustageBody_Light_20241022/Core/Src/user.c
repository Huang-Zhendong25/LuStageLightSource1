#include "user.h"
//#include "usart.h"
#include "string.h"
#include "main.h"
//#include "bsp_flash.h"
//#include "dac.h"
#include "tm1650.h"
#include "drv_mcp4728.h"
#include "bsp_flash.h"


#define Comm_Frame_RxSize       0x06              /* 一帧数据6字节 */
#define Comm_Frame_TxSize       0x06              /* 一帧数据6字节 */
#define Comm_Frame_Sum          0x05              /* 校验位角标 */
#define Comm_Frame_ID           0x02              /* 发送帧,ID角标 */
#define Comm_RxFrame_Header1    0x00              /* 接收帧,帧头角标 */
#define Comm_RxFrame_Header2    0x01              /* 接收帧,帧头角标 */
#define Comm_RxFrame_Len        0x02              /* 接收帧,数据长度角标 */

#define Comm_RxBuffSzie         800           /* 接收缓冲区大小 */
#define Comm_TxBuffSzie         256          /* 发送缓冲区大小 */

uint8_t Comm_RxBuff[Comm_RxBuffSzie];        /* 接收缓冲区 */
uint8_t Comm_TxBuff[Comm_TxBuffSzie];        /* 发送缓冲区 */



#define Comm_Valid    1                      /* 数据有效 */
#define Comm_Invalid  0                      /* 数据无效 */

#define CTRL_Valid    1                      /* 控制有效 */
#define CTRL_Invalid  0                      /* 控制无效 */


#define Send_Frame_Start   0xDF              /* 发送帧起始位 */
#define Send_Frame_Stop    0xDE              /* 发送帧停止位 */

uint8_t CR_LED_state;
uint8_t CG_LED_state;
uint8_t CB_LED_state;
uint8_t CW_LED_state;
uint8_t CA_LED_state;
uint8_t CC_LED_state;
uint8_t MR_LED_state;
uint8_t MG_LED_state;
uint8_t MB_LED_state;
uint8_t MW_LED_state;
uint8_t AR_LED_state;
uint8_t AG_LED_state;
uint8_t AB_LED_state;
uint8_t AW_LED_state;

const float Liu_R[151] = 
	{
		3.29,    3.28 ,   3.41,    3.63,    3.89,    4.26,    4.74,    5.57,    6.26,    7.33,    7.86,    8.78,    9.76,    10.64,   11.90, 
		12.62,   13.71,   15.05,   16.40,   17.22 ,  18.27,   19.23,   20.37,   21.38,   22.45,   23.56,   24.56,   25.58,   26.85,   27.94,
		28.97,   30.08,   30.91,   32.26,   33.28,   34.06,   35.03,   36.07,   37.18,   38.30,   39.58,   40.86,   41.93,   43.31,   44.16, 
		45.13,   46.52,   47.42,   48.61,   49.73,   50.88,   51.89,   52.80,   53.77,   54.99,   55.82,   56.95,   58.30,   59.30,   60.39, 
		61.47,   62.51,   63.46,   64.77,   65.65,   66.80,   67.87,   68.60,   69.75,   70.93,   72.05,   73.10,   74.13,   75.33,   76.54, 
		77.46,   78.40,   79.37,   80.23,   81.08,   83.03,   91.90,   84.80,   85.37,   86.31,   87.47,   88.25,   88.96,   90.19,   91.11, 
		91.85,   92.90,   93.86,   94.52,   95.69,   96.71,   97.43,   98.19,   99.09,   99.85,   100.48,  102.44,  103.61,  103.69,  104.95, 
		105.89,  107.95,  108.54,  109.44,  110.79,  111.47,  112.66,  113.27,  114.30,  115.25,  116.26,  117.14,  118.48,  119.02,  120.11, 
		120.66,  121.66,  122.54,  123.13,  124.46,  125.47,  126.56,  127.44,  128.11,  129.32,  129.95,  130.50,  131.38,  132.38,  133.47, 
		133.77,  134.32,  135.20,  136.53,  137.08,  138.09,  138.97,  139.64,  140.52,  141.40,  142.28,  143.16,  143.71,  145.05,  146.05, 
		147.06 
	};

	const float Liu_G[151] = 
	{
		10.68,   10.76,   12.32,   13.36,   14.60,   17.35,   19.39,   21.93,   24.30,   27.37,   29.97,   33.92,   36.85,   40.51,   43.24, 
    46.62,   49.09,   51.60,   56.26,   59.93,   62.58,   65.50,   68.73,   72.40,   76.36,   78.79,   81.57,   85.24,   87.90,   91.37, 
    93.66,   96.46,   100.07,  103.21,  104.76,  107.44,  110.52,  112.92,  115.42,  117.49,  120.30,  122.83,  125.42,  127.41,  130.29, 
    132.88,  135.04,  137.86,  140.30,  143.04,  145.84,  148.11,  150.79,  153.15,  154.97,  157.17,  159.58,  161.53,  163.89,  166.27, 
    168.32,  169.77,  171.85,  173.91,  175.92,  178.22,  180.33,  182.54,  184.34,  186.36,  188.45,  190.17,  192.33,  193.83,  196.14, 
    198.14,  200.23,  201.94,  204.08,  205.61,  207.45,  211.62,  212.83,  214.73,  216.70,  217.90,  219.92,  221.63,  223.18,  224.27, 
    225.99,  226.49,  227.93,  228.84,  230.32,  232.34,  233.38,  234.86,  236.23,  237.76,  238.60,  240.28,  243.20,  245.02,  246.43, 
    247.97,  249.58,  251.26,  253.02,  254.60,  256.21,  258.45,  260.83,  261.89,  263.35,  263.97,  265.61,  266.08,  268.26,  269.89, 
    271.88,  274.04,  275.46,  276.99,  278.15,  279.00,  282.07,  283.31,  284.69,  285.96,  288.08,  289.46,  290.91,  293.36,  294.89, 
    296.11,  297.30,  298.90,  300.47,  302.38,  303.76,  305.17,  305.55,  306.88,  307.31,  308.65,  309.79,  311.31,  310.87,  312.73, 
    313.92                                                                                                                        
	};
	
	const float Liu_B[151] = 
	{
		1.58,   1.65,   1.74,   1.90,   2.14,   2.34,   2.74,   3.00,   3.35,   3.67,   4.07,   4.45,   4.72,   5.17,   5.50,
		5.93,   6.34,   6.74,   7.14,   7.49,   7.93,   8.32,   8.69,   9.09,   9.43,   9.88,   10.22,  10.62,  11.05,  11.46, 
    11.80,  12.17,  12.61,  12.87,  13.30,  13.70,  14.09,  14.50,  14.86,  15.22,  15.56,  15.95,  16.30,  16.60,  16.90, 
    17.37,  17.54,  18.07,  18.45,  18.81,  19.14,  19.59,  19.88,  20.23,  20.50,  20.81,  21.11,  21.44,  21.84,  22.28, 
    22.68,  22.95,  23.36,  23.64,  23.92,  24.29,  24.55,  24.92,  25.25,  25.86,  26.18,  26.42,  26.69,  27.04,  27.34, 
    27.63,  27.92,  28.10,  28.49,  28.94,  29.19,  29.63,  30.01,  30.21,  30.55,  30.87,  31.26,  31.40,  31.82,  32.13, 
    32.43,  32.76,  33.08,  33.39,  33.66,  34.00,  34.33,  34.62,  34.95,  35.27,  35.58,  35.93,  36.22,  36.39,  36.66, 
    36.95,  37.26,  37.55,  37.79,  37.85,  38.14,  38.34,  38.58,  38.90,  39.14,  39.41,  39.68,  39.92,  40.19,  40.46, 
    40.84,  41.24,  41.64,  41.93,  42.06,  42.44,  42.55,  42.95,  43.02,  43.33,  43.48,  43.68,  43.97,  44.21,  44.43, 
    44.78,  45.09,  45.35,  45.44,  45.68,  46.03,  46.39,  46.25,  46.58,  46.80,  47.00,  47.11,  47.30,  47.63,  47.87,  
    48.07                                                                                                                      
	};
	
	const float Liu_W[151] = 
	{
    9.25,    9.41,    10.32,   11.96,   13.77,   15.06,   17.67,   19.53,   20.97,   23.14,   24.93,   27.27,   29.36,   31.96,   34.52, 
    36.30,   38.60,   40.82,   43.28,   45.49,   47.96,   50.21,   52.59,   55.11,   57.27,   59.69,   62.35,   64.23,   66.52,   68.78, 
    71.11,   72.75,   75.46,   78.58,   80.17,   81.90,   84.00,   86.08,   88.33,   90.48,   92.34,   94.53,   95.93,   97.89,   99.87, 
    101.99,  103.27,  105.24,  107.28,  109.07,  110.85,  112.33,  114.30,  116.17,  118.31,  119.88,  121.72,  123.07,  124.94,  127.15, 
    128.97,  131.05,  132.89,  134.46,  136.30,  138.01,  140.23,  141.71,  143.36,  145.64,  147.27,  149.84,  151.15,  152.65,  154.16, 
    155.93,  157.85,  159.09,  161.27,  162.16,  164.08,  165.18,  166.89,  168.80,  170.85,  172.23,  173.60,  175.47,  177.44,  178.67, 
    180.05,  183.53,  185.18,  187.73,  188.42,  189.38,  190.62,  192.13,  193.84,  195.62,  197.66,  199.75,  201.80,  203.31,  204.55, 
    206.06,  207.43,  208.93,  210.30,  211.68,  212.41,  213.92,  215.03,  216.27,  218.27,  219.64,  220.25,  221.75,  222.99,  224.10, 
    225.34,  227.76,  228.87,  230.11,  230.71,  232.58,  233.32,  234.56,  236.29,  237.40,  238.76,  240.00,  241.84,  242.60,  244.57, 
    244.96,  245.96,  246.34,  246.97,  249.38,  250.62,  251.72,  253.22,  254.19,  255.29,  256.40,  257.50,  258.60,  259.95,  261.45, 
    262.68                                                                  
	};
	
	const float Liu_M[151] = 
	{
		101.86,  101.93,  113.48,  122.68,  134.11,  155.03,  174.13,  197.53,  219.79,  248.52,  271.42,  304.96,  331.88,  364.03,  392.31, 
    421.37,  447.52,  475.60,  516.60,  547.46,  574.75,  602.48,  633.14,  665.84,  700.19,  726.51,  753.27,  786.16,  814.79,  846.98, 
    870.94,  898.84,  930.26,  960.99,  981.22,  1006.35, 1035.18, 1060.52, 1086.49, 1110.11, 1138.94, 1166.58, 1192.73, 1216.99, 1242.97, 
    1269.54, 1293.78, 1321.69, 1348.12, 1375.60, 1403.47, 1428.18, 1453.25, 1477.30, 1499.19, 1521.07, 1546.20, 1570.18, 1594.95, 1620.89, 
    1644.37, 1662.75, 1685.57, 1709.65, 1730.36, 1755.58, 1778.01, 1799.70, 1821.54, 1847.38, 1870.71, 1890.51, 1913.09, 1933.61, 1958.75, 
    1979.69, 2001.45, 2020.13, 2042.55, 2061.68, 2088.69, 2179.63, 2140.68, 2157.93, 2179.31, 2197.51, 2218.52, 1235.14, 2256.89, 2272.53,
    2290.81, 2304.15, 2322.32, 2335.23, 2354.66, 2377.02, 2391.23, 2408.09, 2425.49, 2443.00, 2455.11, 2482.10, 2510.47, 2523.50, 2543.19, 
    2561.63, 2588.44, 2605.27, 2624.36, 2643.75, 2660.76, 2684.40, 2705.13, 2721.48, 2739.12, 2752.29, 2770.74, 2785.12, 2804.41, 2824.22, 
    2843.37, 2867.08, 2885.34, 2901.24, 2918.76, 2934.27, 2961.32, 2978.49, 2992.04, 3010.98, 3029.47, 3043.39, 3060.81, 3084.73, 3103.50, 
    3116.10, 3129.82, 3147.98, 3167.49, 3185.03, 3203.46, 3221.25, 3227.05, 3244.18, 3254.86, 3270.82, 3284.82, 3299.58, 3309.18, 3329.55, 
    3345.49                                                                                                              
	};



#define Comm_Uart         huart4                    /* 调用串口 */



#pragma pack(1)
typedef struct
{
	uint8_t ID[2];                          /* 设备ID */
	uint8_t Data_Len[2];                    /* 数据长度 */
	uint8_t Cmd[2];                         /* 命令字符 */
	uint8_t Data[600];                     /* 数据 */
	uint8_t sum;                         /* 校验和 */
}Comm_Format_TypeDef;           
													           
typedef struct                       
{                                    
	uint8_t valid;                       /* 数据有效标记 */
	uint16_t comm_cmd;
	Comm_Format_TypeDef  Format_Data;    /* 帧数据结构体 */
}Comm_Function_TypeDef;

#pragma pack()




Comm_Function_TypeDef Comm_Function_Struct; /* 通讯帧结构体 */

LED_Para_TypeDef LED_Para_Struct ;  /*LED结构体*/

Bootloader_Address_Typedef  Bootloader_Address_Struct;

Material_solve_parameter_Typedef  Material_solve_parameter_Struct ;
OLAT_parameter_Typedef     OLAT_parameter_Struct ;
Relighting_parameter_Typedef  Relighting_parameter_Struct ;

//Material_solve_parameter_Typedef  Bootloader_Address_Struct;

 Power_Data_TypeDef M_RGBW_POWER;
 Power_Data_TypeDef AREA_POWER;
 Power_Data_TypeDef CENTER_RGBW_POWER;
 Power_Data_TypeDef CENTEN_AC_POWER;

MCP4728_Config_TypeDef MCP4728_1_Config_Struct = 
{
	.MCP4728_I2C         = &hi2c1,
	.MCP4728_DevAddress  = 0xC0,
};
MCP4728_Config_TypeDef MCP4728_2_Config_Struct = 
{
	.MCP4728_I2C         = &hi2c2,
	.MCP4728_DevAddress  = 0xC0,
};
MCP4728_Config_TypeDef MCP4728_3_Config_Struct = 
{
	.MCP4728_I2C         = &hi2c3,
	.MCP4728_DevAddress  = 0xC0,
};


static int8_t Temp_Filter_5Get1_Handler(int8_t *pData);

/* 数据发送函数 */
//static void Comm_Send_Data(uint8_t cmd_id,uint8_t cmd,uint8_t *data,uint8_t len);
//static void Comm_Send_Data(uint8_t *Data,uint8_t Len);



void Comm_Init(void)
{
	RS485_Mode_Rx();
	HAL_UARTEx_ReceiveToIdle_DMA(&Comm_Uart,Comm_RxBuff,Comm_RxBuffSzie);
}


static void Comm_Frame_Handler(uint8_t *pData,uint16_t Size)
{
	uint8_t sum = 0;
	uint16_t  data_len;
	data_len = (pData[Comm_RxFrame_Len+1]<<8) +  pData[Comm_RxFrame_Len];    //数据长度两个字节
	
	if( Size >= Comm_Frame_RxSize)
	{
		for(uint16_t i = 0 ; i < data_len; i++)
		{
			sum += pData[i+4];
		}
		Comm_Function_Struct.Format_Data.sum = sum;
		if(sum == pData[data_len+4])
		{
			if(Comm_Header_1 == pData[Comm_RxFrame_Header1] && (pData[Comm_RxFrame_Header2] == Comm_Header_2)) /*帧头4C  55*/
			{
				Comm_Function_Struct.valid = Comm_Valid;
				memcpy(&Comm_Function_Struct.Format_Data.ID,pData,(data_len + 5));				
			}
		}
	}
}

#define ParameterMem_Default  0x55
uint8_t Flag_ParameterMem = ParameterMem_Default;


FlashParam_TypeDef FlashParam_Struct = {0};




void Comm_Function_Handler(void)
{
	uint8_t DataBuff[16];
  uint16_t Comm_cmd;

	memset(DataBuff,0x00,sizeof(DataBuff));
	
//	pDataBuff = DataBuff;
	
	if(Comm_Function_Struct.valid == Comm_Valid)
	{
		Comm_Function_Struct.valid = Comm_Invalid;
		Comm_cmd = (Comm_Function_Struct.Format_Data.Cmd[0]<<8) + Comm_Function_Struct.Format_Data.Cmd[1];
		Comm_Function_Struct.comm_cmd = Comm_cmd;
		switch ( Comm_Function_Struct.comm_cmd )
		{
			case CMD_Set_Adj_mode_parameter:                                /*装调模式参数配置*/
			{

				if((LED_Para_Struct.Device_ID == (Comm_Function_Struct.Format_Data.Data[1]<<8)+Comm_Function_Struct.Format_Data.Data[0])||(ALL_ID == (Comm_Function_Struct.Format_Data.Data[1]<<8)+Comm_Function_Struct.Format_Data.Data[0]))
				{
					LED_Para_Struct.Area_LED.data = Comm_Function_Struct.Format_Data.Data[2];
					LED_Para_Struct.Channel_Color.data = Comm_Function_Struct.Format_Data.Data[3];
					LED_Para_Struct.Power = (Comm_Function_Struct.Format_Data.Data[5]<<8) + Comm_Function_Struct.Format_Data.Data[4];
				  LED_Para_Struct.State_LED = Comm_Function_Struct.Format_Data.Data[6];	
					LED_Para_Struct.Adj_mode_valid = 0x01;
					
					if(LED_Para_Struct.Power > CMD_Power_Max_Install_adjust) LED_Para_Struct.Power = CMD_Power_Max_Install_adjust;

				}
			}
			break;
			
			case CMD_Set_Adj_mode_Relighting_parameter:                                /*装调模式重光照参数配置*/
			{

				if((LED_Para_Struct.Device_ID == (Comm_Function_Struct.Format_Data.Data[1]<<8)+Comm_Function_Struct.Format_Data.Data[0])||(ALL_ID == (Comm_Function_Struct.Format_Data.Data[1]<<8)+Comm_Function_Struct.Format_Data.Data[0]))
				{
					LED_Para_Struct.Relighting_parameter.Coordinate_X = (Comm_Function_Struct.Format_Data.Data[3]<<8)+Comm_Function_Struct.Format_Data.Data[2];
					LED_Para_Struct.Relighting_parameter.Coordinate_Y = (Comm_Function_Struct.Format_Data.Data[5]<<8)+Comm_Function_Struct.Format_Data.Data[4];
					LED_Para_Struct.Power = (Comm_Function_Struct.Format_Data.Data[7]<<8) + Comm_Function_Struct.Format_Data.Data[6];
			    LED_Para_Struct.Relighting_State_LED = Comm_Function_Struct.Format_Data.Data[8];
					if((LED_Para_Struct.Power>0)&&(LED_Para_Struct.Relighting_State_LED))
					{
					AREA_POWER.Power_A = LED_Para_Struct.Power;
					AREA_POWER.Power_B = LED_Para_Struct.Power;
					AREA_POWER.Power_C = LED_Para_Struct.Power;
					AREA_POWER.Power_D = LED_Para_Struct.Power;
					M_RGBW_POWER.Power_A = LED_Para_Struct.Power;
					M_RGBW_POWER.Power_B = LED_Para_Struct.Power;
					M_RGBW_POWER.Power_C = LED_Para_Struct.Power;
					M_RGBW_POWER.Power_D = LED_Para_Struct.Power;
					CENTEN_AC_POWER.Power_A = LED_Para_Struct.Power;
					CENTEN_AC_POWER.Power_B = LED_Para_Struct.Power;
					CENTEN_AC_POWER.Power_C = LED_Para_Struct.Power;

					}
					else
					{
            LED_Power_Min();	
            LED_Para_Struct.Relighting_State_LED = 0;						

					}
					LED_Power_Updata();
					
          LED_Dimming_Updata();
				}
			}
			break;
			
			
			case  CMD_Set_LED_Parameter_Material :                                /*材质解算模式下光源参数配置*/
			 {
				 LED_Para_Struct.Material_solve_parameter.data_pake_num = (Comm_Function_Struct.Format_Data.Data[1]<<8)+Comm_Function_Struct.Format_Data.Data[0];   //数据包序号
				 LED_Para_Struct.Material_solve_parameter.sub_pake_num = Comm_Function_Struct.Format_Data.Data[2];
        /*一帧数据中的分包数据包含49个光源数据，首先先确定光源ID对应的数据在第几个分包数据中，再确定光源ID对应的数据在分包数据中的位置*/
        /*光源数据包序号从1开始，数据分包序号也从1开始，光源数据数组编号从0开始*/
        /*光源数据低位在前，高位在后，数据格式为：数据包序号（2字节）+分包号（1字节）+光场数据（588字节对应49个光源）*/				 
				 if(((LED_Para_Struct.Device_ID %49 == 0) && ((LED_Para_Struct.Device_ID /49 ) ==LED_Para_Struct.Material_solve_parameter.sub_pake_num))||
					 ((LED_Para_Struct.Device_ID /49 +1) ==LED_Para_Struct.Material_solve_parameter.sub_pake_num) )  //一帧分包数据包含49个光源数据
				 {

					 LED_Para_Struct.Material_solve_parameter.R_Power[LED_Para_Struct.Material_solve_parameter.data_pake_num-1] = Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*12]
					                                                                                                             +(Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*12+1]<<8);
				 	 LED_Para_Struct.Material_solve_parameter.R_Pulse[LED_Para_Struct.Material_solve_parameter.data_pake_num-1] = Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*12+2]
					                                                                                                             +(Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*12+3]<<8);
					 LED_Para_Struct.Material_solve_parameter.G_Power[LED_Para_Struct.Material_solve_parameter.data_pake_num-1] = Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*12+4]
					                                                                                                             +(Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*12+5]<<8);
				 	 LED_Para_Struct.Material_solve_parameter.G_Pulse[LED_Para_Struct.Material_solve_parameter.data_pake_num-1] = Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*12+6]
					                                                                                                             +(Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*12+7]<<8);
					 LED_Para_Struct.Material_solve_parameter.B_Power[LED_Para_Struct.Material_solve_parameter.data_pake_num-1] = Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*12+8]
					                                                                                                             +(Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*12+9]<<8);
				 	 LED_Para_Struct.Material_solve_parameter.B_Pulse[LED_Para_Struct.Material_solve_parameter.data_pake_num-1] = Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*12+10]
					                                                                                                             +(Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*12+11]<<8);			
			    
				 
				 }
			 }
			 break;
			 
			 case  CMD_Set_OLAT_mode_parameter :                                /*OLAT模式下光源参数配置*/
			 {
				 LED_Para_Struct.OLAT_parameter.data_pake_num = (Comm_Function_Struct.Format_Data.Data[1]<<8)+Comm_Function_Struct.Format_Data.Data[0];   //数据包序号
				 LED_Para_Struct.OLAT_parameter.sub_pake_num = Comm_Function_Struct.Format_Data.Data[2];                                                  //分包号
        /*一帧数据中的分包数据包含97个光源数据，首先先确定光源ID对应的数据在第几个分包数据中，再确定光源ID对应的数据在分包数据中的位置*/
        /*光源数据包序号从1开始，数据分包序号也从1开始，光源数据数组编号从0开始*/
        /*光源数据低位在前，高位在后，数据格式为：数据包序号（2字节）+分包号（1字节）+光场数据（388字节对应97个光源）*/				 
//				 if((LED_Para_Struct.Device_ID %97 == 0) && ((LED_Para_Struct.Device_ID /97 ) ==LED_Para_Struct.OLAT_parameter.sub_pake_num))
//				 {
//					 LED_Para_Struct.OLAT_parameter.W_Power[LED_Para_Struct.OLAT_parameter.data_pake_num-1] = Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %97)*4]
//					                                                                                         +(Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %97)*4+1]<<8);
//				 	 LED_Para_Struct.OLAT_parameter.W_Pulse[LED_Para_Struct.OLAT_parameter.data_pake_num-1] = Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %97)*4+2]
//   			                                                                                           +(Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %97)*4+3]<<8);
//				 }
//				 else
//				 {
				 if(((LED_Para_Struct.Device_ID %97 == 0) && ((LED_Para_Struct.Device_ID /97 ) ==LED_Para_Struct.OLAT_parameter.sub_pake_num))||
					 ((LED_Para_Struct.Device_ID /97 +1) ==LED_Para_Struct.OLAT_parameter.sub_pake_num) )  //一帧分包数据包含97个光源数据
				 {
					 LED_Para_Struct.OLAT_parameter.W_Power[LED_Para_Struct.OLAT_parameter.data_pake_num-1] = Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %97)*4]
					                                                                                         +(Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %97)*4+1]<<8);
				 	 LED_Para_Struct.OLAT_parameter.W_Pulse[LED_Para_Struct.OLAT_parameter.data_pake_num-1] = Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %97)*4+2]
   			                                                                                           +(Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %97)*4+3]<<8);
				 }
			 
			 
			 }
			 break;
			 
			 case  CMD_Set_Relighting_mode_parameter :                                /*重光照模式下光源参数配置*/
			 {
				 LED_Para_Struct.Relighting_parameter.data_pake_num = (Comm_Function_Struct.Format_Data.Data[1]<<8)+Comm_Function_Struct.Format_Data.Data[0];   //数据包序号
				 LED_Para_Struct.Relighting_parameter.sub_pake_num = Comm_Function_Struct.Format_Data.Data[2];
        /*一帧数据中的分包数据包含49个光源数据，首先先确定光源ID对应的数据在第几个分包数据中，再确定光源ID对应的数据在分包数据中的位置*/
        /*光源数据包序号从1开始，数据分包序号也从1开始，光源数据数组编号从0开始*/
        /*光源数据低位在前，高位在后，数据格式为：数据包序号（2字节）+分包号（1字节）+光场数据（392字节对应49个光源）*/				 
				if(((LED_Para_Struct.Device_ID %49 == 0) && ((LED_Para_Struct.Device_ID /49 ) ==LED_Para_Struct.Relighting_parameter.sub_pake_num))||
					 ((LED_Para_Struct.Device_ID /49 +1) ==LED_Para_Struct.Relighting_parameter.sub_pake_num) )  //一帧分包数据包含49个光源数据
				 {
					 LED_Para_Struct.Relighting_parameter.Color_Coordinate_X[LED_Para_Struct.Relighting_parameter.data_pake_num-1] = Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*8]
                                                                                                                           +(Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*8+1]<<8);					 
					 LED_Para_Struct.Relighting_parameter.Color_Coordinate_Y[LED_Para_Struct.Relighting_parameter.data_pake_num-1] = Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*8+2]
                                                                                                                           +(Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*8+3]<<8);
					 LED_Para_Struct.Relighting_parameter.Power[LED_Para_Struct.Relighting_parameter.data_pake_num-1] = Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*8+4]
                                                                                                             +(Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*8+5]<<8);					 
					 LED_Para_Struct.Relighting_parameter.Pulse[LED_Para_Struct.Relighting_parameter.data_pake_num-1] = Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*8+6]
                                                                                                             +(Comm_Function_Struct.Format_Data.Data[3+((LED_Para_Struct.Device_ID-1) %49)*8+7]<<8);
				 
				 }
			 }
			 break;
			 
			 
			 case  CMD_parameter_Set_Over :                                /*参数配置完成*/
			 {
				 if(LED_Para_Struct.Mode == Material_solve_MODE)
				 {
					 for(uint16_t i=0;i<2500;i++)
					 {					
						 Material_solve_parameter_Struct.R_Power[i] = LED_Para_Struct.Material_solve_parameter.R_Power[i];
						 Material_solve_parameter_Struct.R_Pulse[i] = LED_Para_Struct.Material_solve_parameter.R_Pulse[i];				
						 Material_solve_parameter_Struct.G_Power[i] = LED_Para_Struct.Material_solve_parameter.G_Power[i];				
						 Material_solve_parameter_Struct.G_Pulse[i] = LED_Para_Struct.Material_solve_parameter.G_Pulse[i];			
						 Material_solve_parameter_Struct.B_Power[i] = LED_Para_Struct.Material_solve_parameter.B_Power[i];					
						 Material_solve_parameter_Struct.B_Pulse[i] = LED_Para_Struct.Material_solve_parameter.B_Pulse[i];
					 }
					 STM32_Flash_EraseWrite(FlashAddr_Material_Solve ,(uint8_t *)&Material_solve_parameter_Struct,  sizeof(Material_solve_parameter_Struct));
					 
				 }
				 	else if(LED_Para_Struct.Mode == OLAT_MODE)
				 {
					 for(uint16_t i=0;i<2500;i++)
					 {					
						 OLAT_parameter_Struct.W_Power[i] = LED_Para_Struct.OLAT_parameter.W_Power[i];
						 OLAT_parameter_Struct.W_Pulse[i] = LED_Para_Struct.OLAT_parameter.W_Pulse[i];				

					 }
					 STM32_Flash_EraseWrite(FlashAddr_OLAT ,(uint8_t *)&OLAT_parameter_Struct,  sizeof(OLAT_parameter_Struct));
					 
				 }
				 
				 	else if(LED_Para_Struct.Mode == Relighting_MODE)
				 {
					 for(uint16_t i=0;i<2500;i++)
					 {					
						 Relighting_parameter_Struct.Color_Coordinate_X[i] = LED_Para_Struct.Relighting_parameter.Color_Coordinate_X[i];
						 Relighting_parameter_Struct.Color_Coordinate_Y[i] = LED_Para_Struct.Relighting_parameter.Color_Coordinate_Y[i];
						 Relighting_parameter_Struct.Power[i] = LED_Para_Struct.Relighting_parameter.Power[i];
             Relighting_parameter_Struct.Pulse[i] = LED_Para_Struct.Relighting_parameter.Pulse[i];
					 }
					 STM32_Flash_EraseWrite(FlashAddr_Relighting ,(uint8_t *)&Relighting_parameter_Struct,  sizeof(Relighting_parameter_Struct));
					 
				 }
				 
			 }
			 break;			 

			case CMD_Set_Loopdata_parameter:  /*循环数据组配置*/
			{
				LED_Para_Struct.Data_Cycle_quantity = ((Comm_Function_Struct.Format_Data.Data[1]<<8)+ Comm_Function_Struct.Format_Data.Data[0])+0x01;        /*循环数据量*/

			}
			break;
			 			 
			

			/*设置ID*/
	   case CMD_Set_DeviceID:
			{
				if(LED_Para_Struct.Device_ID == (Comm_Function_Struct.Format_Data.Data[1]<<8)+Comm_Function_Struct.Format_Data.Data[0])
				{
					LED_Para_Struct.Device_ID = (Comm_Function_Struct.Format_Data.Data[3]<<8)+Comm_Function_Struct.Format_Data.Data[2];
				
				

				
				/* 参数保存 */
				FlashParam_Struct.DeviceID.Flag = ParameterMem_Default;
				FlashParam_Struct.DeviceID._0   = LED_Para_Struct.Device_ID;
				FlashParam_Struct.DeviceID._1   = LED_Para_Struct.Device_ID;
				FlashParam_Struct.DeviceID._2   = LED_Para_Struct.Device_ID;
				STM32_Flash_EraseWrite(FlashAddr_DeviceID,&FlashParam_Struct.DeviceID.Flag,  sizeof(FlashParam_Struct.DeviceID));
				Display_update(Display_Mode_7,LED_Para_Struct.Device_ID,LED_Para_Struct.State_display);

				}
			}
			break;

			case CMD_Set_MODE:/* 设置模式 */
			{
				LED_Para_Struct.Mode = Comm_Function_Struct.Format_Data.Data[0];
				
				/* 参数保存 */		
//				LED_Para_Struct.OLAT_cnt = 0;
//				LED_Para_Struct.OLAT_Full_cnt = 0;
//				LED_Para_Struct.OLAT_Stop_Flag = 0;
				LED_Para_Struct.Trig_Cnt = 0;            /*设置模式，触发次数清0*/
				FlashParam_Struct.Mode.Flag = ParameterMem_Default;
				FlashParam_Struct.Mode._0   = LED_Para_Struct.Mode;
				FlashParam_Struct.Mode._1   = LED_Para_Struct.Mode;
				FlashParam_Struct.Mode._2   = LED_Para_Struct.Mode;
				STM32_Flash_EraseWrite(FlashAddr_Mode,&FlashParam_Struct.Mode.Flag,  sizeof(FlashParam_Struct.Mode));
				

				
			}
			break;
			
	
		  case CMD_Set_Display:  /* 光源ID显示配置 */
			{
				if((((Comm_Function_Struct.Format_Data.Data[1]<<8)+Comm_Function_Struct.Format_Data.Data[0]) == LED_Para_Struct.Device_ID)||(((Comm_Function_Struct.Format_Data.Data[1]<<8)+Comm_Function_Struct.Format_Data.Data[0]) == ALL_ID))
				{
					LED_Para_Struct.State_display = Comm_Function_Struct.Format_Data.Data[2];			
				
					Display_update(Display_Mode_7,LED_Para_Struct.Device_ID,LED_Para_Struct.State_display);

				}
			}
			break;	
			
			case CMD_Set_Frequency:  /*系统工作频率配置*/
			{
				LED_Para_Struct.Trig_Frequency = (Comm_Function_Struct.Format_Data.Data[1]<<8)+Comm_Function_Struct.Format_Data.Data[0];        /*光源触发频率*/
				LED_Para_Struct.Trig_Cnt_Set = (Comm_Function_Struct.Format_Data.Data[3]<<8)+Comm_Function_Struct.Format_Data.Data[2];          /*光源触发次数*/
				LED_Para_Struct.Camera_Type = Comm_Function_Struct.Format_Data.Data[4];                                                         /*相机类型*/
				LED_Para_Struct.Camera_div_Frequency = Comm_Function_Struct.Format_Data.Data[5];                                                /*相机分频参数*/
				LED_Para_Struct.Camera_Trig_Pulse = (Comm_Function_Struct.Format_Data.Data[7]<<8)+Comm_Function_Struct.Format_Data.Data[6];     /*相机触发脉宽*/
				LED_Para_Struct.Camera_Trig_Delay = (Comm_Function_Struct.Format_Data.Data[9]<<8)+Comm_Function_Struct.Format_Data.Data[8];     /*相机触发延迟*/
			}
			break;
			
			case CMD_Set_start:      /*软起动*/
			{
  			LED_Para_Struct.Trig_Cnt = 0;      //触发次数清0
				LED_Para_Struct.Camera_Trig_cnt = 0;
				HAL_NVIC_EnableIRQ(EXTI0_IRQn);    //外部中断开启


			}
			break;
			
			case CMD_Set_stop:      /*停止*/
			{
  			LED_Para_Struct.Trig_Cnt = 0;      //触发次数清0
				LED_Para_Struct.Camera_Trig_cnt = 0;
				HAL_NVIC_DisableIRQ(EXTI0_IRQn);    //外部中断关闭
				LED_Para_Struct.Start_LED_TRIG_Flag = 0;              /*状态清0*/		


				LED_Power_Min();
				LED_Power_Updata();
			}
			break;
			
			case CMD_Set_Firmware_Update_ready:   /*固件升级准备*/
			{
				LED_Para_Struct.Upgrade_Device = Comm_Function_Struct.Format_Data.Data[0];
				LED_Para_Struct.Upgrade_Size = Comm_Function_Struct.Format_Data.Data[1];
				if(LED_Para_Struct.Upgrade_Device == 0x00 )   /*光源固件升级*/
				{
					if(Bootloader_Address_Struct.APP_Run_Flag ==  APP_Backup1)
					{
						STM32_Flash_Erase(GetSector(APP_Backup2_ADDR), APP_Sectors_NUM); // Erase app running area	
					}
					else
					{
						STM32_Flash_Erase(GetSector(APP_Backup1_ADDR), APP_Sectors_NUM); // Erase app running area
					}
				}
			}
			break;

			case CMD_Set_Firmware_Update_data:   /*固件升级数据配置*/
			{
				LED_Para_Struct.Upgrade_Device = Comm_Function_Struct.Format_Data.Data[0];
				LED_Para_Struct.Upgrade_Sequence = Comm_Function_Struct.Format_Data.Data[1];
				LED_Para_Struct.Upgrade_status = 0;
				if(LED_Para_Struct.Upgrade_Device == 0x00 )   /*光源固件升级*/
				{
					
					if( Bootloader_Address_Struct.APP_Run_Flag ==  APP_Backup1)
				
					{
					STM32_Flash_Write(APP_Backup2_ADDR + LED_Para_Struct.Upgrade_Sequence * 500 ,&Comm_Function_Struct.Format_Data.Data[2],500);
				
					LED_Para_Struct.Upgrade_status = !memcmp(&Comm_Function_Struct.Format_Data.Data[2],(uint8_t*)(APP_Backup2_ADDR + LED_Para_Struct.Upgrade_Sequence * 500),500);
									
					}
				else
				{
					STM32_Flash_Write(APP_Backup1_ADDR + LED_Para_Struct.Upgrade_Sequence * 500 ,&Comm_Function_Struct.Format_Data.Data[2],500);
				
					LED_Para_Struct.Upgrade_status = !memcmp(&Comm_Function_Struct.Format_Data.Data[2],(uint8_t*)(APP_Backup1_ADDR + LED_Para_Struct.Upgrade_Sequence * 500),500);

				}
				}
			}
			break;

			case CMD_Set_Firmware_Update_start:   /*开始固件升级*/
			{
				LED_Para_Struct.Upgrade_Device = Comm_Function_Struct.Format_Data.Data[0];
				if(LED_Para_Struct.Upgrade_Device == 0x00 )   /*光源固件升级*/
				{
					
					if( Bootloader_Address_Struct.APP_Run_Flag ==  APP_Backup1)			
					{
					Bootloader_Address_Struct.APP_Backup_Flag = APP_Backup2_ADDR;
					}
				
					else
				
					{
					Bootloader_Address_Struct.APP_Backup_Flag = APP_Backup1_ADDR;
					}

				STM32_Flash_EraseWrite(Bootloader_Address_Base,(uint8_t *)&Bootloader_Address_Struct,sizeof(Bootloader_Address_Struct));
				
				HAL_Delay(1000);
				__set_PRIMASK(1);
				HAL_NVIC_SystemReset();
				}
			}
			break;	

			case CMD_INFO_LightSource_Temperature:                    /*查询指定光源灯板温度*/
			{
				uint16_t Query_ID = (Comm_Function_Struct.Format_Data.Data[1]<<8) + Comm_Function_Struct.Format_Data.Data[0];

				if(LED_Para_Struct.Device_ID == Query_ID)
				{
					Comm_Send_Temperature_Response(LED_Para_Struct.Device_ID, LED_Para_Struct.Temp_Current);
				}
			}
			break;			
			
			default :
			{
				
			}
			break;
		}
	}
}
static uint8_t Comm_RespBuff[16]; 
/* 数据发送函数 */
//static void Comm_Send_Data(uint8_t *Data,uint8_t Len)
//{
//	uint8_t *pData;
//	uint8_t sum = 0;
//	
//	pData = Comm_TxBuff;	

////		*pData++ = Data[0];
////		*pData++ = Data[1];
////		*pData++ = Data[2];
////		*pData++ = Data[3];
////	  *pData++ = Data[4];

//		for( uint8_t i = 0 ; i < Len-1 ; i++ )
//		{
//			sum += *pData++ = Data[i];
//		}

//		*pData++ = sum;
//		
//		RS485_Mode_Tx();

//		
//		HAL_UART_Transmit_DMA(&Comm_Uart,Comm_TxBuff,pData - Comm_TxBuff);

//}
static void Comm_Send_Data(uint8_t *pData,uint16_t Len)
{
	HAL_UART_AbortReceive(&Comm_Uart);                              /* 先停掉常驻接收，避免自发自收 */
	RS485_Mode_Tx();
	HAL_UART_Transmit_DMA(&Comm_Uart,pData,Len);
	/* 发送完成后在 HAL_UART_TxCpltCallback 中切回接收并重新挂起RX */
}

static void Comm_Send_Temperature_Response(uint16_t Light_ID,int8_t Temp)
{
	uint8_t sum = 0;

	Comm_RespBuff[0] = 0x4C;                                        /* 帧头 L */
	Comm_RespBuff[1] = 0x55;                                        /* 帧头 U */
	Comm_RespBuff[2] = 0x05;                                        /* Len 低字节 */
	Comm_RespBuff[3] = 0x00;                                        /* Len 高字节 */
	Comm_RespBuff[4] = 0x03;                                        /* Cmd 高字节 0x0355 */
	Comm_RespBuff[5] = 0x55;                                        /* Cmd 低字节 0x0355 */
	Comm_RespBuff[6] = (uint8_t)(Light_ID & 0xFF);                  /* 光源ID 低字节 */
	Comm_RespBuff[7] = (uint8_t)((Light_ID >> 8) & 0xFF);           /* 光源ID 高字节 */
	Comm_RespBuff[8] = (uint8_t)Temp;                               /* 当前温度 */

	for(uint8_t i = 4 ; i < 9 ; i++)
	{
		sum += Comm_RespBuff[i];
	}
	Comm_RespBuff[9] = sum;                                         /* 校验和 */

	Comm_Send_Data(Comm_RespBuff,10);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart == &Comm_Uart)
	{
		Comm_Frame_Handler(Comm_RxBuff,Size);
		HAL_UARTEx_ReceiveToIdle_DMA(&Comm_Uart,Comm_RxBuff,Comm_RxBuffSzie);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &Comm_Uart)
	{
		RS485_Mode_Rx();			
		HAL_UARTEx_ReceiveToIdle_DMA(&Comm_Uart,Comm_RxBuff,Comm_RxBuffSzie);
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart == &Comm_Uart)
    {
        __HAL_UART_CLEAR_OREFLAG(huart);
        HAL_UART_AbortReceive(huart);
        HAL_UARTEx_ReceiveToIdle_DMA(huart, Comm_RxBuff, Comm_RxBuffSzie);
    }
}


typedef enum
{
	Default_Parameter_OK        ,  /* 数据正常 */
	Default_Parameter_Correct   ,  /* 数据纠偏 */
	Default_Parameter_Err       ,  /* 数据异常 */

}Default_StateTypeDef;


uint8_t Storage_Parameter_Correct(uint8_t* pData_0,uint8_t* pData_1,uint8_t* pData_2,uint16_t Len)
{
	uint8_t State = Default_Parameter_Err;

		if(memcmp(pData_0,pData_1,Len) == RESET)     /* 0号、1号数据相等 */
		{                                            
			if(memcmp(pData_0,pData_2,Len) != RESET)   /* 2号数据出错 */
			{                                          
				memcpy(pData_2,pData_0,Len);             /* 2号数据纠错 */
				State = Default_Parameter_Correct;       /* 数据需要重新保存 */
			}                                          
			else                                       /* 数据无异常 */
			{                                          
				State = Default_Parameter_OK;            /* 数据不需要保存 */
			}                                          
		}                                            
		else                                         /* 0号、1号数据不等 */
		{                                            
			if(memcmp(pData_0,pData_2,Len) == RESET)   /* 0号、2号相等，1号出错 */
			{                                          
				memcpy(pData_1,pData_0,Len);             /* 1号数据纠错 */
				State = Default_Parameter_Correct;       /* 数据需要重新保存 */
			}                                          
			else                                       /* 0号、2号不等 */
			{                                          
				if(memcmp(pData_1,pData_2,Len) == RESET) /* 1号、2号相等，0号出错 */
				{                                        
					memcpy(pData_0,pData_1,Len);           /* 0号数据纠错 */
					State = Default_Parameter_Correct;     /* 数据需要重新保存 */
				}                                        
				else                                     /* 0号、1号、2号，数据不等 */
				{                                        
					State = Default_Parameter_Err;         /* 数据全部异常 */
				}
			}
		}
		return State;
}

void Parameter_Init(void)/* 参数初始化*/
{	
	uint8_t State;
	
 // LED_Para_TypeDef LED_Para_Struct= {0};
	
	Material_solve_parameter_Typedef  Material_solve_parameter_Struct = {0};
	OLAT_parameter_Typedef     OLAT_parameter_Struct ={0};
  Relighting_parameter_Typedef  Relighting_parameter_Struct ={0};
	
	LED_Para_Struct._18b20_EN =  SET;
  LED_Para_Struct.Start_LED_TRIG_ON = 0;

	// 读取APP参数 Bootloader_Address_Struct
		STM32_Flash_Read(Bootloader_Address_Base,(uint8_t *)&Bootloader_Address_Struct,sizeof(Bootloader_Address_Struct));	

		if(Bootloader_Address_Struct.APP_Backup_Flag == 0xFFFFFFFF)
		{
			Bootloader_Address_Struct.APP_Backup_Flag = 0xFFFFFFFF;
			
			STM32_Flash_EraseWrite(Bootloader_Address_Base,(uint8_t *)&Bootloader_Address_Struct,sizeof(Bootloader_Address_Struct));
		}
	
	
	/* 光源ID */
	STM32_Flash_Read(FlashAddr_DeviceID,&FlashParam_Struct.DeviceID.Flag,sizeof(FlashParam_Struct.DeviceID));
	if(FlashParam_Struct.DeviceID.Flag != ParameterMem_Default)/* 默认情况 */
	{
		LED_Para_Struct.Device_ID = Default_ID;
	}
	else/* 有数据*/
	{
		State = Storage_Parameter_Correct((uint8_t *)&FlashParam_Struct.DeviceID._0,(uint8_t *)&FlashParam_Struct.DeviceID._1,(uint8_t *)&FlashParam_Struct.DeviceID._2,sizeof(FlashParam_Struct.DeviceID._0));
		
		if(State != Default_Parameter_OK)/*数据出现异常*/
		{
			if(State == Default_Parameter_Err) /*数据全部异常*/
			{
				FlashParam_Struct.DeviceID._0 = 
				FlashParam_Struct.DeviceID._1 = 
				FlashParam_Struct.DeviceID._2 = Default_ID;
			}
			/*数据纠偏*/
			STM32_Flash_EraseWrite(FlashAddr_DeviceID,&FlashParam_Struct.DeviceID.Flag,  sizeof(FlashParam_Struct.DeviceID));
		}
		LED_Para_Struct.Device_ID = FlashParam_Struct.DeviceID._0;
	}
	
	
	
	
	/* LED功率最小化*/
   LED_Power_Min();

	
	/* 默认模式*/
	STM32_Flash_Read(FlashAddr_Mode,&FlashParam_Struct.Mode.Flag,sizeof(FlashParam_Struct.Mode));
	if(FlashParam_Struct.Mode.Flag != ParameterMem_Default)/* 默认情况*/
	{
		LED_Para_Struct.Mode = Default_Mode;
	}
	else/*有数据*/
	{
		State = Storage_Parameter_Correct(&FlashParam_Struct.Mode._0,&FlashParam_Struct.Mode._1,&FlashParam_Struct.Mode._2,sizeof(FlashParam_Struct.Mode._0));
		if(State != Default_Parameter_OK)/*数据出现异常*/
		{
			if(State == Default_Parameter_Err) /*数据全部异常*/
			{
				FlashParam_Struct.Mode._0 = 
				FlashParam_Struct.Mode._1 = 
				FlashParam_Struct.Mode._2 = Default_Mode;
			}
			/* 数据纠偏*/
			STM32_Flash_EraseWrite(FlashAddr_Mode,&FlashParam_Struct.Mode.Flag,  sizeof(FlashParam_Struct.Mode));
		}
		LED_Para_Struct.Mode = FlashParam_Struct.Mode._0;
	}
	
		/* 读取光场数据*/
	
	/* 材质解算光场数据*/
	STM32_Flash_Read(FlashAddr_Material_Solve ,(uint8_t *)&Material_solve_parameter_Struct,  sizeof(Material_solve_parameter_Struct));
	for(uint16_t i=0;i<2500;i++)
	{						
		LED_Para_Struct.Material_solve_parameter.R_Power[i] = Material_solve_parameter_Struct.R_Power[i];
		if(LED_Para_Struct.Material_solve_parameter.R_Power[i] == 0xFFFF)LED_Para_Struct.Material_solve_parameter.R_Power[i] = 0;
		LED_Para_Struct.Material_solve_parameter.R_Pulse[i] = Material_solve_parameter_Struct.R_Pulse[i];	
		if(LED_Para_Struct.Material_solve_parameter.R_Pulse[i] == 0xFFFF) LED_Para_Struct.Material_solve_parameter.R_Pulse[i] = 0;		
		LED_Para_Struct.Material_solve_parameter.G_Power[i] = Material_solve_parameter_Struct.G_Power[i];	
	  if(LED_Para_Struct.Material_solve_parameter.G_Power[i] == 0xFFFF) LED_Para_Struct.Material_solve_parameter.G_Power[i] = 0;		
		LED_Para_Struct.Material_solve_parameter.G_Pulse[i] = Material_solve_parameter_Struct.G_Pulse[i];
		if(LED_Para_Struct.Material_solve_parameter.G_Pulse[i] == 0xFFFF) LED_Para_Struct.Material_solve_parameter.G_Pulse[i] = 0;			
		LED_Para_Struct.Material_solve_parameter.B_Power[i] = Material_solve_parameter_Struct.B_Power[i];	
		if(LED_Para_Struct.Material_solve_parameter.B_Power[i] == 0xFFFF) LED_Para_Struct.Material_solve_parameter.B_Power[i] = 0;
	  LED_Para_Struct.Material_solve_parameter.B_Pulse[i] = Material_solve_parameter_Struct.B_Pulse[i];
		if(LED_Para_Struct.Material_solve_parameter.B_Pulse[i] == 0xFFFF) LED_Para_Struct.Material_solve_parameter.B_Pulse[i] = 0;	
	}
	
	/* OLAT光场数据*/	
	STM32_Flash_Read(FlashAddr_OLAT ,(uint8_t *)&OLAT_parameter_Struct,  sizeof(OLAT_parameter_Struct));
	for(uint16_t i=0;i<2500;i++)
	{						
		LED_Para_Struct.OLAT_parameter.W_Power[i] = OLAT_parameter_Struct.W_Power[i];
		if(LED_Para_Struct.OLAT_parameter.W_Power[i] == 0xFFFF) LED_Para_Struct.OLAT_parameter.W_Power[i] = 0;			
		LED_Para_Struct.OLAT_parameter.W_Pulse[i] = OLAT_parameter_Struct.W_Pulse[i];	
		if(LED_Para_Struct.OLAT_parameter.W_Pulse[i] == 0xFFFF) LED_Para_Struct.OLAT_parameter.W_Pulse[i] = 0;			
	}
	
	/* 重光照光场数据*/			
	STM32_Flash_Read(FlashAddr_Relighting ,(uint8_t *)&Relighting_parameter_Struct,  sizeof(Relighting_parameter_Struct));
	for(uint16_t i=0;i<2500;i++)
	{						
		LED_Para_Struct.Relighting_parameter.Color_Coordinate_X[i] = Relighting_parameter_Struct.Color_Coordinate_X[i];
		LED_Para_Struct.Relighting_parameter.Color_Coordinate_Y[i] = Relighting_parameter_Struct.Color_Coordinate_Y[i];				
		LED_Para_Struct.Relighting_parameter.Power[i] = Relighting_parameter_Struct.Power[i];
		if(LED_Para_Struct.Relighting_parameter.Power[i] == 0xFFFF) LED_Para_Struct.Relighting_parameter.Power[i] = 0;		
		LED_Para_Struct.Relighting_parameter.Pulse[i] = Relighting_parameter_Struct.Pulse[i];	
		if(LED_Para_Struct.Relighting_parameter.Pulse[i] == 0xFFFF) LED_Para_Struct.Relighting_parameter.Pulse[i] = 0;				

	}
	
//	/* 默认开关更新*/
//	LED_Para_Struct.Switch_LED.data = (LED_Para_Struct.Switch_Default * 0x0F);
	LED_Power_Updata();

}

void LED_Power_Updata(void)
{
	MCP4728_Set_Value(&MCP4728_1_Config_Struct, &M_RGBW_POWER);
	MCP4728_Set_Value(&MCP4728_2_Config_Struct, &CENTEN_AC_POWER);
	MCP4728_Set_Value(&MCP4728_3_Config_Struct, &AREA_POWER);
}

void LED_State_Updata(void)
{
	switch(LED_Para_Struct.Mode)
	{
		case Install_adjust_MODE:    //装调模式
		{
			if(LED_Para_Struct.Adj_mode_valid) 
			{
	     if(LED_Para_Struct.Area_LED.bits.Center_Area)        //中心区域
			 {
				 if(LED_Para_Struct.Channel_Color.bits.Channel_R)   //R通道
				 {
					 if(LED_Para_Struct.State_LED)
					 {
						 M_RGBW_POWER.Power_A = LED_Para_Struct.Power;
						 if(LED_Para_Struct.Power !=0)	 LED_Para_Struct.CR_LED_State = LIGHT_ON;
						 else LED_Para_Struct.CR_LED_State = LIGHT_OFF;
						// LED_FAN_ON();
					 }
					 else 
					 {
						 M_RGBW_POWER.Power_A = 0;
						 LED_Para_Struct.CR_LED_State = LIGHT_OFF;
						// LED_FAN_OFF();
					 }
			   }
				 if(LED_Para_Struct.Channel_Color.bits.Channel_G)   //G通道
				 {
					 if(LED_Para_Struct.State_LED)
					 {
						 M_RGBW_POWER.Power_B = LED_Para_Struct.Power; 						 
						 if(LED_Para_Struct.Power !=0)	LED_Para_Struct.CG_LED_State = LIGHT_ON;
						 else LED_Para_Struct.CG_LED_State = LIGHT_OFF;
						
						//LED_FAN_ON();
					 }
					 else 
					 {
						 M_RGBW_POWER.Power_B = 0;
						 LED_Para_Struct.CG_LED_State = LIGHT_OFF;
						// LED_FAN_OFF();
					 }
			   }
				 if(LED_Para_Struct.Channel_Color.bits.Channel_B)   //B通道
				 {
					 if(LED_Para_Struct.State_LED)
					 {
						 M_RGBW_POWER.Power_C = LED_Para_Struct.Power;
						 if(LED_Para_Struct.Power !=0)	LED_Para_Struct.CB_LED_State = LIGHT_ON;
						 else LED_Para_Struct.CB_LED_State = LIGHT_OFF;
						// LED_FAN_ON();
					 }
					 else 
					 {
						 M_RGBW_POWER.Power_C = 0;
						 LED_Para_Struct.CB_LED_State = LIGHT_OFF;
						// LED_FAN_OFF();
					 }
			   }
				 if(LED_Para_Struct.Channel_Color.bits.Channel_W)   //W通道
				 {
					 if(LED_Para_Struct.State_LED)
					 {
						 M_RGBW_POWER.Power_D = LED_Para_Struct.Power;
						 if(LED_Para_Struct.Power !=0)	LED_Para_Struct.CW_LED_State = LIGHT_ON;
						 else LED_Para_Struct.CW_LED_State = LIGHT_OFF;
					//	 LED_FAN_ON();
					 }
					 else 
					 {
						 M_RGBW_POWER.Power_D = 0;
						 LED_Para_Struct.CW_LED_State = LIGHT_OFF;
					//	 LED_FAN_OFF();
					 }
			   }
				 if(LED_Para_Struct.Channel_Color.bits.Channel_A)   //A通道
				 {
					 if(LED_Para_Struct.State_LED)
					 {
						CENTEN_AC_POWER.Power_A = LED_Para_Struct.Power;						 
						 if(LED_Para_Struct.Power !=0)	LED_Para_Struct.CA_LED_State = LIGHT_ON;
						 else LED_Para_Struct.CA_LED_State = LIGHT_OFF;
					//	 LED_FAN_ON();
					 }
					 else 
					 {
						 CENTEN_AC_POWER.Power_A = 0;
						 LED_Para_Struct.CA_LED_State = LIGHT_OFF;
				//		 LED_FAN_OFF();
					 }
			   }
				 if(LED_Para_Struct.Channel_Color.bits.Channel_C)   //C通道
				 {
					 if(LED_Para_Struct.State_LED)
					 {
						CENTEN_AC_POWER.Power_B = LED_Para_Struct.Power;
						 if(LED_Para_Struct.Power !=0)	LED_Para_Struct.CC_LED_State = LIGHT_ON;
						 else LED_Para_Struct.CC_LED_State = LIGHT_OFF;
					//	 LED_FAN_ON();
					 }
					 else 
					 {
						 CENTEN_AC_POWER.Power_B = 0;
						 LED_Para_Struct.CC_LED_State = LIGHT_OFF;
					//	 LED_FAN_OFF();
					 }
			   }			 
			 }
			if(LED_Para_Struct.Area_LED.bits.Annular_Area)        //环形区域 
			{
				if(LED_Para_Struct.Channel_Color.bits.Channel_R)   //R通道
				 {
					 if(LED_Para_Struct.State_LED)
					 {
						M_RGBW_POWER.Power_A = LED_Para_Struct.Power;
						 if(LED_Para_Struct.Power !=0)	LED_Para_Struct.MR_LED_State = LIGHT_ON;
						 else LED_Para_Struct.MR_LED_State = LIGHT_OFF;
					 }
					 else 
					 {
						 M_RGBW_POWER.Power_A = 0;
						 LED_Para_Struct.MR_LED_State = LIGHT_OFF;
					 }
			   }
					 
				 if(LED_Para_Struct.Channel_Color.bits.Channel_G)   //G通道
				 {
					 if(LED_Para_Struct.State_LED)
					 {
						 M_RGBW_POWER.Power_B = LED_Para_Struct.Power;
						 if(LED_Para_Struct.Power !=0)	LED_Para_Struct.MG_LED_State = LIGHT_ON;
						 else LED_Para_Struct.MG_LED_State = LIGHT_OFF;
					 }
					 else 
					 {
						 M_RGBW_POWER.Power_B = 0;
						 LED_Para_Struct.MG_LED_State = LIGHT_OFF;
					 }
			   }
				 if(LED_Para_Struct.Channel_Color.bits.Channel_B)   //B通道
				 {
					 if(LED_Para_Struct.State_LED)
					 {
						 M_RGBW_POWER.Power_C = LED_Para_Struct.Power;
						 if(LED_Para_Struct.Power !=0)	LED_Para_Struct.MB_LED_State = LIGHT_ON;
						 else LED_Para_Struct.MB_LED_State = LIGHT_OFF;
					 }
					 else 
					 {
						 M_RGBW_POWER.Power_C = 0;
						 LED_Para_Struct.MB_LED_State = LIGHT_OFF;
					 }
			   }
				 if(LED_Para_Struct.Channel_Color.bits.Channel_W)   //W通道
				 {
					 if(LED_Para_Struct.State_LED)
					 {
						 M_RGBW_POWER.Power_D = LED_Para_Struct.Power;
						 if(LED_Para_Struct.Power !=0)	LED_Para_Struct.MW_LED_State = LIGHT_ON;
						 else LED_Para_Struct.MW_LED_State = LIGHT_OFF;
					 }
					 else 
					 {
						 M_RGBW_POWER.Power_D = 0;
						 LED_Para_Struct.MW_LED_State = LIGHT_OFF;
					 }
			   }
				 if(LED_Para_Struct.Channel_Color.bits.Channel_D)   //预留通道
				 { 
					 if(LED_Para_Struct.State_LED)
					 {
						CENTEN_AC_POWER.Power_C = LED_Para_Struct.Power; 
							 if(LED_Para_Struct.Power !=0)	LED_Para_Struct.Reserve_LED_State = LIGHT_ON;
						 else LED_Para_Struct.Reserve_LED_State = LIGHT_OFF;
					 }
					 else 
					 {
						 CENTEN_AC_POWER.Power_C = 0;
						 LED_Para_Struct.Reserve_LED_State = LIGHT_OFF;
					 }
			   }
			}
		  if(LED_Para_Struct.Area_LED.bits.Edge_Area)        //边缘区域 
			{
				if(LED_Para_Struct.Channel_Color.bits.Channel_R)   //R通道
				 {				 
					 if(LED_Para_Struct.State_LED)
					 {
						AREA_POWER.Power_A = LED_Para_Struct.Power;
						 if(LED_Para_Struct.Power !=0)	LED_Para_Struct.AR_LED_State = LIGHT_ON;
						 else LED_Para_Struct.AR_LED_State = LIGHT_OFF;
					 }
					 else
					 {
						AREA_POWER.Power_A = 0;
						 LED_Para_Struct.AR_LED_State = LIGHT_OFF;
					 }
			   }
				 if(LED_Para_Struct.Channel_Color.bits.Channel_G)   //G通道
				 {
					 
					 if(LED_Para_Struct.State_LED)
					 {
						AREA_POWER.Power_B = LED_Para_Struct.Power;
						 if(LED_Para_Struct.Power !=0)	LED_Para_Struct.AG_LED_State = LIGHT_ON;
						 else LED_Para_Struct.AG_LED_State = LIGHT_OFF;
						 
					 }
					 else
					 {
						AREA_POWER.Power_B = 0;
						 LED_Para_Struct.AG_LED_State = LIGHT_OFF;
					 }
			   }
				 if(LED_Para_Struct.Channel_Color.bits.Channel_B)   //B通道
				 {
					
					 if(LED_Para_Struct.State_LED)
					 {
						 AREA_POWER.Power_C = LED_Para_Struct.Power;
						 if(LED_Para_Struct.Power !=0)	LED_Para_Struct.AB_LED_State = LIGHT_ON;
						 else LED_Para_Struct.AB_LED_State = LIGHT_OFF;
					 }
					 else
					 {
						 AREA_POWER.Power_C = 0;
						 LED_Para_Struct.AB_LED_State = LIGHT_OFF;
					 }
			   }
				 if(LED_Para_Struct.Channel_Color.bits.Channel_W)   //W通道
				 {
					 
					 if(LED_Para_Struct.State_LED)
					 {
						AREA_POWER.Power_D = LED_Para_Struct.Power;
						 if(LED_Para_Struct.Power !=0)	LED_Para_Struct.AW_LED_State = LIGHT_ON;
						 else LED_Para_Struct.AW_LED_State = LIGHT_OFF;
					 }
					 else
					 {
						 AREA_POWER.Power_D = 0;
						 LED_Para_Struct.AW_LED_State = LIGHT_OFF;
					 }
			   }
			}			
     LED_Para_Struct.Adj_mode_valid = 0;

			LED_Power_Updata();
    }
	}
	  	break;
	     
		case Material_solve_MODE:                                         /*材质解算模式下，赋值相应参数*/
				{	
					if(LED_Para_Struct.Start_LED_TRIG_Flag)
					{						
					 					 /*功率赋值*/
			     M_RGBW_POWER.Power_A	= LED_Para_Struct.Material_solve_parameter.R_Power[LED_Para_Struct.Trig_Cnt-1];
					if(M_RGBW_POWER.Power_A > CMD_Power_Max)  M_RGBW_POWER.Power_A = CMD_Power_Max;
					 M_RGBW_POWER.Power_B	= LED_Para_Struct.Material_solve_parameter.G_Power[LED_Para_Struct.Trig_Cnt-1];
					 if(M_RGBW_POWER.Power_B > CMD_Power_Max)  M_RGBW_POWER.Power_B = CMD_Power_Max;
					 M_RGBW_POWER.Power_C	= LED_Para_Struct.Material_solve_parameter.B_Power[LED_Para_Struct.Trig_Cnt-1];
           if(M_RGBW_POWER.Power_C > CMD_Power_Max)  M_RGBW_POWER.Power_C = CMD_Power_Max;						
						
					MCP4728_Set_Value(&MCP4728_1_Config_Struct, &M_RGBW_POWER);   

						/*触发脉宽赋值及开关状态判断*/

		//				LED_Para_Struct.Material_solve_parameter.Switch_Plus_R = LED_Para_Struct.Material_solve_parameter.R_Plus[LED_Para_Struct.Trig_Cnt-1];					
            if((LED_Para_Struct.Material_solve_parameter.Switch_Pulse_R!=0)&&(M_RGBW_POWER.Power_A !=0)) LED_Para_Struct.MR_LED_State = LIGHT_ON;
						else 	LED_Para_Struct.MR_LED_State = LIGHT_OFF;
						
		//				LED_Para_Struct.Material_solve_parameter.Switch_Plus_G = LED_Para_Struct.Material_solve_parameter.G_Plus[LED_Para_Struct.Trig_Cnt-1];					
            if((LED_Para_Struct.Material_solve_parameter.Switch_Pulse_G!=0)&&(M_RGBW_POWER.Power_B !=0)) LED_Para_Struct.MG_LED_State = LIGHT_ON;
						else 	LED_Para_Struct.MG_LED_State = LIGHT_OFF;
						
		//				LED_Para_Struct.Material_solve_parameter.Switch_Plus_B = LED_Para_Struct.Material_solve_parameter.B_Plus[LED_Para_Struct.Trig_Cnt-1];					
            if((LED_Para_Struct.Material_solve_parameter.Switch_Pulse_B!=0)&&(M_RGBW_POWER.Power_C !=0)) LED_Para_Struct.MB_LED_State = LIGHT_ON;
						else 	LED_Para_Struct.MB_LED_State = LIGHT_OFF;
					 /*开启定时器7，进行光源触发脉宽控制，基础步长10us*/
				__HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);	
				__HAL_TIM_SetCounter(&htim7, 0);      
					HAL_TIM_Base_Start_IT(&htim7);

          	LED_Para_Struct.Start_LED_TRIG_Flag = 0;              /*状态清0*/		 			
			   }	
			 }
      break;				
									   
		case OLAT_MODE:                                         /*OLAT模式下，赋值相应参数*/
				{	
					if(LED_Para_Struct.Start_LED_TRIG_Flag)
					{						
					  					 /*功率赋值*/
						M_RGBW_POWER.Power_D	= LED_Para_Struct.OLAT_parameter.W_Power[LED_Para_Struct.Trig_Cnt-1];
            if(M_RGBW_POWER.Power_D > CMD_Power_Max)  M_RGBW_POWER.Power_D = CMD_Power_Max;						
						MCP4728_Set_Value(&MCP4728_1_Config_Struct, &M_RGBW_POWER);   
						
						/*触发脉宽赋值及开关状态判断*/

		//				LED_Para_Struct.OLAT_parameter.Switch_Plus_W = LED_Para_Struct.OLAT_parameter.W_Plus[LED_Para_Struct.Trig_Cnt-1];					
						if((LED_Para_Struct.OLAT_parameter.Switch_Pulse_W!=0)&&(M_RGBW_POWER.Power_D !=0)) LED_Para_Struct.MW_LED_State = LIGHT_ON;
						else 	LED_Para_Struct.MW_LED_State = LIGHT_OFF;		

					 /*开启定时器7，进行光源触发脉宽控制，基础步长10us*/
				
						__HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);	
						__HAL_TIM_SetCounter(&htim7, 0);      
						HAL_TIM_Base_Start_IT(&htim7);	
						
						LED_Para_Struct.Start_LED_TRIG_Flag = 0;              /*状态清0*/		
            LED_Para_Struct.OLAT_parameter.W_Pulse_cnt = 0;						
					
					}	
			  }
      break;
				
		case Relighting_MODE:                                         /*重光照模式下，赋值相应参数*/
				{	
					if(LED_Para_Struct.Start_LED_TRIG_Flag)
					{						
						if(LED_Para_Struct.Start_LED_TRIG_ON == 0)
						{ 
						/*功率赋值*/
					 LED_Para_Struct.Power = LED_Para_Struct.Relighting_parameter.Power[LED_Para_Struct.Trig_Cnt-1];		
			     M_RGBW_POWER.Power_A	= LED_Para_Struct.Power ;	
           M_RGBW_POWER.Power_B	= LED_Para_Struct.Power ;	
           M_RGBW_POWER.Power_C	= LED_Para_Struct.Power ;	
           M_RGBW_POWER.Power_D	= LED_Para_Struct.Power ;	
						
						MCP4728_Set_Value(&MCP4728_1_Config_Struct, &M_RGBW_POWER); 
									    
					 AREA_POWER.Power_A	= LED_Para_Struct.Power ;	
           AREA_POWER.Power_B	= LED_Para_Struct.Power ;	
           AREA_POWER.Power_C	= LED_Para_Struct.Power ;	
           AREA_POWER.Power_D	= LED_Para_Struct.Power ;	
						
						MCP4728_Set_Value(&MCP4728_1_Config_Struct, &AREA_POWER); 
						
				
						/*触发脉宽赋值及开关状态判断*/

		//				LED_Para_Struct.Relighting_parameter.Switch_Plus = LED_Para_Struct.Relighting_parameter.Plus[LED_Para_Struct.Trig_Cnt-1];					
            if((LED_Para_Struct.Relighting_parameter.Switch_Pulse != 0)&&(LED_Para_Struct.Relighting_parameter.Power[LED_Para_Struct.Trig_Cnt-1]!=0))
						{
							LED_Para_Struct.Relighting_State_LED = LIGHT_ON;
						}
						else 	LED_Para_Struct.Relighting_State_LED = LIGHT_OFF;								
				 					
//						if(LED_Para_Struct.Start_LED_TRIG_ON == 0)
//						{
						LED_Dimming_Updata();                       /*色坐标解析及光源状态判断*/	
//						LED_Para_Struct.Start_LED_TRIG_ON  = 1;
//						}	
						
					/*开启定时器7，进行光源触发脉宽控制，基础步长10us*/
					__HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
					__HAL_TIM_SetCounter(&htim7, 0);    
					HAL_TIM_Base_Start_IT(&htim7);
						LED_Para_Struct.Start_LED_TRIG_ON  = 1;
						}	
						
						LED_Para_Struct.Relighting_parameter.Pulse_cnt = 0;

						LED_Para_Struct.Start_LED_TRIG_Flag = 0;              /*状态清0*/	
				  }	
				LED_Relighting_state_Updata();						/*光源状态更新（中心区域RGB和边缘区域RGB同时更新）*/
			  }
      break;	
					 
		case Adj_Relighting_MODE:
		   {
				 LED_Relighting_state_Updata();
				 
			 }
			 break;
				
  }
	
	if(LED_Para_Struct.Start_Camera_TRIG_Flag)
	{	
		
		LED_Para_Struct.Camera_Trig_cnt = 0;
			
		/*相机触发延时不为0，进行相机触发延迟控制*/	
		if(LED_Para_Struct.Camera_Trig_Delay != 0)		
		{		
			/*开启定时器10，进行相机触发延迟控制，基础步长10us*/		
			__HAL_TIM_CLEAR_IT(&htim10, TIM_IT_UPDATE);			
			__HAL_TIM_SetCounter(&htim10, 0);      		
			HAL_TIM_Base_Start_IT(&htim10);		
		}		
		/*相机触发延时为0，则直接进行相机触发及触发脉宽控制*/			
		else 		
		{		
			Camera_Trig_ON();		
			/*开启定时器12，进行相机触发脉宽控制，基础步长10us*/		
			__HAL_TIM_CLEAR_IT(&htim12, TIM_IT_UPDATE);			
			__HAL_TIM_SetCounter(&htim12, 0);      		
			HAL_TIM_Base_Start_IT(&htim12);		
		}
   LED_Para_Struct.Start_Camera_TRIG_Flag	= 0;  //相机触发状态清零
	}		
	

	if(LED_Para_Struct.Idle_Flag )
	{
//		memset(&M_RGBW_POWER, 0, sizeof( Power_Data_TypeDef ));		
//		memset(&AREA_POWER, 0, sizeof(Power_Data_TypeDef ));	
//		memset(&CENTEN_AC_POWER, 0, sizeof(Power_Data_TypeDef ));
    LED_Power_Min();
    LED_Power_Updata();		
		LED_Para_Struct.Idle_Flag = 0;
	}
	
		LED_Switch_Updata();
}

 void LED_Power_Min(void)
 {
		M_RGBW_POWER.Power_A = 0;
		M_RGBW_POWER.Power_B = 0;
		M_RGBW_POWER.Power_C = 0;
		M_RGBW_POWER.Power_D = 0;
		AREA_POWER.Power_A = 0;
		AREA_POWER.Power_B = 0;
		AREA_POWER.Power_C = 0;
		AREA_POWER.Power_D = 0;
		CENTEN_AC_POWER.Power_A = 0;
		CENTEN_AC_POWER.Power_B = 0;
		CENTEN_AC_POWER.Power_C = 0;
		CENTEN_AC_POWER.Power_D = 0; 
 }

 void LED_Switch_Updata(void)
{
	
	/*装调模式LED开关控制*/
	if(LED_Para_Struct.CR_LED_State)  CR_LED_ON();
	else CR_LED_OFF();
	if(LED_Para_Struct.CG_LED_State)  CG_LED_ON();
	else CG_LED_OFF();
	if(LED_Para_Struct.CB_LED_State)  CB_LED_ON();
	else CB_LED_OFF();
	if(LED_Para_Struct.CW_LED_State)  CW_LED_ON();
	else CW_LED_OFF();
	if(LED_Para_Struct.AR_LED_State)  AR_LED_ON();
	else AR_LED_OFF();
	if(LED_Para_Struct.AG_LED_State)  AG_LED_ON();
	else AG_LED_OFF();
	if(LED_Para_Struct.AB_LED_State)  AB_LED_ON();
	else AB_LED_OFF();
	if(LED_Para_Struct.AW_LED_State)  AW_LED_ON();
	else AW_LED_OFF();
	if(LED_Para_Struct.CA_LED_State)  CA_LED_ON();
	else CA_LED_OFF();
	if(LED_Para_Struct.CC_LED_State)  CC_LED_ON();
	else CC_LED_OFF();
	if(LED_Para_Struct.MR_LED_State)  MR_LED_ON();
	else MR_LED_OFF();
	if(LED_Para_Struct.MG_LED_State)  MG_LED_ON();
	else MG_LED_OFF();
	if(LED_Para_Struct.MB_LED_State)  MB_LED_ON();
	else MB_LED_OFF();
	if(LED_Para_Struct.MW_LED_State)  MW_LED_ON();
	else MW_LED_OFF();
	if(LED_Para_Struct.Reserve_LED_State) Reserve_LED_ON();
  else Reserve_LED_OFF();
}


void LED_Dimming_Updata(void)
{
  double Xm,Ym;
	double Tr,Tg,Tb,T1,T2,T3,Dr,Dg,Db;
	double Lr,Lg,Lb,Lm;
	uint8_t cnt_Liu;
	if(LED_Para_Struct.Power%10 == 0)
		cnt_Liu = LED_Para_Struct.Power/10  ;
	else cnt_Liu = LED_Para_Struct.Power/10 + 1;
	Lr = Liu_R[cnt_Liu]*14;
	Lg = Liu_G[cnt_Liu]*12;
	Lb = Liu_B[cnt_Liu]*18;
//	Lr = Liu_R[cnt_Liu]*8;
//	Lg = Liu_G[cnt_Liu]*6;
//	Lb = Liu_B[cnt_Liu]*10;
	Lm = Liu_M[cnt_Liu]/2;
	Xm=(double)LED_Para_Struct.Relighting_parameter.Coordinate_X/10000.0;
	Ym=(double)LED_Para_Struct.Relighting_parameter.Coordinate_Y/10000.0;
	Tr = (((Yg-Yb)*(Xb-Xm)+(Ym-Yb)*(Xg-Xb))*(Yr * Lm));
	Tg = (((Yb-Yr)*(Xr-Xm)+(Ym-Yr)*(Xb-Xr))*(Yg * Lm));
	Tb = (((Yg-Yr)*(Xr-Xm)+(Ym-Yr)*(Xg-Xr))*(Yb * Lm));
	T1 = (((Yg-Yb)*(Xb-Xr)+(Yr-Yb)*(Xg-Xb)) * (Ym * Lr));
	T2 = (((Yb-Yr)*(Xr-Xg)+(Yg-Yr)*(Xb-Xr)) * (Ym * Lg));
	T3 = (((Yg-Yr)*(Xr-Xb)+(Yb-Yr)*(Xg-Xr)) * (Ym * Lb));
	Dr = Tr/T1*100.0;
	Dg = Tg/T2*100.0;
	Db = Tb/T3*100.0;
	LED_Para_Struct.Relighting_parameter.Dr = (uint8_t)(Dr+0.5);
	if(LED_Para_Struct.Relighting_parameter.Dr>100) LED_Para_Struct.Relighting_parameter.Dr = 100;
	LED_Para_Struct.Relighting_parameter.Dg = (uint8_t)(Dg+0.5);
	if(LED_Para_Struct.Relighting_parameter.Dg>100) LED_Para_Struct.Relighting_parameter.Dg = 100;
	LED_Para_Struct.Relighting_parameter.Db = (uint8_t)(Db+0.5);
	if(LED_Para_Struct.Relighting_parameter.Db>100) LED_Para_Struct.Relighting_parameter.Db = 100;
	
	if(LED_Para_Struct.Relighting_State_LED)
	{
	  if(LED_Para_Struct.Relighting_parameter.Dr!=0) 		
		{
			LED_Para_Struct.Relighting_parameter.State_LED_R = LIGHT_ON;
			
			htim2.Init.Period = LED_Para_Struct.Relighting_parameter.Dr *84-1;
			__HAL_TIM_SetAutoreload(&htim2,htim2.Init.Period);
			__HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);         
		 __HAL_TIM_SetCounter(&htim2, 0);       /*开启定时器2，进行占空比输出*/
			HAL_TIM_Base_Start_IT(&htim2);
			
		}
		else
		{
			LED_Para_Struct.Relighting_parameter.State_LED_R = LIGHT_OFF;			
		 __HAL_TIM_SetCounter(&htim2, 0);       /*关闭定时器2，停止占空比输出*/
			HAL_TIM_Base_Stop_IT(&htim2);
		}
		if(LED_Para_Struct.Relighting_parameter.Dg!=0) 		
		{
			LED_Para_Struct.Relighting_parameter.State_LED_G = LIGHT_ON;
			
			htim3.Init.Period = LED_Para_Struct.Relighting_parameter.Dg *84-1;
			__HAL_TIM_SetAutoreload(&htim3,htim3.Init.Period);
			__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);         
		 __HAL_TIM_SetCounter(&htim3, 0);       /*开启定时器3，进行占空比输出*/
			HAL_TIM_Base_Start_IT(&htim3);
			
		}
		else
		{
			LED_Para_Struct.Relighting_parameter.State_LED_G = LIGHT_OFF;			
		 __HAL_TIM_SetCounter(&htim3, 0);       /*关闭定时器3，停止占空比输出*/
			HAL_TIM_Base_Stop_IT(&htim3);
		}
		if(LED_Para_Struct.Relighting_parameter.Db!=0) 		
		{
			LED_Para_Struct.Relighting_parameter.State_LED_B = LIGHT_ON;
			
			htim4.Init.Period = LED_Para_Struct.Relighting_parameter.Db *84-1;
			__HAL_TIM_SetAutoreload(&htim4,htim4.Init.Period);
			__HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);         
		 __HAL_TIM_SetCounter(&htim4, 0);       /*开启定时器4，进行占空比输出*/
			HAL_TIM_Base_Start_IT(&htim4);
			
		}
		else
		{
			LED_Para_Struct.Relighting_parameter.State_LED_B = LIGHT_OFF;			
		 __HAL_TIM_SetCounter(&htim4, 0);       /*关闭定时器4，停止占空比输出*/
			HAL_TIM_Base_Stop_IT(&htim4);
		}
	
		__HAL_TIM_CLEAR_IT(&htim5, TIM_IT_UPDATE);         
		 __HAL_TIM_SetCounter(&htim5, 0);       /*开启定时器5，进行PWM频率输出*/
			HAL_TIM_Base_Start_IT(&htim5);
		
		LED_Para_Struct._18b20_EN = RESET;     /*18B20关闭*/
		
	}
	else 
	{
		LED_Para_Struct.Relighting_parameter.State_LED_R = LIGHT_OFF;
		LED_Para_Struct.Relighting_parameter.State_LED_G = LIGHT_OFF;
		LED_Para_Struct.Relighting_parameter.State_LED_B = LIGHT_OFF;
		HAL_TIM_Base_Stop_IT(&htim2);
	  __HAL_TIM_SetCounter(&htim2, 0);
				HAL_TIM_Base_Stop_IT(&htim3);
	  __HAL_TIM_SetCounter(&htim3, 0);
				HAL_TIM_Base_Stop_IT(&htim4);
	  __HAL_TIM_SetCounter(&htim4, 0);
				HAL_TIM_Base_Stop_IT(&htim5);
	  __HAL_TIM_SetCounter(&htim5, 0);
		
		LED_Para_Struct._18b20_EN = SET;     /*18B20开启*/

	}
	
}
void LED_Relighting_state_Updata(void)	
{
	if(LED_Para_Struct.Relighting_State_LED == 0)
	{
		LED_Para_Struct.Relighting_parameter.State_LED_R = LIGHT_OFF;
		LED_Para_Struct.Relighting_parameter.State_LED_G = LIGHT_OFF;
		LED_Para_Struct.Relighting_parameter.State_LED_B = LIGHT_OFF;
		HAL_TIM_Base_Stop_IT(&htim2);
	  __HAL_TIM_SetCounter(&htim2, 0);
				HAL_TIM_Base_Stop_IT(&htim3);
	  __HAL_TIM_SetCounter(&htim3, 0);
				HAL_TIM_Base_Stop_IT(&htim4);
	  __HAL_TIM_SetCounter(&htim4, 0);
				HAL_TIM_Base_Stop_IT(&htim5);
	  __HAL_TIM_SetCounter(&htim5, 0);
	}
	
	if(LED_Para_Struct.Relighting_parameter.State_LED_R)
	{

			LED_Para_Struct.AR_LED_State =  LIGHT_ON;
			LED_Para_Struct.CR_LED_State =  LIGHT_ON;
	}
			  
	else	 
	{
			LED_Para_Struct.AR_LED_State =  LIGHT_OFF;
			LED_Para_Struct.CR_LED_State =  LIGHT_OFF;		 
	}	 
	if(LED_Para_Struct.Relighting_parameter.State_LED_G)	 
	{  	
		LED_Para_Struct.AG_LED_State =  LIGHT_ON;  
		LED_Para_Struct.CG_LED_State =  LIGHT_ON;	 
	}
	else	  	
	{
	  
		LED_Para_Struct.AG_LED_State =  LIGHT_OFF;  
		LED_Para_Struct.CG_LED_State =  LIGHT_OFF;	 
	} 
	if(LED_Para_Struct.Relighting_parameter.State_LED_B)	 
	{ 
		LED_Para_Struct.AB_LED_State =  LIGHT_ON;	  
		LED_Para_Struct.CB_LED_State =  LIGHT_ON;  
	} 
	else
	{
		LED_Para_Struct.AB_LED_State =  LIGHT_OFF; 	
		LED_Para_Struct.CB_LED_State =  LIGHT_OFF;  
	}
}


_18B20_Config_TypeDef _18B20_Config_Struct_Temp = 
{
	.GPIOx = Temp_DQ_GPIO_Port,
	.GPIO_Pin = Temp_DQ_Pin,
};

void LED_OTP_Handler(void) /* LED overtemperature protection module */
{
	int8_t temp_value;
		
//	static uint8_t flag_fan = 0;
	
	static uint32_t Time_Tick = 0;
	static uint8_t Start_Temp = RESET;
	static uint8_t err_count = 0;
	
	static _18B20_StateTypeDef _18b20_state ;
	
	//static uint8_t		_18b20_state ;

	static uint8_t Covert_OK = RESET;
	uint8_t val;
	
	if((HAL_GetTick() - Time_Tick) > TIME_TempProtection_ms)
	{
		Time_Tick = HAL_GetTick();
		
		Start_Temp = SET;

	}
	
	/* 读取温度 */
	if(( Start_Temp != RESET )&&(LED_Para_Struct._18b20_EN != RESET))/* 没有转换完成 */
	{
		_18b20_state = _18B20_Get_Temp(&_18B20_Config_Struct_Temp,&LED_Para_Struct.Convert_Temp);

		if(_18b20_state == _18B20_State_ResetErr)
		{
			err_count++;
		}
		
		if(err_count > 3)/* 复位出错尝试3次, 转换结束*/
		{
			Covert_OK = RESET;
			LED_Para_Struct.State_TempErr.bits.ResetCurrent_1 = SET;
			LED_Para_Struct.State_TempErr.bits.ResetOnce_1    = SET;
			Start_Temp = RESET; /* 单次转换完成 */
			err_count = 0;/* 错误计数清零 */
		}
		
		if(_18b20_state == _18B20_State_Covert)/* 转换完成 */
		{
			Covert_OK = SET;
			LED_Para_Struct.State_TempErr.bits.ResetCurrent_1 = RESET;/* 转换完成 */
			Start_Temp = RESET; /* 单次转换完成 */
			err_count = 0;/* 错误计数清零 */
			
			for(uint8_t i = 0; i< 4; i++)
			{
				LED_Para_Struct.TempFilt[0].Buffer[i] = LED_Para_Struct.TempFilt[0].Buffer[i+1]; 
				LED_Para_Struct.TempFilt[0].Flag_Range[i] = LED_Para_Struct.TempFilt[0].Flag_Range[i+1];
			}
			LED_Para_Struct.TempFilt[0].Buffer[4] = (int8_t)LED_Para_Struct.Convert_Temp.Temp_float;
			
			//温度超界检查
			if ((LED_Para_Struct.TempFilt[0].Buffer[4] > TEMPT_HILG_LIMIT) || (LED_Para_Struct.TempFilt[0].Buffer[4] < TEMPT_LOW_LIMIT))
			{
				LED_Para_Struct.TempFilt[0].Flag_Range[4] = SET;
			}
			else
			{
				LED_Para_Struct.TempFilt[0].Flag_Range[4] = RESET;
			}
			
			val = 0;
			for(uint8_t i = 0 ; i<5; i++)
			{
				val += LED_Para_Struct.TempFilt[0].Flag_Range[i];
			}
			if (val > 2)
			{
				LED_Para_Struct.State_TempErr.bits.RangeCurrent_1 = SET;
				LED_Para_Struct.State_TempErr.bits.RangeOnce_1    = SET;
			}
			else
			{
				LED_Para_Struct.State_TempErr.bits.RangeCurrent_1 = RESET;
			}
			
			
			LED_Para_Struct.Temp_Current = Temp_Filter_5Get1_Handler(LED_Para_Struct.TempFilt[0].Buffer);
			
		}
	}
	
	


	if(LED_Para_Struct._18b20_state == _18B20_State_ResetErr) 
	{	
//		LED_Para_Struct.State_LaserErr.bits.OverTEMP_Protect = RESET; /* 转换异常,温度保护关 */
		LED_Para_Struct.State_Fan = Switch_ON;	/* 温度传感器异常,风扇开 */
	}
	
		/* 当前温度传感器没有错误,且转换完成 */
	else
	{
		
			temp_value = LED_Para_Struct.Temp_Current;
			 						
			
			/* 风扇降温 */
			if ((temp_value > Default_Fan_High) && ( LED_Para_Struct.Fan_flag == 0 ))/* 温度过高,开启风扇 */
			{
				LED_Para_Struct.State_Fan = Switch_ON;
				LED_Para_Struct.Fan_flag = 1;
			}
			else if ((temp_value < Default_Fan_Low) && (LED_Para_Struct.Fan_flag == 1))/* 温度正常,关闭风扇 */
			{				
				LED_Para_Struct.State_Fan = Switch_OFF;
				LED_Para_Struct.Fan_flag = 0;
			}
	}
	
	if(LED_Para_Struct.State_Fan == Switch_ON) /* 风扇控制 */
	{
		LED_FAN_ON();
		LED_Para_Struct.Fan_flag = Switch_ON;/* 风扇实际状态 */
	}
	else
	{
		LED_FAN_OFF();
		LED_Para_Struct.Fan_flag = Switch_OFF;/* 风扇实际状态 */
	}
}


static int8_t Temp_Filter_5Get1_Handler(int8_t *pData)
{
	int8_t filter_buf[5];
	int8_t mv;
	
	filter_buf[0] = pData[0];
	filter_buf[1] = pData[1];
	filter_buf[2] = pData[2];
	filter_buf[3] = pData[3];
	filter_buf[4] = pData[4];

	//单路对5点直接找中值
	//首次冒泡排序
	if(filter_buf[0] > filter_buf[1])
	{
		mv = filter_buf[0];
		filter_buf[0] = filter_buf[1];
		filter_buf[1]=mv;
	}
	if(filter_buf[1] > filter_buf[2])
	{
		mv = filter_buf[1];
		filter_buf[1] = filter_buf[2];
		filter_buf[2]=mv;
	}
	if(filter_buf[2] > filter_buf[3])
	{
		mv = filter_buf[2];
		filter_buf[2] = filter_buf[3];
		filter_buf[3]=mv;
	}
	if(filter_buf[3] > filter_buf[4])
	{
		mv = filter_buf[3];
		filter_buf[3] = filter_buf[4];
		filter_buf[4]=mv;
	}
	//第二次冒泡排序
	if(filter_buf[0] > filter_buf[1])
	{
		mv = filter_buf[0];
		filter_buf[0] = filter_buf[1];
		filter_buf[1]=mv;
	}
	if(filter_buf[1] > filter_buf[2])
	{
		mv = filter_buf[1];
		filter_buf[1] = filter_buf[2];
		filter_buf[2]=mv;
	}
	if(filter_buf[2] > filter_buf[3])
	{
		mv = filter_buf[2];
		filter_buf[2] = filter_buf[3];
		filter_buf[3]=mv;
	}
	//第三次冒泡排序
	if(filter_buf[0] > filter_buf[1])
	{
		mv = filter_buf[0];
		filter_buf[0] = filter_buf[1];
		filter_buf[1]=mv;
	}
	if(filter_buf[1] > filter_buf[2])
	{
		mv = filter_buf[1];
		filter_buf[1] = filter_buf[2];
		filter_buf[2]=mv;
	}
	return filter_buf[2];		//返回中间值
}
