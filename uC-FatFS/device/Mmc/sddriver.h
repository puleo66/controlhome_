/****************************************Copyright (c)**************************************************
**                                 Co.,LTD.
**                                     
**                                 http://
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			sddriver.c
** Last modified Date:	2007-10-15
** Last Version:		V1.0
** Descriptions:		SD/MMC卡读写模块: 物理层 用户API函数(头文件)
**						Soft Packet of SD Card: user API funciton (header file)
**
**------------------------------------------------------------------------------------------------------
** Created by:			lhlzjut@hotmail.com
** Created date:		2007-10-15
** Version:				V1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

#ifndef __SDDRIVER_H__
#define __SDDRIVER_H__

#include "sdconfig.h"
 
/* the information structure variable of SD Card*/
typedef struct SD_STRUCT
{	
	INT8U card_type;
	INT32U block_num;				
	INT32U block_len;				
	INT32U erase_unit;				
	
	INT32U timeout_read;			
	INT32U timeout_write;			
	INT32U timeout_erase;			
	
}sd_struct;

extern sd_struct sds;				

		
		

#define   SD_NO_ERR			     	0x00			
#define   SD_ERR_NO_CARD		 	0x01			
#define   SD_ERR_USER_PARAM      	        0x02			
#define   SD_ERR_CARD_PARAM		 	0x03			
#define	  SD_ERR_VOL_NOTSUSP                    0x04			
#define   SD_ERR_OVER_CARDRANGE		        0x05			
#define   SD_ERR_UNKNOWN_CARD                   0x06			


#define   SD_ERR_CMD_RESPTYPE	 	        0x10			
#define   SD_ERR_CMD_TIMEOUT     	        0x11			
#define   SD_ERR_CMD_RESP		 	0x12			
			

#define   SD_ERR_DATA_CRC16      	        0x20			
#define   SD_ERR_DATA_START_TOK		        0x21			
#define	  SD_ERR_DATA_RESP		 	0x22			


#define   SD_ERR_TIMEOUT_WAIT    	        0x30			
#define   SD_ERR_TIMEOUT_READ    	        0x31			
#define	  SD_ERR_TIMEOUT_WRITE	 	        0x32			
#define   SD_ERR_TIMEOUT_ERASE   	        0x33			
#define	  SD_ERR_TIMEOUT_WAITIDLE 	        0x34			


#define	  SD_ERR_WRITE_BLK			0x40			
#define	  SD_ERR_WRITE_BLKNUMS                  0x41			
#define   SD_ERR_WRITE_PROTECT		        0x42			


#define   SD_ERR_CREATE_SEMSD		        0xA0			



#define	  CARDTYPE_SD				0x00			
#define   CARDTYPE_MMC				0x01			

#define   SD_UCOSII_SMALLWAIT		        900 //256				
#define   SD_READREG_TIMEOUT		        8				
	
		
/* initialize sd card */
extern INT8U SD_Initialize(void);	
							
/* read a single block from sd card */
extern INT8U SD_ReadBlock(INT32U blockaddr,INT32U len, INT8U *recbuf);  

/* read multi blocks from sd card */																	
extern INT8U SD_ReadMultiBlock(INT32U blockaddr, INT32U blocknum, INT8U *recbuf); 
 
/* write a block to sd card */               
extern INT8U SD_WriteBlock(INT32U blockaddr, INT32U len, INT8U *sendbuf);  

/* write multi blocks to sd card */  
extern INT8U SD_WriteMultiBlock(INT32U blockaddr, INT32U blocknum, INT8U *recbuf); 

/* Erase the block of sd card */
extern INT8U SD_EraseBlock(INT32U startaddr, INT32U blocknum); 

/* get information of sd card */
extern INT8U SD_GetCardInfo(void);

/* calculate timeout from CSD register */
extern INT8U SD_CalTimeout(INT8U *csdbuf);

/* active sd to initialize process */
extern INT8U SD_ActiveInit(void);

/* request semaphore accessed card to OS */
extern void SD_StartSD(void);

/* return semaphore accessed card to OS */
extern void SD_EndSD(void);


	  
	    	    	    
/* Mask off the bits in the OCR corresponding to voltage range 3.2V to
 * 3.4V, OCR bits 20 and 21 */
#define MSK_OCR_33			 	0x30

/* Number of tries to wait for the card to go idle during initialization */
#define SD_IDLE_WAIT_MAX     	                1000

/* timeout of command */
#define SD_CMD_TIMEOUT   	 	        100
	
/* 100ms correspond to SPI clock(unit: 8 clocks)*/
#define READ_TIMEOUT_100MS                      100 * (SPI_CLOCK / 1000 / 8)

/* 250ms correspond to SPI clock(unit: 8 clocks)*/
#define WRITE_TIMEOUT_250MS		        250 * (SPI_CLOCK / 1000 / 8)


#define TAAC_POS 			        1			//TACC
#define NSAC_POS			        2			//NSAC

#define READ_BL_LEN_POS		                5			//READ_BL_LEN

#define C_SIZE_POS1		               	6			//C_SIZE upper  2-bit
#define C_SIZE_POS2		        	7			//C_SIZE middle 8-bit
#define C_SIZE_POS3		        	8			//C_SIZE lower	2-bit

#define C_SIZE_MULT_POS1	                9			//C_SIZE_MULT upper 2-bit
#define C_SIZE_MULT_POS2	                10			//C_SIZE_MULT lower 1-bit	

#define SECTOR_SIZE_POS1	                10			//SECTOR_SIZE upper 5-bit
#define SECTOR_SIZE_POS2	                11			//SECTOR_SIZE lower 2-bit

#define R2WFACTOR_POS 		                12			//R2WFACTOR_POS

/**/
#define ERASE_GRP_SIZE_POS   	                10		       // ERASE_GRP_SIZE 5-bit
#define ERASE_GRP_MULTI_POS1 	                10		       // ERASE_GRP_MULTI 2-bit
#define ERASE_GRP_MULTI_POS2 	                11		       // ERASE_GRP_MULTI 3-bit


#define TAAC_MSK			        0x07		       //TACC 
#define NSAC_MSK			        0x78		       //NSAC 

#define READ_BL_LEN_MSK		                0x0F		       //READ_BL_LEN 

#define C_SIZE_MSK1			        0x03		       //C_SIZE 
#define C_SIZE_MSK3			        0xC0		       //C_SIZE 

#define C_SIZE_MULT_MSK1 	                0x03		       //C_SIZE_MULT 
#define C_SIZE_MULT_MSK2 	                0x80		       //C_SIZE_MULT 

#define R2WFACTOR_MSK		                0x1C		       //R2WFACTOR 

#define SECTOR_SIZE_MSK1	                0x3F		       //SECTOR_SIZE
#define SECTOR_SIZE_MSK2	                0x80		       //SECTOR_SIZE 

#define ERASE_GRP_SIZE_MSK 	         	0x7C		       // ERASE_GRP_SIZE 
#define ERASE_GRP_MULTI_MSK1 	                0x03		       // ERASE_GRP_MULTI 
#define ERASE_GRP_MULTI_MSK2 	                0xE0		       // ERASE_GRP_NULTI 

#endif
