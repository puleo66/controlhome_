/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*
* Filename      : bsp.c
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
#include <rtc.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
#define SYS_CLK10           72000000    /* 72 MHz */
#define SYS_CLK             50000000    /* 50 MHz */
//#define SYS_CLK10           10000000    /* 10 MHz */
#define DELAY_TIM_FREQUENCY 1000000     /* = 1MHZ -> timer runs in microseconds */

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*********************************************************************************************************
*/

void  BSP_Init (void)
{
    
    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);
    RCC_WaitForHSEStartUp();


    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK2Config(RCC_HCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    FLASH_SetLatency(FLASH_Latency_2);
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    RCC_PLLCmd(ENABLE);

    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {
        ;
    }

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    while (RCC_GetSYSCLKSource() != 0x08) {
        ;
    }
       
    #if (RTC_MODULE == DEF_ENABLED)
      RTC_Config();
      RTC_IntEn();
    #endif
    
    #if (LCD_MODULE == DEF_ENABLED)
      BSP_LCD_Init();
    #endif
       
    #if (I2COUT_MODULE == DEF_ENABLED)
      i2c_init();
    #endif
      
    #if (RFM69CW_MODULE == DEF_ENABLED)
      SPI2_HardWareInit();
      Reset_RFM69CW_Init();
    #endif
    
    #if (WDTLED_MODULE == DEF_ENABLED)
      green_led_init();
      direct_btn_init();
    #endif
}

/*
*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Read CPU registers to determine the CPU clock frequency of the chip.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    RCC_ClocksTypeDef  rcc_clocks;


    RCC_GetClocksFreq(&rcc_clocks);

    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                                         OS CORTEX-M3 FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         OS_CPU_SysTickClkFreq()
*
* Description : Get system tick clock frequency.
*
* Argument(s) : none.
*
* Return(s)   : Clock frequency (of system tick).
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

INT32U  OS_CPU_SysTickClkFreq (void)
{
    INT32U  freq;


    freq = BSP_CPU_ClkFreq();
    return (freq);
}

/*
*********************************************************************************************************
*                                              Us_TmrInit()
*
* Description : Select & initialize a timer for use with uC/OS.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/
void  Us_TmrInitIO (void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    TIM_TimeBaseInitTypeDef  tim_init;
    TIM_TimeBaseStructInit(&tim_init);


    tim_init.TIM_Period        = 0xFFFF;
    tim_init.TIM_Prescaler     = (SYS_CLK / DELAY_TIM_FREQUENCY) - 1;
    tim_init.TIM_ClockDivision = 0;
    tim_init.TIM_CounterMode   = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &tim_init);
    TIM_Cmd(TIM2, DISABLE);
   
}

/*
*********************************************************************************************************
*                                              Us_Delay(INT32U nCount)
*
* Description : Select & initialize a timer for use with uC/OS.
*
* Argument(s) : INT32U nCount
*
* Return(s)   : none.
*********************************************************************************************************
*/
void Us_DelayIO(INT16U nCount)
{
  TIM_SetCounter(TIM2, 0);
  TIM_Cmd(TIM2, ENABLE);
  while (TIM_GetCounter(TIM2) <= nCount);
  TIM_Cmd(TIM2, DISABLE);
}

/*
*********************************************************************************************************
*                                              Ms_Delay(INT32U nCount)
*
* Description : Select & initialize a timer for use with uC/OS.
*
* Argument(s) : INT32U nCount
*
* Return(s)   : none.
*********************************************************************************************************
*/
void Ms_DelayIO( INT16U ms )
{
  int i;
  for (i=0; i < ms; i++){
    Us_DelayIO(1000);
  }
}

/*
*********************************************************************************************************
*                                              Us_Delay10(INT32U nCount)
*
* Description : Select & initialize a timer for use with uC/OS.
*
* Argument(s) : INT32U nCount
*
* Return(s)   : none.
*********************************************************************************************************
*/
void Us_Delay10(INT16U nCount)
{
  TIM_SetCounter(TIM3, 0);
  TIM_Cmd(TIM3, ENABLE);
  while (TIM_GetCounter(TIM3) <= nCount);
  TIM_Cmd(TIM3, DISABLE);
}
