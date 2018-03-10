/*
*********************************************************************************************************
*                                           FS20 Interface
*
*
*
* Filename   : fs20.c
* Version    : V1.00
* Programmer : Puleo
*********************************************************************************************************
*/

#define  FS20_GLOBALS
#include <includes.h>

#if RFM69CW_MODULE == DEF_ENABLED

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/
#define  FS20_UNKNOWN  0
#define  FS20_T0       1
#define  FS20_T1       2
#define  FS20_OK       3
#define  FS20_DONE     4

#define  _BV(bit) (1 << (bit)) 
#define  magic_byte    12

/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/

INT8U  buf[10];                  // stored FS20 data ook868 poll(buf)
INT16S width;
INT8U  res1;
INT16U res2 = 0;
INT32S count;
INT8U  sum;
INT16U mask;

INT8S  FHTNr;
INT8S  TFNr;

INT8U  FTHLcdStr[20];
INT8U  TempBuf[10];
INT8U  n;

INT8U tmp, tmp1;

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              FS20_TimeISRHandler
*
* Description: This routine is called from the Rx interrupt service handler.
*********************************************************************************************************
*/

void reset_FS20 (void) 
{
  FS20.bits = -1;
  FS20.data = 0xFF;
  FS20.state = FS20_UNKNOWN;
}
/*
*********************************************************************************************************
*/
INT8U FS20_Setbit(INT8U bit)
{
  FS20.data = (FS20.data << 1) | bit;
  if (FS20.bits < 0 && (INT8U) FS20.data != 0x01)
    return FS20_OK;
  if (++FS20.bits == 45 && ((FS20.data >> 15) & 1) == 0 || FS20.bits == 54 && ((FS20.data >> 24) & 1)){
    return FS20_DONE;
  }
  return FS20_OK;
}
/*
*********************************************************************************************************
*                                       INT8U ook868poll (void)
*
*    Poll and return a count > 0 when a valid packet has been received:
*    5 = FS20 6 = FS20/FHT 
*********************************************************************************************************
*/
INT8U ook868poll (void) 
{
  INT8U len, i, b;
  
  if (FS20.state == FS20_DONE) {
      len = FS20.bits / 9;
      sum = magic_byte;
      for (i = 0; i < len; i++) {
        b = FS20.data >> (1 + 9 * i);
        buf[len-i-1] = b;
        if (i > 0)
          sum += b;
      }
      
      if (sum == buf[len-1]) {
         reset_FS20();
        if(FS20.prev != buf[len-1]){
          FS20.prev = buf[len-1];
          memcpy(TempBuf,buf,10);
          return len;
        }
      }
           
      FS20.prev = 0;
      reset_FS20();
  }
  return 0;
}
/*
*********************************************************************************************************
*/
void  FS20_TimeISRHandler (void)
{
  if (FS20.state != FS20_DONE)
    reset_FS20();
}

/*
*********************************************************************************************************
*                                              FS20_ISRHandler
*
* Description: This routine is called from the Rx interrupt service handler.
*********************************************************************************************************
*/

void  FS20_ISRHandler (void)
{
    
    // see http://fhz4linux.info/tiki-index.php?page=FS20%20Protocol
    count = Free_TmrRd ();
    TIM_SetCounter(TIM1, 0);
    
    if (FS20.state != FS20_DONE)    
    {
      //  1uS  puls length 300 - 500  gemiddelde pulselengte is 400uS volgens specs FS
      if (count > 300  && count < 500 )
      {
        if (FS20.state == FS20_T0)
          FS20.state=FS20_Setbit(0x00);                      //ready or done
        else
          FS20.state = FS20_T0;
      }

      //  1uS  pulse length 500 - 700 gemiddelde pulselengte is 600uS  volgens Specs FS
      if (count > 500 && count < 700 )
      {
         if (FS20.state == FS20_T1)
           FS20.state=FS20_Setbit(0x01);
         else
            FS20.state = FS20_T1;
      }
      
    }else{
        n = ook868poll();
    }   
}

