/*
**********************************************************************
*                          Micrium, Inc.
*                      949 Crestview Circle
*                     Weston,  FL 33327-1848
*
*                            uC/FS
*
*             (c) Copyright 2001 - 2003, Micrium, Inc.
*                      All rights reserved.
*
***********************************************************************

----------------------------------------------------------------------
File        : mmc_drv.c
Purpose     : File system generic MMC/SD driver
---------------------------END-OF-HEADER------------------------------
*/

/*********************************************************************
*
*             #include Section
*
**********************************************************************
*/
#include <stdio.h>
#include "fs_api.h"
#include "fs_clib.h"
#include "fs_dev.h"

#if FS_USE_MMC_DRIVER

#include "bsp.h"

#include "sddriver.h"
#include "fs_lbl.h"


#define FS__DEV_MBRFLAG_CLI     0xFA
#define FS__DEV_MBRFLAG_JUMP1   0xEB
#define FS__DEV_MBRFLAG_JUMP2   0xE9
#define FS__DEV_MBRSECTORFLAG_L 0x55
#define FS__DEV_MBRSECTORFLAG_H 0xAA

#define FS__DEV_PART_0_TABLE    0x01BE

/*********************************************************************
*
*             Local Variables        
*
**********************************************************************
*/

static FS_u32   _FS_mmc_logicalstart[FS_MMC_MAXUNIT]; /* start of partition */
static char     _FS_mmc_mbrbuffer[0x200];             /* buffer for reading MBR */   
static char     _FS_mmc_diskchange[FS_MMC_MAXUNIT];   /* signal flag for driver */
static char     _FS_mmc_busycnt[FS_MMC_MAXUNIT];      /* counter for BSY LED on/off */
/*********************************************************************
*
*             Local functions
*
**********************************************************************
*/


/*********************************************************************
*
*             FS_MMC_HW_X_BusyLedOff
*
*  Description:
*    FS low level function. Switches the busy LED off.
*
*  Parameters:
*    Unit      - Device Index
*
*  Return value:
*    void
*/
static void FS_MMC_HW_X_BusyLedOff (FS_u8 Unit) {
	//GPIO_ResetBits(GPIOC, DEF_BIT_07);
}

/*********************************************************************
*
*             FS_MMC_HW_X_BusyLedOn
*
*  Description:
*    FS low level function. Switches the busy LED off.
*
*  Parameters:
*    Unit      - Device Index
*
*  Return value:
*    void
*/

static void FS_MMC_HW_X_BusyLedOn(FS_u8 Unit) {
	//GPIO_SetBits(GPIOC, DEF_BIT_07);
}


/*********************************************************************
*
*             FS_MMC_HW_X_IsPresent
*
*  Description:
*    Returns the state of the media. If you do not know the state, return
*    FS_MEDIA_STATEUNKNOWN and the higher layer will try to figure out if
*    a media is present.
*
*  Parameters:
*    Unit                 - Device Index
*
*  Return value:
*    FS_MEDIA_STATEUNKNOWN       - the state of the media is unkown
*    FS_MEDIA_ISNOTPRESENT       - no card is present
*    FS_MEDIA_ISPRESENT          - a card is present
*/

static char FS_MMC_HW_X_IsPresent(FS_u8 Unit) {
  //return FS_MEDIA_STATEUNKNOWN;
  return FS_MEDIA_ISPRESENT;
}

/*********************************************************************
*
*             FS_GetMediaStartSec
*
*  Description:
*    FS api function. Get logical start sector from master boot record
*    or partition table.
*
*  Parameters:
*    DevIndex        - DevIndex number.
*    pBuffer         - A pointer to a buffer containing the first
*                      sector of the media. This should contain the
*                      master boot record (MBR) or Bios Parameter Block
*                      (BPB) if the device has no partition table.
* 
*  Return value:
*    < 0xFFFF        - Number of the first sector of the medium.
*    ==0xFFFF        - No valid MBR/BPB found.
*/

