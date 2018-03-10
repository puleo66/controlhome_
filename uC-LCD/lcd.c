/*
*********************************************************************************************************
*                                        LCD MODULE SOURCE FILE
*
* Filename      : lcd.c
* Version       : V0.1
* Programmer(s) : Puleo
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include "includes.h"
#if LCD_MODULE == DEF_ENABLED
/*
*********************************************************************************************************
*                                            LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL VARIABLES
*********************************************************************************************************
*/

/* Cache buffer in SRAM 84*48 bits or 504 bytes */
static CPU_CHAR  LcdCache [ LCD_CACHE_SIZE ];

/* Log Cache buffer  */
//static CPU_CHAR  LogCache[69];
//static CPU_INT16S LogCasheCount= 0;

/* Cache index */
static CPU_INT32S   LcdCacheIdx;

/* Lower part of water mark */
static CPU_INT32S   LoWaterMark;

/* Higher part of water mark */
static CPU_INT32S   HiWaterMark;

//extern CPU_INT08U time_buffer[20];
//static CPU_CHAR taskMessage[20];
//static CPU_INT08U msg[20];
/*
*********************************************************************************************************
*                                        LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
//static void strTimecpy(void);

/*
*********************************************************************************************************
*  Name         :  LcdInit
*  Description  :  Performs MCU SPI & LCD controller initialization.
*  Argument(s)  :  None.
*  Return value :  None.
********************************************************************************************************
*/

void LcdInit ( void )
{
    LcdSend( 0x21, LCD_CMD ); /* LCD Extended Commands. */
    LcdSend( 0x05, LCD_CMD );
    LcdSend( 0xC8, LCD_CMD ); /* Set LCD Vop (Contrast).*/
    LcdSend( 0x06, LCD_CMD ); /* Set Temp coefficent. */
    LcdSend( 0x14, LCD_CMD ); /* LCD bias mode 1:48. */
//    LcdSend( 0x12, LCD_CMD ); /* LCD bias mode 1:68. */
    
    
    LcdSend( 0x20, LCD_CMD ); /* LCD Standard Commands,Horizontal addressing mode */
    LcdSend( 0x01, LCD_CMD );
    LcdSend( 0x0C, LCD_CMD ); /* LCD in normal mode. */
    LcdSend( 0x40, LCD_CMD );
    LcdSend( 0x80, LCD_CMD );
    
    LoWaterMark = LCD_CACHE_SIZE; /* Reset watermark pointers to empty */
    HiWaterMark = 0;
    LogActual = 0;
    
    LcdClear(); /* Clear display on first time use */   
    
}

/*
*********************************************************************************************************
* Name         :  LcdContrast
* Description  :  Set display contrast.
* Argument(s)  :  contrast -> Contrast value from 0x00 to 0x7F.
* Return value :  None.
*********************************************************************************************************
*/
void LcdContrast ( CPU_INT08U contrast )
{
    /* LCD Extended Commands. */
    LcdSend( 0x21, LCD_CMD );

    /* Set LCD contrast level. */
    LcdSend( 0x80 | contrast, LCD_CMD );

    /* LCD Standard Commands, horizontal addressing mode. */
    LcdSend( 0x20, LCD_CMD );
}

/*
********************************************************************************************************
* Name         :  LcdClear
* Description  :  Clears the display. LcdUpdate must be called next.
* Argument(s)  :  None.
* Return value :  None.
* Note         :  Based on Sylvain Bissonette's code
********************************************************************************************************
*/
void LcdClear ( void )
{
    DispLock();
    memset(LcdCache,0x00,LCD_CACHE_SIZE); 
    /* Reset watermark pointers to full */
    LoWaterMark = 0;
    HiWaterMark = LCD_CACHE_SIZE - 1;

    /* Set update */
    LcdUpdate();
    DispUnlock();
    LcdGotoXYFont(1,6);
    LcdStr(FONT_1X,"              ");
}

