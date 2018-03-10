/****************************************Copyright (c)**************************************************
**                               Hangzhou yh software  Co.,LTD.
**                                     
**                                 http://www.armgcc.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			sddriver.c
** Last modified Date:	2007-10-15
** Last Version:		V1.0
** Descriptions:		SD/MMC卡读写模块: 物理层 用户API函数
**						
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
#include "sdcmd.h"
#include "sddriver.h"

#if SD_UCOSII_EN    
#include  "ucos_ii.h"
#endif

#if SD_UCOSII_EN
	OS_EVENT *pSemSD = NULL;	/* semaphore that access card */
#endif 

/* the information structure variable of SD Card */
sd_struct sds;			

/* (0.000000001ns) timeout unit table */
const INT32U time_unit[8] = {1000000000,100000000,10000000, 1000000,100000,10000,1000,100};

/* timeout value table */							 
const INT8U time_value[16] = {0,10,12,13,15,20,25,30,35,40,45,50,55,60,70,80};
 
/* timeout factor table */                              
const INT8U r2w_fator[8] = {1,2,4,8,16,32,64,128};                           
 

void SD_Sleep(INT32U cpt_loop)
{
  while (cpt_loop -- != 0);
}   
	/*       
	***************************************************************************************************
		
	     User API Function: Initialize,read,write,erase SD Card 
				
	***************************************************************************************************
	*/
				
/*******************************************************************************************************************
** Name:	  INT8U SD_Initialize()
** Function: initialize SD/MMC card
** Input:	  NULL
** Output:	  0:  right			>0:  error code
********************************************************************************************************************/
INT8U SD_Initialize(void)
{
	INT8U recbuf[4],ret;
	
#if SD_UCOSII_EN
	if (pSemSD == NULL)
	{
    	pSemSD = OSSemCreate(1);					/* create SD/MMC semaphore */ 
		if (pSemSD == NULL)					
		 	return SD_ERR_CREATE_SEMSD;
	}				    
#endif	
   
    SD_StartSD();
//    SD_HardWareInit();					    		/* Initialize the hardware that access SD Card */
    
//    if (SD_ChkCard() != 1)						/* check weather card is inserted */
//    {	
//    	ret = SD_ERR_NO_CARD;   
//    	goto SD_ERR;
//    }
        
    MSD_CS_Assert();							/* 1. assert CS */  
	
    SD_SPIDelay(200);   						/* 2. clock delay more than 74 clock */
    
    MSD_CS_Deassert();  						/* 3. dessert CS */
    SD_SPIDelay(4);							/* 4. delay 2(8 clock) */
     
    SD_Sleep(1000000);
    
    ret = SD_ResetSD();							/* 5. send CMD0 command to reset sd card */
   	
    if (ret != SD_NO_ERR)
        goto SD_ERR;									
 	
    ret = SD_ActiveInit();						/* 6. active card initialize process */
    if (ret != SD_NO_ERR)
 	goto SD_ERR;
    
    ret = SD_ReadOCR(4, recbuf);  					/* 7. read OCR register,get the supported voltage */
    if (ret != SD_NO_ERR)
        goto SD_ERR;
    
    if ((recbuf[1] & MSK_OCR_33) != MSK_OCR_33)
    { 	 
        ret = SD_ERR_VOL_NOTSUSP;					/* not support 3.3V,return error code */
    	goto SD_ERR;
    }
          
#if SD_CRC_EN        
    ret = SD_EnableCRC(1);						/* enable CRC check */
	if (ret != SD_NO_ERR)  
	  	goto SD_ERR;
#endif

    ret = SD_SetBlockLen(SD_BLOCKSIZE);				        /* 8. Set the block length: 512Bytes */
    if (ret != SD_NO_ERR)  
        goto SD_ERR;
        
    ret = SD_GetCardInfo();						/* 9. read CSD register, get the information of SD card */    
	if (ret != SD_NO_ERR)
		goto SD_ERR;

	SD_EndSD();	

//	SPI_ClkToMax();							/* 10. set SPI clock to maximum */		

	return SD_NO_ERR;						/* initialize sucessfully */

SD_ERR:	
	SD_EndSD();	
	return ret;
}