static FS_u32 FS_GetMediaStartSec(const FS_u8 *pBuffer) {
  FS_u8  c;
  FS_u32 StartSec;
 
  c = pBuffer[0x1FE]; /* check first part of the signature */
  if (c != FS__DEV_MBRSECTORFLAG_L) {
     return 0xFFFFFFFF;
  }

  c = pBuffer[0x1FF]; /* check second part of the signature */
  if (c != FS__DEV_MBRSECTORFLAG_H) {
    return 0xFFFFFFFF;
  }

  c = pBuffer[0x000]; /* check if boot or partition record */
  if (c == FS__DEV_MBRFLAG_JUMP1) {
    return 0; /* if first byte is a jump command, there is no partition table */
  } 

  if (c == FS__DEV_MBRFLAG_JUMP2) {
    return 0; /* if first byte is a jump command, there is no partition table */
  } /* this calculation makes sure big/little endian system get the right value from the table */
  /* read first entry of the partition table */
  StartSec  = (FS_u32)pBuffer[FS__DEV_PART_0_TABLE + 11] & 0xFFUL;
  StartSec <<= 8;
  StartSec += (FS_u32)pBuffer[FS__DEV_PART_0_TABLE + 10] & 0xFFUL;
  StartSec <<= 8;
  StartSec += (FS_u32)pBuffer[FS__DEV_PART_0_TABLE + 9] & 0xFFUL;
  StartSec <<= 8;
  StartSec += (FS_u32)pBuffer[FS__DEV_PART_0_TABLE + 8] & 0xFFUL; 

 return StartSec;
}

/*********************************************************************
*
*             _FS_MMC_DevStatus
*
  Description:
  FS driver function. Get status of the media.

  Parameters:
  Unit        - Unit number.
 
  Return value:
  ==1 (FS_LBL_MEDIACHANGED) - The media of the device has changed.
  ==0                       - Device okay and ready for operation.
  <0                        - An error has occured.
*/

static int _FS_MMC_DevStatus(FS_u32 Unit) {
	static int init = 0;
	int x;
	char a;
	FS_u32 ul;
	FS_MMC_HW_X_BusyLedOn(1);
	if (!init) {
		/* 
		The function is called the first time. For each unit,
		the flag for 'diskchange' is set. That makes sure, that
		FS_LBL_MEDIACHANGED is returned, if there is already a
		media in the reader.
		*/
		for (init = 0; init < FS_MMC_MAXUNIT; init++) {
			_FS_mmc_diskchange[init] = 1;
		}
		init = 1;
	}
	if (Unit >= FS_MMC_MAXUNIT) {
		return -1;  /* No valid unit number */
	}
	a = FS_MMC_HW_X_IsPresent(Unit);/* Check if a card is present */
	if (!a) {
	  return -1;  /* No card in reader */
	}
	/* When you get here, then there is a card in the reader */
	a = _FS_mmc_diskchange[Unit];  /* Check if the media has changed */
	if (a) {
		/* 
		A diskchange took place. The following code reads the MBR of the
		card to get its partition information.
		*/
		_FS_mmc_diskchange[Unit] = 0;  /* Reset 'diskchange' flag */
		x = SD_Initialize();
		if (x > 0) {
			printf("SD_Initialize error! CODE: 0x%02X\r\n", x);
			return -1;
		}
		x = SD_ReadBlock(0, 512, (FS_u8*)&_FS_mmc_mbrbuffer[0]);
		if (x > 0) {
			printf("SD_ReadBlock error! CODE: 0x%02X\r\n", x);
			return -1;
		}
		/* Calculate start sector of the first partition */
	    ul = FS_GetMediaStartSec((FS_u8*)&_FS_mmc_mbrbuffer[0]);
	    if (ul == 0xFFFFFFFF) { /* check if MBR was invalid */
	      return -1;
	    }
		_FS_mmc_logicalstart[Unit] = ul;
		return FS_LBL_MEDIACHANGED;
	}
	FS_MMC_HW_X_BusyLedOff(1);
	return 0;
}
/*********************************************************************
*
*             _FS_MMC_DevRead
*
  Description:
  FS driver function. Read a sector from the media.

  Parameters:
  Unit        - Unit number.
  Sector      - Sector to be read from the device.
  pBuffer     - Pointer to buffer for storing the data.
 
  Return value:
  ==0         - Sector has been read and copied to pBuffer.
  <0          - An error has occured.
*/

static int _FS_MMC_DevRead(FS_u32 Unit, FS_u32 Sector, void *pBuffer){
	int x;
	if (Unit >= FS_MMC_MAXUNIT) {
		return -1;  /* No valid unit number */
	}
	FS_MMC_HW_X_BusyLedOn(1);
	x = SD_ReadBlock(Sector+_FS_mmc_logicalstart[Unit], 512, (FS_u8*)pBuffer);
	if(x > 0) {
		printf("SD_ReadBlock error! CODE: 0x%02X\r\n", x);
		x = -1;
	}    
	else {
		x = 0;
	}
	FS_MMC_HW_X_BusyLedOff(1);
	return x;
}
/*********************************************************************
*
*             _FS_MMC_DevWrite
*
  Description:
  FS driver function. Write sector to the media.

  Parameters:
  Unit        - Unit number.
  Sector      - Sector to be written to the device.
  pBuffer     - Pointer to data to be stored.
 
  Return value:
  ==0         - Sector has been written to the device.
  <0          - An error has occured.
*/

