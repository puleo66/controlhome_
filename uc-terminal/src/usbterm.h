/*
***********************************************************************************************
*                                           USBTerminal Interface
*
*                            
*
* Filename   : usbterm.h
* Version    : V1.0
* Programmer : Puleo
***********************************************************************************************
*/


#ifdef  USBTERM_GLOBALS
#define  USBTERM_EXT
#else
#define  USBTERM_EXT  extern
#endif

#ifndef  USBTERM_H
#define  USBTERM_H

/*
***********************************************************************************************
*                                         CONSTANS
***********************************************************************************************
*/


/*
***********************************************************************************************
*                                         VARIABLES
***********************************************************************************************
*/


/*
***********************************************************************************************
*                                        PROTOTYPES
*                                    (PORT INDEPENDENT)
***********************************************************************************************
*/


/*
*********************************************************************************************************
*                                      FUNCTION PROTOTYPES
*********************************************************************************************************
*/

USBTERM_EXT     void   USBTerm_Init (void);



#endif