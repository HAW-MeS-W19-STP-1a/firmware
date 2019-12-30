/*!****************************************************************************
 * @file
 * GPSHandler.h
 *
 * Ansteuerung des GPS-Moduls und Verarbeitung der eingehenden NMEA-Daten
 *
 * @date  06.11.2019
 ******************************************************************************/
 
/*- Headerdateien ------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "stm8l15x.h"
#include "io_map.h"
#include "commlib.h"
#include "ATCmd.h"
#include "GPSHandler.h"


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * NMEA Sentence Type
 *
 * @date  06.11.2019
 ******************************************************************************/
typedef enum tag_GPSHandler_Sentence {
  /*! GPRMC - (NMEA0183) Recommended Minimum Data         */
  GPSHandler_Sentence_RMC,
  
  /*! GPGGA - (NMEA0183) GPS system fix data              */
  GPSHandler_Sentence_GGA,
  
  /*! GPTXT - (proprietär) Text message                   */
  GPSHandler_Sentence_TXT,
  
  /*! sonstige - unbekannt                                */
  GPSHandler_Sentence_Unknown
} GPSHandler_Sentence;


/*- Globale Variablen --------------------------------------------------------*/
/*! GPS Sensordaten-Speicher                                                  */
GPSHandler_Data sSensorGPS;


/*- Modulglobale Variablen ---------------------------------------------------*/
/*! GPS Aktivierungszustand                                                   */
static bool bGpsActive;


/*- Lokale Funktionen --------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Zeichen in Ziffer umwandeln
 *
 * @param[in] c     Ziffer
 * @return    char  Zahlenwert oder 0, wenn ungültiges Zeichen
 *
 * @date  15.11.2019
 ******************************************************************************/
static char CtoI(char c)
{
  if ((c >= '0') && (c <= '9'))
  {
    /* ASCII Zeichencode in Zahlenwert                    */
    return c - '0';
  }
  else
  {
    /* Zeichen kann nicht in Ziffer konvertiert werden    */
    return 0;
  }
}

/*!****************************************************************************
 * @brief
 * Berechnung von 10er-Potenzen mit Exponenten [0, 4].
 *
 * @param[in] b     Exponent
 * @return    int   Ergebnis für 10^b
 *
 * @date  30.12.2019
 ******************************************************************************/
static int pow10(uint8_t b)
{
  uint8_t x;
  int res = 0;
  
  if ((b >= 0) && (b <= 4))
  {
    res = 1;
    for (x = 0; x < b; ++x)
    {
      res *= 10;
    }
  }
  
  return res;
}

/*!****************************************************************************
 * @brief
 * Sentence-Typ ermitteln
 *
 * @param[in] *pszBuf   NMEA_IN Puffer
 * @return    GPSHandler_Sentence Sentence-Typ
 *
 * @date  06.11.2019
 ******************************************************************************/
static GPSHandler_Sentence GPSHandler_GetType(char* pszBuf)
{
  GPSHandler_Sentence eType;
  char* pSenTypeStart = &pszBuf[3];
  
  if (strncmp(pSenTypeStart, "GGA", 3) == 0)
  {
    eType = GPSHandler_Sentence_GGA;
  } 
  else if (strncmp(pSenTypeStart, "RMC", 3) == 0)
  {
    eType = GPSHandler_Sentence_RMC;
  }
  else if (strncmp(pSenTypeStart, "TXT", 3) == 0)
  {
    eType = GPSHandler_Sentence_TXT;
  }
  else
  {
    eType = GPSHandler_Sentence_Unknown;
  }
  
  return eType;
}

/*!****************************************************************************
 * @brief
 * Einzelne Felder in den Sentences verarbeiten 
 *
 * @param[in] *pszBuf Start des Feldes
 * @param[in] iLen    Länge des Feldes
 * @param[in] eType   Sentence-Typ
 *
 * @date  06.11.2019
 * @date  23.12.2019  Auflösung korrigiert
 * @date  30.12.2019  Anzahl der Satelliten, Höhe über NN hinzugefügt
 ******************************************************************************/
