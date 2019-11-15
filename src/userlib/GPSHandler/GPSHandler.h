/*!****************************************************************************
 * @file
 * GPSHandler.h
 *
 * Ansteuerung des GPS-Moduls und Verarbeitung der eingehenden NMEA-Daten
 *
 * @date  06.11.2019
 ******************************************************************************/

#ifndef GPSHANDLER_H_
#define GPSHANDLER_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Aus dem GPS-Datenstrom ermittelte Daten 
 *
 * @date  06.11.2019
 ******************************************************************************/
typedef struct tag_GPSHandler_Data {
  /*! Positionierungsinformationen                        */
  struct {
    /*! Breitengrad in 10^-6°                             */
    int32_t lLat;
    
    /*! Längengrad in 10^-6°                              */
    int32_t lLong;
    
    /*! Höhe über MSL in 0.1m                             */
    int16_t iAlt;
  } sPosition;
  
  /*! Informationen über den Datensatz                    */
  struct {
    /*! Anzahl der aktiven Satelliten                     */
    uint8_t ucNumSV;
    
    /*! HDOP in 0.01                                      */
    uint16_t uiHDOP;
        
    /*! Zeitsangabe gültig                                */
    bool bTimeValid;
    
    /*! Datumsangabe gültig                               */
    bool bDateValid;
    
    /*! Positionsinformation gültig                       */
    bool bLatValid;
    bool bLongValid;
    bool bAltValid;
  } sInfo;
  
  /*! GPS-Zeit                                            */
  struct {
    /*! UTC Stunde (24h)                                  */
    uint8_t ucHour;
    
    /*! UTC Minute                                        */
    uint8_t ucMin;
    
    /*! UTC Sekunde                                       */
    uint8_t ucSec;
    
    /*! UTC Dezisekunde                                   */
    uint8_t ucDec;
  } sTime;
  
  /*! Datumsangabe                                        */
  struct {
    /*! Tag im Monat                                      */
    uint8_t ucDay;
    
    /*! Monat                                             */
    uint8_t ucMonth;
    
    /*! Jahr im Jahrhundert                               */
    uint8_t ucYear;
  } sDate;
} GPSHandler_Data;


/*- Globale Variablen --------------------------------------------------------*/
/*! GPS-Datenstruktur                                                         */
extern GPSHandler_Data sSensorGPS;


/*- Funktionsprototypen ------------------------------------------------------*/
void GPSHandler_Init(void);
bool GPSHandler_Poll(void);
void GPSHandler_TaskWakeup(void);
void GPSHandler_Task1s(void);

#endif /* GPSHANDLER_H_ */