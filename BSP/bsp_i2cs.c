/*
*********************************************************************************************************
*
*                                        I2C (Software) INTERFACE
*
*
* Filename      : bsp_i2cs.c
* Version       : V0.1
* Programmer(s) : Puleo
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_I2C_MODULE
#include <bsp.h>
#include <bsp_i2cs.h>
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
#define SCL_H         GPIOB->BSRR = GPIO_Pin_6	 /* GPIO_SetBits(GPIOC , GPIO_Pin_6)   */
#define SCL_L         GPIOB->BRR  = GPIO_Pin_6  /* GPIO_ResetBits(GPIOC , GPIO_Pin_6) */
   
#define SDA_H         GPIOB->BSRR = GPIO_Pin_7	 /* GPIO_SetBits(GPIOC , GPIO_Pin_7)   */
#define SDA_L         GPIOB->BRR  = GPIO_Pin_7	 /* GPIO_ResetBits(GPIOC , GPIO_Pin_7) */

#define SCL_read      GPIOB->IDR  & GPIO_Pin_6	 /* GPIO_ReadInputDataBit(GPIOC , GPIO_Pin_6) */
#define SDA_read      GPIOB->IDR  & GPIO_Pin_7	 /* GPIO_ReadInputDataBit(GPIOC , GPIO_Pin_7) */

#define ADDR_PCF8574  0x40


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
**                                         GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*******************************************************************************
* Function Name  : I2C_Configuration
* Description    : EEPROM�-�++��+
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void I2C_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
  /* Configure I2C2 pins: PC6->SCL and PC7->SDA */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : I2C_delay
* Description    : ��--����
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C_delay(void)
{	
   INT8U i=50; /* ��L��-���+��T-�L,׺-Ԧ����=��5�ܦ-�+L� */
   while(i) 
   { 
     i--; 
   } 
}

/*******************************************************************************
* Function Name  : I2C_Start
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static FunctionalState I2C_Start(void)
{
	SDA_H;
	SCL_H;
	I2C_delay();
	if(!SDA_read)return DISABLE;	/* SDA�-+��=��ï��-�-+�,=T-� */
	SDA_L;
	I2C_delay();
	if(SDA_read) return DISABLE;	/* SDA�-+��-��ï��-�--�+�,=T-� */
	SDA_L;
	I2C_delay();
	return ENABLE;
}

/*******************************************************************************
* Function Name  : I2C_Stop
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C_Stop(void)
{
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_H;
	I2C_delay();
}

/*******************************************************************************
* Function Name  : I2C_Ack
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C_Ack(void)
{	
	SCL_L;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

/*******************************************************************************
* Function Name  : I2C_NoAck
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C_NoAck(void)
{	
	SCL_L;
	I2C_delay();
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SCL_L;
	I2C_delay();
}

/*******************************************************************************
* Function Name  : I2C_WaitAck
* Description    : None
* Input          : None
* Output         : None
* Return         : ��+�:=1��ACK,=0+�ACK
* Attention		 : None
*******************************************************************************/
static FunctionalState I2C_WaitAck(void) 	
{
	SCL_L;
	I2C_delay();
	SDA_H;			
	I2C_delay();
	SCL_H;
	I2C_delay();
	if(SDA_read){
          SCL_L;
          return DISABLE;
        }
	SCL_L;
	return ENABLE;
}

 /*******************************************************************************
* Function Name  : I2C_SendByte
* Description    : ����+˪-+����=+�
* Input          : - SendByte: ��T=������
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void I2C_SendByte(INT8U SendByte) 
{
    INT8U i=8;
    while(i--)
    {
        SCL_L;
        I2C_delay();
      if(SendByte&0x80)
        SDA_H;  
      else 
        SDA_L;   
        SendByte<<=1;
        I2C_delay();
	SCL_H;
        I2C_delay();
    }
    SCL_L;
}


/*******************************************************************************
* Function Name  : I2C_ReceiveByte
* Description    : ����+˪-+����=+�
* Input          : None
* Output         : None
* Return         : I2C�-�-�쿦����
* Attention		 : None
*******************************************************************************/
static INT8U I2C_ReceiveByte(void)  
{ 
    INT8U i=8;
    INT8U ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
      ReceiveByte<<=1;      
      SCL_L;
      I2C_delay();
      SCL_H;
      I2C_delay();	
      if(SDA_read)
      {
        ReceiveByte|=0x01;
      }
    }
    SCL_L;
    return ReceiveByte;
}