/*
********************************************************************************************************
* Name         :  LcdNumber
* Description  :  Decimal numbers convert to Lcd string
* Argument(s)  :  n  -> str
* Return value :  none
********************************************************************************************************
*/
void LcdNumber(LcdFontSize size, CPU_INT08U x, CPU_INT08U y, CPU_INT32S n) 
{
  CPU_CHAR str[80];
  
 
  LcdGotoXYFont(x,y);
  sprintf ((char*)str, "%d", n);
  LcdStr(size, str);
}

/*
********************************************************************************************************
* Name         :  LcdHex
* Description  :  HexDecimal number convert to Lcd string
* Argument(s)  :  HexByte  -> str (0x00)
* Return value :  none
********************************************************************************************************
*/
void LcdHex( LcdFontSize size, CPU_INT08U x,CPU_INT08U y, CPU_INT08U data_ )
{
 CPU_INT08U h1,h2;

    DispLock();
    h1=data_&0xf0;
    h1=h1>>4;
    h2=data_&0x0f;
    
    if(h1<10)
      h1+=0x30;
    else 
      h1+=0x37;
    
    if(h2<10)
      h2+=0x30;
    else 
      h2+=0x37 ;
    
    LcdGotoXYFont(x++,y);
    LcdChr (size, h1 );
    LcdGotoXYFont(x++,y);
    LcdChr (size, h2 );
      
      /* Set update */
    LcdUpdate();
    DispUnlock();
}

/*
********************************************************************************************************
* Name         :  LcdWord
* Description  :  HexDecimal numbers convert to decimal Lcd string
* Argument(s)  :  HexWord  -> str (0x0000)
* Return value :  none
********************************************************************************************************
*/
void LcdWord( LcdFontSize size, CPU_INT08U x, CPU_INT08U y, CPU_INT16U data_ )
{
  CPU_INT16U h;
  CPU_INT08U d1,d2,d3,d4,d5,d6;
  
    DispLock();
    h=data_;
    d1=((h%=1000000)/100000)+48;
    d2=((h%=100000)/10000)+48;
    d3=((h%=10000)/1000)+48;
    d4=((h%=1000)/100)+48;
    d5=((h%=100)/10)+48;
    d6=(h%=10)+48;
  
    LcdGotoXYFont(x++,y);
    LcdChr (size, d1 );
    LcdGotoXYFont(x++,y);
    LcdChr (size, d2 );
    LcdGotoXYFont(x++,y);
    LcdChr (size, d3 );
    LcdGotoXYFont(x++,y);
    LcdChr (size, d4 );
    LcdGotoXYFont(x++,y);
    LcdChr (size, d5 );
    LcdGotoXYFont(x++,y);
    LcdChr (size, d6 );
  
    /* Set update */
    LcdUpdate();
    DispUnlock();
}

/*
********************************************************************************************************
* Name         :  LcdGotoXYFont
* Description  :  Sets cursor location to xy location corresponding to basic
*                 font size.
* Argument(s)  :  x, y -> Coordinate for new cursor position. Range: 1,1 .. 14,6
* Return value :  see return value in pcd8544.h
* Note         :  Based on Sylvain Bissonette's code
********************************************************************************************************
*/
CPU_INT08U LcdGotoXYFont ( CPU_INT08U x, CPU_INT08U y )
{
    /* Boundary check, slow down the speed but will guarantee this code wont fail */
    /* Version 0.2.5 - Fixed on Dec 25, 2008 (XMAS) */
    if( x > 14)  
        return OUT_OF_BORDER;
    if( y > 6)
        return OUT_OF_BORDER;
    /*  Calculate index. It is defined as address within 504 bytes memory */

    LcdCacheIdx = ( x - 1 ) * 6 + ( y - 1 ) * 84;
    return OK;
}

