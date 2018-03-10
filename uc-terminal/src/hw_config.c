/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
 * File Name          : hw_config.c
 * Author             : MCD Application Team
 * Version            : V3.3.0
 * Date               : 21-March-2011
 * Description        : Hardware Configuration & Setup
 ********************************************************************************
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 * AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
 * CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
 * INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *******************************************************************************/

#include <includes.h>
#if USBD_MODULE == DEF_ENABLED
   
USB_COM_FIFO_T g_tUsbFifo;

extern INT16U i; 
INT16U usRxWrite, usData;
INT8U  ucData, idx;
INT32U Device_Serial0, Device_Serial1, Device_Serial2;

void Set_USBClock(void);
void USB_Interrupts_Config(void);
void IntToUnicode(INT32U value, INT8U *pbuf, INT8U len);
void USB_LP_CAN1_RX0_IRQHandler(void);


/*******************************************************************************
 * Description    : Configures Main system clocks & power
 *******************************************************************************/
void Set_USBSystem(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
        RCC_APB2PeriphResetCmd(  RCC_APB2Periph_GPIOA
                               | RCC_APB2Periph_GPIOC
                               | RCC_APB2Periph_AFIO, DISABLE);
        RCC_APB2PeriphClockCmd(  RCC_APB2Periph_GPIOA
                               | RCC_APB2Periph_GPIOC
                               | RCC_APB2Periph_AFIO, ENABLE);


         // Configure PC11 USB connect.
        GPIO_WriteBit(GPIOC,GPIO_Pin_11,Bit_SET);
        GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        // Configure PC12 USB reset.
        GPIO_WriteBit(GPIOC,GPIO_Pin_12,Bit_SET);
        GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        
        Set_USBClock();
        USB_Interrupts_Config();
        USB_Init();
        
        g_tUsbFifo.usRxWrite = 0;
	g_tUsbFifo.usRxRead = 0;
	g_tUsbFifo.usTxWrite = 0;
	g_tUsbFifo.usTxRead = 0;
  
}
/*******************************************************************************
 * Description    : Configures USB Clock input (48MHz)
 *******************************************************************************/
void Set_USBClock(void)
{

	// Select USBCLK source 
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

	// Enable the USB clock 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
       
}

/*******************************************************************************
 * Description    : Power-off system clocks and power while entering suspend mode
 *******************************************************************************/
void usb_EnterLowPowerMode(void)
{
	/* Set the device state to suspend */
	bDeviceState = SUSPENDED;
}

/*******************************************************************************
 * Description    : Restores system clocks and power while exiting suspend mode
 *******************************************************************************/
void usb_LeaveLowPowerMode(void)
{
	DEVICE_INFO *pInfo = &Device_Info;

	/* Set the device state to the correct state */
	if (pInfo->Current_Configuration != 0)
	{
		/* Device configured */
		bDeviceState = CONFIGURED;
	}
	else
	{
		bDeviceState = ATTACHED;
	}
}

/*******************************************************************************
 * Description    : Configures the USB interrupts
 *******************************************************************************/
void USB_Interrupts_Config(void)
{
        BSP_IntVectSet(BSP_INT_ID_USB_LP_CAN_RX0, USB_LP_CAN1_RX0_IRQHandler);
        BSP_IntPrioSet(BSP_INT_ID_USB_LP_CAN_RX0,1);
	BSP_IntEn(BSP_INT_ID_USB_LP_CAN_RX0);
}

/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    OS_CPU_SR  cpu_sr;


    OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
    OSIntNesting++;
    OS_EXIT_CRITICAL();

	usb_Istr();

    OSIntExit();
	
}
/*******************************************************************************
 * Description    : Software Connection/Disconnection of USB Cable
 *******************************************************************************/
void usb_CableConfig(INT8U _ucMode)
{
       if (_ucMode != DISABLE)
         {
           GPIO_ResetBits(GPIOC, GPIO_Pin_12);
         } else
           {
             GPIO_SetBits(GPIOC, GPIO_Pin_12);
           }

}

/*******************************************************************************
 * Description    : Create the serial number string descriptor.
 *******************************************************************************/
void Get_SerialNum(INT8U *_pBuf)

