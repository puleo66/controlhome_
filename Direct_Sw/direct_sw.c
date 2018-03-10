/*
*********************************************************************************************************
*                                           Direct Sw Interface
*
*
*
* Filename   : direct_sw.c
* Version    : V1.00
* Programmer : Puleo
*********************************************************************************************************
*/

#define  DIRECT_SW_GLOBALS
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

static  OS_STK     Direct_Sw_TaskStk[DIRECT_SW_TASK_STK_SIZE];

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void    Direct_Sw_InitOS(void);
static  void    Direct_Sw_Task(void *p_arg);


/*
*********************************************************************************************************
*                                       INITIALIZE DIRECT_SW
*********************************************************************************************************
*/

void   Direct_Sw_Init (void)
{
    Direct_Sw_InitOS();
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

static  void  Direct_Sw_EventCreate (void)
{
#if (OS_EVENT_NAME_SIZE > 12)
    CPU_INT08U  os_err;
#endif


    Direct_Sw_Mbox = OSMboxCreate((void *)0);                                   /* Create MBOX for communication between Direct switch and Led Out.*/
#if (OS_EVENT_NAME_SIZE > 12)
    OSEventNameSet(Direct_Sw_Mbox, "Direct switch to Led Out Mbox", &os_err);
#endif
}

/*
*********************************************************************************************************
*                             INITIALIZE THE DIRECT_SW TASK AND OS OBJECTS
*********************************************************************************************************
*/

static  void  Direct_Sw_InitOS (void)
{
#if OS_TASK_NAME_SIZE > 0 || OS_EVENT_NAME_SIZE > 7
    INT8U  err;
#endif

    (void)OSTaskCreateExt(Direct_Sw_Task,
                          (void *)0,                                          /* No arguments passed to Direct Sw Task() */
                          &Direct_Sw_TaskStk[DIRECT_SW_TASK_STK_SIZE - 1],    /* Set Top-Of-Stack                     */
                          DIRECT_SW_TASK_PRIO,                                /* Lowest priority level                */
                          DIRECT_SW_TASK_ID,
                          &Direct_Sw_TaskStk[0],                              /* Set Bottom-Of-Stack                  */
                          DIRECT_SW_TASK_STK_SIZE,
                          (void *)0,                                          /* No TCB extension                     */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);         /* Enable stack checking + clear stack  */

#if OS_TASK_NAME_SIZE > 11
    OSTaskNameSet(DIRECT_SW_TASK_PRIO, (INT8U *)"Direct Sw Task", &err);
#endif
    
    Direct_Sw_EventCreate ();
}

/*
*********************************************************************************************************
*                                           Direct switch TASK
*********************************************************************************************************
*/

static  void  Direct_Sw_Task (void *p_arg)
{
    
    CPU_BOOLEAN  b1_prev;
    CPU_BOOLEAN  b1;
    CPU_INT08U   key;


    (void)p_arg;

    b1_prev = DEF_FALSE;
    key     = 0;

    while (DEF_TRUE) {
        b1 = load_btn();

        if ((b1 == DEF_TRUE) && (b1_prev == DEF_FALSE)) {
            if (key == 1) {
                key = 0;
            } else {
                key++;
            }

            OSMboxPost(Direct_Sw_Mbox, (void *)key);
        }

        b1_prev = b1;

        OSTimeDlyHMSM(0, 0, 0, 20);
    }
}    

#endif

