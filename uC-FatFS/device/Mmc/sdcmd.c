/****************************************Copyright (c)**************************************************
**                               Hangzhou yh software  Co.,LTD.
**                                     
**                                 http://www.armgcc.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			sdcmd.c
** Last modified Date:	2007-10-15
** Last Version:		V1.0
** Descriptions:		SD/MMC 读写模块: 物理层 ---- SD/MMC卡SPI模式支持的命令 						
**------------------------------------------------------------------------------------------------------
** Created by:			lhlzjut@hotmail.com
** Created date:		2007-10-15
** Version:				V1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include <stdio.h>
//#include "sdhal.h"
#include "sdcrc.h"
#include "sdcmd.h"
#include "sddriver.h"

#if SD_UCOSII_EN    
#include  "ucos_ii.h"
#endif


/********************************************************************************************************************
** Name:	  INT8U SD_SendCmd()
** Function:      send command to the card,and get a response
** Input:	  INT8U cmd	    : command byte	
** 	          INT8U *param	: command parameter,length is 4 bytes  
**	          INT8U resptype: response type
**	          INT8U *resp	: response,length is 1-5 bytes
** Output:	  0:  right		>0:  error code
********************************************************************************************************************/
INT8U SD_SendCmd(INT8U cmd, INT8U *param, INT8U resptype, INT8U *resp)
{
	CPU_INT32S i,rlen;
	INT8U tmp;
    
	MSD_CS_Assert();
   
    SPI2_SendByte((cmd & 0x3F) | 0x40);				 /*  send command header and word */
    
    for (i = 3; i >= 0; i--)
        SPI2_SendByte(param[i]);					 /*  send parameters */

#if SD_CRC_EN
	tmp = SD_GetCmdByte6((cmd & 0x3F) | 0x40, param);
	SPI2_SendByte(tmp);
#else
    SPI2_SendByte(0x95);					        /*  CRC,only used for the first command */
#endif 
    
    rlen = 0;
    switch (resptype)								 
    {								/* according various command,get the various response length */
  		case R1:  rlen = 1;  break;
   	 	case R1B: rlen = 1;  break;
       		 
    	case R2:  rlen = 2;	 break;
       		 
   		case R3:  rlen = 5;	 break;
       		 
    	default:  
			 SPI2_SendByte(0xFF);	
      		 MSD_CS_Deassert();						 
        	 return SD_ERR_CMD_RESPTYPE;		        /*  return error of command response type */
    }
    i = 0;				
    do 												 
    {								/* Wait for a response,a response is a start bit(zero) */ 
        tmp = SPI2_RecByte();
        i++;
    }while (((tmp & 0x80) != 0) && (i < SD_CMD_TIMEOUT));
    
    if (i >= SD_CMD_TIMEOUT)
    {												 
        MSD_CS_Deassert();
        return SD_ERR_CMD_TIMEOUT;				/* return response timeout of command */
    }
    
    for (i = rlen - 1; i >= 0; i--)
    {
        resp[i] = tmp;
        tmp = SPI2_RecByte();					/* clock  at the last recycle,clock out 8 clock */
    }
      
    MSD_CS_Deassert();
    return SD_NO_ERR;						/* return perform sucessfully */
}

/********************************************************************************************************************
** Name:	  void SD_PackParam()
** Function:      change 32bit parameter to bytes form 
** Input:	  INT8U *parameter: the buffer of bytes parameter
**	          INT32U value    : 32bit parameter
** Output:	  NULL
*********************************************************************************************************************/
void SD_PackParam(INT8U *parameter, INT32U value)
{
    parameter[3] = (INT8U)(value >> 24);
    parameter[2] = (INT8U)(value >> 16);
    parameter[1] = (INT8U)(value >> 8);
    parameter[0] = (INT8U)(value);
}