/*
*********************************************************************************************************
*/
#if (FS_MODULE == DEF_ENABLED)
void PaarLoad(void)
{
  INT8U i;

  i            = 0;
  FHTTFPaarAct = 0;
  
  do{
      FHTTFPaar[FHTTFPaarAct].hc1 = (FileBuffer[i] - 0x30)*10 + (FileBuffer[i+1] - 0x30);
      FHTTFPaar[FHTTFPaarAct].idx = (FileBuffer[i+3] - 0x30)*10 + (FileBuffer[i+4] - 0x30);
      i += 6;
      FHTTFPaarAct += 1;
  }while(i < strlen((char*)FileBuffer)-3);
}
#endif
/*
*********************************************************************************************************
*/
#if (FS_MODULE == DEF_ENABLED)
void TFDevLoad(void)
{
  INT8U i;

  i             = 0;
  TFLoadAct     = 0;
  do{
      TFDevAccess[TFLoadAct].hc1 = (FileBuffer[i] - 0x30)*10 + (FileBuffer[i+1] - 0x30);
      TFDevAccess[TFLoadAct].hc2 = ((FileBuffer[i+3] - 0x30)*100) + ((FileBuffer[i+4] - 0x30)*10) + (FileBuffer[i+5] - 0x30);
      TFDevAccess[TFLoadAct].hc3 = (FileBuffer[i+7] - 0x30)*10 + (FileBuffer[i+8] - 0x30);
    
      i += 10;
      TFLoadAct += 1;
  }while(i < strlen((char*)FileBuffer)-3);
}
#endif
/*
*********************************************************************************************************
*/
#if (FS_MODULE == DEF_ENABLED)
void FHTDevLoad(void)
{
  INT8U i;

  i          = 0;
  FHTLoadAct = 0;
  
  do{
      FHTDevAccess[FHTLoadAct].hc1 = (FileBuffer[i] - 0x30)*10 + (FileBuffer[i+1] - 0x30);
      FHTDevAccess[FHTLoadAct].hc2 = (FileBuffer[i+3] - 0x30)*10 + (FileBuffer[i+4] - 0x30);;
    
      i += 7;
      FHTLoadAct += 1;
  }while(i < strlen((char*)FileBuffer)-3);
}
#endif
/*
*********************************************************************************************************
*/
static void TFDevClear(void)
{
  INT8U i;
  
  for (i=0; i < TFmaxDev; i++)
    {
      TFDev[i].hc1 = 0x00;
      TFDev[i].hc2 = 0x00;
      TFDev[i].hc3 = 0x00;
      TFDev[i].command = 0x00;
      TFDev[i].lastVal = 0x00;
    }
    
  TFDevAct = 0x00;
}

/*
*********************************************************************************************************
*/

static void FHTDevClear(void)
{
  INT8U i;
  
  for (i=0; i < maxDevices; i++)
    {
      FHTDev[i].hc1 = 0x00;
      FHTDev[i].hc2 = 0x00;
      FHTDev[i].command = 0x00;
      FHTDev[i].extension = 0x00;
      FHTDev[i].lastVal = 0x00;
    }
    
  FHTDevAct = 0x00;
}

/*
*********************************************************************************************************
*/

static void FloorClear(void)
{
  INT8U i;
  
  for (i=0; i < maxFloor; i++)
    {
      FloorNum[i].devAct = 0x00;
      FloorNum[i].value = 0x00;
    }
}


/*
*********************************************************************************************************
*/
void InitStructs(void){
  TFDevClear();
  FHTDevClear();
  FloorClear();
  FHTLedVal = 0;
}
/*
*********************************************************************************************************
*/

