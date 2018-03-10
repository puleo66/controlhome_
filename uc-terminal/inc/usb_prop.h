/*
*********************************************************************************************************
*	                                  
*	模块名称 : USB虚拟串口相关的程序
*	文件名称 : usb_istr.c
*	版    本 : V2.0
*	USB固件库驱动 : V3.3.0
*	说    明 : 头文件
*
*	Copyright (C), 2010-2011, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __usb_prop_H
#define __usb_prop_H

typedef struct
{
	INT32U bitrate;
	INT8U format;
	INT8U paritytype;
	INT8U datatype;
}LINE_CODING;

#define Virtual_Com_Port_GetConfiguration          NOP_Process
//#define Virtual_Com_Port_SetConfiguration          NOP_Process
#define Virtual_Com_Port_GetInterface              NOP_Process
#define Virtual_Com_Port_SetInterface              NOP_Process
#define Virtual_Com_Port_GetStatus                 NOP_Process
#define Virtual_Com_Port_ClearFeature              NOP_Process
#define Virtual_Com_Port_SetEndPointFeature        NOP_Process
#define Virtual_Com_Port_SetDeviceFeature          NOP_Process
//#define Virtual_Com_Port_SetDeviceAddress          NOP_Process

#define SEND_ENCAPSULATED_COMMAND   0x00
#define GET_ENCAPSULATED_RESPONSE   0x01
#define SET_COMM_FEATURE            0x02
#define GET_COMM_FEATURE            0x03
#define CLEAR_COMM_FEATURE          0x04
#define SET_LINE_CODING             0x20
#define GET_LINE_CODING             0x21
#define SET_CONTROL_LINE_STATE      0x22
#define SEND_BREAK                  0x23

void Virtual_Com_Port_init(void);
void Virtual_Com_Port_Reset(void);
void Virtual_Com_Port_SetConfiguration(void);
void Virtual_Com_Port_SetDeviceAddress (void);
void Virtual_Com_Port_Status_In (void);
void Virtual_Com_Port_Status_Out (void);
RESULT Virtual_Com_Port_Data_Setup(INT8U);
RESULT Virtual_Com_Port_NoData_Setup(INT8U);
RESULT Virtual_Com_Port_Get_Interface_Setting(INT8U Interface, INT8U AlternateSetting);
INT8U *Virtual_Com_Port_GetDeviceDescriptor(INT16U );
INT8U *Virtual_Com_Port_GetConfigDescriptor(INT16U);
INT8U *Virtual_Com_Port_GetStringDescriptor(INT16U);

INT8U *Virtual_Com_Port_GetLineCoding(INT16U Length);
INT8U *Virtual_Com_Port_SetLineCoding(INT16U Length);

#endif

