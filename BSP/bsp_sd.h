/*
*********************************************************************************************************
*

*                                       SD/MMC (SPI2) INTERFACE
*
* Filename      : bsp_sd.h
* Version       : V1.00
* Programmer(s) : Puleo
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MODULE
*
* Note(s) : (1) This BSP serial header file is protected from multiple pre-processor inclusion through
*               use of the BSP serial present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  BSP_SD_PRESENT                                        /* See Note #1.                                         */
#define  BSP_SD_PRESENT


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_SD_MODULE
#define  BSP_SD_EXT
#else
#define  BSP_SD  extern
#endif

#include  <app_cfg.h>

/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                   BACKWARD COMPATIBILITY MACRO'S
*
* Note(s) : (1) Those macro's are provided to preserve backward compatibility with earlier versions of
*               this BSP.
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                              DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
/* check weather card is insert entirely */
INT8U SD_ChkCard(void);

/* check weather card is write protect */
INT8U SD_ChkCardWP(void);

/* initialize the hardware that access sd card */
void SD_HardWareInit(void);

/* set the clock of SPI less than 400kHZ */
void SPI_Clk400k(void);

/* -ÀÍ+SPIÁ¦clockÁ¯Îþ+¡ÍÁ set the clock of SPI to maximum */
void SPI_ClkToMax(void);

/* send a byte by SPI interface */
INT8U SPI_SendByte(INT8U byte);

/*  receive a byte from SPI interface */
INT8U SPI_RecByte(void);

/* select the SPI slave */
void SPI_CS_Assert(void);

/* not select the SPI slave */
void SPI_CS_Deassert(void);

/* select the SPI slave */
void RF_CS_Assert(void);

/* not select the SPI slave */
void RF_CS_Deassert(void);

/* Get RF IRQ           */
BOOLEAN RF_IsReady(void);

void RF_ISRHandler(void);
void ADC1_ISRHandler(void);
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             MODULE END
*
* Note(s) : (1) See 'bsp_sd.h  MODULE'.
*********************************************************************************************************
*/

#endif                                                          /* End of BSP sd module include.                       */