/********************************************************************************************************************
** Name:	  INT8U SD_ReadBlock()
** Function:      read a single block from SD/MMC card
** Input:         INT32U blockaddr: address of block
**   	          INT8U *recbuf   : the buffer of receive,length is 512Bytes
** Output:	  0:  right			>0:  error code
*********************************************************************************************************************/
INT8U SD_ReadBlock(INT32U blockaddr,INT32U len, INT8U *recbuf)
{
	INT8U ret; 
	
	SD_StartSD();							/* request semaphore acessed SD/MMC to OS */
 
//        if (SD_ChkCard() != 1)							
//        {
//                SD_EndSD();	
//                return SD_ERR_NO_CARD;   					/* card is not inserted entirely */
//	}
		
	if (blockaddr > sds.block_num)	
	{
		SD_EndSD();	
		return SD_ERR_OVER_CARDRANGE;				/* operate over the card range */
	}
	
 	ret = SD_ReadSingleBlock(blockaddr);			        /* read single blocks command */						
 	if (ret != SD_NO_ERR)
 	{
 		SD_EndSD();									
 		return ret;
 	}
 	
  	ret = SD_ReadBlockData(len, recbuf);			        /* read data from sd card */	
	SD_EndSD();							/* return semaphore acessed SD/MMC to OS */
	
	return ret;
}

/********************************************************************************************************************
** Name:	  INT8U SD_ReadMultiBlock()
** Function: read multi blocks from SD/MMC card
** Input:	  INT32U blockaddr: address of block
**		  INT32U blocknum : the numbers of block
**		  INT8U *recbuf   : the buffer of receive,each block length is 512Bytes
** Output:	  0:  right			>0:  error code
*********************************************************************************************************************/
#if SD_ReadMultiBlock_EN
INT8U SD_ReadMultiBlock(INT32U blockaddr, INT32U blocknum, INT8U *recbuf)
{
    INT32U i;
    INT8U ret;
       
	SD_StartSD();							/* request semaphore acessed SD/MMC to OS */
    if (SD_ChkCard() != 1)							
    {
    	SD_EndSD();	
    	return SD_ERR_NO_CARD;   					/* card is not inserted entirely */
	}
		
	if ((blockaddr + blocknum) > sds.block_num)	
	{
		SD_EndSD();	
		return SD_ERR_OVER_CARDRANGE;				/* operate over the card range */
	}       
    
	ret = SD_ReadMultipleBlock(blockaddr);			        /* read multiple blocks command */
	if (ret != SD_NO_ERR)
	{	
		SD_EndSD();						/* return semaphore acessed SD/MMC to OS */						
		return ret;
    }
    
    for (i = 0; i < blocknum; i++)
    {									/* read data from SD/MMC card */
    	ret = SD_ReadBlockData(SD_BLOCKSIZE, recbuf);
    	if (ret == SD_NO_ERR)
       		recbuf = recbuf + SD_BLOCKSIZE;
    	else
    	{
    		SD_EndSD();
    		return ret;
    	}
    }
 	 
 	ret = SD_StopTransmission();				        /* stop transmission operation */ 
 	
 	SD_EndSD();
	return ret; 
}

#endif