/********************************************************************************************************************
** Name:	  INT8U SD_BlockCommand()
** Function:      command about block operation
** Input:	  INT8U cmd	      : command byte 
**                INT8U resptype  : response type
**	          INT32U parameter: parameter of block operation
** Output:	  0:  right		>0:  error code
*********************************************************************************************************************/
INT8U SD_BlockCommand(INT8U cmd, INT8U resptype, INT32U parameter)
{
	INT8U param[4],resp,ret;
	
	parameter <<= SD_BLOCKSIZE_NBITS;					/* adjust address: move 9 bits left */

	SD_PackParam(param, parameter);						/* change the parameter to bytes form */	

	ret = SD_SendCmd(cmd, param, resptype, &resp);
	if (ret != SD_NO_ERR)
	   	 return ret;							/* stop transmission operation fail */
	
	if (resp != 0) {
		 return SD_ERR_CMD_RESP;		 			/* response is error */
	}
		 
	return SD_NO_ERR;
}
	
/********************************************************************************************************************
** Name:	  INT8U SD_ResetSD()
** Function:      reset SD/MMC card
** Input:	  NULL
** Output:	  0:  right		>0:  error code
*********************************************************************************************************************/
INT8U SD_ResetSD(void)
{
	INT8U param[4] = {0,0,0,0},resp;
	
    return (SD_SendCmd(CMD0, param, CMD0_R, &resp));	/* command that reset card */
}

/********************************************************************************************************************
** Name:	  INT8U SD_ReadCSD()
** Function: read CSD register of SD/MMC card 
** INT8U csdlen  : len of register (fixed,is 16)
**	           INT8U *recbuf : recbuffer	
** Output:	  0:  right		>0:  error code
*********************************************************************************************************************/
INT8U SD_ReadCSD(INT8U csdlen, INT8U *recbuf)
{
    INT8U param[4] = {0,0,0,0},resp,ret;
  	
    ret = SD_SendCmd(CMD9, param, CMD9_R, &resp);		/* command that read CSD register */
    if (ret != SD_NO_ERR) 									
        return ret;									
  	
    if (resp != 0)
        return SD_ERR_CMD_RESP;					/* response is error */
     
	return (SD_ReadRegister(csdlen, recbuf));
}

/*******************************************************************************************************************
** Name:	  INT8U SD_ReadCID()
** Function: read CID register of sd card
** INT8U cidlen  : len of register (fixed,is 16)
** INT8U *recbuf : recbuffer	
** Output:	  0:  right		>0:  error code
********************************************************************************************************************/
#if SD_ReadCID_EN
INT8U SD_ReadCID(INT8U cidlen, INT8U *recbuf)
{
    INT8U param[4] = {0,0,0,0},resp,ret;
 
    ret = SD_SendCmd(CMD10, param, CMD10_R, &resp);		/* command that read CID register */
    if ( ret != SD_NO_ERR)
   		return ret;			  									
   
    if (resp != 0)
        return SD_ERR_CMD_RESP;					/* response is error */
      
  	return (SD_ReadRegister(cidlen, recbuf));
}
#endif

/********************************************************************************************************************
** Name:	  INT8U SD_StopTransmission()
** Function: stop data transmission 
** Input:    NULL
** Output:	  0:  right		>0:  error code
*********************************************************************************************************************/
INT8U SD_StopTransmission(void)
{
	INT8U param[4] = {0,0,0,0},resp;
	
 	return (SD_SendCmd(CMD12, param, CMD12_R, &resp));	/* stop transmission command fail */
}

/*********************************************************************************************************************
** Name:	  INT8U SD_ReadCard_Status()
** Function: read Card Status register of SD/MMC card 
** INT8U len:      len of register (fixed,is 2)
** INT8U *recbuf : recbuffer
** Output:	  0:  right		>0:  error code
**********************************************************************************************************************/
INT8U SD_ReadCard_Status(INT8U len, INT8U *buffer)
{
    INT8U param[4] = {0,0,0,0};

    return (SD_SendCmd(CMD13, param, CMD13_R, buffer)); 	/* read register of Card Status */
}


/********************************************************************************************************************
** Name:	  INT8U SD_SetBlockLen()
** Function:      set a block len of card 
** Input:	  INT32U length	: the length of a block
** Output:	  0:  right		>0:  error code
*********************************************************************************************************************/
INT8U SD_SetBlockLen(INT32U length)
{
	INT8U param[4],resp,ret;
  
    SD_PackParam(param, length);					/* change the parameter to bytes form */
          												
    ret = SD_SendCmd(CMD16, param, CMD16_R, &resp);
    if (ret != SD_NO_ERR)
 		return ret;						/* set the length of block to length fail */
	
	if (resp != 0)
    	return SD_ERR_CMD_RESP;			   			/* response is error */
    
    return SD_NO_ERR; 							/* return perform sucessfully */			
}

