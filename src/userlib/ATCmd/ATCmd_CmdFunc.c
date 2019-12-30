/*!****************************************************************************
 * @file AT_CmdFunc.c
 *
 * Funktionsdefinitionen für die einzelnen AT-Befehle
 *
 * @date  07.12.2019
 ******************************************************************************/

/*- Headerdateien ------------------------------------------------------------*/
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "stm8l15x.h"
#include "commlib.h"
#include "app_sensors.h"
#include "sensorlog.h"
#include "ff.h"
#include "ATCmd.h"
#include "ATCmd_CmdFunc.h"


/*- Symbolische Konstanten ---------------------------------------------------*/
/*! @brief Sendepuffer                                                        */
#define AT_TXBUF  ((volatile char*)&aucUart1TxBuf)


/*- Lokale Funktionen --------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Pufferinhalt senden 
 * 
 * @date  09.12.2019
 ******************************************************************************/
static void AT_Send(void)
{
  if (AT_TXBUF[0] != '\0')
  {
    UART1_SendUntil('\0', COMMLIB_UART1_MAX_BUF);
    while(!UART1_IsTxReady());
  }
  UART1_FlushTx();
}

/*!****************************************************************************
 * @brief
 * Anzahl der Argumente im Eingabestring lesen
 *
 * @param[in] *pszBuf   Eingabestring
 * @return    int       Anzahl der erkannten Argumente
 *
 * @date  10.12.2019
 ******************************************************************************/
static int CountArgs(const char* pszBuf)
{
  int count = 1;
  
  if ((*pszBuf == '\r') || (*pszBuf == '\0'))
  {
    return 0;
  }
  
  while ((*pszBuf != '\r') && (*pszBuf != '\0'))
  {
    if (*pszBuf == ',')
    {
      ++count;
    }
    ++pszBuf;
  }
  
  return count;
}


/*!****************************************************************************
 * @brief
 * Eingabe mit "OK" quittieren, ohne weitere Funktion
 *
 * @param[in] *pszBuf   Nicht genutzt
 * @return    bool      true
 *
 * @date  08.12.2019
 ******************************************************************************/
bool ATCmd_OK(const char* pszBuf)
{
  return true;
}

/*!****************************************************************************
 * @brief
 * Temperaturmesswerte lesen
 *
 * @param[in] *pszBuf   Nicht genutzt
 * @return    bool      true
 *
 * @date  07.12.2019
 * @date  08.12.2019  UART-Modul Funktionsaufrufe ausgelagert
 ******************************************************************************/
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

/*!****************************************************************************
 * @brief
 * Luftdruck-Messwerte lesen
 *
 * @param[in] *pszBuf   Nicht genutzt
 * @return    bool      true
 *
 * @date  09.12.2019
 ******************************************************************************/
bool ATCmd_PresRead(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CPRES: %ld\r\n",
    sSensorBME280.sMeasure.ulPressure
  );
  AT_Send();
  return true;
}

/*!****************************************************************************
 * @brief
 * Luftfeuchtigkeitsmesswerte lesen
 *
 * @param[in] *pszBuf   Nicht genutzt
 * @return    bool      true
 *
 * @date  09.12.2019
 ******************************************************************************/
bool ATCmd_HumRead(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CHUM: %ld\r\n",
    sSensorBME280.sMeasure.ulHumidity
  );
  AT_Send();
  return true;
}

/*!****************************************************************************
 * @brief
 * Winddaten lesen
 *
 * @param[in] *pszBuf   Nicht genutzt
 * @return    bool      true
 *
 * @date  09.12.2019
 ******************************************************************************/
bool ATCmd_WindRead(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CWIND: %d,%d\r\n",
    sSensorWind.sMeasure.eDirection,
    sSensorWind.sMeasure.uiAvgVelocity
  );
  AT_Send();
  return true;
}

/*!****************************************************************************
 * @brief
 * Test-Aufruf für AT+CTIME
 *
 * @param[in] *pszBuf   Nicht genutzt
 * @return    bool      true
 *
 * @date  09.12.2019
 ******************************************************************************/
bool ATCmd_TimeTest(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CTIME: 00-99,00-12,00-31,00-23,00-59,00-59\r\n");
  AT_Send();
  return true;
}