/********************************************************************************************************************
** Name:	  INT8U SD_WriteBlock()
** Function: write a block to SD/MMC card
** Input: 	  INT32U blockaddr: address of block
**	  	  INT8U *sendbuf  : the buffer of send,length is 512Bytes
** Output:	  0:  right			>0:  error code
*********************************************************************************************************************/
INT8U SD_WriteBlock(INT32U blockaddr, INT32U len, INT8U *sendbuf)
{
	INT8U ret,tmp[2];
	
	SD_StartSD();							/* request semaphore acessed SD/MMC to OS */

//        if (SD_ChkCard() != 1)							
//        {
//                SD_EndSD();	
//                return SD_ERR_NO_CARD;   					/* card is not inserted entirely */
//	}	
	
	if (blockaddr > sds.block_num)	
	{
		SD_EndSD();	
		return SD_ERR_OVER_CARDRANGE;				/* operate over the card range */
	}
/*	
	if (SD_ChkCardWP() == 1)
	{
		SD_EndSD();
		return SD_ERR_WRITE_PROTECT;							
	}
*/	
	ret = SD_WriteSingleBlock(blockaddr);				/* write single block */
	if (ret != SD_NO_ERR)
	{	
		SD_EndSD();
		return ret;
	}
	
	ret = SD_WriteBlockData(0, len, sendbuf);			/* write data */
 	
	if (ret == SD_NO_ERR)						
 	{								/* read Card Status register to check write wheather sucessfully */
 		ret = SD_ReadCard_Status(2, tmp);
 		if (ret != SD_NO_ERR)
 		{
 			SD_EndSD();
 			return ret;					/* read register fail */
		}

 		if((tmp[0] != 0) || (tmp[1] != 0))
 		{
 			SD_EndSD();
			ret = SD_ERR_WRITE_BLK; 			/* response indicate write fail */
 		}
 	}
  
     SD_EndSD();
     return ret;							/* return the result of writing */									
}

/**********************************************************************************************************************
** Name:	  INT8U SD_WriteMultiBlock()
** Function: write multi blocks to SD/MMC card
** Input:	  INT32U blockaddr: address of block
**      	  INT32U blocknum : the numbers of block
**	          INT8U *sendbuf  : the send buffer,each block length is 512Bytes
** Output:	  0:  right			>0:  error code
***********************************************************************************************************************/
#if SD_WriteMultiBlock_EN
INT8U SD_WriteMultiBlock(INT32U blockaddr, INT32U blocknum, INT8U *sendbuf)
{
	INT32U i;
	INT8U ret,tmp[2];
	
	SD_StartSD();						/* request semaphore acessed SD/MMC to OS */
    if (SD_ChkCard() != 1)							
    {
    	SD_EndSD();	
    	return SD_ERR_NO_CARD;   				/* card is not inserted entirely */
	}
		
	if ((blockaddr + blocknum) > sds.block_num)	
	{
		SD_EndSD();	
		return SD_ERR_OVER_CARDRANGE;			/* operate over the card range */
	}
		
	if (SD_ChkCardWP() == 1)
	{
		SD_EndSD();
		return SD_ERR_WRITE_PROTECT;				
	}
	
	ret = SD_WriteMultipleBlock(blockaddr);			/* write multiple blocks command */
	if (ret != SD_NO_ERR)
	{
		SD_EndSD();
		return ret;
	}
	
    for (i = 0; i < blocknum; i++)
    {															
 		ret = SD_WriteBlockData(1, SD_BLOCKSIZE, sendbuf);	/* write data */
 		if (ret == SD_NO_ERR)	
 			sendbuf = sendbuf + SD_BLOCKSIZE;
 		else
 		{						        /* write fail */
			SD_StopTransmission();				/* stop data transmission */	
			SD_WaitBusy(SD_WAIT_WRITE);			/* waiting */
			SD_EndSD();
			return ret;
		}
	}
   
    SD_StopMultiToken();					/* send data stop token */
      
    ret = SD_WaitBusy(SD_WAIT_WRITE);				/* wait for finishing writing */
    if (ret != SD_NO_ERR)
    {
    	SD_EndSD();
    	return SD_ERR_TIMEOUT_WRITE;
    }
    
    if (sds.card_type == CARDTYPE_SD)
    {
   		ret = SD_GetNumWRBlcoks(&i);			/* read the blocks that be written correctly */
   		if (ret != SD_NO_ERR)
   		{
   			SD_EndSD();
   		  	return ret;
   		}
   		if(i != blocknum)
			ret =  SD_ERR_WRITE_BLKNUMS;		/* the blocks that be written correctly is error */
   	}
   	else
   	{
   	 	ret = SD_ReadCard_Status(2, tmp);
 		if (ret != SD_NO_ERR)
 		{
 			SD_EndSD();
 			return ret;				/* read register fail */
		}
 		if((tmp[0] != 0) || (tmp[1] != 0))
			ret = SD_ERR_WRITE_BLK; 		/* response indicate write fail */
   	}
   	 	
   	SD_EndSD();   	     
	return ret;						/* return write sucessfully */			
}
#endif

