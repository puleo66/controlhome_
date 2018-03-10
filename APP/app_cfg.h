/*
*********************************************************************************************************
*
*                                      APPLICATION CONFIGURATION
*
*                                     
*
* Filename      : app_cfg.h
* Version       : V0.1
* Programmer(s) : Puleo
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/
//#define  RTC_MODULE                  DEF_DISABLED	     	/* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */
#define  RTC_MODULE                  DEF_ENABLED	     	/* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */
#define  RFM69CW_MODULE              DEF_ENABLED                /* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */
#define  FS_MODULE                   DEF_ENABLED	     	/* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */
//#define  FS_MODULE                   DEF_DISABLED	     	/* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */
#define  LCD_MODULE                  DEF_DISABLED	     	/* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */


#define  I2COUT_MODULE               DEF_ENABLED                /* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */
//#define  I2COUT_MODULE               DEF_DISABLED                /* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */
#define  WDTLED_MODULE               DEF_ENABLED                /* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */
//#define  WDTLED_MODULE               DEF_DISABLED                /* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */
#define  USBD_MODULE                 DEF_ENABLED                /* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */
//#define  USBD_MODULE                 DEF_DISABLED                /* DEF_ENABLED = Present, DEF_DISABLED = Not Present        */

#define  LOG_MODULE                  DEF_DISABLED
/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_TASK_START_PRIO                               3

#define  I2C_TASK_PRIO                                     4
#define  RF868_TASK_PRIO                                   5
#define  DIRECT_SW_TASK_PRIO                               6
#define  WDTLED_TASK_PRIO                                  7
#define  USBTERM_TASK_PRIO                                 8


#define  DIRECT_SW_TASK_ID                                 9
#define  I2C_TASK_ID 	                                  10
#define  WDTLED_TASK_ID	                                  11
#define  RF868_TASK_ID                                    12
#define  USBTERM_TASK_ID                                  13

#define  OS_TASK_TMR_PRIO                (OS_LOWEST_PRIO - 2)


/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE                         128

#define  I2C_TASK_STK_SIZE                                64
#define  DIRECT_SW_TASK_STK_SIZE                          64
#define  WDTLED_TASK_STK_SIZE                             32
#define  RF868_TASK_STK_SIZE                              64
#define  USBTERM_TASK_STK_SIZE                           128


/*
*********************************************************************************************************
*                                         MODULS CONFIGURATION
*********************************************************************************************************
*/
#define  I2C_PARSE_TASK                              DEF_ENABLED           	/* Parsing of received packets will be done by a task   */
#define  RF868_PARSE_TASK                            DEF_ENABLED           	/* Parsing of received packets will be done by a task   */
#define  WDTLED_PARSE_TASK                           DEF_ENABLED           	/* Parsing of received packets will be done by a task   */
#define  USBTERM_PARSE_TASK                          DEF_ENABLED           	/* Parsing of received packets will be done by a task   */

/*
*********************************************************************************************************
*                                                  LIB
*********************************************************************************************************
*/

#define  uC_CFG_OPTIMIZE_ASM_EN                 DEF_ENABLED
#define  LIB_STR_CFG_FP_EN                      DEF_DISABLED

/*
*********************************************************************************************************
*                                       BSP CONFIGURATION
*********************************************************************************************************
*/



#endif