/*!****************************************************************************
 * @brief
 * RTC Datum und Uhrzeit lesen
 *
 * @param[in] *pszBuf   Nicht genutzt
 * @return    bool      true
 *
 * @date  09.12.2019
 ******************************************************************************/
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

/*!****************************************************************************
 * @brief
 * RTC Datum und Uhrzeit setzen
 * 
 * @todo  Implementierung fehlt noch
 *
 * @param[in] *pszBuf   Eingabeargumente
 * @return    bool      true, wenn Datum und Uhrzeit gültig und gesetzt
 *
 * @date  09.12.2019
 ******************************************************************************/ 
bool ATCmd_TimeWrite(const char* pszBuf)
{
  RTC_DateTypeDef sDate;
  RTC_TimeTypeDef sTime;
  char result;
  int i;
  
  if (CountArgs(pszBuf) != 6)
  {
    sprintf(AT_TXBUF, "+CTIME: narg\r\n");
    AT_Send();
    return false;
  }
  else
  {   
    result = true;
    for (i = 0; i < 6; ++i)
    {
      int value = atoi(pszBuf);
      
      switch (i)
      {
        case 0:
          result &= (value >= 0) && (value <= 99);
          sDate.RTC_Year = value;
          break;
        
        case 1:
          result &= (value >= 1) && (value <= 12);
          sDate.RTC_Month = value;
          break;
          
        case 2:
          result &= (value >= 1) && (value <= 31);
          sDate.RTC_Date = value;
          break;
          
        case 3:
          result &= (value >= 0) && (value <= 23);
          sTime.RTC_Hours = value;
          break;
          
        case 4:
          result &= (value >= 0) && (value <= 59);
          sTime.RTC_Minutes = value;
          break;
          
        case 5:
          result &= (value >= 0) && (value <= 59);
          sTime.RTC_Seconds = value;
          break;
          
        default:
          result = false;
      }
      
      if (i < 5)
      {
        while (*(pszBuf++) != ',');
      }
      
      if (!result)
      {
        /* Ungültiger Wert                                */
        sprintf(AT_TXBUF, "+CTIME: value\r\n");
        AT_Send();
        return false;
      }
    }
    
    RTC_SetDate(RTC_Format_BIN, &sDate);
    RTC_SetTime(RTC_Format_BIN, &sTime);
    
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
}

/*!****************************************************************************
 * @brief
 * Ausrichtungswerte lesen
 *
 * @param[in] *pszBuf   Nicht genutzt
 * @return    bool      true
 *
 * @date  09.12.2019
 ******************************************************************************/
bool ATCmd_AlignRead(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CALIGN: %d,%d\r\n",
    sSensorQMC5883.sMeasure.uiAzimuth,
    sSensorMPU6050.sMeasure.sAngle.iXZ
  );
  AT_Send();
  return true;
}

/*!****************************************************************************
 * @brief
 * Test-Befehl für "AT+CGNSPOS"
 *
 * @param[in] *pszBuf   Nicht genutzt
 * @return    bool      true
 *
 * @date  09.12.2019
 ******************************************************************************/
bool ATCmd_PosTest(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CGNSPOS: -900000-900000,-1800000-1800000,-32768-32767\r\n");
  AT_Send();
  return true;
}

/*!****************************************************************************
 * @brief
 * GPS Position und Höhe lesen
 *
 * @param[in] *pszBuf   Nicht genutzt
 * @return    bool      true
 *
 * @date  09.12.2019
 ******************************************************************************/
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

/*!****************************************************************************
 * @brief
 * GPS Position und Höhe setzen
 * 
 * @todo Implementierung fehlt noch
 *
 * @param[in] *pszBuf   Nicht genutzt
 * @return    bool      true, wenn Position und Höhe gültig sind
 * 
 * @date  09.12.2019
 ******************************************************************************/
