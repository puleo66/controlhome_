/*
*********************************************************************************************************
*
*                                        I2C DRIVER (MASTER ONLY)
*                                                                         
*
* Filename      : bsp_i2c.c
* Version       : V1.00
* Programmer(s) : Puleo
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_I2C_MODULE
#include <bsp.h>


/*
*********************************************************************************************************
*                                              LOCAL DEFINES
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
*
* Note(s) :  (1) The 'BSP_I2C_DEV_STATUS' structure defines the status of the current transfer
*
*            (2) The 'BSP_I2C_REG' defines the register set for the I2C1/I2C2 peripherals.
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
void i2c_init() 
{
  GPIO_InitTypeDef GPIO_InitStructure;
  I2C_InitTypeDef I2C_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
   
  /*I2C Init */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C; 
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 100000;
  I2C_Init(I2C2, &I2C_InitStructure);
  I2C_Cmd(I2C2, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

}


CPU_INT08U read_pcf8574(CPU_INT08U addr) {
         CPU_INT08U pcf_pins;
//         CPU_INT08U pcf_addr = addr;
//         CPU_INT08U tmpH, tmpL;

         pcf_pins = 0;
         /* While the bus is busy */
//         while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
         /* Send START condition */
//         I2C_GenerateSTART(I2C2, ENABLE);
         /* Test on EV5 and clear it */
//         while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
         /* Send TMP100 address for write */
//         I2C_Send7bitAddress(I2C2, pcf_addr, I2C_Direction_Transmitter);
         /* Test on EV6 and clear it */
//         while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
         /*set TMP100 register*/
//         I2C_SendData(I2C2, 0x01);
         /* Test on EV8 and clear it */
//         while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
         /*set TMP100 register value*/
//         I2C_SendData(I2C2, 0x60); //12 bit
         /* Test on EV8 and clear it */
//         while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
         /* Send stop condition */
//         I2C_GenerateSTOP(I2C2, ENABLE);

//         Delay_ms(320);

         /* Send START condition */
//         I2C_GenerateSTART(I2C2, ENABLE);
         /* Test on EV5 and clear it */
//         while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
         /* Send TMP100 address for write */
//         I2C_Send7bitAddress(I2C2, 0x90, I2C_Direction_Transmitter);
         /* Test on EV6 and clear it */
//         while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
         /*set TMP100 register*/
//         I2C_SendData(I2C2, 0x00);
         /* Test on EV8 and clear it */
//         while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

         /* Send RESTART condition */
//         I2C_GenerateSTART(I2C2, ENABLE);
         /* Test on EV5 and clear it */
//         while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
         /* Send TMP100 address for read */
//         I2C_Send7bitAddress(I2C2, 0x91, I2C_Direction_Receiver);
//         while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
          /* Test on EV6 and clear it */
         //while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED));

//        I2C_AcknowledgeConfig(I2C2, ENABLE);
//        while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED));
//        tmpH=I2C_ReceiveData(I2C2);


//        I2C_AcknowledgeConfig(I2C2, DISABLE);
//        while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED));
//        tmpL=I2C_ReceiveData(I2C2);


        /* Send STOP Condition */
//        I2C_GenerateSTOP(I2C2, ENABLE);
        //temp = (((uint16_t)(tmpH))<<8)|((uint16_t)(tmpL));

//        tmpL = tmpL/25.6;
        //return tmpH;
//        return tmpL;
	
	return pcf_pins;	
}

void write_pcf8574(CPU_INT08U addr, CPU_INT08U data) 
{
  
	CPU_INT08U pcf_data = data;
	CPU_INT08U pcf_addr = addr;
 
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2, pcf_addr, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
        I2C_SendData(I2C2, pcf_data);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTOP(I2C2, ENABLE);
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_STOPF));

	return;
}