/*
*******************************************************************************************************
* Name         :  LcdChr
* Description  :  Displays a character at current cursor location and
*                 increment cursor location.
* Argument(s)  :  size -> Font size. See enum in pcd8544.h.
*                 ch   -> Character to write.
* Return value :  see pcd8544.h about return value
*******************************************************************************************************
*/
CPU_INT08U LcdChr ( LcdFontSize size, CPU_INT08U ch )
{
    CPU_INT08U i, c;
    CPU_INT08U b1, b2;
    CPU_INT16S tmpIdx;

    if ( LcdCacheIdx < LoWaterMark )
    {
        /* Update low marker. */
        LoWaterMark = LcdCacheIdx;
    }

    if ( (ch < 0x20) || (ch > 0x7b) )
    {
        /* Convert to a printable character. */
        ch = 92;
    }

    if ( size == FONT_1X )
    {
        for ( i = 0; i < 5; i++ )
        {
          /* Copy lookup table from Flash ROM to LcdCache */
          LcdCache[LcdCacheIdx++] = FontLookup[ ch - 32 ][ i ]  << 1;
        }
    }
    else if ( size == FONT_2X )
    {
        tmpIdx = LcdCacheIdx - 84;

        if ( tmpIdx < LoWaterMark )
        {
            LoWaterMark = tmpIdx;
        }

        if ( tmpIdx < 0 ) return OUT_OF_BORDER;

        for ( i = 0; i < 5; i++ )
        {
            /* Copy lookup table from Flash ROM to temporary c */
            c = FontLookup[ch - 32][i] << 1;
            /* Enlarge image */
            /* First part */
            b1 =  (c & 0x01) * 3;
            b1 |= (c & 0x02) * 6;
            b1 |= (c & 0x04) * 12;
            b1 |= (c & 0x08) * 24;

            c >>= 4;
            /* Second part */
            b2 =  (c & 0x01) * 3;
            b2 |= (c & 0x02) * 6;
            b2 |= (c & 0x04) * 12;
            b2 |= (c & 0x08) * 24;

            /* Copy two parts into LcdCache */
            LcdCache[tmpIdx++] = b1;
            LcdCache[tmpIdx++] = b1;
            //LcdCache[tmpIdx + 82] = b2;
            //LcdCache[tmpIdx + 83] = b2;
            LcdCache[tmpIdx + 100] = b2;
            LcdCache[tmpIdx + 101] = b2;
        }
        
        /* Update x cursor position. */
        /* Version 0.2.5 - Possible bug fixed on Dec 25,2008 */
        //LcdCacheIdx = (LcdCacheIdx + 11) % LCD_CACHE_SIZE;
        LcdCacheIdx = (LcdCacheIdx + 28) % LCD_CACHE_SIZE;
    }

    if ( LcdCacheIdx > HiWaterMark )
    {
        /* Update high marker. */
        HiWaterMark = LcdCacheIdx;
    }

    /* Horizontal gap between characters. */
    /* Version 0.2.5 - Possible bug fixed on Dec 25,2008 */
    LcdCache[LcdCacheIdx] = 0x00;
    /* At index number LCD_CACHE_SIZE - 1, wrap to 0 */
    if(LcdCacheIdx == (LCD_CACHE_SIZE - 1) )
    {
        LcdCacheIdx = 0;
        return OK_WITH_WRAP;
    }
    /* Otherwise just increment the index */
    if ( size == FONT_2X )
     LcdCacheIdx-=16;
    else
     LcdCacheIdx++; 
    
    return OK;
}

/*
*******************************************************************************************************
* Name         :  LcdStr
* Description  :  Displays a character at current cursor location and increment
*                 cursor location according to font size. This function is
*                 dedicated to print string laid in SRAM
* Argument(s)  :  size      -> Font size. See enum.
*                 dataArray -> Array contained string of char to be written
*                              into cache.
* Return value :  see return value on pcd8544.h
*******************************************************************************************************
*/
CPU_INT08U LcdStr ( LcdFontSize size, CPU_INT08U dataArray[] )
{
    CPU_INT08U tmpIdx=0;
    CPU_INT08U response;
    
    DispLock();
    while( dataArray[ tmpIdx ] != '\0' )
	{
        /* Send char */
		response = LcdChr( size, dataArray[ tmpIdx ] );
        /* Just in case OUT_OF_BORDER occured */
        /* Dont worry if the signal == OK_WITH_WRAP, the string will
        be wrapped to starting point */
        if( response == OUT_OF_BORDER)
            return OUT_OF_BORDER;
        /* Increase index */
	tmpIdx++;
	}
    /* Set update */
    LcdUpdate();
    DispUnlock();
    return OK;
}