bool ATCmd_PosWrite(const char* pszBuf)
{
  long lValue[3];
  int i;
  bool bResult = true;
  
  if (CountArgs(pszBuf) != 3)
  {
    sprintf(AT_TXBUF, "+CGNSPOS: narg\r\n");
    AT_Send();
    return false;
  }
  else
  {
    for (i = 0; i < 3; ++i)
    {
      lValue[i] = atol(pszBuf);
      
      if (i < 2)
      {
        while (*(pszBuf++) != ',');
      }
    }
    
    sSensorGPS.sPosition.lLat = lValue[0];
    sSensorGPS.sInfo.bLatValid = true;
    sSensorGPS.sPosition.lLong = lValue[1];
    sSensorGPS.sInfo.bLongValid = true;
    sSensorGPS.sPosition.iAlt = (int)lValue[2];
    sSensorGPS.sInfo.bAltValid = true;
    
    sprintf(AT_TXBUF, "+CGNSPOS: %ld,%ld,%d\r\n",
      sSensorGPS.sPosition.lLat,
      sSensorGPS.sPosition.lLong,
      sSensorGPS.sPosition.iAlt
    );
    AT_Send();
    return true;
  }
}

/*!****************************************************************************
 * @brief
 * Test-Befehl für "AT+CGNSPWR"
 * 
 * @param[in] *pszBuf   Nicht genutzt
 * @return    bool      true
 *
 * @date  09.12.2019
 ******************************************************************************/
bool ATCmd_GnsPwrTest(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CGNSPWR: 0-1\r\n");
  AT_Send();
  return true;
}

/*!****************************************************************************
 * @brief
 * GPS Einschaltzustand setzen
 *
 * @param[in] *pszBuf   Eingabedaten
 * @return    bool      true, wenn Einschaltzustand gültig
 *
 * @date  09.12.2019
 ******************************************************************************/
bool ATCmd_GnsPwrWrite(const char* pszBuf)
{
  if (*pszBuf == '0') 
  {
    /* GPS ausschalten                                    */
    GPIO_WriteBit(GPIOD, GPIO_Pin_2, false);
    sprintf(AT_TXBUF, "+CGNSPWR: 0\r\n");
    AT_Send();
    return true;
  }
  else if (*pszBuf == '1') 
  {
    /* GPS einschalten                                    */
    GPIO_WriteBit(GPIOD, GPIO_Pin_2, true);
    sprintf(AT_TXBUF, "+CGNSPWR: 1\r\n");
    AT_Send();
    return true;
  }
  else
  {
    /* Ungültige Eingabe                                  */
    return false;
  }
}

bool ATCmd_GnsTstTest(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CGNSTST: 0-1\r\n");
  AT_Send();
  return true;
}

bool ATCmd_GnsTstRead(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CGNSTST: %d\r\n", (int)(eDataModeSrc == ATCmd_DataModeSrc_GPS));
  AT_Send();
  return true;
}

bool ATCmd_GnsTstWrite(const char* pszBuf)
{
  if (*pszBuf == '0')
  {
    if (eDataModeSrc == ATCmd_DataModeSrc_GPS)
    {
      eDataModeSrc = ATCmd_DataModeSrc_None;
      bDataMode = false;
      sprintf(AT_TXBUF, "+CGNSTST: 0\r\n");
      AT_Send();
      return true;
    }
    else
    {
      sprintf(AT_TXBUF, "+CGNSTST: not act\r\n");
      AT_Send();
      return false;
    }
  }
  else if (*pszBuf == '1')
  {
    if (eDataModeSrc == ATCmd_DataModeSrc_None)
    {
      sprintf(AT_TXBUF, "+CGNSTST: 1\r\n");
      AT_Send();
      eDataModeSrc = ATCmd_DataModeSrc_GPS;
      bDataMode = true;
      return true;
    }
    else
    {
      sprintf(AT_TXBUF, "+CGNSTST: busy\r\n");
      AT_Send();
      return false;
    }
  }
  else
  {
    return false;
  }
}

/*!****************************************************************************
 * @brief
 * Leistungsmesswerte lesen
 *
 * @param[in] *pszBuf   Nicht genutzt
 * @return    bool      true
 *
 * @date  09.12.2019
 ******************************************************************************/
bool ATCmd_PwrRead(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CPWR: 0,0,0,0,0\r\n");
  AT_Send();
  return true;
}

/*!****************************************************************************
 * @brief
 * Test-Befehl für "AT+CINTV"
 *
 * @param[in] *pszBuf   Nicht genutzt
 * @return    bool      true
 *
 * @date  09.12.2019
 ******************************************************************************/
