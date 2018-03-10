/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2007; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                           MASTER INCLUDES
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   IAR STM32-SK Evaluation Board
*
* Filename      : includes.h
* Version       : V1.10
* Programmer(s) : BAN
*********************************************************************************************************
*/

#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__

#include  <stdio.h>
#include  <string.h>
#include  <ctype.h>
#include  <stdlib.h>
#include  <stdarg.h>

#include  <ucos_ii.h>

#include  <cpu.h>
#include  <lib_def.h>
#include  <lib_mem.h>
#include  <lib_str.h>
#include  <lib_ascii.h>

#include  <stm32f10x_conf.h>
#include  <stm32f10x_lib.h>

#include  <app_cfg.h>
#include  <bsp.h>

#if (RTC_MODULE == DEF_ENABLED)
 #include  <rtc.h>
#endif

#if (LCD_MODULE == DEF_ENABLED)
 #include  <bsp_lcd.h>
 #include  <lcd.h>
#endif

#if (FS_MODULE == DEF_ENABLED)
 #include <sddriver.h>
 #include <fs_api.h>
 #include <file_rutin.h> 
#endif

#if (I2COUT_MODULE == DEF_ENABLED)
 #include <bsp_i2ch.h>
 #include <i2cout.h>
#endif

#if (RFM69CW_MODULE == DEF_ENABLED)
 #include <bsp_tmr.h>
 #include <bsp_spi2.h>
 #include <bsp_reset_rfm69cw.h>
 #include <rf868.h>
 #include <rfm69cw.h>
 #include <fs20.h>
#endif

#if (WDTLED_MODULE == DEF_ENABLED)
 #include <direct_sw.h>
 #include <bsp_green_led.h>
 #include <wdtled.h>
#endif

#if (USBD_MODULE == DEF_ENABLED)
 #include "usbterm.h"
 #include "hw_config.h"
 #include "usb_regs.h"
 #include "usb_core.h"
 #include "usb_init.h"
 #include "usb_istr.h"
 #include "usb_pwr.h"
 #include "usb_mem.h"
 #include "usb_desc.h"
 #include "usb_prop.h"
 #include "usb_sil.h"
 #include "usb_int.h"
 #include "usb_def.h"
 #include "view.h"
 #include "terutil.h"
#endif


/* Boolean type */
// typedef enum { FALSE = 0, TRUE } BOOL;
#include <stdbool.h>
#ifndef FALSE
#define FALSE false
#define TRUE true
#endif

#endif
