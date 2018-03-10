/****************************************Copyright (c)**************************************************
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			sdconfig.h
** Last modified Date:	        2005-3-11
** Last Version:		V2.0
** Descriptions:		Soft Packet of SD/MMC Card: configuration header file
**
**------------------------------------------------------------------------------------------------------
** Created by:			lhlzjut@hotmail.com
** Created date:		2007-10-21
** Version:				V1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Created by:			Puleo
** Created date:		2013-06-18
** Version:			V1.1
** Descriptions:		
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

#ifndef __SDCONFIG_H__
#define __SDCONFIG_H__

#include "includes.h"


#define SD_UCOSII_EN			        1			

#define SD_CRC_EN		   		1			

#define SPI_CLOCK				18000000	

#define SD_BLOCKSIZE 			        512			

#define SD_BLOCKSIZE_NBITS		        9  


#define SD_ReadMultiBlock_EN                    0			

#define SD_WriteMultiBlock_EN                   0			

#define SD_EraseBlock_EN		        0			

#define SD_ProgramCSD_EN   		        0			
	
#define SD_ReadCID_EN	   		        0			
	
#define	SD_ReadSD_Status_EN		        0			
 
#define	SD_ReadSCR_EN			        0			

#define  SD_INSERT_STATUS()  	                0			

#define  SD_WP_STATUS()  		        0			

#endif




