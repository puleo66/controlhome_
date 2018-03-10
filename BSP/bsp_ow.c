/*
*********************************************************************************************************
*
*                                        ONEWIRE INTERFACE
*
*
* Filename      : bsp_ow.c
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
#include <bsp_ow.h>

#if THERMO_MODULE == DEF_ENABLED

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
#define OW_BIT                GPIO_Pin_8
#define OW_PORT               GPIOC

#define OW_OUT_LOW()          GPIO_ResetBits(OW_PORT, OW_BIT)
#define OW_OUT_HIGH()         GPIO_SetBits(OW_PORT, OW_BIT)
#define OW_GET_IN()           GPIO_ReadInputDataBit(OW_PORT, OW_BIT)


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
void Ow_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // PC8-DQ
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin =  OW_BIT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(OW_PORT, &GPIO_InitStructure);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void OW_DIR_OUT(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Pin = OW_BIT;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(OW_PORT, &GPIO_InitStruct);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void OW_DIR_IN(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.GPIO_Pin = OW_BIT;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(OW_PORT, &GPIO_InitStruct);
}

INT8U OneWire_ReadBit(void)
{
  INT8U bit;
  
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr = 0;
#endif
    
  OW_DIR_OUT();    // drive bus low
  OW_OUT_LOW();
  Us_DelayIO(1); 
  
OS_ENTER_CRITICAL();

  OW_DIR_IN();
  Us_DelayIO(1);

  bit = OW_GET_IN();
  
OS_EXIT_CRITICAL();

  Us_DelayIO(40);
  return bit;
}
 
void OneWire_WriteBit(INT8U bit)
{
    
  OW_DIR_OUT();    // drive bus low
  OW_OUT_LOW();
  Us_DelayIO(2);
        

  if(bit)
    OW_OUT_HIGH();
  
  Us_DelayIO(50); 
  
  OW_OUT_HIGH();
}

void OneWire_WriteByte(INT8U data)
{
  INT8U _data = data;
 
  for(int i=0; i<8; i++)
  {
    OneWire_WriteBit(_data & 0x01);
    _data = _data >> 1;              // now the next bit is in the least sig bit position.
  }
}
 
INT8U OneWire_ReadByte(void)
{
  INT8U answer = 0x00;
 
  for(int i=0; i<8; i++)
  {
    answer = answer >> 1; // shift over to make room for the next bit
    if(OneWire_ReadBit())
      answer = answer | 0x80; // if the data port is high, make this bit a 1
  }
 
  return answer;
}	

INT8U ow_input_pin_state(void)
{
  return OneWire_ReadBit();      
}

INT8U ow_reset(void)
{
  INT8U presence;
  
  OW_DIR_OUT();
  OW_OUT_LOW();
 
  Us_DelayIO(500);
  
  
  OW_DIR_IN();
  Us_DelayIO(60);			// 15-60us   (20)
  
  presence = OW_GET_IN();
  Us_DelayIO(250);		// wait till the end of presence pulse (time slot)  (300)  
  
  if( OW_GET_IN() == 0 ) {
    presence = 1;               // short circuit, expected low but got high
  }
  
  return presence;
}

INT8U ow_rom_get( void )
{
  if( ow_reset() ) {
    return OW_PRESENCE_ERR;           // error, no device found <--- early exit!
  }
	
  OneWire_WriteByte(OW_READ_ROM);
  for(int j=0; j < 8; j++){
    id[j] = OneWire_ReadByte();
  }

  return 0;
}

INT8U ow_rom_search( INT8U diff, INT8U *id )
{
  INT8U i, j, next_diff;
  INT8U b;
  INT8U k = 0;

 
  if( ow_reset() ) {
    return OW_PRESENCE_ERR;           // error, no device found <--- early exit!
  }
  
  OneWire_WriteByte(OW_SEARCH_ROM);   // ROM search command
  next_diff = OW_LAST_DEVICE;         // unchanged on last device
  
  i = OW_ROMCODE_SIZE * 8;            // 8 bytes
  
  do {
    j = 8;                            // 8 bits
    do {
      b = OneWire_ReadBit();          // read bit
      if( OneWire_ReadBit() ) {       // read complement bit
        if( b ) {                     // 0b11
          return OW_DATA_ERR;         // data error <--- early exit!
        }
      }
      else {
        if( !b ) {                    // 0b00 = 2 devices
          if( diff > i || (( id[k] & 1 ) && diff != i) ) {
            b = 1;                    // now 1
            next_diff = i;            // next pass 0
          }
        }
      }
      OneWire_WriteBit(b);            // write bit
      id[k] >>= 1;
      
      if( b ) {
        id[k] |= 0x80;                // store bit
      }
      i--;
    } 
    while( --j );
    k++;                              // next byte
  } 
  while( i );
  
  return next_diff;                   // to continue search
}

void ow_command( INT8U command, INT8U *id )
{
  INT8U i;
  INT8U k = 0;
        
  ow_reset();

  OneWire_WriteByte( OW_MATCH_ROM );     // to a single device
  i = OW_ROMCODE_SIZE;
  do {
    OneWire_WriteByte( id[k] );
    k++;
  } while( --i );
  
  OneWire_WriteByte( command );
}
#endif