/********************************************************************************************************************
** Name:	  INT8U SD_ReadSingleBlock()
** Function: read single block command
** Input:	  INT32U blockaddr: block address
** Output:	  0:  right	>0:  error code
*********************************************************************************************************************/
INT8U SD_ReadSingleBlock(INT32U blockaddr)
{
	return (SD_BlockCommand(CMD17, CMD17_R, blockaddr));           /* command that read single block */
}

/********************************************************************************************************************
** Name:	  INT8U SD_ReadMultipleBlock()
** Function:      read multiple block command 
** Input:	  INT32U blockaddr: block address
** Output:	  0:  right		>0:  error code
*********************************************************************************************************************/
INT8U SD_ReadMultipleBlock(INT32U blockaddr)
{
	return (SD_BlockCommand(CMD18, CMD18_R, blockaddr));          /* command that read multiple block */
}

/********************************************************************************************************************
** Name:	  INT8U SD_WriteSingleBlock()
** Function:      write single block command
** Input:	  INT32U blockaddr: block address
** Output:	  0:  right		>0:  error code
*********************************************************************************************************************/
INT8U SD_WriteSingleBlock(INT32U blockaddr)
{
	return (SD_BlockCommand(CMD24, CMD24_R, blockaddr)); /* command that write single block */
}

/********************************************************************************************************************
** Name:	  INT8U SD_WriteMultipleBlock()
** Function:      write multiple block command
** Input:	  INT32U blockaddr: block address
** Output:	  0:  right	>0:  error code
*********************************************************************************************************************/
INT8U SD_WriteMultipleBlock(INT32U blockaddr)
{
	return (SD_BlockCommand(CMD25, CMD25_R, blockaddr)); /* command that write multiple block */
}

/********************************************************************************************************************
** Name:	  INT8U SD_ProgramCSD()
** Function: write CSD register
** Input:	  INT8U *buff   	  : the content of CSD register	
**                INT8U len		  : the length of CSD register
** Output:	  0:  right		>0:  error code
********************************************************************************************************************/
#if SD_ProgramCSD_EN
INT8U SD_ProgramCSD(INT8U len, INT8U *buff)
{
	INT8U param[4] = {0,0,0,0},resp,ret;
	
	if (len != 16) return SD_ERR_USER_PARAM;

	ret = SD_SendCmd(CMD27, param, CMD27_R, &resp); 	/* send command that write CSD */
	if (ret != SD_NO_ERR)
		return ret;
		        
    if (resp != 0)    
        return SD_ERR_CMD_RESP;
		
	buff[15] = (SD_GetCRC7(buff, 15) << 1) + 0x01;  	/* calculate crc field in CSD */
		
	return(SD_WriteBlockData(0, 16, buff));
}

/********************************************************************************************************************
** Name:	  INT8U SD_GetCRC7()
** Function:      calculate crc7
** Input:         INT8U *pSource: data
**                INT16U len   : data length
** Output:	  CRC7 code
*********************************************************************************************************************/
INT8U SD_GetCRC7(INT8U *pSource, INT16U len)
{
	INT8U i = 0, j;
	INT8U reg = 0;
	
	do
	{
	    for (j = 0; j < 8; j++)
	    {
			reg <<= 1;
			reg ^= ((((pSource[i] << j) ^ reg) & 0x80) ? 0x9 : 0);
	    }
	    
	    i++;
	    
	}while(i < len);
	
	return reg;
}	
#endif	

#if SD_EraseBlock_EN
/********************************************************************************************************************
** Name:	  INT8U SD_EraseStartBlock()
** Function: select the start block address of erasing operation 
** Input: 	  INT32U startblock	: block address
** Output:	  0:  right	   >0:  error code
*********************************************************************************************************************/
INT8U SD_EraseStartBlock(INT32U startblock)
{
	if (sds.card_type == CARDTYPE_SD)
		return (SD_BlockCommand(CMD32, CMD32_R, startblock));	/* send the start block address of erasing operation */
	else
		return (SD_BlockCommand(CMD35, CMD35_R, startblock));	/* send the start block address of erasing operation */
}

