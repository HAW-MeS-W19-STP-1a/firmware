#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stm8l15x.h"
#include "commlib.h"
#include "app_sensors.h"
#include "ATCmd_CmdFunc.h"

#define AT_TXBUF  ((volatile char*)&aucUart1TxBuf)

static void AT_Send(void)
{
  if (AT_TXBUF[0] != '\0')
  {
    UART1_SendUntil('\0', COMMLIB_UART1_MAX_BUF);
    while(!UART1_IsTxReady());
  }
  UART1_FlushTx();
}

bool ATCmd_OK(const char* pszBuf)
{
  return true;
}

bool ATCmd_TempRead(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CTEMP: %d,%d,%d,%d\r\n", 
    sSensorBME280.sMeasure.iTemperature, 
    (int)sSensorCPUTemp.sMeasure.cTemp, 
    sSensorQMC5883.sMeasure.iTemperature, 
    sSensorMPU6050.sMeasure.iTemperature
  );
  AT_Send();
  return true;
}

bool ATCmd_PresRead(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CPRES: %ld\r\n",
    sSensorBME280.sMeasure.ulPressure
  );
  AT_Send();
  return true;
}

bool ATCmd_HumRead(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CHUM: %ld\r\n",
    sSensorBME280.sMeasure.ulHumidity
  );
  AT_Send();
  return true;
}

bool ATCmd_WindRead(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CWIND: %d,%d\r\n",
    sSensorWind.sMeasure.eDirection,
    sSensorWind.sMeasure.uiVelocity
  );
  AT_Send();
  return true;
}

bool ATCmd_TimeTest(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CTIME: 00-99,00-12,00-31,00-23,00-59,00-59\r\n");
  AT_Send();
  return true;
}

bool ATCmd_TimeRead(const char* pszBuf)
{
  RTC_DateTypeDef sDate;
  RTC_TimeTypeDef sTime;
  RTC_GetDate(RTC_Format_BIN, &sDate);
  RTC_GetTime(RTC_Format_BIN, &sTime);
  
  sprintf(AT_TXBUF, "+CTIME: %02d,%02d,%02d,%02d,%02d,%02d\r\n",
    (unsigned)sDate.RTC_Year, 
    (unsigned)sDate.RTC_Month, 
    (unsigned)sDate.RTC_Date,
    (unsigned)sTime.RTC_Hours,
    (unsigned)sTime.RTC_Minutes,
    (unsigned)sTime.RTC_Seconds
  );
  AT_Send();
  return true;
}

bool ATCmd_TimeWrite(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CTIME: Not yet implemented\r\n");
  AT_Send();
  return false;
}

bool ATCmd_AlignRead(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CALIGN: %d,%d\r\n",
    sSensorQMC5883.sMeasure.uiAzimuth,
    sSensorMPU6050.sMeasure.sAngle.iXZ
  );
  AT_Send();
  return true;
}

bool ATCmd_PosTest(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CGNSPOS: -90.0000-90.0000,-180.0000-180.0000,-3276.8-3276.7\r\n");
  AT_Send();
  return true;
}

bool ATCmd_PosRead(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CGNSPOS: %ld,%ld,%d\r\n",
    sSensorGPS.sPosition.lLat,
    sSensorGPS.sPosition.lLong,
    sSensorGPS.sPosition.iAlt
  );
  AT_Send();
  return true;
}

bool ATCmd_PosWrite(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CGNSPOS: Not yet implemented\r\n");
  AT_Send();
  return false;
}

bool ATCmd_PwrRead(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CPWR: 0,0,0,0,0\r\n");
  AT_Send();
  return true;
}

bool ATCmd_IntvTest(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CINTV: 10-3600\r\n");
  AT_Send();
  return true;
}

bool ATCmd_IntvWrite(const char* pszBuf)
{
  short iInterval = atoi(pszBuf);
  if ((iInterval >= 10) && (iInterval <= 3600))
  {
    RTC_SetWakeUpCounter((unsigned short)iInterval);
    sprintf(AT_TXBUF, "+CINTV: %d\r\n", iInterval);
    printf("WakeupTimer Reset: %d\r\n", iInterval);
    AT_Send();
    return true;
  }
  else
  {
    return false;
  }
}

bool ATCmd_GuiRead(const char* pszBuf)
{
  RTC_DateTypeDef sDate;
  RTC_TimeTypeDef sTime;
  
  /* Header und RTC Zeitstempel                           */
  RTC_GetDate(RTC_Format_BIN, &sDate);
  RTC_GetTime(RTC_Format_BIN, &sTime);
  sprintf(AT_TXBUF, "+CGUI: %02d,%02d,%02d,%02d,%02d,%02d,",
    (unsigned)sDate.RTC_Year, 
    (unsigned)sDate.RTC_Month, 
    (unsigned)sDate.RTC_Date,
    (unsigned)sTime.RTC_Hours,
    (unsigned)sTime.RTC_Minutes,
    (unsigned)sTime.RTC_Seconds
  );
  AT_Send();
  
  /* Temperaturmesswerte                                  */
  sprintf(AT_TXBUF, "%d,%d,%d,%d,", 
    sSensorBME280.sMeasure.iTemperature,
    (int)sSensorCPUTemp.sMeasure.cTemp,
    sSensorQMC5883.sMeasure.iTemperature,
    sSensorMPU6050.sMeasure.iTemperature
  );
  AT_Send();
  
  /* Luftdruck, Luftfeuchte, Ausrichtung                  */
  sprintf(AT_TXBUF, "%ld,%ld,%d,%d,", 
    sSensorBME280.sMeasure.ulPressure,
    sSensorBME280.sMeasure.ulHumidity,
    sSensorMPU6050.sMeasure.sAngle.iXZ,
    sSensorQMC5883.sMeasure.uiAzimuth
  );
  AT_Send();
  
  /* GPS Position                                         */
  sprintf(AT_TXBUF, "%ld,%ld,",
    sSensorGPS.sPosition.lLat,
    sSensorGPS.sPosition.lLong
  );
  AT_Send();
  
  /* GPS Höhe, Leistungsmessdaten                         */
  sprintf(AT_TXBUF, "%d,%d,%d,%d,%d,%d\r\n",
    sSensorGPS.sPosition.iAlt,
    0, 0, 0, 0, 0, 0
  );
  AT_Send();
            
  return true;
}