static int _FS_MMC_DevWrite(FS_u32 Unit, FS_u32 Sector, void *pBuffer){
	
	int x;
	
	if (Unit >= FS_MMC_MAXUNIT) {
		return -1;  /* No valid unit number */
	}
	FS_MMC_HW_X_BusyLedOn(1);
	x = SD_WriteBlock(Sector+_FS_mmc_logicalstart[Unit], 512, (FS_u8*)pBuffer); 
	if (x > 0) {
		printf("SD_WriteBlock error! CODE: 0x%02X\r\n", x);
		x = -1;
	}
	else {
		x = 0;
	}
	FS_MMC_HW_X_BusyLedOff(1);
	return x;
}
/*********************************************************************
*
*             _FS_MMC_DevIoCtl
*
  Description:
  FS driver function. Execute device command.

  Parameters:
  Unit        - Unit number.
  Cmd         - Command to be executed.
  Aux         - Parameter depending on command.
  pBuffer     - Pointer to a buffer used for the command.
 
  Return value:
  Command specific. In general a negative value means an error.
*/

static int _FS_MMC_DevIoCtl(FS_u32 Unit, FS_i32 Cmd, FS_i32 Aux, void *pBuffer){
   FS_u32 *info, ul;
   int x;

   FS_MMC_HW_X_BusyLedOn(1);
   Aux = Aux;  /* Get rid of compiler warning */
   if (Unit >= FS_MMC_MAXUNIT) {
      return -1;  /* No valid unit number */
   }
   switch(Cmd){
     case FS_CMD_INC_BUSYCNT:
       _FS_mmc_busycnt[Unit]++;
       if(_FS_mmc_busycnt[Unit] > 0){
          FS_MMC_HW_X_BusyLedOn(Unit);
       }
       break;
     case FS_CMD_DEC_BUSYCNT:
       _FS_mmc_busycnt[Unit]--;
       if(_FS_mmc_busycnt[Unit] <= 0){
          _FS_mmc_busycnt[Unit] = 0;
          FS_MMC_HW_X_BusyLedOff(Unit);
       }
       break;
     case FS_CMD_CHK_DSKCHANGE:
       //a = FS_MMC_HW_X_DetectStatus(Unit);
       //if(a){
       //   _FS_mmc_diskchange[Unit] = 1;
       //}    
       break;
     case FS_CMD_GET_DEVINFO:
       if (!pBuffer) {
         return -1;
       }
       info = pBuffer;
	   x = SD_Initialize();
       if (x > 0) {
          return -1;
       }
	   x = SD_ReadBlock(0, 512, (FS_u8*)&_FS_mmc_mbrbuffer[0]);
       if (x > 0) {
          return -1;
       }
      ul = FS_GetMediaStartSec((FS_u8*)&_FS_mmc_mbrbuffer[0]);
      if (ul == 0xFFFFFFFF) { /* check if MBR was invalid */
        return -1;
      }
      if (ul != 0) {
        /* READ (1) 512 BYTE SECTOR */
		x = SD_ReadBlock(ul, 512, (FS_u8*)&_FS_mmc_mbrbuffer[0]);
		if (x > 0) {
		  return -1;
		}
      }
       /* hidden */
       *info = _FS_mmc_mbrbuffer[0x1c6];
       *info += (0x100UL * _FS_mmc_mbrbuffer[0x1c7]);
       *info += (0x10000UL * _FS_mmc_mbrbuffer[0x1c8]);
       *info += (0x1000000UL * _FS_mmc_mbrbuffer[0x1c9]);
       info++;
       /* head */
       *info = _FS_mmc_mbrbuffer[0x1c3]; 
       info++;
       /* sec per track */
       *info = _FS_mmc_mbrbuffer[0x1c4]; 
       info++;
       /* size */
       *info = _FS_mmc_mbrbuffer[0x1ca];
       *info += (0x100UL * _FS_mmc_mbrbuffer[0x1cb]);
       *info += (0x10000UL * _FS_mmc_mbrbuffer[0x1cc]);
       *info += (0x1000000UL * _FS_mmc_mbrbuffer[0x1cd]);
       break;
     default:
       break;
   }
   FS_MMC_HW_X_BusyLedOff(1);
   return 0; 
}
/*********************************************************************
*
*             Global variables
*
**********************************************************************
*/
const FS__device_type FS__mmcdevice_driver = {
  "MMC device",
  _FS_MMC_DevStatus,
  _FS_MMC_DevRead,
  _FS_MMC_DevWrite,
  _FS_MMC_DevIoCtl
};

#endif /* FS_USE_MMC_DRIVER */

/*************************** End of file ****************************/