/********************************************************************************************************************
** Name:	  INT8U SD_EraseEndBlock()
** Function:      select the end block address of erasing operation  
** Input:	  INT32U Length	: block address
** Output:	  0:  right	   >0:  error code
*********************************************************************************************************************/
INT8U SD_EraseEndBlock(INT32U endblock)
{
	if (sds.card_type == CARDTYPE_SD)
		return (SD_BlockCommand(CMD33, CMD33_R, endblock));     /* send the end block address of erasing operation */
	else
		return (SD_BlockCommand(CMD36, CMD36_R, endblock));     /* send the end block address of erasing operation */
}

/********************************************************************************************************************
** Name:	  INT8U SD_EraseSelectedBlock()
** Function:      erase block selected
** Input:	  NULL
** Output:	  0:  right		>0:  error code
*********************************************************************************************************************/
INT8U SD_EraseSelectedBlock(void)
{
	INT8U param[4],resp,tmp;
	
	SD_PackParam(param, 0);
	
	tmp = SD_SendCmd(CMD38, param, CMD38_R, &resp);	 	    /* erase blocks selected */
	if (tmp != SD_NO_ERR)
		return tmp;							 	
	
	if (SD_WaitBusy(SD_WAIT_ERASE) != SD_NO_ERR)		    /* wait for finishing erasing */
		return SD_ERR_TIMEOUT_ERASE;
	else
		return SD_NO_ERR;									
}	
#endif

/*********************************************************************************************************************
** Name:	  INT8U SD_ReadOCR()
** Function: read OCR register of card
** Input:	  INT8U ocrlen  : len of register (fixed,is 4)
**                INT8U *recbuf : recbuffer	
** Output:	  0:  right		>0:  error code
**********************************************************************************************************************/
INT8U SD_ReadOCR(INT8U ocrlen, INT8U *recbuf)
{
    INT8U param[4] = {0,0,0,0},resp[5],tmp;

    tmp = SD_SendCmd(CMD58, param, CMD58_R, resp);		
    if (tmp != SD_NO_ERR)						/* read OCR register command */
    	return tmp;		 										
    												
    if (resp[0] != 0)
        return SD_ERR_CMD_RESP;			 			/* response is error */
    
    for (tmp = 0; tmp < 4; tmp++)
    	recbuf[tmp] = resp[tmp + 1];				
    
    return SD_NO_ERR;
}

/********************************************************************************************************************
** Name:	  INT8U SD_EnableCRC()
** Function:      enable crc check of SD Card 
** Input:	  INT8U bEnable : 1:enable  0: disable
** Output:	  0:  right		>0:  error code
*********************************************************************************************************************/
#if SD_CRC_EN
INT8U SD_EnableCRC(INT8U bEnable)
{
	INT8U param[4],resp,ret;
		
	if (bEnable == 1)
		param[0] = 1;						/* enable crc */
	else
		param[1] = 0; 						/* disalbe crc */

	ret = SD_SendCmd(CMD59, param, CMD59_R, &resp);			/* enable/disable crc command */
	if (ret != SD_NO_ERR)
        return ret;							 
    
    if (resp != 0)    
       	return SD_ERR_CMD_RESP;						/* response is error */		
 	
	return SD_NO_ERR;

}
#endif

/*********************************************************************************************************************
** Name:	   INT8U SD_ReadSD_Status()
** Function: read SD_Status register of sd card 
** Input:    INT8U sdslen: len of register (fixed,is 64)
**           INT8U *recbuf: recbuffer	
** Output:	  0:  right		>0:  error code
** Note:     only SD card have SD Status Register
**********************************************************************************************************************/
#if SD_ReadSD_Status_EN
INT8U SD_ReadSD_Status(INT8U sdslen, INT8U *recbuf)
{
    INT8U param[4] = {0,0,0,0},resp[2],ret;
    
    ret = SD_SendCmd(CMD55, param, CMD55_R, resp);			
    if (ret != SD_NO_ERR)
    	return ret;						/* command that the followed commnad is a specific application */
    												 
    if (resp[0] != 0)
        return SD_ERR_CMD_RESP;					/* response is error */
    
    ret = SD_SendCmd(ACMD13, param, ACMD13_R, resp);		
    if (ret != SD_NO_ERR)
    	return ret;						/* command that read SD_Status register */
   												
    if ((resp[0] != 0) || (resp[1] != 0))
        return SD_ERR_CMD_RESP;					/* response is error */
        
	return (SD_ReadBlockData(sdslen, recbuf));		/* read the content of the register */
}
#endif