/*********************************************************************************************************************
** Name:	  INT8U SD_EraseBlock()
** Function: Erase the block of SD/MMC card
** Input:         INT32U startaddr: start address
**		  INT32U endaddr  : end address
** Output:	  0:  right			>0:  error code
** sds.erase_unit 
*********************************************************************************************************************/
#if SD_EraseBlock_EN
INT8U SD_EraseBlock(INT32U startaddr, INT32U blocknum)
{
	INT32 tmp;
	INT8U ret;
	
	SD_StartSD();									/* request semaphore acessed SD/MMC to OS */
    if (SD_ChkCard() != 1)							
    {
    	SD_EndSD();	
    	return SD_ERR_NO_CARD;   							/* card is not inserted entirely */
	}
	
	if ((startaddr + blocknum) > sds.block_num)	
	{
		SD_EndSD();	
		return SD_ERR_OVER_CARDRANGE;						/* operate over the card range */
	}
		
	if (SD_ChkCardWP() == 1)
	{
		SD_EndSD();
		return SD_ERR_WRITE_PROTECT;							
	}	

	tmp = blocknum - sds.erase_unit;	
	while(tmp >= 0) 								/* once erase is sector size */
	{	
		ret = SD_EraseStartBlock(startaddr);					/* select start address */
		if (ret != SD_NO_ERR)
		{
			SD_EndSD();
			return ret;
		}
		
		ret = SD_EraseEndBlock(startaddr + sds.erase_unit - 1);		        /* select end address */
		if (ret != SD_NO_ERR)
		{
			SD_EndSD();
			return ret;
		}
			
		ret = SD_EraseSelectedBlock();						/* erase blocks selected */
		if (ret != SD_NO_ERR)
		{
			SD_EndSD();
			return ret;
		}

		startaddr += sds.erase_unit;							
		blocknum  -= sds.erase_unit;
		tmp = blocknum - sds.erase_unit;
	};		
	
	if (blocknum > 0)									
	{										/* erase blocks that numbers is not enough once_erase */
		ret = SD_EraseStartBlock(startaddr);
		if (ret != SD_NO_ERR)
		{
			SD_EndSD();
			return ret;
		}
		
		ret = SD_EraseEndBlock(startaddr + blocknum - 1);
		if (ret != SD_NO_ERR)
		{
			SD_EndSD();
			return ret;
		}
			
		ret = SD_EraseSelectedBlock();
		if (ret != SD_NO_ERR)
		{
			SD_EndSD();
			return ret;
		}
	}

	SD_EndSD();
	return SD_NO_ERR;								/* return erase sucessfully */
}
#endif