{
	Device_Serial0 = *(INT32U*) (0x1FFFF7E8);
	Device_Serial1 = *(INT32U*) (0x1FFFF7EC);
	Device_Serial2 = *(INT32U*) (0x1FFFF7F0);

	Device_Serial0 += Device_Serial2;

	if (Device_Serial0 != 0)
	{
		IntToUnicode(Device_Serial0, &_pBuf[2], 8);
		IntToUnicode(Device_Serial1, &_pBuf[18], 4);
	}
}

/*******************************************************************************
 * Description    : Convert Hex 32Bits value into char.
 *******************************************************************************/
void IntToUnicode(INT32U value, INT8U *pbuf, INT8U len)
{
	for (idx = 0; idx < len; idx++)
	{
		if (((value >> 28)) < 0xA)
		{
			pbuf[2 * idx] = (value >> 28) + '0';
		}
		else
		{
			pbuf[2 * idx] = (value >> 28) + 'A' - 10;
		}

		value = value << 4;

		pbuf[2 * idx + 1] = 0;
	}
}
/*
*********************************************************************************************************
*	usb_SendBuf
*********************************************************************************************************
*/
void usb_SendBuf(INT8U *_pTxBuf, INT8U _ucLen)
{
    UserToPMABufferCopy(_pTxBuf, ENDP1_TXADDR, _ucLen);
    SetEPTxCount(ENDP1, _ucLen);
    SetEPTxValid(ENDP1); 
    while(GetEPTxStatus(ENDP1) != EP_TX_NAK);    
}

/*
*********************************************************************************************************
*	SaveHostDataToBuf
*********************************************************************************************************
*/
void usb_SaveHostDataToBuf(INT8U *_pInBuf, INT16U _usLen)
{        
	for (i = 0 ; i < _usLen; i++)
	{
		g_tUsbFifo.aRxBuf[g_tUsbFifo.usRxWrite] = _pInBuf[i];
		
		if (++g_tUsbFifo.usRxWrite >= USB_RX_BUF_SIZE)
		{
			g_tUsbFifo.usRxWrite = 0;
		}
	}
}

/*
*********************************************************************************************************
*	usb_GetRxByte
*********************************************************************************************************
*/
BOOLEAN usb_GetRxByte(void)
{
  usRxWrite = g_tUsbFifo.usRxWrite;
	
  if (g_tUsbFifo.usRxRead == usRxWrite)
  {
    return FALSE;
  }
	
	
  ucData = g_tUsbFifo.aRxBuf[g_tUsbFifo.usRxRead];
	
	
  if (++g_tUsbFifo.usRxRead >= USB_RX_BUF_SIZE)
  {
    g_tUsbFifo.usRxRead = 0;
  }
  		
  return TRUE;		
}

/*
*********************************************************************************************************
*	SendDataToHost
*********************************************************************************************************
*/
void usb_SendDataToHost(INT8U *_pTxBuf, INT16U _usLen)
{
  for (i = 0 ; i < _usLen; i++)
  {
    g_tUsbFifo.aTxBuf[g_tUsbFifo.usTxWrite] = _pTxBuf[i];
	
    if (++g_tUsbFifo.usTxWrite >= USB_RX_BUF_SIZE)
    {
      g_tUsbFifo.usTxWrite = 0;
    }	
  }
}

/*
*********************************************************************************************************
*	usb_GetTxWord
*********************************************************************************************************
*/
INT16U usb_GetTxWord(INT8U *_pByteNum)
{
  if (g_tUsbFifo.usTxRead == g_tUsbFifo.usTxWrite)
  {
    *_pByteNum = 0;
    return 0;
  }
	
  usData = g_tUsbFifo.aTxBuf[g_tUsbFifo.usTxRead];
	
  if (++g_tUsbFifo.usTxRead >= USB_TX_BUF_SIZE)
  {
    g_tUsbFifo.usTxRead = 0;
  }
	
  if (g_tUsbFifo.usTxRead == g_tUsbFifo.usTxWrite)
  {
    *_pByteNum = 1;		
    return usData;
  }	
	
  usData += g_tUsbFifo.aTxBuf[g_tUsbFifo.usTxRead] << 8;

  if (++g_tUsbFifo.usTxRead >= USB_TX_BUF_SIZE)
  {
    g_tUsbFifo.usTxRead = 0;
  }

  *_pByteNum = 2;	
  return usData;		
}

#endif