static void GPSHandler_ParseNmeaField(char* pszBuf, int iLen, int iNum, GPSHandler_Sentence eType)
{
  switch (eType)
  {
    case GPSHandler_Sentence_RMC:
      switch (iNum)
      {
        case 1:
          /* Time */
          sSensorGPS.sTime.ucHour = CtoI(*pszBuf++) * 10; // 10h
          sSensorGPS.sTime.ucHour += CtoI(*pszBuf++); // 1h
          sSensorGPS.sTime.ucMin = CtoI(*pszBuf++) * 10; // 10m
          sSensorGPS.sTime.ucMin += CtoI(*pszBuf++); // 1m
          sSensorGPS.sTime.ucSec = CtoI(*pszBuf++) * 10; // 10s
          sSensorGPS.sTime.ucSec += CtoI(*pszBuf++); // 1s
          sSensorGPS.sTime.ucDec = 0; // 0.01s, nicht genutzt
          sSensorGPS.sInfo.bTimeValid = true;
          break;
          
        case 2:
          /* Valid flag */
          sSensorGPS.sInfo.bTimeValid &= (*pszBuf == 'A');
          sSensorGPS.sInfo.bDateValid &= (*pszBuf == 'A');
          sSensorGPS.sInfo.bLatValid &= (*pszBuf == 'A');
          sSensorGPS.sInfo.bLongValid &= (*pszBuf == 'A');
          break;
          
        case 3:
          /* Latitude */
          sSensorGPS.sPosition.lLat = CtoI(*pszBuf++) * 100000L; // 10d = *10*10^6
          sSensorGPS.sPosition.lLat += CtoI(*pszBuf++) * 10000L; //  1d = *10^6
          sSensorGPS.sPosition.lLat += CtoI(*pszBuf++) * 1666L; // 10m = /60*10*10^6
          sSensorGPS.sPosition.lLat += CtoI(*pszBuf++) * 166L; //  1m = /60*10^6
          pszBuf++; // "."
          sSensorGPS.sPosition.lLat += CtoI(*pszBuf++) * 16L; // 0.1m = /60*0.1*10^6
          //sSensorGPS.sPosition.lLat += CtoI(*pszBuf++) * 166L; // 0.01m = /60*0.01*10^6
          //sSensorGPS.sPosition.lLat += CtoI(*pszBuf++) * 16L; // 0.001m = /60*0.001*10^6
          sSensorGPS.sInfo.bLatValid = true;
          break;
          
        case 4:
          /* Latitude N or S flag */
          if (*pszBuf == 'S')
          {
            /* Vorzeichen negativ, wenn Position S        */
            sSensorGPS.sPosition.lLat = -sSensorGPS.sPosition.lLat;
          }
          sSensorGPS.sInfo.bLatValid &= true;
          break;
          
        case 5:
          /* Longitude */
          sSensorGPS.sPosition.lLong = CtoI(*pszBuf++) * 1000000L; // 100d = *100*10^6
          sSensorGPS.sPosition.lLong += CtoI(*pszBuf++) * 100000L; // 10d = *10*10^6
          sSensorGPS.sPosition.lLong += CtoI(*pszBuf++) * 10000L; // 1d = *10^6
          sSensorGPS.sPosition.lLong += CtoI(*pszBuf++) * 1666L; // 10m = /60*10*10^6
          sSensorGPS.sPosition.lLong += CtoI(*pszBuf++) * 166L; // 1m = /60*10^6
          pszBuf++; // "."
          sSensorGPS.sPosition.lLong += CtoI(*pszBuf++) * 16L; // 0.1m = /60*0.1*10^6
          //sSensorGPS.sPosition.lLong += CtoI(*pszBuf++) * 166L; // 0.01m = /60*0.01*10^6
          //sSensorGPS.sPosition.lLong += CtoI(*pszBuf++) * 16L; // 0.001m = /60*0.001*10^6
          sSensorGPS.sInfo.bLongValid = true;
          break;
          
        case 6:
          /* Longitude E or W flag */
          if (*pszBuf == 'W')
          {
            /* Vorzeichen negativ, wenn Position W        */
            sSensorGPS.sPosition.lLong = -sSensorGPS.sPosition.lLong;
          }
          sSensorGPS.sInfo.bLongValid &= true;
          break;
          
        case 9:
          /* Date */
          sSensorGPS.sDate.ucDay = CtoI(*pszBuf++) * 10; // 10d
          sSensorGPS.sDate.ucDay += CtoI(*pszBuf++); // 1d
          sSensorGPS.sDate.ucMonth = CtoI(*pszBuf++) * 10; // 10M
          sSensorGPS.sDate.ucMonth += CtoI(*pszBuf++); // 1M
          sSensorGPS.sDate.ucYear = CtoI(*pszBuf++) * 10; // 10y
          sSensorGPS.sDate.ucYear += CtoI(*pszBuf++); // 1y
          sSensorGPS.sInfo.bDateValid = true;
          break;
        
        default:
          ;
      }
      break;
    
    case GPSHandler_Sentence_GGA:
      switch (iNum)
      {
        case 1:
          /* Time */
          sSensorGPS.sTime.ucHour = CtoI(*pszBuf++) * 10;
          sSensorGPS.sTime.ucHour += CtoI(*pszBuf++);
          sSensorGPS.sTime.ucMin = CtoI(*pszBuf++) * 10;
          sSensorGPS.sTime.ucMin += CtoI(*pszBuf++);
          sSensorGPS.sTime.ucSec = CtoI(*pszBuf++) * 10;
          sSensorGPS.sTime.ucSec += CtoI(*pszBuf++);
          sSensorGPS.sTime.ucDec = 0;
          sSensorGPS.sInfo.bTimeValid = true;
          break;
          
        case 6:
          /* Fix valid */
          sSensorGPS.sInfo.bTimeValid &= (*pszBuf != '0');
          sSensorGPS.sInfo.bDateValid &= (*pszBuf != '0');
          break;
          
        case 7:
          /* Number of satellites */
          sSensorGPS.sInfo.ucNumSV = CtoI(*pszBuf++) * 10;
          sSensorGPS.sInfo.ucNumSV += CtoI(*pszBuf++);
          break;
          
        case 9:
        {
          /* Altitude */
          int i;
          int iNumDec;
          int iExp;
          
          /* Anzahl der Vorkommastellen zählen */
          for (i = 0; i < iLen; ++i)
          {
            if (pszBuf[i] == '.')
            {
              iNumDec = i;
              break;
            }
          }
          if ((iNumDec > 0) && (iNumDec < 5))
          {
            /* Gültige Stellenanzahl */
            sSensorGPS.sPosition.iAlt = 0;
            iExp = 0;
            for (i = iNumDec - 1; i >= 0; --i)
            {
              sSensorGPS.sPosition.iAlt += CtoI(pszBuf[i]) * pow10(iExp);
              ++iExp;
            }
          }
          sSensorGPS.sInfo.bAltValid = true;
          break;
        }
        
        default:
          ;
      }
      break;
    
    default:
      ;
  }
}