/*
*******************************************************************************************************
* Name         :  LcdNStr
* Description  :  Displays a character at current cursor location and increment
*                 cursor location according to font size. This function is
*                 dedicated to print string laid in SRAM
* Argument(s)  :  size      -> Font size. See enum.
*                 dataArray -> Array contained string of char to be written
*                              into cache.
* Return value :  see return value on pcd8544.h
*******************************************************************************************************
*/
CPU_INT08U LcdNStr ( LcdFontSize size, CPU_INT08U *dataArray, CPU_INT08U from, CPU_INT08U to)
{
    CPU_INT08U tmpIdx=from;
    CPU_INT08U response;
    
    DispLock();
    while( tmpIdx  != to )
	{
        /* Send char */
		response = LcdChr( size, dataArray[ tmpIdx ] );
        /* Just in case OUT_OF_BORDER occured */
        /* Dont worry if the signal == OK_WITH_WRAP, the string will
        be wrapped to starting point */
        if( response == OUT_OF_BORDER)
            return OUT_OF_BORDER;
        /* Increase index */
	tmpIdx++;
	}
    /* Set update */
    LcdUpdate();
    DispUnlock();
    return OK;
}

/*******************************************************************************
* Log Time Copy
*
********************************************************************************
*/
/*
static void strTimecpy(void)
{
  CPU_INT16S i= 1;
  
  
  Get_Time_Now();
  strcpy((char *)taskMessage,"[");
  strncat((char *)taskMessage,(char const *)time_buffer, strlen((char const *)time_buffer));
  
  while ((i + 11) != 17)
  {
    taskMessage[ i ] = taskMessage[ i + 11];
    i++;
  }       
  taskMessage[6]= '\0';
}
*/
/*
*******************************************************************************************************
* Name         :  LcdLogStr
* Description  :  Displays a character at current cursor location and increment
*                 cursor location according to font size. This function is
*                 dedicated to print string laid in SRAM
* Argument(s)  :  size      -> Font size. See enum.
*                 dataArray -> Array contained string of char to be written
*                              into cache.
* Return value :  see return value on pcd8544.h
*******************************************************************************************************
*/
/*
CPU_INT08U LcdLogStr ( LcdFontSize size, CPU_INT08U message[], CPU_INT08U task[])
{
    CPU_INT08U tmpIdx= 0;
    CPU_INT08U response;
    CPU_INT16S i,j;
    
    DispLock();
    if (LogCasheCount > 1)
    {
      LogCasheCount= 1;
      i= 0;
      j= 34;                              //from three Line to one line
      while( LogCache[ j ] != '\0' )
      {
        LogCache[ i ] = LogCache[ j ];
        i++;
        j++;
      }
      LogCache[ 34 ] = '\0';
    }
    
    strTimecpy();
    strncat((char *)taskMessage,"] ", 2);
    strncat((char *)taskMessage,(char *)task, 9);
    
    if (LogCasheCount != 0)
      strncat((char *)LogCache,(char *)taskMessage,strlen((char const *)taskMessage));
    else
       strcpy((char *)LogCache,(char const *)taskMessage);
    strncat((char *)LogCache,(char *)message,strlen((char const *)message));
    LogCache[ 68 ] = '\0';
    
    LcdCacheIdx= 1;
    LogCasheCount++;
    
    while( LogCache[ tmpIdx ] != '\0' )
	{
        // Send char 
	response = LcdChr( size, LogCache[ tmpIdx ] );
        if( response == OUT_OF_BORDER)
            return OUT_OF_BORDER;
        // Increase index 
	tmpIdx++;
	}
    // Set update 
    LcdUpdate();
    DispUnlock();
    return OK;
}

*/
/*
*********************************************************************************************************
*/
void LcdListLog( CPU_INT08U index, CPU_INT08U *message)
{
  LcdGotoXYFont(1 ,1 + index);
  LcdStr ( FONT_1X, (unsigned char*)message );
}

