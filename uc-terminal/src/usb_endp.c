/*
*********************************************************************************************************
*	                                  
*	ģ������ : USB�˵�������ģ��    
*	�ļ����� : usb_istr.c
*	��    �� : V2.0
*	USB�̼������� : V3.3.0
*	˵    �� : USB�˵��������ӳ���
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v0.1    2009-12-27 armfly  �������ļ���ST�̼���汾ΪV3.1.2
*		v1.0    2011-01-11 armfly  ST�̼���������V3.4.0�汾��
*		v2.0    2011-10-16 armfly  ST�̼���������V3.5.0�汾���Ż����̽ṹ��
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "includes.h"

#if USBD_MODULE == DEF_ENABLED

/* Interval between sending IN packets in frame number (1 frame = 1ms) */
#define VCOMPORT_IN_FRAME_INTERVAL             5

extern  INT8U USART_Rx_Buffer[];
extern INT32U USART_Rx_ptr_out;
extern INT32U USART_Rx_length;
extern INT8U  USB_Tx_State;
extern INT16U *pdwVal;
extern INT16U i;

INT16U usWord;
INT8U ucByteNum;
INT16U usTotalSize;
INT16U usRxCnt;
INT8U USB_Rx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
/*
*********************************************************************************************************
*	�� �� ��: EP1_IN_Callback
*	����˵��: �˵�1 IN�����豸->PC���ص�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void EP1_IN_Callback (void)
{
	/*
	Ϊ����ߴ���Ч�ʣ����ҷ���FIFO�������� UserToPMABufferCopy() �����͵�չ�� 
	void UserToPMABufferCopy(INT8U *pbUsrBuf, INT16U wPMABufAddr, INT16U wNBytes)
	{
	  INT32U n = (wNBytes + 1) >> 1;
	  INT32U i, temp1, temp2;
	  INT16U *pdwVal;
	  pdwVal = (INT16U *)(wPMABufAddr * 2 + PMAAddr);
	  for (i = n; i != 0; i--)
	  {
	    temp1 = (INT16U) * pbUsrBuf;
	    pbUsrBuf++;
	    temp2 = temp1 | (INT16U) * pbUsrBuf << 8;
	    *pdwVal++ = temp2;
	    pdwVal++;
	    pbUsrBuf++;
	  }
	}		
	*/
	
	
	usTotalSize = 0;
	pdwVal = (INT16U *)(ENDP1_TXADDR * 2 + PMAAddr);	
	for (i = 0 ; i < VIRTUAL_COM_PORT_DATA_SIZE / 2; i++)
	{
		usWord = usb_GetTxWord(&ucByteNum);
		if (ucByteNum == 0)
		{
			break;
		}
		
		usTotalSize += ucByteNum;
		
		*pdwVal++ = usWord;
		pdwVal++;		
	}
	
	if (usTotalSize == 0)
	{
		return;
	}
	
	SetEPTxCount(ENDP1, usTotalSize);
	SetEPTxValid(ENDP1); 
}

/*
*********************************************************************************************************
*	�� �� ��: EP3_OUT_Callback
*	����˵��: �˵�3 OUT����PC->�豸���ص�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void EP3_OUT_Callback(void)
{
	/* ��USB�˵�3�յ������ݴ洢��USB_Rx_Buffer�� ���ݴ�С������USB_Rx_Cnt */
	usRxCnt = USB_SIL_Read(EP3_OUT, USB_Rx_Buffer);
	
	/* ���������յ������ݻ��浽�ڴ� */
	usb_SaveHostDataToBuf(USB_Rx_Buffer, usRxCnt);
	
	/* ���� EP3 �˵�������� */
	SetEPRxValid(ENDP3);
}

/*
*********************************************************************************************************
*	�� �� ��: SOF_Callback
*	����˵��: SOF�ص�����  .SOF��host����ָʾframe�Ŀ�ͷ�ġ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SOF_Callback(void)
{
	static INT32U FrameCount = 0;
	
	if (bDeviceState == CONFIGURED)
	{
		if (FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL)
		{
			/* Reset the frame counter */
			FrameCount = 0;
			
			/* Check the data to be sent through IN pipe */
			EP1_IN_Callback();
			//Handle_USBAsynchXfer();
		}
	}  
}

#endif