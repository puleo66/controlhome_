/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
 * File Name          : hw_config.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define USB_TX_BUF_SIZE		120
#define USB_RX_BUF_SIZE		20

typedef struct
{
	INT8U aTxBuf[USB_TX_BUF_SIZE];	
	INT8U aRxBuf[USB_RX_BUF_SIZE];	
	
	INT16U usTxRead;			
	INT16U usTxWrite;			
	INT16U usRxRead;			
	INT16U usRxWrite;			
	
	INT16U usTxState;					
}USB_COM_FIFO_T;

/* Exported functions ------------------------------------------------------- */
void Set_USBSystem(void);
void usb_EnterLowPowerMode(void);
void usb_LeaveLowPowerMode(void);
void usb_CableConfig(INT8U _ucMode);
void Get_SerialNum(INT8U *_pBuf);

void usb_SaveHostDataToBuf(INT8U *_pInBuf, INT16U _usLen);
INT16U usb_GetTxWord(INT8U *_pByteNum);
BOOLEAN usb_GetRxByte(void);
void usb_SendDataToHost(INT8U *_pTxBuf, INT16U _usLen);
void usb_SendBuf(INT8U *_pTxBuf, INT8U _ucLen);
/* External variables --------------------------------------------------------*/

extern USB_COM_FIFO_T g_tUsbFifo;

#endif  /*__HW_CONFIG_H*/
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