/*
*********************************************************************************************************
*/
void LcdLog( CPU_INT08U *message)
{
  CPU_INT08U i;
  

  if (LogActual == 6){
    for (i = 0; i < 6; i++){
      strcpy((char*)LogMessage[i], (char const*)LogMessage[i+1]);
    }
  }
  
  strcpy((char*)LogMessage[LogActual], (char const*)message);
  
  if(LogActual != 6)
    LogActual++;
  
  for (i= 0; i < LogActual; i++){
    LcdGotoXYFont(1 ,1 + i);
    LcdStr ( FONT_1X, (unsigned char*)LogMessage[i] );
  }
  
}

/*
*******************************************************************************************************
* Name         :  LcdFStr
* Description  :  Displays a characters at current cursor location and increment
*                 cursor location according to font size. This function is
*                 dedicated to print string laid in Flash ROM
* Argument(s)  :  size    -> Font size. See enum.
*                 dataPtr -> Pointer contained string of char to be written
*                            into cache.
* Return value :  see return value on pcd8544.h
* Example      :  LcdFStr(FONT_1X, "Hello World");
*                 LcdFStr(FONT_1X, &name_of_string_as_array);
*******************************************************************************************************
*/
CPU_INT08U LcdFStr ( LcdFontSize size, const CPU_INT08U *dataPtr )
{
    CPU_INT08U c;
    CPU_INT08U response;
    
    DispLock();
    for ( c = (*(const CPU_INT08U *)(dataPtr)); c; ++dataPtr, c = (*(const CPU_INT08U *)(dataPtr)))
    {
        /* Put char */
        response = LcdChr( size, c );
        if(response == OUT_OF_BORDER)
            return OUT_OF_BORDER;
    }
    /* Set update */
    LcdUpdate();
    DispUnlock();
    return OK;
}

/*
*******************************************************************************************************
* Name         :  LcdPixel
* Description  :  Displays a pixel at given absolute (x, y) location.
* Argument(s)  :  x, y -> Absolute pixel coordinates
*                 mode -> Off, On or Xor. See enum in pcd8544.h.
* Return value :  see return value on pcd8544.h
* Note         :  Based on Sylvain Bissonette's code
*******************************************************************************************************
*/
CPU_INT08U LcdPixel ( CPU_INT08U x, CPU_INT08U y, LcdPixelMode mode )
{
    CPU_INT16U  index;
    CPU_INT08U  offset;
    CPU_INT08U  data;

    DispLock();
    /* Prevent from getting out of border */
    if ( x > LCD_X_RES ) {DispUnlock(); return OUT_OF_BORDER;}
    if ( y > LCD_Y_RES ) { DispUnlock(); return OUT_OF_BORDER;}

    /* Recalculating index and offset */
    index = ( ( y / 8 ) * 84 ) + x;
    offset  = y - ( ( y / 8 ) * 8 );

    data = LcdCache[ index ];

    /* Bit processing */

	/* Clear mode */
    if ( mode == PIXEL_OFF )
    {
        data &= ( ~( 0x01 << offset ) );
    }

    /* On mode */
    else if ( mode == PIXEL_ON )
    {
        data |= ( 0x01 << offset );
    }

    /* Xor mode */
    else if ( mode  == PIXEL_XOR )
    {
        data ^= ( 0x01 << offset );
    }

    /* Final result copied to cache */
    LcdCache[ index ] = data;

    if ( index < LoWaterMark )
    {
        /*  Update low marker. */
        LoWaterMark = index;
    }

    if ( index > HiWaterMark )
    {
        /*  Update high marker. */
        HiWaterMark = index;
    }
    /* Set update */
    LcdUpdate();
    DispUnlock();
    return OK;
}

