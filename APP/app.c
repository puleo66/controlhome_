/*
*********************************************************************************************************
*
*                                            Main CODE
*
*                                     
*
* Filename      : app.c
* Version       : V1.0
* Programmer(s) : Puleo
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>
#include <controlHome.h>
 
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_STK          App_TaskStartStk[APP_TASK_START_STK_SIZE];
INT8U *logMsgAct;
/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  App_TaskStart              (void        *p_arg);   

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

int  main (void)
{
    CPU_INT08U  os_err;


    BSP_IntDisAll();                                            /* Disable all ints until we are ready to accept them.  */

    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel".         */
    
#if (FS_MODULE == DEF_ENABLED)
    FS_Init();                                                  /* Init the file system                                 */
#endif


    os_err = OSTaskCreateExt((void (*)(void *)) App_TaskStart,  /* Create the start task.                               */
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_START_PRIO,
                             (INT16U          ) APP_TASK_START_PRIO,
                             (OS_STK        * )&App_TaskStartStk[0],
                             (INT32U          ) APP_TASK_START_STK_SIZE,
                             (void          * )0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));

#if (OS_TASK_NAME_SIZE >= 11)
    OSTaskNameSet(APP_TASK_START_PRIO, (CPU_INT08U *)"Start Task", &os_err);
#endif

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II).  */
    
#if (FS_MODULE == DEF_ENABLED)
    FS_Exit();                                                  /* End using the file system                            */
#endif

    return (0);
}

/*
*********************************************************************************************************
*                                          App_TaskStart()
*
* Description : The startup task.  The uC/OS-II ticker should only be initialize once multitasking starts.
*
* Argument(s) : p_arg       Argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_TaskStart (void *p_arg)
{
#if(LOG_MODULE == DEF_ENABLED)
  INT8U err;
#endif
  
  (void)p_arg;

     OS_CPU_SysTickInit();                                       /* Initialize the SysTick.                              */
    
     BSP_Init();                                                 /* Initialize BSP functions.                            */
     
#if (RFM69CW_MODULE == DEF_ENABLED)
       RF868_Init ();                                            /* Initialize RFM69CW functions.                        */
#endif  
     
#if (FS_MODULE == DEF_ENABLED)  
                              
    if (Read_File("mmc:0:\\default\\FHTACC~1.DAT") == 0)
      FHTDevLoad();
    
    if (Read_File("mmc:0:\\default\\TFACCESS.DAT") == 0);
      TFDevLoad();
      
    if (Read_File("mmc:0:\\default\\default.DAT") == 0)
      PaarLoad();
    
#endif

#if (LCD_MODULE == DEF_ENABLED)
       LcdInit();
#endif
     
#if (I2COUT_MODULE == DEF_ENABLED)
       I2cout_Init();                                            /* Initialize I2C functions.                            */
#endif
     
#if (WDTLED_MODULE == DEF_ENABLED)
       Direct_Sw_Init();                                         /* Initialize User switch functions.                    */
       WdtLed_Init();                                            /* Initialize Wdt Led functions.                        */
#endif
       
#if (USBD_MODULE == DEF_ENABLED)
       USBTerm_Init();
#endif

     
       
       
#if (LCD_MODULE == DEF_ENABLED)
       
       //display image
        LcdContrast(0x7F);
        OSTimeDlyHMSM(0, 0, 1, 0);
        LcdContrast(0x00);
        OSTimeDlyHMSM(0, 0, 1, 0); 
        LcdContrast(0x40);
        LcdClear();
        LcdImage(controlHome);
        LcdGotoXYFont(11,5);
        LcdStr(FONT_1X,"V1.2   ControlHome");
        OSTimeDlyHMSM(0, 1, 0, 0);
        LcdClear();
#endif
        
     
    while (DEF_TRUE) {                                           /* Task body, always written as an infinite loop.       */

      
#if (LOG_MODULE == DEF_ENABLED)
      
      void *pmsg = (INT8U*)OSMboxPend(RF868Log_Mbox, 0, &err);

      if ((pmsg != NULL)&&(err == OS_NO_ERR)){
        Log_File((INT8U*)pmsg);
        pmsg = '\0';
      }
#endif
      
      OSTimeDlyHMSM(0, 0, 4, 0);
    }
}
