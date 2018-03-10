/*
*********************************************************************************************************
*                                        RTC MODULE SOURCE FILE
*
* Filename      : rtc.c
* Version       : V0.1
* Programmer(s) : Puleo
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/
#define  RTC_GLOBALS
#include "includes.h"

#if RTC_MODULE == DEF_ENABLED
/*
*********************************************************************************************************
*                                            LOCAL CONSTANTS
*********************************************************************************************************
*/

const INT8U Days_Table[12]={31,28,31,30,31,30,31,31,30,31,30,31};

/*
*********************************************************************************************************
*                                            LOCAL VARIABLES
*********************************************************************************************************
*/
//struct tm time_now;
CPU_INT08U time_buffer[20]="0000-00-00 00:00:00";
/*
*********************************************************************************************************
*                                        LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

// Convert binary coded decimal to normal decimal numbers
INT8U bcdToDec(INT8U val)
{
  return ( (val/16*10) + (val%16) );
}

/*******************************************************************************
* Function Name : isDST
* Description : checks if given time is in Daylight Saving time-span.
* Input : time-struct, must be fully populated including weekday
* Output : none
* Return : false: no DST ("winter"), true: in DST ("summer")
* DST according to German standard
* Based on code from Peter Dannegger found in the microcontroller.net forum.
*******************************************************************************/
static BOOLEAN isDST( struct tm t )
{
  INT8U wday, month;	// locals for faster access
  
  
  month = t.tm_mon;

  if( month < 3 || month > 10 ) {	// month 1, 2, 11, 12
    return false;	// -> Winter
  }

  wday = t.tm_wday;

  if( t.tm_mday - wday >= 25 && (wday || t.tm_hour >= 2) ) { // after last Sunday 2:00
    if( month == 10 ) {	// October -> Winter
      return false;
    }
  } else {	// before last Sunday 2:00
    if( month == 3 ) {	// March -> Winter
      return false;
    }
  }
  
  return true;
}

/*******************************************************************************
* Function Name : adjustDST
* Description : adjusts time to DST if needed
* Input : non DST time-struct, must be fully populated including weekday
* Output : time-stuct gets modified
* Return : false: no DST ("winter"), true: in DST ("summer")
* DST according to German standard
* Based on code from Peter Dannegger found in the mikrocontroller.net forum.
*******************************************************************************/
BOOLEAN adjustDST( struct tm t )
{
  INT8U hour, day, wday, month;	// locals for faster access

  hour = t.tm_hour;
  day = t.tm_mday;
  wday = t.tm_wday;
  month = t.tm_mon;

  if ( isDST(t) ) {
    t.tm_isdst = 1;
    hour++;	// add one hour
    if( hour == 24 ){	// next day
      hour = 0;
      wday++;	// next weekday
      if( wday == 7 ) {
        wday = 0;
      }
      if( day == Days_Table[month-1] ) {	// next month
        day = 0;
        month++;
      }
      day++;
    }
    t.tm_mon = month;
    t.tm_hour = hour;
    t.tm_mday = day;
    t.tm_wday = wday;
    return true;
  } else {
    t.tm_isdst = 0;
    return false;
  }
}

/*
*********************************************************************************************************
*/
INT8U* getDate(void)
{
  time_now = Time_GetCalendarTime();
  
  time_buffer[0] = bcdToDec(time_now.tm_sec);  // second
  time_buffer[1] = bcdToDec(time_now.tm_min);  // minute
  time_buffer[2] = bcdToDec(time_now.tm_hour); // hour
  time_buffer[3] = bcdToDec(time_now.tm_wday); // dayOfWeek
  time_buffer[4] = bcdToDec(time_now.tm_mday); // dayOfMonth
  time_buffer[5] = bcdToDec(time_now.tm_mon);  // month
  time_buffer[6] = bcdToDec(time_now.tm_year); // year
  
  return time_buffer;
}

/*******************************************************************************
* Function Name  : INT8U Is_LeapYear(INT16U year)
* Description    : Get Leap Year
* Input          : Year
* Return         : Yes, or False
*******************************************************************************/