/*
*******************************************************************************************************
* Name         :  LcdLine
* Description  :  Draws a line between two points on the display.
* Argument(s)  :  x1, y1 -> Absolute pixel coordinates for line origin.
*                 x2, y2 -> Absolute pixel coordinates for line end.
*                 mode   -> Off, On or Xor. See enum in pcd8544.h.
* Return value :  see return value on pcd8544.h
*******************************************************************************************************
*/
CPU_INT08U LcdLine ( CPU_INT08U x1, CPU_INT08U x2, CPU_INT08U y1, CPU_INT08U y2, LcdPixelMode mode )
{
    CPU_INT32S dx, dy, stepx, stepy, fraction;
    CPU_INT08U response;

    /* Calculate differential form */
    /* dy   y2 - y1 */
    /* -- = ------- */
    /* dx   x2 - x1 */

    /* Take differences */
    dy = y2 - y1;
    dx = x2 - x1;

    /* dy is negative */
    if ( dy < 0 )
    {
        dy    = -dy;
        stepy = -1;
    }
    else
    {
        stepy = 1;
    }

    /* dx is negative */
    if ( dx < 0 )
    {
        dx    = -dx;
        stepx = -1;
    }
    else
    {
        stepx = 1;
    }

    dx <<= 1;
    dy <<= 1;

    /* Draw initial position */
    response = LcdPixel( x1, y1, mode );
    if(response)
        return response;

    /* Draw next positions until end */
    if ( dx > dy )
    {
        /* Take fraction */
        fraction = dy - ( dx >> 1);
        while ( x1 != x2 )
        {
            if ( fraction >= 0 )
            {
                y1 += stepy;
                fraction -= dx;
            }
            x1 += stepx;
            fraction += dy;

            /* Draw calculated point */
            response = LcdPixel( x1, y1, mode );
            if(response)
                return response;

        }
    }
    else
    {
        /* Take fraction */
        fraction = dx - ( dy >> 1);
        while ( y1 != y2 )
        {
            if ( fraction >= 0 )
            {
                x1 += stepx;
                fraction -= dy;
            }
            y1 += stepy;
            fraction += dx;

            /* Draw calculated point */
            response = LcdPixel( x1, y1, mode );
            if(response)
                return response;
        }
    }

    return OK;
}

/*
*********************************************************************************************************
*/
CPU_INT08U LcdCircle(CPU_INT08U x, CPU_INT08U y, CPU_INT08U radius, LcdPixelMode mode)
{
    CPU_INT08S xc = 0;
    CPU_INT08S yc = 0;
    CPU_INT08S p = 0;

    if ( x >= LCD_X_RES || y >= LCD_Y_RES) return OUT_OF_BORDER;

    yc = radius;
    p = 3 - (radius<<1);
    while (xc <= yc)
    {
        LcdPixel(x + xc, y + yc, mode);
        LcdPixel(x + xc, y - yc, mode);
        LcdPixel(x - xc, y + yc, mode);
        LcdPixel(x - xc, y - yc, mode);
        LcdPixel(x + yc, y + xc, mode);
        LcdPixel(x + yc, y - xc, mode);
        LcdPixel(x - yc, y + xc, mode);
        LcdPixel(x - yc, y - xc, mode);
        if (p < 0) p += (xc++ << 2) + 6;
            else p += ((xc++ - yc--)<<2) + 10;
    }

    return OK;
}

/*******************************************************************************
*
* Widgets!!!!
*
********************************************************************************
*/

void LcdBatt(int x1, int y1, int x2, int y2, CPU_INT08U persent)
{
    CPU_INT08U horizon_line,horizon_line2,i;
        
    if(persent>100)
      return;
        
    LcdLine(x1,x2,y2,y2,PIXEL_ON);  //down
    LcdLine(x2,x2,y1,y2,PIXEL_ON);  //right
    LcdLine(x1,x1,y1,y2,PIXEL_ON);  //left
    LcdLine(x1,x2,y1,y1,PIXEL_ON);  //up
    LcdLine(x1+7,x2-7,y1-1,y1-1,PIXEL_ON);
    LcdLine(x1+7,x2-7,y1-2,y1-2,PIXEL_ON);
	
    horizon_line=persent*(y2-y1-3)/100;
    for(i=0;i<horizon_line;i++)
      LcdLine(x1+2,x2-2,y2-2-i,y2-2-i,PIXEL_ON);

    horizon_line2=(y2-y1-3);
    for(i=horizon_line2;i>horizon_line;i--) 
      LcdLine(x1+2,x2-2,y2-2-i,y2-2-i,PIXEL_OFF);
}