/*******************************************************************************
* Function Name  : I2C_WriteByte
* Description    : �+ϣ�ͯ-����
* Input          : - SendByte: +��+Lۦ���
*           	   - WriteAddress: +��+Lۿ���
*                  - DeviceAddress: ø�L��=(24c16��SD2403)
* Output         : None
* Return         : ��+�:=1--���+L�,=0��--
* Attention		 : None
*******************************************************************************/           
FunctionalState I2C_WriteByte(INT8U WriteAddress)
{		
    if(!I2C_Start())
      return DISABLE;
    I2C_SendByte( ADDR_PCF8574 );
    if(!I2C_WaitAck()){
      I2C_Stop(); 
      return DISABLE;
    }
    I2C_SendByte(WriteAddress);   /* -��+�=í����� */      
    I2C_WaitAck();	  
    I2C_Stop();
    /* Delay_1ms(10); */
    Ms_DelayIO(10);
    return ENABLE;
}									 

/*******************************************************************************
* Function Name  : I2C_WriteByte
* Description    : �+ϣ�ͯ-����
* Input          : - SendByte: +��+Lۦ���
*           	   - WriteAddress: +��+Lۿ���
*                  - DeviceAddress: ø�L��=(24c16��SD2403)
* Output         : None
* Return         : ��+�:=1--���+L�,=0��--
* Attention		 : None
*******************************************************************************/           
FunctionalState I2C_WriteByteMem(INT8U SendByte, INT16U WriteAddress, INT8U DeviceAddress)
{		
    if(!I2C_Start())
      return DISABLE;
    I2C_SendByte(((WriteAddress & 0x0700) >>7) | DeviceAddress & 0xFFFE); /*-��+�-í�����+ø覿��� */
    if(!I2C_WaitAck()){
      I2C_Stop(); 
      return DISABLE;
    }
    I2C_SendByte((INT8U)(WriteAddress & 0x00FF));   /* -��+�=í����� */      
    I2C_WaitAck();	
    I2C_SendByte(SendByte);
    I2C_WaitAck();   
    I2C_Stop(); 
    /* Delay_1ms(10); */
    Ms_DelayIO(10);
    return ENABLE;
}									 

/*******************************************************************************
* Function Name  : I2C_ReadByte
* Description    : �+L�ϣ+Ħ���
* Input          : - pBuffer: +��+�+-�����
*           	   - length: +��+-�-��L
*                  - ReadAddress: +��+-�����
*                  - DeviceAddress: ø�L��=(24c16��SD2403)
* Output         : None
* Return         : ��+�:=1--��+L�,=0��--
* Attention		 : None
*******************************************************************************/          
FunctionalState I2C_ReadByteMem(INT8U* pBuffer,   INT16U length,   INT16U ReadAddress,  INT8U DeviceAddress)
{		
    if(!I2C_Start())return DISABLE;
    I2C_SendByte(((ReadAddress & 0x0700) >>7) | DeviceAddress & 0xFFFE); /* -��+�-í�����+ø覿��� */ 
    if(!I2C_WaitAck()){I2C_Stop(); return DISABLE;}
    I2C_SendByte((INT8U)(ReadAddress & 0x00FF));   /* -��+�=í����� */      
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(((ReadAddress & 0x0700) >>7) | DeviceAddress | 0x0001);
    I2C_WaitAck();
    while(length)
    {
      *pBuffer = I2C_ReceiveByte();
      if(length == 1)I2C_NoAck();
      else I2C_Ack(); 
      pBuffer++;
      length--;
    }
    I2C_Stop();
    return ENABLE;
}