bool ATCmd_IntvTest(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CINTV: 10-3600\r\n");
  AT_Send();
  return true;
}

/*!****************************************************************************
 * @brief
 * Wakeup-Timer Zählerwert zurücksetzn
 * 
 * @param[in] *pszBuf   Eingabewerte
 * @return    bool      true, wenn Eingabe gültig
 *
 * @date  09.12.2019
 ******************************************************************************/
bool ATCmd_IntvWrite(const char* pszBuf)
{
  short iInterval = atoi(pszBuf);
  if ((iInterval >= 10) && (iInterval <= 3600))
  {
    RTC_WakeUpCmd(DISABLE);
    RTC_SetWakeUpCounter((unsigned short)iInterval);
    RTC_WakeUpCmd(ENABLE);
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

/*!****************************************************************************
 * @brief
 * Wertetabelle für GUI lesen
 *
 * @param[in] *pszBuf   Nicht genutzt
 * @return    bool      true
 *
 * @date  07.12.2019
 * @date  08.12.2019  UART-Modul Funktionsaufrufe ausgelagert
 ******************************************************************************/
bool ATCmd_GuiRead(const char* pszBuf)
{
  unsigned uOffset;
  for (uOffset = 0; uOffset < NUM_SENSORLOG_RINGITEMS; ++uOffset)
  {
    SensorLogItem* pLog = SensorLog_Dump(uOffset);
  
    /* Header und RTC Zeitstempel                           */
    sprintf(AT_TXBUF, "+CGUI: %02d,%02d,%02d,%02d,%02d,%02d,",
      (int)pLog->sTimestamp.sDate.RTC_Year,
      (int)pLog->sTimestamp.sDate.RTC_Month,
      (int)pLog->sTimestamp.sDate.RTC_Date,
      (int)pLog->sTimestamp.sTime.RTC_Hours,
      (int)pLog->sTimestamp.sTime.RTC_Minutes,
      (int)pLog->sTimestamp.sTime.RTC_Seconds
    );
    AT_Send();
    
    /* Temperaturmesswerte                                  */
    sprintf(AT_TXBUF, "%d,%d,%d,%d,", 
      pLog->sTemperature.iBME,
      pLog->sTemperature.iCPU,
      pLog->sTemperature.iQMC,
      pLog->sTemperature.iMPU
    );
    AT_Send();
    
    /* Wind                                                 */
    sprintf(AT_TXBUF, "%d,%d,",
      pLog->sWind.uiDir,
      pLog->sWind.uiVelo
    );
    AT_Send();
    
    /* Luftdruck, Luftfeuchte, Ausrichtung                  */
    sprintf(AT_TXBUF, "%ld,%ld,%d,%d,", 
      pLog->ulHumidity,
      pLog->ulPressure,
      pLog->sAlignment.iZenith,
      pLog->sAlignment.uiAzimuth
    );
    AT_Send();
    
    /* GPS Position und Höhe                                */
    sprintf(AT_TXBUF, "%ld,%ld,%d,",
      pLog->sPosition.lLat,
      pLog->sPosition.lLong,
      pLog->sPosition.iAlt
    );
    AT_Send();
    
    /* Leistungsmessdaten                                 */
    sprintf(AT_TXBUF, "%d,%d,%d,%d,%d\r\n",
      pLog->sPower.uiBatVolt,
      pLog->sPower.iBatCurr,
      pLog->sPower.uiPanelVolt,
      pLog->sPower.iPanelCurr,
      330
    );
    AT_Send();
  }
  return true;
}

/*!****************************************************************************
 * @brief
 * Wakeup-Task auslösen
 *
 * @param[in] *pszBuf   Nicht genutzt
 * @return    bool      true
 *
 * @date  23.12.2019
 ******************************************************************************/
bool ATCmd_ForceWkup(const char* pszBuf)
{
  extern volatile bool bTaskWakeupFlag;
  bTaskWakeupFlag = true;
  return true;
}

/*!****************************************************************************
 * @brief
 * Messwerte-Protokoll löschen
 *
 * @param[in] *pszBuf   Nicht genutzt
 * @return    bool      true
 *
 * @date  19.12.2019
 ******************************************************************************/ 
bool ATCmd_LogClear(const char* pszBuf)
{
  SensorLog_Clear();
  return true;
}

bool ATCmd_DebugTest(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CDEBUG: 0-1\r\n");
  AT_Send();
  return true;
}

bool ATCmd_DebugRead(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CDEBUG: %d\r\n", (int)(eDataModeSrc == ATCmd_DataModeSrc_Debug));
  AT_Send();
  return true;
}

bool ATCmd_DebugWrite(const char* pszBuf)
{
  if (*pszBuf == '0')
  {
    if (eDataModeSrc == ATCmd_DataModeSrc_Debug)
    {
      eDataModeSrc = ATCmd_DataModeSrc_None;
      bDataMode = false;
      sprintf(AT_TXBUF, "+CDEBUG: 0\r\n");
      AT_Send();
      return true;
    }
    else
    {
      sprintf(AT_TXBUF, "+CDEBUG: not act\r\n");
      AT_Send();
      return false;
    }
  }
  else if (*pszBuf == '1')
  {
    if (eDataModeSrc == ATCmd_DataModeSrc_None)
    {
      sprintf(AT_TXBUF, "+CDEBUG: 1\r\n");
      AT_Send();
      eDataModeSrc = ATCmd_DataModeSrc_Debug;
      bDataMode = true;
      return true;
    }
    else
    {
      sprintf(AT_TXBUF, "+CDEBUG: busy\r\n");
      AT_Send();
      return false;
    }
  }
  else
  {
    return false;
  }
}

bool ATCmd_FileTest(const char* pszBuf)
{
  sprintf(AT_TXBUF, "+CFILE: 0-2\r\n");
  AT_Send();
  return true;
}

bool ATCmd_FileRead(const char* pszBuf)
{
  FIL fp;
  unsigned int num;
  
  if (f_open(&fp, "LOG.TXT", FA_READ | FA_OPEN_EXISTING) == FR_OK)
  {
    sprintf(AT_TXBUF, "+CFILE: %d\r\n", (int)f_size(&fp));
    AT_Send();
  
    do {
      if (f_read(&fp, AT_TXBUF, COMMLIB_UART1_MAX_BUF, &num) == FR_OK)
      {
        AT_Send();
      }      
    } while (num > 0);
    f_close(&fp);
    return true;
  }
  else
  {
    sprintf(AT_TXBUF, "+CFILE: fopen\r\n");
    AT_Send();
    return false;
  }
}

bool ATCmd_FileWrite(const char* pszBuf)
{
  FIL fp;
  DIR dp;
  
  if (*pszBuf == '0')
  {
    return true;
  }
  else if (*pszBuf == '1')
  {
    /* Datei leeren                                       */
    if (f_open(&fp, "LOG.TXT", FA_WRITE | FA_CREATE_ALWAYS) == FR_OK)
    {
      f_close(&fp);
      return true;
    }
    else
    {
      sprintf(AT_TXBUF, "+CFILE: fopen\r\n");
      return false;
    }
  }
  else if (*pszBuf == '2')
  {
    /* Datei umbenennen                                   */
    char cHelp[28] = "";
    int iRes;
    int iNum;
    FILINFO fno;
    
    iRes = f_opendir(&dp, "/");
    if (iRes == FR_OK)
    {
      iNum = 0;
      do
      {
        iRes = f_readdir(&dp, &fno);
        if ((iRes == FR_OK) && ((fno.fattrib & AM_DIR) != 0))
        {
          ++iNum;
        }
      } while ((iRes == FR_OK) && (fno.fname[0] != '\0'));
      f_closedir(&dp);
    }
    else
    {
      sprintf(AT_TXBUF, "+CFILE: fopendir %d\r\n", iRes);
      AT_Send();
      return false;
    }
    
    sprintf(cHelp, "LOG_%04d.TXT", iNum);    
    iRes = f_rename("LOG.TXT", cHelp);
    if (iRes == FR_OK)
    {
      return true;
    }
    else
    {
      sprintf(AT_TXBUF, "+CFILE: frename %d (%s)\r\n", iRes, cHelp);
      AT_Send();
      return false;
    }
  }
  else
  {
    return false;
  }
}