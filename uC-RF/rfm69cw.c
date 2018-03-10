/*
*********************************************************************************************************
*                                           RFM69CW Interface
*
*
*
* Filename   : rfm69cw.c
* Version    : V1.00
* Programmer : Puleo
*********************************************************************************************************
*/

#define  RFM69CW_GLOBALS
#include <includes.h>
#include <RFM69OOKregisters.h>

#if RFM69CW_MODULE == DEF_ENABLED

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/
#define EXTI_BIT                GPIO_Pin_6
#define EXTI_PORT               GPIOC

#define EXTI_GET_IN()           GPIO_ReadInputDataBit(EXTI_PORT, EXTI_BIT)

/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/
static CPU_BOOLEAN _isRFM69HW =        false;
static INT8U _powerLevel =      31;

//INT8U regVal[0x6F];

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void RFM69CW_ExtiEnabled(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  EXTI_InitTypeDef  EXTI_InitStructure;
  
  /* GPIOB and GPIOD Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC  |  RCC_APB2Periph_AFIO , ENABLE);

  // Configure PC6 pin as input floating 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  // Connect EXTI Line6 to PC6 pin 
  GPIO_EXTILineConfig( GPIO_PortSourceGPIOC, GPIO_PinSource6 );
  
  EXTI_InitStructure.EXTI_Line    = EXTI_Line6;
  EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_GenerateSWInterrupt( EXTI_Line6 );
  EXTI_Init(&EXTI_InitStructure);
  
  BSP_IntVectSet(BSP_INT_ID_EXTI9_5, RF_ISRHandler);

}

/*
*********************************************************************************************************
*/

static INT8U Write_Byte (INT8U out) 
{
    return SPI2_SendByte(out);
}

/*
*********************************************************************************************************
*/

static INT8U Read_Reg(INT8U addr)
{
    RF_CS_Assert();
    Write_Byte(addr & 0x7F);
    INT8U regval = Write_Byte( 0x00 );
    RF_CS_Deassert();
    return regval;
}

/*
*********************************************************************************************************
*/

static void Write_Reg(INT8U addr, INT8U value)
{
    RF_CS_Assert();
    Write_Byte(( addr & 0x7F ) | 0x80);
    Write_Byte(value & 0xFF);
    RF_CS_Deassert();
} 

/*
*********************************************************************************************************
*/

void SetHighPowerRegs(CPU_BOOLEAN onOff) {
  Write_Reg(REG_TESTPA1, onOff ? 0x5D : 0x55);
  Write_Reg(REG_TESTPA2, onOff ? 0x7C : 0x70);
}

/*
*********************************************************************************************************
*/

static void SetMode(INT8U newMode)
{
  Write_Reg(REG_OPMODE, (Read_Reg(REG_OPMODE) & 0xE3) | newMode);
  while ((Read_Reg(REG_IRQFLAGS1) & RF_IRQFLAGS1_MODEREADY) == 0x00);                                                                                   // Wait for ModeReady
}

/*
*********************************************************************************************************
*
* return the frequency (in Hz)
*
*********************************************************************************************************
*/

INT64U RFM69CW_GetFrequency()
{
  return ((((((INT32U)Read_Reg(REG_FRFMSB) << 16) + ((INT16U)Read_Reg(REG_FRFMID) << 8) + Read_Reg(REG_FRFLSB)) >> 8) * (32000000L >> 11)) / 1000);
}

/*
*********************************************************************************************************
*
* Set literal frequency using decimal point KHz value
*
*********************************************************************************************************
*/

void RF69CW_SetFrequencyKHz(INT32U frq) {
  Write_Reg(REG_AFCFEI, (1 << 1));
  
  // accept any frequency scale as input, including KHz and MHz
  while (frq < 100000000)
    frq *= 10; 
  
  INT64U frf = ((INT64U)frq << 8) / (32000000L >> 11);
  Write_Reg(REG_FRFMSB, frf >> 16);
  Write_Reg(REG_FRFMID, frf >> 8);
  Write_Reg(REG_FRFLSB, frf);

} 
/*
*********************************************************************************************************
*
* Set bitrate
*
*********************************************************************************************************
*/

void RFM69CW_SetBitrate(INT32U bitrate)
{
  
  if (bitrate < 489) {
    bitrate = 0xFFFF;
  } else {
    bitrate = 32000000L / bitrate;
  }
  Write_Reg(REG_BITRATEMSB, bitrate >> 8);
  Write_Reg(REG_BITRATELSB + 1, bitrate);
  
}

/*
*********************************************************************************************************
*
* Set OOK bandwidth
*
*********************************************************************************************************
*/

void RFM69CW_SetBandwidth(INT8U bw)
{
  Write_Reg(REG_RXBW, Read_Reg(REG_RXBW) & 0xE0 | bw);
}

/*
*********************************************************************************************************
*
* Set RSSI threshold
*
*********************************************************************************************************
*/

void RFM69CW_SetRSSIThreshold(INT8U rssi)
{
  Write_Reg(REG_RSSITHRESH, (-rssi << 1));
}

/*
*********************************************************************************************************
*
* Set OOK fixed threshold
*
*********************************************************************************************************
*/

void RFM69CW_SetFixedThreshold(INT8U threshold)
{
  Write_Reg(REG_OOKFIX, threshold);
}

/*
*********************************************************************************************************
*
* Set sensitivity boost in REG_TESTLNA
* see: http://www.sevenwatt.com/main/rfm69-ook-dagc-sensitivity-boost-and-modulation-index
*
*********************************************************************************************************
*/

void RFM69CW_SetSensitivityBoost(INT8U value)
{
  Write_Reg(REG_TESTLNA, value);
}

/*
*********************************************************************************************************
*/

void RFM69CW_Sleep() {
  SetMode(RF69OOK_MODE_SLEEP);
}

/*
*********************************************************************************************************
*
* Set output power: 0=min, 31=max
* this results in a "weaker" transmitted signal, and directly results in a lower RSSI at the receiver
*
*********************************************************************************************************
*/

void RFM69CW_SetPowerLevel(INT8U powerLevel)
{
  _powerLevel = powerLevel;
  Write_Reg(REG_PALEVEL, (Read_Reg(REG_PALEVEL) & 0xE0) | (_powerLevel > 31 ? 31 : _powerLevel));
}

/*
*********************************************************************************************************
*/

INT8U RFM69CW_ReadRSSI(CPU_BOOLEAN forceTrigger) {
  if (forceTrigger)
  {
    // RSSI trigger not needed if DAGC is in continuous mode
    Write_Reg(REG_RSSICONFIG, RF_RSSI_START);
    while ((Read_Reg(REG_RSSICONFIG) & RF_RSSI_DONE) == 0x00); // Wait for RSSI_Ready
  }
  return -(Read_Reg(REG_RSSIVALUE) >> 1);
}

/*
*********************************************************************************************************
*/

void RFM69CW_SetHighPower(CPU_BOOLEAN onOff) {
  _isRFM69HW = onOff;
  Write_Reg(REG_OCP, _isRFM69HW ? RF_OCP_OFF : RF_OCP_ON);
  if (_isRFM69HW) // turning ON
    Write_Reg(REG_PALEVEL, (Read_Reg(REG_PALEVEL) & 0x1F) | RF_PALEVEL_PA1_ON | RF_PALEVEL_PA2_ON); // enable P1 & P2 amplifier stages
  else
    Write_Reg(REG_PALEVEL, RF_PALEVEL_PA0_ON | RF_PALEVEL_PA1_OFF | RF_PALEVEL_PA2_OFF | _powerLevel); // enable P0 only
}
/*
*********************************************************************************************************
*/

void RF69CW_SetThd (INT8U thd) {
  Write_Reg(REG_OOKFIX, thd);
}
/*
*********************************************************************************************************
*/

void RF69CW_SetBW (INT8U bw) {
  Write_Reg(REG_RXBW, 0x40 | bw);
} 

/*
*********************************************************************************************************
*/

void RF69CW_OOKthdMode(INT8U thdmode) {
  Write_Reg(REG_OOKPEAK, thdmode);
}

/*
*********************************************************************************************************
*
* for debugging
*
*********************************************************************************************************
*/

void RFM69CW_ReadAllRegs(void)
{
  
  for (INT8U regAddr = 0; regAddr <= 0x6F; regAddr++) {
    regVal[regAddr] = Read_Reg(regAddr);
  }
}

INT8U RFM69CW_ReadTemperature(INT8U calFactor)  // returns centigrade
{
  SetMode(RF69OOK_MODE_STANDBY);
  Write_Reg(REG_TEMP1, RF_TEMP1_MEAS_START);
  while ((Read_Reg(REG_TEMP1) & RF_TEMP1_MEAS_RUNNING));
  return Read_Reg(REG_TEMP2) + COURSE_TEMP_COEF + calFactor; // 'complement' corrects the slope, rising temp = rising val
}                                                            // COURSE_TEMP_COEF puts reading in the ballpark, user can add additional correction

void RFM69CW_RcCalibitrateation(void)
{
  Write_Reg(REG_OSC1, RF_OSC1_RCCAL_START);
  while ((Read_Reg(REG_OSC1) & RF_OSC1_RCCAL_DONE) == 0x00);
} 

/*
*********************************************************************************************************
*/

CPU_BOOLEAN RFM69CW_InitializeOOK(void)
{
  const INT8U CONFIG[][2] =
  {
    /* 0x01 */ { REG_OPMODE, RF_OPMODE_STANDBY },
    /* 0x02 */ { REG_DATAMODUL, RF_DATAMODUL_DATAMODE_CONTINUOUSNOBSYNC | RF_DATAMODUL_MODULATIONTYPE_OOK | RF_DATAMODUL_MODULATIONSHAPING_00 },         // no shaping
    /* 0x03 */ { REG_BITRATEMSB, 0x03 },                                                                                                                 // bitrate: 32768 Hz
    /* 0x04 */ { REG_BITRATELSB, 0xD1 },
    /* 0x0B */ { REG_AFCCTRL, RF_AFCCTRL_LOWBETA_ON },
//    /* 0x18 */ { REG_LNA, RF_LNA_ZIN_200 | RF_LNA_GAINSELECT_MAX },             // default
//    /* 0x18 */ { REG_LNA, RF_LNA_ZIN_50 | RF_LNA_GAINSELECT_AUTO },
    /* 0x18 */ { REG_LNA, RF_LNA_ZIN_50 | RF_LNA_GAINSELECT_MAX },
    /* 0x19 */ { REG_RXBW, RF_RXBW_DCCFREQ_010 },
    /* 0x1B */ { REG_OOKPEAK, RF_OOKPEAK_THRESHTYPE_PEAK | RF_OOKPEAK_PEAKTHRESHDEC_011 },
    /* 0x1C */ { REG_OOKAVG, RF_OOKAVG_AVERAGETHRESHFILT_10 },
    /* 0x1D */ { REG_OOKFIX, 0x38 },     
    /* 0x1E */ { REG_AFCFEI, RF_AFCFEI_AFCAUTO_OFF },
    /* 0x25 */ { REG_DIOMAPPING1, RF_DIOMAPPING1_DIO0_10 },
    /* 0x26 */ { REG_DIOMAPPING2, RF_DIOMAPPING2_CLKOUT_OFF },
    /* 0x29 */ { REG_RSSITHRESH, 0xFF },
    /* 0x2E */ { REG_SYNCCONFIG, RF_SYNC_OFF },
//    /* 0x58 */ { REG_TESTLNA, RF_TESTLNA_HIGH_SENSITIVITY },                      // default false
    /* 0x6F */ { REG_TESTDAGC, RF_DAGC_IMPROVED_LOWBETA1 },
    {255, 0}
  };

  for (INT8U i = 0; CONFIG[i][0] != 255; i++)
    Write_Reg(CONFIG[i][0], CONFIG[i][1]);

  RF69CW_OOKthdMode(0x40);
  RFM69CW_SetBitrate(32768);
  RF69CW_SetFrequencyKHz(868400);
  RF69CW_SetBW( 16 );
  RF69CW_SetThd(55);
  RFM69CW_SetPowerLevel(20); //31 max

  Write_Reg(REG_AFCFEI, (1 << 1));

  SetMode(RF_OPMODE_RECEIVER);
  
  return true;
}

#endif
