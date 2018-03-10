/*
*********************************************************************************************************
*                                           WDT Led Interface
*
*
*
* Filename   : wdtled.c
* Version    : V1.00
* Programmer : Puleo
*********************************************************************************************************
*/

#define  WDTLED_GLOBALS
#include <includes.h>

#if WDTLED_MODULE == DEF_ENABLED

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/

#if WDTLED_PARSE_TASK > 0
static  OS_STK     WdtLed_TaskStk[WDTLED_TASK_STK_SIZE];

#endif


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#if WDTLED_PARSE_TASK > 0
static  void    WdtLed_InitOS(void);
static  void    WdtLed_Task(void *p_arg);
#endif

/*
*********************************************************************************************************
*                                       INITIALIZE WDTLed
*********************************************************************************************************
*/

void   WdtLed_Init (void)
{

#if WDTLED_PARSE_TASK > 0
    WdtLed_InitOS();
#endif

}

/*
*********************************************************************************************************
*                             INITIALIZE THE WDTLed TASK AND OS OBJECTS
*********************************************************************************************************
*/

#if WDTLED_PARSE_TASK > 0
static  void  WdtLed_InitOS (void)
{
#if OS_TASK_NAME_SIZE > 0 || OS_EVENT_NAME_SIZE > 7
    INT8U  err;
#endif

    (void)OSTaskCreateExt(WdtLed_Task,
                          (void *)0,                                    /* No arguments passed to Wifly_Task() */
                          &WdtLed_TaskStk[WDTLED_TASK_STK_SIZE - 1],    /* Set Top-Of-Stack                     */
                          WDTLED_TASK_PRIO,                             /* Lowest priority level                */
                          WDTLED_TASK_ID,
                          &WdtLed_TaskStk[0],                           /* Set Bottom-Of-Stack                  */
                          WDTLED_TASK_STK_SIZE,
                          (void *)0,                                    /* No TCB extension                     */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);   /* Enable stack checking + clear stack  */

#if OS_TASK_NAME_SIZE > 11
    OSTaskNameSet(WDTLED_TASK_PRIO, (INT8U *)"Wdt Led Task", &err);
#endif
}
#endif

/*
*********************************************************************************************************
*                                           WDTLed TASK
*********************************************************************************************************
*/

#if WDTLED_PARSE_TASK > 0
static  void  WdtLed_Task (void *p_arg)
{
  (void)p_arg;
    
    green_led_on();
    OSTimeDlyHMSM(0, 0, 10, 0);

    
    while (1) {
      green_led_off();
      OSTimeDlyHMSM(0, 0, 10, 0);
      
      green_led_on();
      OSTimeDlyHMSM(0, 0, 10, 0);
      
    }      
}    

#endif

#endif