/*
*********************************************************************************************************
*/
void LcdHBar(int x1, int x2, int y1, int y2, CPU_INT08U persent)	
{
   CPU_INT08U line;
        
   if(persent>100)return;
        
   LcdLine(x1+2,x2-2,y2,y2,PIXEL_ON);  //down
   LcdLine(x2-2,x2-2,y1,y2,PIXEL_ON);  //right
   LcdLine(x1+2,x1+2,y1,y2,PIXEL_ON);  //left
   LcdLine(x1+2,x2-2,y1,y1,PIXEL_ON);  //up
	
   LcdLine(x2-1,x2-1,y1+1,y2-1,PIXEL_ON);  //right
   LcdLine(x1+1,x1+1,y1+1,y2-1,PIXEL_ON);  //left
        
   LcdLine(x2,x2,y1+2,y2-2,PIXEL_ON);  //right
   LcdLine(x1,x1,y1+2,y2-2,PIXEL_ON);  //left
        
   line=persent*(x2-x1-7)/100-1;
   LcdLine(x1+4,x2-4,y1+2,y2-2,PIXEL_OFF);
   LcdLine(x1+4,x1+4+line,y1+2,y2-2,PIXEL_ON);
}

/*
*********************************************************************************************************
*/
void LcdHBarLine(CPU_INT08U line, CPU_INT08U persent)
{
  LcdHBar(0,  83, (line-1)*7+1, (line-1)*7+5, persent);
}

/*
*******************************************************************************************************
* Name         :  LcdSingleBar
* Description  :  Display single bar.
* Argument(s)  :  baseX  -> absolute x axis coordinate
*                 baseY  -> absolute y axis coordinate
*				   height -> height of bar (in pixel)
*				   width  -> width of bar (in pixel)
*				   mode   -> Off, On or Xor. See enum in pcd8544.h.
* Return value :  see return value on pcd8544.h
*******************************************************************************************************
*/
CPU_INT08U LcdSingleBar ( CPU_INT08U baseX, CPU_INT08U baseY, CPU_INT08U height, CPU_INT08U width, LcdPixelMode mode )
{
    CPU_INT08U tmpIdxX,tmpIdxY,tmp;
    CPU_INT08U response;

    /* Checking border */
	if ( ( baseX > LCD_X_RES ) || ( baseY > LCD_Y_RES ) ) return OUT_OF_BORDER;

	if ( height > baseY )
		tmp = 0;
	else
		tmp = baseY - height;

    /* Draw lines */
	for ( tmpIdxY = tmp; tmpIdxY < baseY; tmpIdxY++ )
	{
	  for ( tmpIdxX = baseX; tmpIdxX < (baseX + width); tmpIdxX++ )
          {
		response = LcdPixel( tmpIdxX, tmpIdxY, mode );
                if(response)
                 return response;
          }
	}
        return OK;
}