/*******************************************************************************************************************
** Name:	  INT8U SD_GetCardInfo()
** Function: get the information of SD/MMC card
** Input:    INT8U cardtype: card type	
** Output:	  0:  right			>0:  error code
*******************************************************************************************************************/
INT8U SD_GetCardInfo()
{
	INT32U tmp;
	INT8U csdbuf[16],ret;

	ret = SD_ReadCSD(16, csdbuf);	 						/* read CSD register */
	if (ret != SD_NO_ERR)	
		return ret;	
		
	SD_CalTimeout(csdbuf);								/* calculate timeout value */
		
	                          							/* calculate the size of a sector */
	sds.block_len = 1 << (csdbuf[READ_BL_LEN_POS] & READ_BL_LEN_MSK);  		/* (2 ^ READ_BL_LEN) */
	
	                								/* calculate the sector numbers of the SD Card */
	sds.block_num = ((csdbuf[C_SIZE_POS1] & C_SIZE_MSK1) << 10) +
	      			 (csdbuf[C_SIZE_POS2] << 2) +
	 	 			((csdbuf[C_SIZE_POS3] & C_SIZE_MSK3) >> 6) + 1;	/* (C_SIZE + 1)*/
		 	  															
	tmp = ((csdbuf[C_SIZE_MULT_POS1] & C_SIZE_MULT_MSK1) << 1) +   
	      ((csdbuf[C_SIZE_MULT_POS2] & C_SIZE_MULT_MSK2) >> 7) + 2;			/* (C_SIZE_MULT + 2) */
    	
                         								/* get the block numbers in card */
	sds.block_num = sds.block_num * (1 << tmp);					/* (C_SIZE + 1) * 2 ^ (C_SIZE_MULT + 2) */
																			
	
	if (sds.card_type == CARDTYPE_MMC)
	{					    
		tmp  = ((csdbuf[ERASE_GRP_SIZE_POS] & ERASE_GRP_SIZE_MSK) >> 2) + 1;  	/* (ERASE_GRP_SIZE + 1)  */ 
		
		/* (ERASE_GRP_SIZE + 1) * (ERASE_GRP_MULTI + 1) */
		tmp *= ((csdbuf[ERASE_GRP_MULTI_POS1] & ERASE_GRP_MULTI_MSK1) << 3) +
		       ((csdbuf[ERASE_GRP_MULTI_POS2] & ERASE_GRP_MULTI_MSK2) >> 5) + 1;	
	}
	else																		/*calculate the size of sector */
		tmp = ((csdbuf[SECTOR_SIZE_POS1] & SECTOR_SIZE_MSK1) << 1) +  			
	          ((csdbuf[SECTOR_SIZE_POS2] & SECTOR_SIZE_MSK2) >> 7) + 1;		/* SD: SECTOR_SIZE */
	
	sds.erase_unit = tmp;	    							 
	
	return SD_NO_ERR;															/* 返回执行成功 return perform sucessfully */
}


/*******************************************************************************************************************
** Name:	  INT8U SD_CalTimeout()
** Function: calculate timeout of reading,writing,erasing
** Input: 	  INT8U *csdbuf : CSD register content
** Output:	  0:  right			>0:  error code
*******************************************************************************************************************/
INT8U SD_CalTimeout(INT8U *csdbuf)
{
	INT32U tmp;
	INT8U time_u,time_v,fator;
	
	sds.timeout_read = READ_TIMEOUT_100MS;					/* 100ms */
	sds.timeout_write = WRITE_TIMEOUT_250MS;				/* 250ms */
	sds.timeout_erase = WRITE_TIMEOUT_250MS;
		
	time_u = (csdbuf[TAAC_POS] & TAAC_MSK);					/* read timeout unit */
	time_v = (csdbuf[TAAC_POS] & NSAC_MSK) >> 3;				/* read timeout value */
	fator = (csdbuf[R2WFACTOR_POS] & R2WFACTOR_MSK) >> 2;			/* read timeout factor */
	
	if(time_v == 0)	return SD_ERR_CARD_PARAM;				/* card parameter is error */
	
	tmp = SPI_CLOCK * time_value[time_v] / 10 / time_unit[time_u];		/* TACC * f (unit: clock) */
	tmp = tmp + csdbuf[NSAC_POS] * 100;					/* TACC * f + NSAC * 100 (unit: clock) */
	
	/* the timeout value of being calculated */
	sds.timeout_read = tmp;
	sds.timeout_write = tmp * r2w_fator[fator];				/* (TACC * f + NSAC * 100) * R2WFACTOR (unit:clock)*/
	
	if (sds.card_type == CARDTYPE_SD)
	{
		sds.timeout_read  = sds.timeout_read * 100 / 8;					
		sds.timeout_write = sds.timeout_write * 100 / 8;
		if (sds.timeout_read > READ_TIMEOUT_100MS)					
			sds.timeout_read = READ_TIMEOUT_100MS;
		
		if (sds.timeout_write > WRITE_TIMEOUT_250MS)
			sds.timeout_write = WRITE_TIMEOUT_250MS;
	}
	else
	{
		sds.timeout_read  = sds.timeout_read * 10 / 8;					
		sds.timeout_write = sds.timeout_write * 10 / 8;
	}
	
	sds.timeout_erase = sds.timeout_write;
	
#if SD_UCOSII_EN
	sds.timeout_read = (sds.timeout_read << 3) * OS_TICKS_PER_SEC / SPI_CLOCK; 
	sds.timeout_write =(sds.timeout_write << 3) * OS_TICKS_PER_SEC / SPI_CLOCK;
	sds.timeout_erase = sds.timeout_write;							/* (unit: os tick) */
#endif	
	
	return SD_NO_ERR;	
}