/*******************************************************************************************************************
** Name:	  INT8U SD_ReadSCR()
** Function:      read SCR register of SD card 
** Input:         INT8U scrlen		 : len of register (fixed,is 8)
**                INT8U *recbuf		 : recieve buffer	
** Output:	  0:  right		>0:  error code
** Note:	  MMC Card have not this register
********************************************************************************************************************/
#if SD_ReadSCR_EN
INT8U SD_ReadSCR(INT8U scrlen, INT8U *recbuf)
{
    INT8U param[4] = {0,0,0,0},resp,ret;
    
    ret = SD_SendCmd(CMD55, param, CMD55_R, &resp);		
    if (ret != SD_NO_ERR)					/* command that the followed commnad is a specific application */
    	return ret;													
    												 
    if (resp != 0)
        return SD_ERR_CMD_RESP;					/* response is error */
    
    ret = SD_SendCmd(ACMD51, param, ACMD51_R, &resp);           /* SD Status 命令*/
    if (ret != SD_NO_ERR)					/* command that read SD Status register */
   		return ret;													
				    															
    if (resp != 0)
        return SD_ERR_CMD_RESP;					/* response is error */
        
	return (SD_ReadBlockData(scrlen, recbuf));	 	/* read the content of the register */
}
#endif

/********************************************************************************************************************
** Name:	  INT8U SD_GetNumWRBlcoks()
** Function:      get the block numbers that written correctly
** Input:	  INT32U blocknum	: the block numbers returned
** Output:	  0:  right		>0:  error code
** Note:          MMC Card have no this command
*********************************************************************************************************************/
#if SD_WriteMultiBlock_EN
INT8U SD_GetNumWRBlcoks(INT32U *blocknum)
{
    INT8U tmp[4] = {0,0,0,0},resp,ret;
  
    ret = SD_SendCmd(CMD55, tmp, CMD55_R, &resp);	  	 
    if (ret != SD_NO_ERR) 					/* command that the followed commnad is a specific application */
    	return ret;
    	 
    if (resp != 0)
    	return SD_ERR_CMD_RESP;    	
 											
   	ret = SD_SendCmd(ACMD22, tmp, ACMD22_R, &resp);  	 
   	if (ret != SD_NO_ERR)					/* command that read the numbers of block written correctly */
   		return ret;											    
   		 														
	if (resp != 0)
    	return SD_ERR_CMD_RESP;					/* response is error */
    		
    ret = SD_ReadBlockData(4, tmp);				/* read the numbvers of block */
    if (ret != SD_NO_ERR)
    	return ret;
    	
    *blocknum = (tmp[0] << 24) + (tmp[1] << 16) + (tmp[2] << 8) + tmp[3];	
    								/* change to 32 bits */

	return SD_NO_ERR;    					/* return perform sucessfully */		
}
#endif

		
/********************************************************************************************************************
** Name:	  INT8U SD_ReadRegister()
** Function: read data from SD card
** Input:	  INT32U len   : length
**      	  INT8U *recbuf: receive buffer
** Output:	  0:  right		>0:  error code
*********************************************************************************************************************/
INT8U SD_ReadRegister(INT32U len, INT8U *recbuf)
{	
	INT32U i = 0;
	INT8U resp;

    MSD_CS_Assert();
    do{    								/* wait for data start token */
        resp = SPI2_RecByte();
    	i++;
    }while((resp == 0xFF) && (i < SD_READREG_TIMEOUT));    
    
    if (i >= SD_READREG_TIMEOUT)
    {
    	MSD_CS_Deassert();
    	return SD_ERR_TIMEOUT_READ;					/* timeout, return error */
  	}
  	
   	if (resp != SD_TOK_READ_STARTBLOCK)				
   	{								/* not receive data start token */
		recbuf[0] = resp;							
		i = 1;							/* still len - 1 bytes will be received */
   	}
   	else
   		i = 0;							/* received data start token,still len bytes will be received */
   	  	
    for (; i < len; i++)
   		recbuf[i] = SPI2_RecByte();				/* receive data */
   		
    i = SPI2_RecByte();								
    i = (i << 8) + SPI2_RecByte();    				        /* get 16-bit CRC */	

#if SD_CRC_EN 
   	if (i != SD_GetCRC16(recbuf, len))
   	{								/* CRC check is error */
   		SPI2_SendByte(0xFF);
   		MSD_CS_Deassert();		
  		return SD_ERR_DATA_CRC16;				/* return error of CRC16 */				
  	}    
#endif   
  
    SPI2_SendByte(0xFF);							/* clock out 8 clk before return */
    MSD_CS_Deassert();	
    	
	return SD_NO_ERR;
}	

