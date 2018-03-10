/*
***********************************************************************************************
*                                           I2COUT Interface
*
*                            
*
* Filename   : i2cout.h
* Version    : V1.0
* Programmer : Puleo
***********************************************************************************************
*/


#ifdef  I2COUT_GLOBALS
#define  I2COUT_EXT
#else
#define  I2COUT_EXT  extern
#endif

#ifndef  I2COUT_H
#define  I2COUT_H

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
void   I2cout_Init (void);

/*
*********************************************************************************************************
*                                      BSP FUNCTION PROTOTYPES
*********************************************************************************************************
*/



#endif