INT8S TFDevEnabled(INT8U hc1, INT8U hc2, INT8U hc3)
{
  INT8U i;
  
  if ((hc1 == 0x00) && (hc2 == 0x00) && (hc3 == 0x00))
      return -1;
  
  for (i=0; i < TFmaxDev; i++)
    if ((TFDevAccess[i].hc1 == hc1) && (TFDevAccess[i].hc2 == hc2) && (TFDevAccess[i].hc3 == hc3))
      return i;
  return -1;
}

/*
*********************************************************************************************************
*/

INT8S FHTDevEnabled(INT8U hc1, INT8U hc2)
{
  INT8U i;
  
  for (i=0; i < maxDevices; i++)
    if (FHTDevAccess[i].hc1 == hc1 && FHTDevAccess[i].hc2 == hc2)
      return i;
  return -1;
}

/*
*********************************************************************************************************
*/

INT8S TFDevNr(INT8U hc1, INT8U hc2, INT8U hc3)
{
  INT8U i;
  
  for (i=0; i < TFmaxDev; i++)
    if (TFDev[i].hc1 == hc1 && TFDev[i].hc2 == hc2 && TFDev[i].hc3 == hc3)
      return i;
  return -1;
}

/*
*********************************************************************************************************
*/

INT8S FHTDevNr(INT8U hc1, INT8U hc2)
{
  INT8U i;
  
  //Speicherplatz durch hc finden
  for (i=0; i < maxDevices; i++)
    if (FHTDev[i].hc1 == hc1 && FHTDev[i].hc2 == hc2)
      return i;
  return -1;
}

/*
*********************************************************************************************************
*/

void TFValue(INT8U Nr)
{  
  switch ( TFDev[Nr].command )
  {
      case 0x01:
        TFDev[Nr].lastVal = 1;
        break;
      case 0x02:
        TFDev[Nr].lastVal = 0;
        break;
      case 0x81:
        TFDev[Nr].lastVal = 1;
        break;
      case 0x82:
        TFDev[Nr].lastVal = 0;
        break;
      default:
        TFDev[Nr].lastVal = 0;
        break;
  }
}

/*
*********************************************************************************************************
*/

void FHTValue(INT8U Nr)
{  
  switch ( FHTDev[Nr].command & 0x0F)
  {
      case 0x01:
        FHTDev[Nr].lastVal = 1;
        break;
      case 0x02:
        FHTDev[Nr].lastVal = 0;
        break;
      case 0x06:
        FHTDev[Nr].lastVal = 0;
        if ((FHTDev[Nr].extension * 100) / 255 > 5)
          FHTDev[Nr].lastVal = 1;
        break;
      default:
        FHTDev[Nr].lastVal = 0;
        break;
  }
}

