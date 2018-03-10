/*
*********************************************************************************************************
*                                           USBTerminal Interface
*
*
*
* Filename   : usbterm.c
* Version    : V1.00
* Programmer : Puleo
*********************************************************************************************************
*/

#define  USBTERM_GLOBALS
#include <includes.h>

#if USBD_MODULE == DEF_ENABLED

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

#if USBTERM_PARSE_TASK > 0
 OS_STK     USBTerm_TaskStk[USBTERM_TASK_STK_SIZE];
#endif

extern INT8U ucData; 
//extern CPU_CHAR str[60];

//INT32U  timeos;
//void   View_TerminalRxSetCallback(void (*call_back)(INT8U data));
/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#if USBTERM_PARSE_TASK > 0
void    USBTerm_InitOS(void);
void    USBTerm_Task(void *p_arg);
#endif

/*
*********************************************************************************************************
*                                       INITIALIZE THERMO
*********************************************************************************************************
*/

void   USBTerm_Init (void)
{

#if USBTERM_PARSE_TASK > 0
    USBTerm_InitOS();
#endif

}

/*
*********************************************************************************************************
*                             INITIALIZE THE THERMO TASK AND OS OBJECTS
*********************************************************************************************************
*/

#if USBTERM_PARSE_TASK > 0
static  void  USBTerm_InitOS (void)
{
#if OS_TASK_NAME_SIZE > 0 || OS_EVENT_NAME_SIZE > 7
    INT8U  err;
#endif

    (void)OSTaskCreateExt(USBTerm_Task,
                          (void *)0,                                  /* No arguments passed to Wifly_Task() */
                          &USBTerm_TaskStk[USBTERM_TASK_STK_SIZE - 1],    /* Set Top-Of-Stack                     */
                          USBTERM_TASK_PRIO,                            /* Lowest priority level                */
                          USBTERM_TASK_ID,
                          &USBTerm_TaskStk[0],                           /* Set Bottom-Of-Stack                  */
                          USBTERM_TASK_STK_SIZE,
                          (void *)0,                                   /* No TCB extension                     */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  /* Enable stack checking + clear stack  */

#if OS_TASK_NAME_SIZE > 11
    OSTaskNameSet(USBTERM_TASK_PRIO, (INT8U *)"USBTerm Task", &err);
#endif
}
#endif

/*
*********************************************************************************************************
*                                           THERMO TASK
*********************************************************************************************************
*/

#if USBTERM_PARSE_TASK > 0
void  USBTerm_Task (void *p_arg)
{

    (void)p_arg;
    
    Set_USBSystem();
    
      
    while (1) {
      if (usb_GetRxByte() == TRUE){
        View_RxHandler (ucData);
      }

      OSTimeDlyHMSM(0, 0, 0, 100);
    }  
}    

#endif

#endif