/*!****************************************************************************
 * @brief
 * NMEA-Sentence an Kommas aufteilen und Felder parsen 
 *
 * @param[in] *aucBuf   Puffer mit NMEA-Sentence
 * @param[in] iLen      Länge der Zeichenkette im Puffer
 * @param[in] eType     Erkannter Sentence-Type
 *
 * @date  06.11.2019
 ******************************************************************************/
static void GPSHandler_ParseNmeaFields(char* aucBuf, int iLen, GPSHandler_Sentence eType)
{
  int index, prevsep, nextsep = 0;
  int secnum = 0;
  
  /* Bis zum Zeilenende durcharbeiten             */
  for (index = 0; index < iLen; ++index)
  {
    /* Start des nächsten Abschnittes suchen      */
    if (aucBuf[index] == ',')
    {
      prevsep = index;
      ++secnum;
      ++index;
    }
    else
    {
      continue;
    }
    
    /* Ende des Abschnittes suchen                */
    for (nextsep=prevsep+1; (nextsep < iLen) && (aucBuf[nextsep] != ','); ++nextsep);
    
    /* Wenn Abschnitt nicht leer ist, auswerten   */
    if ((nextsep-prevsep) > 1)
    {
      GPSHandler_ParseNmeaField(&aucBuf[prevsep+1], nextsep-prevsep, secnum, eType);
      index = nextsep-1;
    }
  }
}

/*!****************************************************************************
 * @brief
 * Eingehende NMEA-Sentences verarbeiten
 *
 * @param[in] *pszBuf NMEA-Datenpuffer. Startet mit $, endet mit \r
 * @param[in] iLen    Anzahl der empfangenen Zeichen
 * @return    bool    true, wenn ein NMEA Sentence verarbeitet wurde
 *
 * @date  06.11.2019
 ******************************************************************************/
