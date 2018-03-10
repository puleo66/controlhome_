/*
*********************************************************************************************************
*
*                                        SD/MMC (SPI2) INTERFACE
*
*
* Filename      : bsp_sd.c
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
/* Select MSD Card: ChipSelect pin low  */
#define MSD_CS_LOW()     GPIO_ResetBits(GPIOD, GPIO_Pin_2)
/* Deselect MSD Card: ChipSelect pin high */
#define MSD_CS_HIGH()    GPIO_SetBits(GPIOD, GPIO_Pin_2)

/* Select RF modul: ChipSelect pin low  */
#define RF_CS_LOW()     GPIO_ResetBits(GPIOB, GPIO_Pin_12)
/* Deselect RF modul: ChipSelect pin high */
#define RF_CS_HIGH()    GPIO_SetBits(GPIOB, GPIO_Pin_12)

/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/
BOOLEAN SPI_ASSERT_FLAGH;
/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static void SPI_Config(void);

/*
*********************************************************************************************************
*********************************************************************************************************
**                                         GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/
extern void FS20_ISRHandler (void);
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
#if RF12B_MODULE == DEF_ENABLED

void RF_ISRHandler(void)
{

  CPU_SR          cpu_sr;


    CPU_CRITICAL_ENTER();                                       /* Tell uC/OS-II that we are starting an ISR            */
    OSIntNesting++;
    CPU_CRITICAL_EXIT();
    
    if(EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
       FS20_ISRHandler();   
       EXTI_ClearITPendingBit(EXTI_Line7);
    }
    
     OSIntExit();                                                /* Tell uC/OS-II that we are leaving the ISR            */

}
#endif

/*******************************************************************************************************************
** Name:	  void SD_HardWareInit()
** Function: initialize the hardware condiction that access sd card
** Input:	  NULL
** Output:	  NULL
********************************************************************************************************************/
void SD_HardWareInit(void)
{ 
	SPI_Config();								
    
        SPI_ASSERT_FLAGH = FALSE;
	MSD_CS_HIGH();
        RF_CS_HIGH();

	SPI_Clk400k();	
}

/*******************************************************************************************************************
** Name:	  void SPI_Clk400k()
** Function: set the clock of SPI less than 400kHZ
** Input:	  NULL
** Output:	  NULL
********************************************************************************************************************/
void SPI_Clk400k(void)
{
	SPI_InitTypeDef   SPI_InitStructure;

  	/* SPI2 Config */
  	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;  
  	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  	SPI_InitStructure.SPI_CRCPolynomial = 7;
  	SPI_Init(SPI2, &SPI_InitStructure);

	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE | SPI_I2S_IT_RXNE | SPI_I2S_IT_ERR, DISABLE);
  	
        /* SPI2 enable */
  	SPI_Cmd(SPI2, ENABLE);
}


/*******************************************************************************************************************
** Name:	  void SPI_ClkToMax()
** Function: set the clock of SPI to maximum
** Input:	  NULL
** Output:	  NULL
********************************************************************************************************************/
void SPI_ClkToMax(void)
{
	SPI_InitTypeDef   SPI_InitStructure;

  	/* SPI2 Config */
  	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;  //9MHz = 72M / 2 / 4
  	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  	SPI_InitStructure.SPI_CRCPolynomial = 7;
  	SPI_Init(SPI2, &SPI_InitStructure);

	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE | SPI_I2S_IT_RXNE | SPI_I2S_IT_ERR, DISABLE);
  	/* SPI2 enable */
  	SPI_Cmd(SPI2, ENABLE);
}


/*******************************************************************************************************************
** Name:	  void SPI_SendByte()
** Function: send a byte by SPI interface
** Input:	  INT8U byte: the byte that will be send
** Output:	  NULL
********************************************************************************************************************/
INT8U SPI_SendByte(INT8U byte)
{
  
	/* Wait until the transmit buffer is empty */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	/* Send the byte */
	SPI_I2S_SendData(SPI2, byte);
         /* Wait to receive a byte */
        while ( SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_RXNE ) == RESET );
        /* Return the byte read from the SPI bus */
        return SPI_I2S_ReceiveData( SPI2 );
}


/*******************************************************************************************************************
** Name:	  INT8U SPI_RecByte()
** Function: receive a byte from SPI interface
** Input:	  NULL
** Output:	  the byte that be received
********************************************************************************************************************/
INT8U SPI_RecByte(void)
{
	INT8U Data = 0;
	
	/* Wait until the transmit buffer is empty */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	/* Send the byte */
	SPI_I2S_SendData(SPI2, 0xFF);

	/* Wait until a data is received */
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	/* Get the received data */
	Data = SPI_I2S_ReceiveData(SPI2);
	
	/* Return the shifted data */
	return Data;
}


/*******************************************************************************************************************
** Name:	  void SPI_CS_Assert()
** Function: select the SPI MMC slave 
** Input:	  NULL
** Output:	  NULL
********************************************************************************************************************/
void SPI_CS_Assert(void)
{
        while (SPI_ASSERT_FLAGH);
	MSD_CS_LOW();			   					/* select the SPI slave */ 
        SPI_ASSERT_FLAGH = TRUE;
}


/*******************************************************************************************************************
** Name:	  void SPI_CS_Deassert()
** Function: not select the SPI MMC slave 
** Input:	  NULL
** Output:	  NULL
********************************************************************************************************************/
void SPI_CS_Deassert(void)
{
	MSD_CS_HIGH();			    		         		/* not select the SPI slave */
        SPI_ASSERT_FLAGH = FALSE;
}

/*******************************************************************************************************************
** Name:	  void RF_CS_Assert()
** Function: select the SPI RF slave 
** Input:	  NULL
** Output:	  NULL
********************************************************************************************************************/
void RF_CS_Assert(void)
{
        while (SPI_ASSERT_FLAGH);
        RF_CS_LOW();			   		        		/* select the SPI slave */ 
        SPI_ASSERT_FLAGH = TRUE;
}


/*******************************************************************************************************************
** Name:	  void RF_CS_Deassert()
** Function: not select the SPI RF slave 
** Input:	  NULL
** Output:	  NULL
********************************************************************************************************************/
void RF_CS_Deassert(void)
{
	RF_CS_HIGH();			    	 	        		/* not select the SPI slave */
        SPI_ASSERT_FLAGH = FALSE;
}

/*******************************************************************************
* Function Name  : SPI_Config
* Description    : Initializes the SPI1 and CS pins.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_Config(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* GPIOB and GPIOD Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD, ENABLE);
  /* SPI2 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

  /* Configure SPI2 pins: SCK, MISO and MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure PD2 pin: MMC CS pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  /* Configure PB12 pin: RF CS pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
}


