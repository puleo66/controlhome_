/*
*********************************************************************************************************
*
*                                        Direct Button Modul
*
* Filename      : bsp_direct_btn.h
* Version       : V1.00
* Programmer(s) : Puleo
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_DIRECT_BTN_GLOBALS
#define  BSP_DIRECT_BTN_EXT
#else
#define  BSP_DIRECT_BTN_EXT  extern
#endif

#ifndef  BSP_DIRECT_BTN_H
#define  BSP_DIRECT_BTN_H
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
*                                              DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/

BSP_DIRECT_BTN_EXT void direct_btn_init();
BSP_DIRECT_BTN_EXT CPU_BOOLEAN load_btn();

/*
*********************************************************************************************************
*                                              MODULE END
*********************************************************************************************************
*/


#endif