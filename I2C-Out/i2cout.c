/*
*********************************************************************************************************
*                                           I2COUT Interface
*
*
*
* Filename   : i2cout.c
* Version    : V1.00
* Programmer : Puleo
*********************************************************************************************************
*/

#define  I2COUT_GLOBALS
#include <includes.h>

#if I2COUT_MODULE == DEF_ENABLED

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/
#define ADDR_PCF8574  ((CPU_INT08U)0x40)

/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/

#if I2C_PARSE_TASK > 0
static  OS_STK     I2cout_TaskStk[I2C_TASK_STK_SIZE];

INT8U      LedGreen[4] = { 3, 2, 0, 1 };
INT8U      LedRed[3] = { 6, 5, 4 };


INT8U      prevLedShow, nextLedShow;
INT8U      *msg;
INT8U      err;
#endif


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

#if I2C_PARSE_TASK > 0
static  void    I2cout_InitOS(void);
static  void    I2cout_Task(void *p_arg);
#endif

/*
*********************************************************************************************************
*/
static void I2cout_LedReset(void)
{
    write_pcf8574(ADDR_PCF8574, 0x00);
    
    write_pcf8574(ADDR_PCF8574, 0x0F);
    OSTimeDlyHMSM(0, 0, 20, 0);
    
    write_pcf8574(ADDR_PCF8574, 0x70);
    OSTimeDlyHMSM(0, 0, 20, 0);
    
    write_pcf8574(ADDR_PCF8574, 0x00);

}

/*
*********************************************************************************************************
*/
static void LedShow(void)
{
  INT8U value, i;
  
  msg = (CPU_INT08U *)(OSMboxPend(RF868_Mbox, OS_TICKS_PER_SEC / 10, &err));
  
  if (err == OS_NO_ERR) {
      nextLedShow = (CPU_INT32U)msg;
  }
  
  if ( nextLedShow != prevLedShow)
  {
    prevLedShow = nextLedShow;
    value = 0;
    
    for( i=0; i<4; i++ ){
      if( (nextLedShow >> i) & 1)
      {
        value |= (0x01<<LedGreen[i]);                    // Setting a green bit
      }
    }  
    
    if( (nextLedShow & 0xF0) != 0){
      for( i=0; i<3; i++ )
      {
        if( (nextLedShow >> (i+4)) & 1)
        {
          value |= (0x01<<LedRed[i]);                      // Setting a red bit
        }
      }  
    }
    
    if (( value & 0x0F) > 0){
      value |= (0x01<<7);
    }else{
      value &= ~(0x01<<7);
    }
    
    write_pcf8574(ADDR_PCF8574, value);
  }
}
/*
*********************************************************************************************************
*                                       INITIALIZE I2COUT
*********************************************************************************************************
*/

void   I2cout_Init (void)
{

#if I2C_PARSE_TASK > 0
    I2cout_InitOS();
#endif

   
}

/*
*********************************************************************************************************
*                             INITIALIZE THE I2COUT TASK AND OS OBJECTS
*********************************************************************************************************
*/

#if I2C_PARSE_TASK > 0
static  void  I2cout_InitOS (void)
{
#if OS_TASK_NAME_SIZE > 0 || OS_EVENT_NAME_SIZE > 7
    INT8U  err;
#endif

    (void)OSTaskCreateExt(I2cout_Task,
                          (void *)0,                                  /* No arguments passed to Wifly_Task() */
                          &I2cout_TaskStk[I2C_TASK_STK_SIZE - 1],    /* Set Top-Of-Stack                     */
                          I2C_TASK_PRIO,                            /* Lowest priority level                */
                          I2C_TASK_ID,
                          &I2cout_TaskStk[0],                           /* Set Bottom-Of-Stack                  */
                          I2C_TASK_STK_SIZE,
                          (void *)0,                                   /* No TCB extension                     */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);  /* Enable stack checking + clear stack  */

#if OS_TASK_NAME_SIZE > 11
    OSTaskNameSet(I2C_TASK_PRIO, (INT8U *)"I2C Task", &err);
#endif
}
#endif

/*
*********************************************************************************************************
*                                           I2COUT TASK
*********************************************************************************************************
*/

#if I2C_PARSE_TASK > 0
static  void  I2cout_Task (void *p_arg)
{

  CPU_INT08U  *msg;
  CPU_INT08U   err;
  CPU_INT32U   nstate;
  CPU_INT32U   pstate;
  CPU_INT08U   value;

  (void)p_arg;
    
    prevLedShow = 0;
    nextLedShow = 0;
    
    nstate = 0;
    pstate = 0;
    value = 0;
      
    I2cout_LedReset();
    
    while (1) {
      
      msg = (CPU_INT08U *)(OSMboxPend(Direct_Sw_Mbox, OS_TICKS_PER_SEC / 10, &err));
      if (err == OS_NO_ERR) {
          nstate = (CPU_INT32U)msg;
      }

      if (nstate != pstate) {
          pstate  = nstate;
          
          if (nstate == 1) {
            value += 0x80;
            write_pcf8574(ADDR_PCF8574, value);
          } else {
            value -= 0x80;
            write_pcf8574(ADDR_PCF8574, value);
          }  
      }
      
      LedShow();
/*      
      msg = (CPU_INT08U *)(OSMboxPend(RF868_Mbox, OS_TICKS_PER_SEC / 10, &err));
      if (err == OS_NO_ERR) {
          nstate0 = (CPU_INT32U)msg;
      }
      
      if ( nstate0 != pstate0)
      {
        pstate0 = nstate0;
        
        for( i=0; i<4; i++ )
        {
          if( (nstate0 >> i) & 1)
          {
            value |= (0x01<<LedOn[i]);                    // Setting a bit
          }
        }  
        
        if (( value & 0x0F) > 0)
        {
          value |= (0x01<<7);
        }
      
        write_pcf8574(ADDR_PCF8574, value);
        
        value = 0;

      }
*/
    }      
}    

#endif



#endif