static bool GPSHandler_ParseNmeaIn(char* pszBuf, int iLen)
{
  bool bRet = false;
  
  /* Startzeichen prüfen                                  */
  if (*pszBuf == '$')
  {
    /* Mindestlänge 6 für Sentence-Typ                    */
    if (iLen > 6)
    {
      /* Sentence-Typ ermitteln                           */
      GPSHandler_Sentence eType;
      eType = GPSHandler_GetType(pszBuf);
      
      /* Unbekannte sentences rausfiltern                 */
      if (eType != GPSHandler_Sentence_Unknown)
      {
        /* Auf Zeilenende warten                          */
        if (UART3_IsRxReady())
        {
          /* Debugausgabe                                 */
          printf("NmeaInProc: %s\r\n", pszBuf);
          if (ATCmd_GetDataMode(ATCmd_DataModeSrc_GPS))
          {
            strcpy((volatile char*)aucUart1TxBuf, pszBuf);
            strcat((volatile char*)aucUart1TxBuf, "\r\n");
            if (aucUart1TxBuf[0] != '\0')
            {
              UART1_SendUntil('\0', COMMLIB_UART1_MAX_BUF);
              while(!UART1_IsTxReady());
              UART1_FlushTx();
            }
          }
          
          switch (eType)
          {
            case GPSHandler_Sentence_GGA:
            case GPSHandler_Sentence_RMC:
              GPSHandler_ParseNmeaFields(pszBuf, iLen, eType);
              break;
              
            default:
              ;
          }
          
          /* Sentence ausgewertet                         */
          bRet = true;
          UART3_FlushRx();
          UART3_ReceiveUntilTrig('$', '\r', COMMLIB_UART3RX_MAX_BUF);
        }
      }
      else
      {
        /* Unbekannter Typ - verwerfen                    */
        aucUart3RxBuf[6] = '\0';
        printf("NmeaInProc: %s\r\n", aucUart3RxBuf);
        if (ATCmd_GetDataMode(ATCmd_DataModeSrc_GPS))
        {
          strcpy((volatile char*)aucUart1TxBuf, pszBuf);
          strcat((volatile char*)aucUart1TxBuf, "\r\n");
          if (aucUart1TxBuf[0] != '\0')
          {
            UART1_SendUntil('\0', COMMLIB_UART1_MAX_BUF);
            while(!UART1_IsTxReady());
            UART1_FlushTx();
          }
        }
        UART3_FlushRx();
        UART3_ReceiveUntilTrig('$', '\r', COMMLIB_UART3RX_MAX_BUF);
      }
    }
  }
  else
  {
    /* Falsches Startzeichen                              */
    UART3_FlushRx();
    UART3_ReceiveUntilTrig('$', '\r', COMMLIB_UART3RX_MAX_BUF);
  }
  
  return bRet;
}

/*!****************************************************************************
 * @brief
 * Modul initialisieren 
 *
 * @date  06.11.2019
 ******************************************************************************/
void GPSHandler_Init(void)
{
  bGpsActive = false;
    
  GPIO_Init(USART3_GPS_PORT, USART3_GPS_RX_PIN, GPIO_Mode_In_FL_No_IT);
  GPIO_Init(USART3_GPS_PORT, USART3_GPS_TX_PIN, GPIO_Mode_Out_PP_High_Fast);
  GPIO_Init(GPS_PWREN_PORT, GPS_PWREN_PIN, GPIO_Mode_Out_PP_Low_Slow);
  
  memset(&sSensorGPS, 0, sizeof(sSensorGPS));
  
  UART3_Init();
  UART3_ReceiveUntilTrig('$', '\r', COMMLIB_UART3RX_MAX_BUF);
}

/*!****************************************************************************
 * @brief
 * Pollingroutine, aufgerufen bei Auslösung eines UART-Interrupts 
 *
 * @date  06.11.2019
 ******************************************************************************/
bool GPSHandler_Poll(void)
{
  return GPSHandler_ParseNmeaIn(aucUart3RxBuf, UART3_GetRxCount());
}

/*!****************************************************************************
 * @brief
 * Wakeup-Routine; Aktiviert das GPS-Modul nach längerer Wartezeit
 *
 * @date  06.11.2019
 ******************************************************************************/
void GPSHandler_TaskWakeup(void)
{
  GPIO_WriteBit(GPS_PWREN_PORT, GPS_PWREN_PIN, ENABLE);
  bGpsActive = true;
}

/*!****************************************************************************
 * @brief
 * 1-Sekunden Task für die GPS-Datenverarbeitung
 *
 * Derzeit nicht genutzt 
 *
 * @date  06.11.2019
 ******************************************************************************/
void GPSHandler_Task1s(void)
{
  
}