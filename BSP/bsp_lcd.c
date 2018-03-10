/*
*********************************************************************************************************
*
*                                       LCD INTERFACE
*
* Filename      : bsp_lcd.c
* Version       : V1.00
* Programmer(s) : Puleo
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_LCD_MODULE
#include <bsp.h>
#include <bsp_lcd.h>
#include <lcd.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  Lcd_SCE(CPU_BOOLEAN Conn);  // 0: Reset; 1: Set;
static  void  Lcd_DC(CPU_BOOLEAN Conn);
static  void  Lcd_RST(CPU_BOOLEAN Conn);
static  void  Lcd_SCLK(CPU_BOOLEAN Conn);
static  void  Lcd_SDIN(CPU_BOOLEAN Conn);

static  void  DispDly_uS (CPU_INT32U us);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
**                                         GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           BSP_LCD_Init()
*
* Description : Initialize the LCD.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LCD_Init(void)
{
    GPIO_InitTypeDef  gpio_init;
   
    RCC_APB2PeriphClockCmd( LCD_CTL | LCD_RESET_CTL, ENABLE );
	  
    gpio_init.GPIO_Pin = LCD_SDIN | LCD_SCLK  ;
    gpio_init.GPIO_Mode =  GPIO_Mode_Out_PP ; 	
    gpio_init.GPIO_Speed = LCD_Speed ;
    GPIO_Init( LCD_CTL_Pins , &gpio_init );
    GPIO_SetBits( LCD_CTL_Pins ,  LCD_SCLK | LCD_SDIN  );

    gpio_init.GPIO_Pin = LCD_RST | LCD_DC | LCD_SCE ;
    gpio_init.GPIO_Mode =  GPIO_Mode_Out_PP ; 	
    gpio_init.GPIO_Speed = LCD_Speed ;
    GPIO_Init( LCD_RESET_CTL_Pins , &gpio_init );
    GPIO_SetBits( LCD_RESET_CTL_Pins ,   LCD_RST | LCD_DC | LCD_SCE );
    
    Lcd_RST(1);
    DispDly_uS(1000);
    Lcd_RST(0);
    
    Lcd_SCE(0);
    
    DispInitOS();                      /* Initialize the RTOS services                                 */
}

/*
*********************************************************************************************************
*                                            Lcd_SCE(Boolean Conn)
*
* Description : LCD port enabled/Disabled.
*
* Argument(s) : 0: Reset; 1: Set;
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static void Lcd_SCE(CPU_BOOLEAN Conn) 
{
    GPIO_WriteBit(LCD_RESET_CTL_Pins,LCD_SCE,((Conn)?Bit_RESET:Bit_SET));
}

/*
*********************************************************************************************************
*                                           Lcd_DC(Boolean Conn)
*
* Description : LCD DC port reset/set.
*
* Argument(s) : 0: Reset; 1: Set;
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static void Lcd_DC(CPU_BOOLEAN Conn)
{
    GPIO_WriteBit(LCD_RESET_CTL_Pins,LCD_DC,((Conn)?Bit_RESET:Bit_SET));
}

/*
*********************************************************************************************************
*                                           Lcd_Rst(Boolean Conn)
*
* Description : LCD reset port reset/set.
*
* Argument(s) : 0: Reset; 1: Set;
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static void Lcd_RST(CPU_BOOLEAN Conn)
{
    GPIO_WriteBit(LCD_RESET_CTL_Pins,LCD_RST,((Conn)?Bit_RESET:Bit_SET));
}

/*
*********************************************************************************************************
*                                           Lcd_SCLK(Boolean Conn)
*
* Description : LCD Clk port reset/set.
*
* Argument(s) : 0: Reset; 1: Set;
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static void Lcd_SCLK(CPU_BOOLEAN Conn)
{
    GPIO_WriteBit(LCD_CTL_Pins,LCD_SCLK,((Conn)?Bit_RESET:Bit_SET));
}

/*
*********************************************************************************************************
*                                           Lcd_SDIN(Boolean Conn)
*
* Description : LCD Data port reset/set.
*
* Argument(s) : 0: Reset; 1: Set;
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static void Lcd_SDIN(CPU_BOOLEAN Conn)
{
    GPIO_WriteBit(LCD_CTL_Pins,LCD_SDIN,((Conn)?Bit_RESET:Bit_SET));
}

/*
*********************************************************************************************************
*                                           LcdSend(byte data, LcdCmdData cd)
*
* Description : Sends data to display controller.
*
* Argument(s) : data -> Data to be sent
*               cd   -> Command or data (see enum in pcd8544.h)
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void LcdSend ( INT8U data, LcdCmdData cd )
{
    /*  Enable display controller (active low). */

    Lcd_SCE(1);

    if ( cd == LCD_DATA )
     Lcd_DC(0);
    else
     Lcd_DC(1);

    /*  Send data to display controller. */
    Lcd_SCLK(1); 
    DispDly_uS(10);
    for(int i=0;i<8;i++){ 
      if((data&0x80)==0x80)
        Lcd_SDIN(0);
      else 
         Lcd_SDIN(1);
      
      DispDly_uS(10);
      Lcd_SCLK(0);
      DispDly_uS(10);
      Lcd_SCLK(1); 
      data<<=1; 
   } 

    /* Disable display controller. */
   Lcd_SCE(1);
   
}

/*
*********************************************************************************************************
*                                              DispDly_uS()
*
* Description : Delay for the specified number of microseconds.
*
* Argument(s) : us          Number of microseconds
*
* Return(s)   : none.
*
* Caller(s)   : uC/LCD
*
* Notes       : none.
*********************************************************************************************************
*/

static void  DispDly_uS (CPU_INT32U us)
{
    CPU_INT32U  us_per_tick;
    CPU_INT32U  ticks;


    us_per_tick = BSP_CPU_ClkFreq() / OS_TICKS_PER_SEC;
    ticks       = us / us_per_tick + 2;
    OSTimeDly(ticks);
}
