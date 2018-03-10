/*
*********************************************************************************************************
*
*                                       SERIAL (Uart2) INTERFACE
*
* Filename      : bsp_ser.c
* Version       : V1.00
* Programmer(s) : Puleo
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_SER_MODULE
#include <bsp.h>
#include <bsp_ser.h>

#if (WIFLY_MODULE == DEF_ENABLED)

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
#define  BSP_GPIOA_UART2_TX                       DEF_BIT_02
#define  BSP_GPIOA_UART2_RX                       DEF_BIT_03

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
extern void  Wifly_RxHandler (INT8U rx_data);
extern void  Wifly_TxHandler (void);
/*
*********************************************************************************************************
*                                Rx/Tx Communication handler for Wifly
*
* Description: This function is NOT called because the M16C has a separate Rx and Tx ISR.
*********************************************************************************************************
*/
static void  Wifly_RxTxISRHandler (void)
{

    USART_TypeDef  *usart;
    CPU_INT08U      rx_data;
    CPU_SR          cpu_sr;


    CPU_CRITICAL_ENTER();                                       /* Tell uC/OS-II that we are starting an ISR            */
    OSIntNesting++;
    CPU_CRITICAL_EXIT();

    usart = USART2;
    if (USART_GetITStatus(usart, USART_IT_RXNE) != RESET) {
        rx_data = USART_ReceiveData(usart) & 0xFF;              /* Read one byte from the receive data register         */
        Wifly_RxHandler(rx_data);

        USART_ClearITPendingBit(usart, USART_IT_RXNE);          /* Clear the USART2 Receive interrupt                   */
    }

    if (USART_GetITStatus(usart, USART_IT_TXE) != RESET) {
        Wifly_TxHandler();

        USART_ClearITPendingBit(usart, USART_IT_TXE);           /* Clear the USART2 transmit interrupt                  */
    }

    OSIntExit();                                                /* Tell uC/OS-II that we are leaving the ISR            */
}

/*
*********************************************************************************************************
*                                            Wifly_InitTarget()
*
* Description : Initialize the SER.
*
* Argument(s) : baudrate
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/
void  Wifly_InitTarget(INT32U baud_rate)
{
    GPIO_InitTypeDef        gpio_init;
    USART_InitTypeDef       usart_init;
    USART_ClockInitTypeDef  usart_clk_init;

                                                                /* ----------------- INIT USART STRUCT ---------------- */
    usart_init.USART_BaudRate            = baud_rate;
    usart_init.USART_WordLength          = USART_WordLength_8b;
    usart_init.USART_StopBits            = USART_StopBits_1;
    usart_init.USART_Parity              = USART_Parity_No ;
    usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_init.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;


    usart_clk_init.USART_Clock           = USART_Clock_Disable;
    usart_clk_init.USART_CPOL            = USART_CPOL_Low;
    usart_clk_init.USART_CPHA            = USART_CPHA_2Edge;
    usart_clk_init.USART_LastBit         = USART_LastBit_Disable;


                                                                /* ----------------- SETUP USART2 GPIO ---------------- */
    BSP_PeriphEn(BSP_PERIPH_ID_USART2);

                                                                /* ----------------- SETUP USART2 GPIO ---------------- */
    BSP_PeriphEn(BSP_PERIPH_ID_IOPA);

                                                                /* Configure GPIOA.2 as push-pull                       */
    gpio_init.GPIO_Pin   = GPIO_Pin_2;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio_init);

                                                                /* Configure GPIOA.3 as input floating                  */
    gpio_init.GPIO_Pin   = GPIO_Pin_3;
    gpio_init.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio_init);

                                                                /* ------------------ SETUP USART2 -------------------- */
    USART_Init(USART2, &usart_init);
    USART_ClockInit(USART2, &usart_clk_init);
    USART_Cmd(USART2, ENABLE);

    BSP_IntVectSet(BSP_INT_ID_USART2, Wifly_RxTxISRHandler);
    BSP_IntEn(BSP_INT_ID_USART2);

}

/*
*********************************************************************************************************
*                                       Disable Rx Interrupts
*********************************************************************************************************
*/

void  Wifly_RxIntDis (void)
{
    USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
}

/*
*********************************************************************************************************
*                                       Enable Rx Interrupts
*********************************************************************************************************
*/

void  Wifly_RxIntEn (void)
{
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

/*
*********************************************************************************************************
*                                 Rx Communication handler for Wifly
*
* Description: This function is called by Wifly_RxISR to process a received
*              character interrupt.
*
* Note(s)    : This adaptation of Wifly assumes that a 'combined' interrupt is generated by the UART
*              and thus this function is not needed.
*********************************************************************************************************
*/

void  Wifly_RxISRHandler (void)
{
}

/*
*********************************************************************************************************
*                                      Communication for  Wifly
*
* Description: Send 1 character to COM Port
*********************************************************************************************************
*/

void   Wifly_Tx (INT8U c)
{
    USART_SendData(USART2, c);
}
/*
*********************************************************************************************************
*                                       Disable Tx Interrupts
*********************************************************************************************************
*/

void  Wifly_TxIntDis (void)
{
    USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
}

/*
*********************************************************************************************************
*                                       Enable Tx Interrupts
*********************************************************************************************************
*/

void  Wifly_TxIntEn (void)
{
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

#endif
