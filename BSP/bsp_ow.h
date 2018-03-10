/*
*********************************************************************************************************
*

*                                       ONEWIRE INTERFACE
*
* Filename      : bsp_ow.h
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

#ifndef  BSP_OW_PRESENT                                        /* See Note #1.                                         */
#define  BSP_OW_PRESENT


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_OW_MODULE
#define  BSP_OW_EXT
#else
#define  BSP_OW  extern
#endif

#include  <app_cfg.h>

/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/
#define OW_CONF_DELAYOFFSET       0

// Recovery time (T_Rec) minimum 1usec - increase for long lines 
// 5 usecs is a value give in some Maxim AppNotes
// 30u secs seem to be reliable for longer lines
//#define OW_RECOVERY_TIME        5  /* usec */
//#define OW_RECOVERY_TIME      300 /* usec */
#define OW_RECOVERY_TIME          27 /* usec */

// Use STM32's internal pull-up resistor instead of external 2,3k (3.3V)resistor.
// Based on information from Sascha Schade. Experimental but worked in tests
// with one DS18B20 and one DS18S20 on a rather short bus (60cm), where both 
// sensores have been parasite-powered.
#define OW_USE_INTERNAL_PULLUP     0  /* 0=external, 1=internal */

/*******************************************/

#define OW_READ_ROM                0x33
#define OW_MATCH_ROM               0x55
#define OW_SKIP_ROM                0xCC
#define OW_SEARCH_ROM              0xF0

#define OW_SEARCH_FIRST            0xFF        // start new search
#define OW_PRESENCE_ERR            0xFF
#define OW_DATA_ERR                0xFE
#define OW_LAST_DEVICE             0x00        // last device found

// rom-code size including CRC
#define OW_ROMCODE_SIZE            8

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
INT8U ow_reset(void);
INT8U ow_rom_search( INT8U diff, INT8U *id );
INT8U ow_input_pin_state(void);
INT8U ow_rom_get( void );
INT8U OneWire_ReadByte(void);
INT8U OneWire_ReadBit(void);

void  Ow_init(void);
void  OneWire_WriteByte(INT8U data);
void  ow_command( INT8U command, INT8U *id );

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