static INT8U Is_LeapYear(INT16U year)
{         
  
   if(year%4==0)               //????4??
   {
     if(year%100==0)
     {
       if(year%400==0){
        return 1;      
       } else{
         return 0;
       }
     }else{
      return 1;  
     }
   }else{
     return 0; 
   }
}

/*******************************************************************************
* Function Name  : Time_ConvWiflyRTC(INT8U *s, INT8U p)
* Description    : NTP time Convert to LocalTime
* Input          : RTC Time of NTP, Time zone
* Output         : None
* Return         : struct tm
*******************************************************************************/
void Time_ConvUSBRTC(INT32U s)
{
   INT32U secs,days,temp,years = 1970,months = 0;    
	
   secs = s;
	 
    days = secs/86400;
    if(days > 0)    
    {
        temp = days;
        while(temp >= 365)  
        {
            if(Is_LeapYear(years))
            {
                if(temp >= 366)
                    temp -= 366;  
                else
                    break;
            }else{
                temp -= 365;
            }          
            years++;
        }
	         
        time_now.tm_year = years; 
 
        while(days >= 28)
        {
            if(Is_LeapYear(years) && months ==1)
            {
                if(temp >= 29)
                    temp -= 29;
                else
                    break;
            }else{
                if(temp >= Days_Table[months])      
                    temp -= Days_Table[months];
                else
                    break;
            }
	 
            months++;  
        }
	 
        time_now.tm_mon = months+1; 
        time_now.tm_mday  = temp+1; 
    }
	 
    temp = secs % 86400;            
    time_now.tm_hour = temp/3600 + 2;    //UTC +1
    time_now.tm_min = (temp%3600)/60;         
    time_now.tm_sec = (temp%3600)%60;
//    adjustDST(time_now);
    Time_SetCalendarTime(time_now);
}
/*******************************************************************************
* Function Name  : Time_ConvUnixToCalendar(time_t t)
* Description    : UnixTime Convert to LocalTime
* Input          : 
* Output         : None
* Return         : struct tm
*******************************************************************************/
struct tm Time_ConvUnixToCalendar(time_t t)
{
        struct tm *t_tm;
        
        t_tm = localtime(&t);
        t_tm->tm_year += 1900;  // We want years since 1900... 
        return *t_tm;
}

/*******************************************************************************
* Function Name  : Time_ConvCalendarToUnix(struct tm t)
* Description    : 
* Input          : struct tm t
* Output         : None
* Return         : time_t
*******************************************************************************/
time_t Time_ConvCalendarToUnix(struct tm t)
{
        t.tm_year -= 1900;  
        return mktime(&t);
}

/*******************************************************************************
* Function Name  : Time_GetUnixTime()
* Description    :
* Input          : None
* Output         : None
* Return         : time_t t
*******************************************************************************/
time_t Time_GetUnixTime(void)
{
        return (time_t)RTC_GetCounter();
}

/*******************************************************************************
* Function Name  : Time_GetCalendarTime()
* Description    :
* Input          : None
* Output         : None
* Return         : time_t t
*******************************************************************************/
struct tm Time_GetCalendarTime(void)
{
        time_t t_t;
        struct tm t_tm;

        t_t = (time_t)RTC_GetCounter();
        t_tm = Time_ConvUnixToCalendar(t_t);
        return t_tm;
}

/*******************************************************************************
* Function Name  : Time_SetUnixTime()
* Description    :
* Input          : time_t t
* Output         : None
* Return         : None
*******************************************************************************/
void Time_SetUnixTime(time_t t)
{
        RTC_WaitForLastTask();
        RTC_SetCounter((u32)t);
        RTC_WaitForLastTask();
        return;
}

/*******************************************************************************
* Function Name  : Time_SetCalendarTime()
* Description    :
* Input          : struct tm t
* Output         : None
* Return         : None
*******************************************************************************/
void Time_SetCalendarTime(struct tm t)
{
        Time_SetUnixTime(Time_ConvCalendarToUnix(t));
        return;
}

