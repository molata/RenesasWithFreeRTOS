#ifndef HIT_COMMON_H
#define HIT_COMMON_H

#include "Serial.h"
#pragma address HIT_y=0x00100000

/**********************       上位机指令       ***********************************
序号	名称	字节数	单位	范围(处理后)	比例尺	备注
1	起始字节	2	0x55AA	
2	数据长度	1	0x0C	
3~4	俯仰下倾角指令	2	度	-17.5~+17.5	100	
5~6	俯仰下倾角指令	2	度	-17.5~+17.5	100	
7	捕获指令	1	0x55：捕获  0xFF:不捕获	1	
8	码型	1	见表4	1	
9~14	备用	6				
15	校验和	1			1	
*********************************************************/
typedef struct st_pc_cmd{
	float fpPitchDeg;   // 俯仰上倾角指令  
	float fpSailDeg;   // 俯仰下倾角指令
	unsigned char ucCapture;  //捕获指令  for 激光板
	unsigned char ucMod_type;  // 码型    for 激光板
}ST_PC_CMD;                 // 计算机指令

/********************   内部的数值量    **************************************
1	起始字节	2	0x55AA	
2	数据长度	1	0x18	
3~6	包计数	4		
7	xx头工作状态信号	1	该字节由高到低依次为D7、D6…、 
D0=0 未捕获     D0=1捕获
D1=0无光电信号  D1=1有光电信号
D2=0无光电信号  D2=1有光电信号
D3=0未进入盲区  D3=1进入盲区	
8	码型	1	见表4	
9~10	俯仰框架角	2	度	-17.5~+17.5	100	
11~12	偏航框架角	2	度	-17.5~+17.5	100	
13~14	俯仰视线角速度	2	度/秒	-10~+10	100	
15~16	偏航视线角速度	2		-10~+10	100	
17~18	俯仰跟踪角偏差	2	度	-15~+15	100	
19~20	偏航跟踪角偏差	2	度	-15~+15	100	
21~24	备用	4				
25	校验和	1				
***********************************************************/
typedef struct st_serial_data{
	unsigned int package_count;
	unsigned char work_status;
	unsigned char code_pattern;
	float elevation_deg;
	float sheer_deg;
	float elevation_view_deg_speed;
	float sheer_view_deg_speed;
	float elevation_trace_deg_offset;
	float sheer_trace_deg_offset;
}ST_SERIAL_DATA;             // 内部状态的数据

/************************************************************
ucLaser_work_states： 激光板的工作状态
uc62TA_work_states:   62TA的工作状态
uc62TB_work_states:   62TB的工作状态
*************************************************************/
typedef union unSensor_states{
		unsigned char BYTE;
		struct {
			unsigned char LASER:1;    // 结构体的第一个是最低位
			unsigned char Motor62TA:1;
			unsigned char Motor62TB:1;
			unsigned char BITE:5;
		} STATE;
} UNSENSOR_STATES;




#endif