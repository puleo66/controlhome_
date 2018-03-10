/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : usb_mem.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Utility functions for memory transfers to/from PMA
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
#ifndef STM32F10X_CL

/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#if USBD_MODULE == DEF_ENABLED

INT32U i, temp1, temp2;
INT16U *pdwVal;
INT32U *pdwVal1;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : UserToPMABufferCopy
* Description    : Copy a buffer from user memory area to packet memory area (PMA)
* Input          : - pbUsrBuf: pointer to user memory area.
*                  - wPMABufAddr: address into PMA.
*                  - wNBytes: no. of bytes to be copied.
* Output         : None.
* Return         : None	.
*******************************************************************************/
void UserToPMABufferCopy(INT8U *pbUsrBuf, INT16U wPMABufAddr, INT16U wNBytes)
{ 
  pdwVal = (INT16U *)(wPMABufAddr * 2 + PMAAddr);
  for (i = (wNBytes + 1) >> 1; i != 0; i--)
  {
    temp1 = (INT16U) * pbUsrBuf;
    pbUsrBuf++;
    temp2 = temp1 | (INT16U) * pbUsrBuf << 8;
    *pdwVal++ = temp2;
    pdwVal++;
    pbUsrBuf++;
  }
}
/*******************************************************************************
* Function Name  : PMAToUserBufferCopy
* Description    : Copy a buffer from user memory area to packet memory area (PMA)
* Input          : - pbUsrBuf    = pointer to user memory area.
*                  - wPMABufAddr = address into PMA.
*                  - wNBytes     = no. of bytes to be copied.
* Output         : None.
* Return         : None.
*******************************************************************************/
void PMAToUserBufferCopy(INT8U *pbUsrBuf, INT16U wPMABufAddr, INT16U wNBytes)
{
  pdwVal1 = (INT32U *)(wPMABufAddr * 2 + PMAAddr);
  for (i = (wNBytes + 1) >> 1; i != 0; i--)
  {
    *(INT16U*)pbUsrBuf++ = *pdwVal1++;
    pbUsrBuf++;
  }
}

#endif /* STM32F10X_CL */
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
#endif