/*******************************************************************************************************************
** Name:	  INT8U SD_ReadBlockData()
** Function: read block data from card
** Input:	  INT32U len    : length
**      	  INT8U *recbuf : the buffer of receive
** Output:	  0:  right		>0:  error code
*******************************************************************************************************************/
INT8U SD_ReadBlockData(INT32U len, INT8U *recbuf)
{
    INT8U tmp;
    INT32U i = 0,timeout;

#if SD_UCOSII_EN    
    timeout = SD_UCOSII_SMALLWAIT;						/* small wait time */
#else
	timeout = sds.timeout_read;						/* wait time that receive data start token */
#endif  
    
    MSD_CS_Assert();    
    do
    { 									    	/* wait for receiving data start token 0xFE */
        tmp = SPI2_RecByte();
        i++;
    }while((tmp == 0xFF) && (i < timeout));
	
#if SD_UCOSII_EN    
    if (i >= timeout)
   	{									/* continue to wait(suspend task) */
   		timeout = sds.timeout_read;
   		i = 0;
	    do
    	{
        	OSTimeDly(1);							/* tick(suspend task 1 tick) */
        	tmp = SPI2_RecByte();
       	 	i++;
   		}while((tmp == 0xFF) && (i < timeout));
	}
#endif

	if (i >= timeout)
	{
		MSD_CS_Deassert();
       	return SD_ERR_TIMEOUT_READ;						/* return error timeout error code of reading */
	}
	
	if (tmp != SD_TOK_READ_STARTBLOCK)					/* read start block token is error */
	{
		SPI2_SendByte(0xFF);
		MSD_CS_Deassert();
		return SD_ERR_DATA_START_TOK;
	}

	for (i = 0; i < len; i++) {
   		recbuf[i] = SPI2_RecByte();					/* receive data */
	}
   		
    i = SPI2_RecByte();								
    i = (i << 8) + SPI2_RecByte();    					        /* get 16-bit CRC */	

#if SD_CRC_EN 
   	if (i != SD_GetCRC16(recbuf, len))
   	{	
   		SPI2_SendByte(0xFF); 						/* CRC check is error */
   		MSD_CS_Deassert();		
  		return SD_ERR_DATA_CRC16;					/* return error of CRC16 */				
  	}    
#endif   

	SPI2_SendByte(0xFF); 
	MSD_CS_Deassert();

  	return SD_NO_ERR;							/* return function perform sucessfully */
}

