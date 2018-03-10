/*
*********************************************************************************************************
*                                           RF868 Interface
*
*
*
* Filename   : rf868.c
* Version    : V1.00
* Programmer : Puleo
*********************************************************************************************************
*/

#define  RF868_GLOBALS
#include <includes.h>

#if RFM69CW_MODULE == DEF_ENABLED

/*
Read_Directory("mmc:0:\\");
Read_Directory("mmc:0:\\html");
Delete_File("mmc:0:\\FLASH1.TXT");
Del_Directory("mmc:0:\\SYSTEM");              
Write_File("mmc:0:\\index.html", html0);
Create_Directory("mmc:0:\\html");
Read_File("mmc:0:\\html\\index.html");
Write_File("mmc:0:\\html\\greet.html", html4);
Delete_File("mmc:0:\\html\\index.htm");

Delete_File("mmc:0:\\html\\index.htm");
Write_File("mmc:0:\\html\\index.html", html2);
Delete_File("mmc:0:\\html\\404.htm");
Write_File("mmc:0:\\html\\404.html", html3);
Delete_File("mmc:0:\\html\\greet.htm");
Write_File("mmc:0:\\html\\greet.html", html4);
Write_File("mmc:0:\\default\\default.DAT", "10\r\n\0");
*/

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

#if RF868_PARSE_TASK > 0
static  OS_STK     RF868_TaskStk[RF868_TASK_STK_SIZE];

int p;
INT16U memCount;
INT8U anz, nr;

#endif

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#if RF868_PARSE_TASK > 0
static  void    RF868_InitOS(void);
static  void    RF868_Task(void *p_arg);
OS_EVENT *RFMailbox;
#endif

/*
*********************************************************************************************************
*                                       INITIALIZE WIFLY
*********************************************************************************************************
*/

void   RF868_Init (void)
{
#if RF868_PARSE_TASK > 0
    RF868_InitOS();
#endif
}


/*
*********************************************************************************************************
*                                             Direct_Sw_EventCreate()
*
* Description : Create the application events.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  RF868_EventCreate (void)
{
#if (OS_EVENT_NAME_SIZE > 12)
    CPU_INT08U  os_err;
#endif


    RF868_Mbox = OSMboxCreate((void *)0);                                   /* Create MBOX for communication between Rf868 and Led Out.*/
#if (OS_EVENT_NAME_SIZE > 12)
    OSEventNameSet(RF868_Mbox, "Direct rf868 to Led Out Mbox", &os_err);
#endif
    

    RF868Log_Mbox = OSMboxCreate((void *)0);                                 /* Create MBOX for communication between Rf868 and App Log.*/
#if (OS_EVENT_NAME_SIZE > 12)
    OSEventNameSet(RF868Log_Mbox, "Direct rf868 to App Log Mbox", &os_err);
#endif
}

/*
*********************************************************************************************************
*                             INITIALIZE THE WIFLY TASK AND OS OBJECTS
*********************************************************************************************************
*/

#if RF868_PARSE_TASK > 0
static  void  RF868_InitOS (void)
{
#if OS_TASK_NAME_SIZE > 0 || OS_EVENT_NAME_SIZE > 7
    INT8U  err;
#endif

    (void)OSTaskCreateExt(RF868_Task,
                          (void *)0,                                  /* No arguments passed to Wifly_Task() */
                          &RF868_TaskStk[RF868_TASK_STK_SIZE - 1],    /* Set Top-Of-Stack                     */
                          RF868_TASK_PRIO,                            /* Lowest priority level                */
                          RF868_TASK_ID,
                          &RF868_TaskStk[0],                           /* Set Bottom-Of-Stack                  */
                          RF868_TASK_STK_SIZE,
                          (void *)0,                                   /* No TCB extension                     */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  /* Enable stack checking + clear stack  */

#if OS_TASK_NAME_SIZE > 11
    OSTaskNameSet(RF868_TASK_PRIO, (INT8U *)"RF868 Task", &err);
#endif
    
    RF868_EventCreate();
}

#endif

/*
*********************************************************************************************************
*                                           FHT TASK
*********************************************************************************************************
*/

#if RF868_PARSE_TASK > 0
static  void  RF868_Task (void *p_arg)
{
  //INT8U prewState;
  
 
    (void)p_arg;
    
    FHTLedVal = 0x00;
    //prewState = 0x00;
    logMsg[0] = '\0';
    
    InitStructs();    
    
    RFM69CW_ExtiEnabled();
   
    Free_TmrInit();
    Us_TmrInit10();
    
    do{
        RFM69CW_ReadAllRegs();            ///  Teszteléshez kell!!!!
        //OSTimeDly(100);
    }while(!( regVal[0x00] == 0x00 && regVal[0x01] == 0x04 ));
    
    RFM69CW_RcCalibitrateation();

    RFM69CW_InitializeOOK();
    //RFM69CW_ReadAllRegs();            ///  Teszteléshez kell!!!!
            
    
    
    OSTimeDlyHMSM(0, 0, 1, 0);

    BSP_IntEn(BSP_INT_ID_EXTI9_5);

    
    while (1) 
    {
      ReceiverScan();
      
      //if (FHTLedVal != prewState)
      //{
        OSMboxPost(RF868_Mbox, (void *)FHTLedVal);
        //prewState = FHTLedVal;
        
#if (LOG_MODULE == DEF_ENABLED)
        
        Get_Time_Now();
      
        if (time_now.tm_year != 1970){

          OSMboxPost(RF868Log_Mbox, (void *)logMsg);
        }
#endif        
      //}

     

      OSTimeDlyHMSM(0, 0, 0, 1);
    }  
}    

#endif

/*
******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 10-15 interrupt request.
* Input          : None
* Output         : None
* Return         : None
* Attention     : None
******************************************************************************
*/

void RF_ISRHandler(void)
{

  CPU_SR          cpu_sr;


  CPU_CRITICAL_ENTER();                                       /* Tell uC/OS-II that we are starting an ISR            */
  OSIntNesting++;
  CPU_CRITICAL_EXIT();
  
  if(EXTI_GetITStatus(EXTI_Line6) != RESET)
  {
     FS20_ISRHandler();   
     EXTI_ClearITPendingBit(EXTI_Line6);
  }
  
  OSIntExit();                                                /* Tell uC/OS-II that we are leaving the ISR            */

}

#endif