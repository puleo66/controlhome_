/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : usb_sil.h
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Simplified Interface Layer function prototypes.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_SIL_H
#define __USB_SIL_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

INT32U USB_SIL_Init(void);
INT32U USB_SIL_Write(INT8U bEpAddr, INT8U* pBufferPointer, INT32U wBufferSize);
INT32U USB_SIL_Read(INT8U bEpAddr, INT8U* pBufferPointer);

/* External variables --------------------------------------------------------*/

#endif /* __USB_SIL_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