/*******************************************************************************
* Function Name  : Get_Time_Now
* Description    : 
* Input          : None
* Output         : CPU_INT08U *set_buffer
* Return         : None
*******************************************************************************/
INT8U* Get_Time_Now()
{

  time_now = Time_GetCalendarTime();
  
  time_buffer[0]=time_now.tm_year/1000+'0';
  time_buffer[1]=(time_now.tm_year%1000)/100+'0';
  time_buffer[2]=(time_now.tm_year%100)/10+'0';
  time_buffer[3]=time_now.tm_year%10+'0';
  
  time_buffer[5]=((time_now.tm_mon)%100)/10+'0';
  time_buffer[6]=(time_now.tm_mon)%10+'0';
  
  time_buffer[8]=(time_now.tm_mday%100)/10+'0';
  time_buffer[9]=time_now.tm_mday%10+'0';

  time_buffer[11]=(time_now.tm_hour%100)/10+'0';
  time_buffer[12]=time_now.tm_hour%10+'0';

  time_buffer[14]=(time_now.tm_min%100)/10+'0';
  time_buffer[15]=time_now.tm_min%10+'0';

  time_buffer[17]=(time_now.tm_sec%100)/10+'0';
  time_buffer[18]=time_now.tm_sec%10+'0';


  return time_buffer;
}

/*******************************************************************************
* Function Name  : Set_DateTime
* Description    : 
* Input          : CPU_INT08U *set_buffer
* Output         : time_now
* Return         : None
*******************************************************************************/
void Set_DateTime(CPU_INT08U *set_buffer)
{
  time_now.tm_year = (*set_buffer-'0')*1000+(*(set_buffer+1)-'0')*100+(*(set_buffer+2)-'0')*10+(*(set_buffer+3)-'0');
  time_now.tm_mon =  (*(set_buffer+5)-'0')*10+(*(set_buffer+6)-'0'-1);
  time_now.tm_mday = (*(set_buffer+8)-'0')*10+(*(set_buffer+9)-'0');
  time_now.tm_hour = (*(set_buffer+11)-'0')*10+(*(set_buffer+12)-'0');
  time_now.tm_min =  (*(set_buffer+14)-'0')*10+(*(set_buffer+15)-'0');
  time_now.tm_sec =  (*(set_buffer+17)-'0')*10+(*(set_buffer+18)-'0');
  Time_SetCalendarTime(time_now);
}

/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Configuration(void)
{
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

        PWR_BackupAccessCmd(ENABLE);

        BKP_DeInit();

        RCC_LSEConfig(RCC_LSE_ON);
        
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

        RCC_RTCCLKCmd(ENABLE);

        RTC_WaitForSynchro();

        RTC_WaitForLastTask();

        RTC_SetPrescaler(32767);

        RTC_WaitForLastTask();

        RTC_ITConfig(RTC_IT_SEC, ENABLE);

        RTC_WaitForLastTask();

        return;
}

/*******************************************************************************
* Function Name  : RTC_Config
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Config(void)
{
        if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
        {
                RTC_Configuration();
                BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
        }
        else
        {
                if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
                {
                        ;
                }
                else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
                {
                        ;
                }
                RCC_ClearFlag();

                RCC_RTCCLKCmd(ENABLE);

                RTC_WaitForSynchro();

                RTC_ITConfig(RTC_IT_SEC, ENABLE);

                RTC_WaitForLastTask();
        }
        RCC_ClearFlag();

        /* Enable PWR and BKP clocks */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);

        return;
}

/*
*********************************************************************************************************
*                                       Disable RTC Interrupts
*********************************************************************************************************
*/

void  RTC_IntDis (void)
{
    RTC_ITConfig(RTC_IT_SEC, DISABLE);
}

/*
*********************************************************************************************************
*                                       Enable RTC Interrupts
*********************************************************************************************************
*/

void  RTC_IntEn (void)
{
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
}

#endif