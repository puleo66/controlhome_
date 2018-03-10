/*
*********************************************************************************************************
*
*                                        RTC MODULE SOURCE FILE
*
*
* Filename      : rtc.h
* Version       : V0.1
* Programmer(s) : Puleo
*********************************************************************************************************
*/
#ifdef   RTC_GLOBALS
#define  RTC_EXT
#else
#define  RTC_EXT  extern
#endif

#ifndef  RTC_H
#define  RTC_H


/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include  <time.h>
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
RTC_EXT         struct tm time_now;


/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/
RTC_EXT         void    Time_ConvUSBRTC(INT32U s);
RTC_EXT         struct tm   Time_ConvUnixToCalendar(time_t t);
RTC_EXT         time_t      Time_ConvCalendarToUnix(struct tm t);
RTC_EXT         time_t      Time_GetUnixTime(void);
RTC_EXT         struct tm   Time_GetCalendarTime(void);
RTC_EXT         void        Time_SetUnixTime(time_t);
RTC_EXT         void        Time_SetCalendarTime(struct tm t);

RTC_EXT         void        Set_DateTime(CPU_INT08U *set_buffer);
RTC_EXT         INT8U*      Get_Time_Now();
RTC_EXT         INT8U*      getDate(void);

                void        RTC_Configuration(void);
RTC_EXT         void        RTC_Config(void);
RTC_EXT         void        RTC_IntDis (void);
RTC_EXT         void        RTC_IntEn (void);
/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/


#endif                                                          /* End of module include.                               */