/*
*********************************************************************************************************
*/
INT8S TFFloor(INT8U FHTNr)
{
  INT8U i, TFPaar, TFvalue, FHTpaar;
  INT8S TFdev;
  INT8S ret;
  
  ret = -1;
  
  if(TFDevAct != 0){
    for(i=0; i < FHTTFPaarAct; i++){
      TFPaar = FHTTFPaar[i].idx;
      TFdev = TFDevNr(TFDevAccess[TFPaar].hc1, TFDevAccess[TFPaar].hc2, TFDevAccess[TFPaar].hc3);
      if ( TFdev == -1){
        return ret;
      }else{
        TFvalue = TFDev[TFdev].lastVal;
      }
      
      FHTpaar = FHTTFPaar[i].hc1;
      if( FHTpaar != FHTDev[FHTNr].hc1){
        return ret;
      }else{
        ret = TFvalue;
        break;
      }
    }
  }
  return ret;
}
/*
*********************************************************************************************************
*/
void FHTFloor(INT8U Nr)
{
  INT8U i, j, value, FTvalue;

  FHTValue(Nr);

  for (i=0; i < maxFloor; i++)
  {
    value = 0;
    for (j=0; j < FloorNum[i].devAct ; j++)
    {
      value += FHTDev[ FloorNum[i].devNum[j] ].lastVal;
    }
    
    if ( value > 0 )
    {
      FHTLedVal |= (0x01<<i);                    // Setting a bit
    }else
    {
      FHTLedVal &= ~(0x01<<i);                   // Clearing a bit
    }
    
    FTvalue = TFFloor(FloorNum[i].devNum[0]);
    
    switch (FTvalue){
    case 0:
      FHTLedVal &= ~(0x01<<(i+4));               // Clearing a red bit
      break;
    case 1:
      FHTLedVal &= ~(0x01<<i);                   // Clearing a green bit
      FHTLedVal |= (0x01<<(i+4));                // Setting a red bit
      break;
    }
  } 
}
/*
*********************************************************************************************************
*/
void TFScan(INT8U hc1, INT8U hc2, INT8U hc3, INT8U hc4)
{
  logMsgTF[0] ='\0';
  sprintf((char*)logMsgTF," %02d %03d %02d %02d\r\n\0", 0x63,0x63,0x63,0x63);    // 99 099 99 99
  
  TFNr = TFDevNr(hc1, hc2, hc3);
  
  if( TFDevEnabled( hc1, hc2, hc3) != -1)
  {
    if (TFNr == -1)
    {
      TFDev[TFDevAct].hc1    = hc1;
      TFDev[TFDevAct].hc2    = hc2;
      TFDev[TFDevAct].hc3    = hc3;
      TFNr = TFDevAct;
      TFDevAct++;
    }  
        
    if ( TFDev[TFNr].command != hc4 )
    {
      TFDev[TFNr].command = hc4;
      TFValue(TFNr);
      
      logMsg[0] ='\0';
      sprintf((char*)logMsg," %02d %03d %02d %02d\r\n\0", hc1,hc2,hc3,hc4);
    }
  }else{
      
    if ((hc1 != 0x00) && (hc2 != 0x00) && (hc3 != 0x00))
    {
      if ((hc4 == 0x82) || (hc4 == 0x81) || (hc4 == 0x02) || (hc4 == 0x01))
      {
        logMsgTF[0] ='\0';
        sprintf((char*)logMsgTF," %02d %03d %02d %02d\r\n\0", hc1,hc2,hc3,hc4);
      }
    }    
  }
}

/*
*********************************************************************************************************
*/
void FHTScan(INT8U hc1, INT8U hc2, INT8U hc4, INT8U hc5)
{

  FHTNr = FHTDevNr(hc1, hc2);
  
  if( FHTDevEnabled(hc1, hc2) != -1 )
  {
    if (FHTNr == -1)
      {
        FHTDev[FHTDevAct].hc1    = hc1;
        FloorNum[(hc1 / 10) - 1].devNum[ FloorNum[(hc1 / 10) - 1].devAct ] = FHTDevAct;      
        FHTDev[FHTDevAct].hc2    = hc2;
        FHTNr = FHTDevAct;
        FloorNum[(hc1 / 10) - 1].devAct++;
        FHTDevAct++;
      }  
    
    
    if ((hc4 != 0x67) && (hc5 != 0x64)){
      
      if((FHTDev[FHTNr].command != hc4) || (FHTDev[FHTNr].extension != hc5))
      {

         FHTDev[FHTNr].command = hc4;
         FHTDev[FHTNr].extension = hc5;
         FHTFloor(FHTNr);
         
         logMsg[0] ='\0';
         sprintf((char*)logMsg," %02d%02d %02d %02d\r\n\0", hc1,hc2,hc4,hc5);
      }
    }
  }
}

/*
*********************************************************************************************************
*/
void ReceiverScan(void)
{  
  
  if (n > 0)
  {
    if (n == 5)
    {
      TFScan(TempBuf[0], TempBuf[1], TempBuf[2], TempBuf[3]);
    }
    
    if (n == 6)
    {
      FHTScan(TempBuf[0], TempBuf[1], TempBuf[3],  TempBuf[4]);      
    }

    n = 0;
  }
}

#endif