/*
*******************************************************************************************************
* Name         :  LcdBars
* Description  :  Display multiple bars.
* Argument(s)  :  data[] -> data which want to be plotted
*                 numbBars  -> number of bars want to be plotted
*				   width  -> width of bar (in pixel)
* Return value :  see return value on pcd8544.h
* Note         :  Please check EMPTY_SPACE_BARS, BAR_X, BAR_Y in pcd8544.h
*******************************************************************************************************
*/
CPU_INT08U LcdBars ( CPU_INT08U data[], CPU_INT08U numbBars, CPU_INT08U width, CPU_INT08U multiplier )
{
	CPU_INT08U b;
	CPU_INT08U tmpIdx = 0;
        CPU_INT08U response;

	for ( b = 0;  b < numbBars ; b++ )
	{
        /* Preventing from out of border (LCD_X_RES) */
		if ( tmpIdx > LCD_X_RES ) return OUT_OF_BORDER;

		/* Calculate x axis */
		tmpIdx = ((width + EMPTY_SPACE_BARS) * b) + BAR_X;

		/* Draw single bar */
		response = LcdSingleBar( tmpIdx, BAR_Y, data[ b ] * multiplier, width, PIXEL_ON);
        if(response == OUT_OF_BORDER)
            return response;
	}
        return OK;
}
/*
*******************************************************************************************************
* Name         :  LcdRect
* Description  :  Display a rectangle.
* Argument(s)  :  x1   -> absolute first x axis coordinate
*                 y1   -> absolute first y axis coordinate
*				   x2   -> absolute second x axis coordinate
*				   y2   -> absolute second y axis coordinate
*				   mode -> Off, On or Xor. See enum in pcd8544.h.
* Return value :  see return value on pcd8544.h.
*******************************************************************************************************
*/
CPU_INT08U LcdRect ( CPU_INT08U x1, CPU_INT08U x2,CPU_INT08U y1, CPU_INT08U y2, LcdPixelMode mode )
{
	CPU_INT08U tmpIdxX,tmpIdxY;

	/* Checking border */
	if ( ( x1 > LCD_X_RES ) ||  ( x2 > LCD_X_RES ) || ( y1 > LCD_Y_RES ) || ( y2 > LCD_Y_RES ) )
		/* If out of border then return */
		return OUT_OF_BORDER;

	if ( ( x2 > x1 ) && ( y2 > y1 ) )
	{
		for ( tmpIdxY = y1; tmpIdxY < y2; tmpIdxY++ )
		{
		       /* Draw line horizontally */
		      for ( tmpIdxX = x1; tmpIdxX < x2; tmpIdxX++ )
                      {        
                         /* Draw a pixel */
			 LcdPixel( tmpIdxX, tmpIdxY, mode );
                      }
		}
	}
        return OK;
}
/*
*******************************************************************************************************
* Name         :  LcdImage
* Description  :  Image mode display routine.
* Argument(s)  :  Address of image in hexes
* Return value :  None.
* Example      :  LcdImage(&sample_image_declared_as_array);
*******************************************************************************************************
*/
void LcdImage ( const CPU_INT08U *imageData )
{
    DispLock();
    /* Initialize cache index to 0 */   
    memcpy(LcdCache,imageData,LCD_CACHE_SIZE);

    /* Reset watermark pointers to be full */
    LoWaterMark = 0;
    HiWaterMark = LCD_CACHE_SIZE - 1;

    /* Set update */
    LcdUpdate();
    DispUnlock();
}

/*
*******************************************************************************************************
* Name         :  LcdUpdate
* Description  :  Copies the LCD cache into the device RAM.
* Argument(s)  :  None.
* Return value :  None.
*******************************************************************************************************
*/
void LcdUpdate ( void )
{
    CPU_INT32S i;

    if ( LoWaterMark < 0 )
        LoWaterMark = 0;
    else if ( LoWaterMark >= LCD_CACHE_SIZE )
        LoWaterMark = LCD_CACHE_SIZE - 1;

    if ( HiWaterMark < 0 )
        HiWaterMark = 0;
    else if ( HiWaterMark >= LCD_CACHE_SIZE )
        HiWaterMark = LCD_CACHE_SIZE - 1;

    /*  Set base address according to LoWaterMark. */
    LcdSend( 0x80 | ( LoWaterMark % LCD_X_RES ), LCD_CMD );
    LcdSend( 0x40 | ( LoWaterMark / LCD_X_RES ), LCD_CMD );

    /*  Serialize the display buffer. */
    for ( i = LoWaterMark; i <= HiWaterMark; i++ )
    {
        LcdSend( LcdCache[ i ], LCD_DATA );
    }

    /*  Reset watermark pointers. */
    LoWaterMark = LCD_CACHE_SIZE - 1;
    HiWaterMark = 0;
}
#endif