/*******************************************************************************************************************
** Name:	  INT8U SD_WriteBlockData()
** Function: write block data to card
** INT8U bmulti   : multi blocks operate 1:Y 0:N 
**	  	  INT32U len     : length
**	 	  INT8U *sendbuf : the buffer of send
**Output:	  0:  right		>0:  error code
********************************************************************************************************************/
INT8U SD_WriteBlockData(INT8U bmulti, INT32U len, INT8U *sendbuf)
{
    INT16U i;
    #if 0
	INT16U tmp;
    #endif  

    MSD_CS_Assert();
		
    SPI2_SendByte(0xFF);						/* clock out 8 clk before start */
    
    if (bmulti == 1)
     SPI2_SendByte(SD_TOK_WRITE_STARTBLOCK_M);	                /* start token of write multi blocks */
    else
     SPI2_SendByte(SD_TOK_WRITE_STARTBLOCK);		        /* start token of write single block */

    for (i = 0; i < len; i++)
     SPI2_SendByte(sendbuf[i]);				        /* send data */

#if SD_CRC_EN	
	i = SD_GetCRC16(sendbuf,len);				/* calculate CRC16 */
#endif
	
	SPI2_SendByte((i >> 8) & 0xFF);
	SPI2_SendByte(i & 0xFF); 				/* send CRC16 check code */
			    
   #if 0
	tmp = SPI2_RecByte();

  	if ((tmp & SD_RESP_DATA_MSK) != SD_RESP_DATA_ACCETPTED)	
  	{		
   		SPI2_SendByte(0xFF);				/* clock out 8 clk before return */
   		MSD_CS_Deassert();
   		return SD_ERR_DATA_RESP;			/* data response error */
    }
    #endif   

    MSD_CS_Deassert();
     		
    if (SD_WaitBusy(SD_WAIT_WRITE) != SD_NO_ERR)			
    	return SD_ERR_TIMEOUT_WRITE;				/* write time out */
    else
    	return SD_NO_ERR; 					/* write right */
 }

/*******************************************************************************************************************
** Name:	  void SD_StopMultiToken(void)
** Function: send the token that stop multiple block write
** Input:	  NULL
** Output:	  NULL
********************************************************************************************************************/
void SD_StopMultiToken(void)
{
	MSD_CS_Assert();
	
	SPI2_SendByte(0xFF);						/* send 8 clock first */
	SPI2_SendByte(SD_TOK_STOP_MULTI);				/* send stop transmission token */
	SPI2_RecByte();
	
    MSD_CS_Deassert();
}


/********************************************************************************************************************
** Name:	  void SD_WaitBusy()
** Function: poll SD Card wheather it is busy
** Input:	  INT32U timeout: time out type
** Output:	  0: not time out   > 0: error code
*********************************************************************************************************************/
INT8U SD_WaitBusy(INT8U waittype)
{
    INT32U timeout, i = 0;
    INT8U tmp;
    
  	if (waittype == SD_WAIT_WRITE)
  		timeout = sds.timeout_write;			/* wait type is write operation */
  	else
  		timeout = sds.timeout_erase;   			/* wait type is erase operation */
    	
#if SD_UCOSII_EN
        timeout = SD_UCOSII_SMALLWAIT;				/* small wait time */
#endif
   
	MSD_CS_Assert();
   	do
   	{ 							/* wait for being busy end */
        tmp = SPI2_RecByte();
        i++;
    }while ((tmp != 0xFF) && (i < timeout));		        /* always receive 0 when card is busy */    

#if SD_UCOSII_EN
	if (i >= timeout)
	{							/* after small wait, card is still busy */
  		if (waittype == SD_WAIT_WRITE)
  			timeout = sds.timeout_write;
  		else
  			timeout = sds.timeout_erase;   
			
		i = 0;
   		do
   		{ 	
   		  OSTimeDly(1); 				/* OS suspend 1 tick */
       		  tmp = SPI2_RecByte();
       		  i++;
    	        }while ((tmp != 0xFF) && (i < timeout));        /* always receive 0 when card is busy */    
	}
#endif	

	if(i < timeout) 
		tmp = SD_NO_ERR;				/* return 0 indicate that operation is not time out */
	else 
		tmp = SD_ERR_TIMEOUT_WAIT;			/* return error code indicate that operation is time out */

	SPI2_SendByte(0xFF);
	MSD_CS_Deassert();								
	return tmp;							
}

/********************************************************************************************************************
** Name:	  void SD_SPIDelay()
** Function: SPI Bus delay 
** Input:	  INT8U value : delay value,do not beyond 255
** Output:	  NULL
*********************************************************************************************************************/
void SD_SPIDelay(INT8U value)
{
    INT8U i;

    for (i = 0; i < value; i++)
        SPI2_SendByte(0xFF);					/* clock out 0xFF */
}








