/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*
* Filename      : bsp_tmr.c
* Version       : V0.1
* Programmer(s) : Puleo
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_MODULE
#include <bsp.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
#define SYS_CLK10           72000000    /* 72 MHz */
#define SYS_CLK             50000000    /* 50 MHz */
#define DELAY_TIM_FREQUENCY 1000000     /* = 1MHZ -> timer runs in microseconds */

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
extern void  FS20_TimeISRHandler (void);

/*
*********************************************************************************************************
*                                              Us_TmrInit10()
*
* Description : Select & initialize a timer for use with uC/OS.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/
void  Us_TmrInit10 (void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    TIM_TimeBaseInitTypeDef  tim_init;
    TIM_TimeBaseStructInit(&tim_init);


    tim_init.TIM_Period        = 0xFFFF;
    tim_init.TIM_Prescaler     = (SYS_CLK10 / DELAY_TIM_FREQUENCY) - 1;
    tim_init.TIM_ClockDivision = 0;
    tim_init.TIM_CounterMode   = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM3, &tim_init);
    TIM_Cmd(TIM3, DISABLE);
   
}

/******************************************************************************
*                       Free_TmrISRHandler
*******************************************************************************/
static void Free_TmrISRHandler (void)
{
  CPU_SR          cpu_sr;

  CPU_CRITICAL_ENTER();                                       /* Tell uC/OS-II that we are starting an ISR            */
  OSIntNesting++;
  CPU_CRITICAL_EXIT();
  if(TIM_GetITStatus(TIM1,TIM_FLAG_Update) != RESET)
  {
    FS20_TimeISRHandler();
    TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update);
       
  }
  else{}
   
  OSIntExit();                                                /* Tell uC/OS-II that we are leaving the ISR            */
}

/*
*********************************************************************************************************
*                                              Free_TmrInit()
*
* Description : Select & initialize a timer for use with RF.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/
void  Free_TmrInit (void)
{
    TIM_TimeBaseInitTypeDef  tim_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE );


    tim_init.TIM_Period        = 5000;  
    tim_init.TIM_Prescaler     = 72;
    tim_init.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_init.TIM_CounterMode   = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM1, &tim_init);
    
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);        
    TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update); 
    TIM_ARRPreloadConfig(TIM1, ENABLE);        
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);    
    TIM_Cmd(TIM1, ENABLE);

    BSP_IntVectSet(BSP_INT_ID_TIM1_UP, Free_TmrISRHandler);
    BSP_IntPrioSet(BSP_INT_ID_TIM1_UP,3);  
    BSP_IntEn(BSP_INT_ID_TIM1_UP);
}

/*
*********************************************************************************************************
*                                         Free_TmrRd()
*
* Description : Read the current counts of a 32-bit free running timer.
*
* Argument(s) : none.
*
* Return(s)   : The 32 bit counts of the timer.
*********************************************************************************************************
*/
CPU_INT32U  Free_TmrRd (void)
{
    return ((CPU_INT32U)TIM_GetCounter(TIM1));
}

/*
*********************************************************************************************************
*                                         Free_TmrClr()
*
* Description : Clear the current counts of a 32-bit free running timer.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/
void Free_TmrClr (void)
{
  TIM_SetCounter(TIM1, 0);
}