/*******************************************************************************************************************
** Name:	  INT8U SD_ActiveInit()
** Function: active card, and get the card type 
** Input:    NULL
** Output:	  0:  right			>0:  error code
*******************************************************************************************************************/
INT8U SD_ActiveInit(void)
{
	INT8U param[4] = {0,0,0,0},resp[5],ret;
	INT32U i = 0;
	
 	do 
    {										/* send CMD1 to poll card status */
        ret = SD_SendCmd(CMD1, param, CMD1_R, resp);
        if (ret != SD_NO_ERR)
       		return ret;
        i ++;
    }while (((resp[0] & MSK_IDLE) == MSK_IDLE) && (i <= SD_IDLE_WAIT_MAX));
    										/* if response R1 Idle bit is 1,continue recycle */    
    if (i >= SD_IDLE_WAIT_MAX)
        return SD_ERR_TIMEOUT_WAITIDLE;						/* time out,return error */	
	
    ret = SD_SendCmd(CMD55, param, CMD55_R, resp);
    if (ret != SD_NO_ERR)
       	return ret;
       	
    ret = SD_SendCmd(ACMD41, param, ACMD41_R, resp);		                /* active card to initialize process internal */
    if (ret != SD_NO_ERR)	
    	return SD_ERR_UNKNOWN_CARD;
    
    if ((resp[0] & 0xFE) == 0)
      	sds.card_type = CARDTYPE_SD;						/* the card is SD card */	
    else	
    	sds.card_type = CARDTYPE_MMC;						/* the card is MMC card */

	return SD_NO_ERR;
}

/*******************************************************************************************************************
** Name:	  INT8U SD_StartSD()
** Function: request the right of operating sd to OS
** Input:	  NULL
** return:	  NULL
********************************************************************************************************************/
void SD_StartSD(void)
{
#if SD_UCOSII_EN
	INT8U ret;
	
	OSSemPend(pSemSD, 0, &ret);					/* wait for semaphore that accessed Card */
#endif
}

/*******************************************************************************************************************
** Name:	  INT8U SD_EndSD()
** Function: return the right of operating sd to OS
** Input:	  NULL
** return:	  NULL
********************************************************************************************************************/
void SD_EndSD(void)
{
#if SD_UCOSII_EN
	OSSemPost(pSemSD);						/* return the semaphore accessing Card to OS */
#endif
}	


/*******************************************************************************************************************
** Name:	  INT16U SD_GetZLGSDVer()
** Function: get the version of ZLG/SD
** Input:    NULL
** Output:	  Version
*******************************************************************************************************************/
INT16U SD_GetZLGSDVer(void)
{
	return 0x0200;						
}










