/*- Headerdateien ------------------------------------------------------------*/
#include "stm8l15x.h"
#include "motorlib.h"
#include "app_sensors.h"
#include "SolarTracking_Internal.h"
#include "SolarTracking.h"


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Strukturdefinition für den Ausrichtungs-Sollwert
 *
 * @date  30.12.2019
 ******************************************************************************/
typedef struct {
  /*! Azimuth in 0.1°                                     */
  int iAzimuth;
  
  /*! Zenith in 0.1°                                      */
  int iZenith;
  
  /*! Gültigkeits-Flag                                    */
  bool bValid;
} Tracking_Setpoint_TypeDef;

/*!****************************************************************************
 * @brief
 * Strukturdefinition für Sollwertgrenzen
 *
 * @date  30.12.2019
 ******************************************************************************/
typedef struct {
  /*! Aktivierung der Grenzen                             */
  bool bActive;
  
  /*! Minimalwert in 0.1°                                 */
  int iMin;
  
  /*! Maximalwert in 0.1°                                 */
  int iMax;
} Tracking_Limit_TypeDef;


/*- Modulglobale Variablen ---------------------------------------------------*/
/*! Aktivierungszustand des Moduls                                            */
static bool bTrackingActive;

/*! Sollwert für die Ausrichtung                                              */
static Tracking_Setpoint_TypeDef sSetpoint;

/*! Sollwertbegrenzung                                                        */
static Tracking_Limit_TypeDef sLimAzimuth;
static Tracking_Limit_TypeDef sLimZenithUser;
static const Tracking_Limit_TypeDef sLimZenithDevice = { true, 0, 900 };


/*- Lokale Funktionen --------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Prüfen, ob die Sollwerte für Azimuth und Zenith innerhalb der Grnezen liegen
 *
 * @param[in] iAzimuth  Azimuth in 0.1°
 * @param[in] iZenith   Zenith in 0.1°
 * @return    bool      true, wenn innerhalb der Grenzen 
 * @date  30.12.2019
 ******************************************************************************/ 
static bool CheckLimits(int iAzimuth, int iZenith)
{
  bool bSetpValid = true;
  
  /* Physikalische Grenzen des Aufbaus                                        */
  bSetpValid &= (iZenith >= sLimZenithDevice.iMin);
  bSetpValid &= (iZenith <= sLimZenithDevice.iMax);
  
  /* Benutzerdefinierte Grenzen                                               */
  if (sLimAzimuth.bActive)
  {
    bSetpValid &= (iAzimuth >= sLimAzimuth.iMin);
    bSetpValid &= (iAzimuth <= sLimAzimuth.iMax);
  }
  if (sLimZenithUser.bActive)
  {
    bSetpValid &= (iZenith >= sLimZenithUser.iMin);
    bSetpValid &= (iZenith <= sLimZenithUser.iMax);
  }
  
  return bSetpValid;
}

/*!****************************************************************************
 * @brief
 * Neuen Sollwert für die Ausrichtung berechnen
 *
 * @return  bool    true, wenn Sollwert gültig
 *
 * @date 30.12.2019
 ******************************************************************************/
static bool Tracking_CalcSetpoint(void)
{
  double dZenith;
  double dAzimuth;
  int iAzimuth;
  int iZenith;
  RTC_DateTypeDef sDate;
  RTC_TimeTypeDef sTime;
  
  if (!sSensorGPS.sInfo.bLatValid || !sSensorGPS.sInfo.bLongValid)
  {
    /* Kein Position fix                                  */
    sSetpoint.bValid = false;
    return false;
  }
  
  /* Aktuellen Zeitstempel aus RTC                        */
  RTC_GetDate(RTC_Format_BIN, &sDate);
  RTC_GetTime(RTC_Format_BIN, &sTime);
  
  /* Azimuth und Zenit berechnen                          */
  calculate_current_sun_position(
    2000 + sDate.RTC_Year,  /* Jahr                       */
    sDate.RTC_Month,        /* Monat                      */
    sDate.RTC_Date,         /* Tag                        */
    sTime.RTC_Hours +       /* UTC Zeit im Dezimalformat  */
      (sTime.RTC_Minutes / 60.) + 
      (sTime.RTC_Seconds / 3600.0),
    0,                      /* Zeitangabe in UTC          */
    sSensorGPS.sPosition.lLat / 10000.0, /* Breitengrad   */
    sSensorGPS.sPosition.lLong / 10000.0, /* Längengrad   */
    &dAzimuth,
    &dZenith
  );
  iAzimuth = (int)(dAzimuth * 10);
  iZenith = (int)(dZenith * 10);
  
  /* Grenzwerte für Drehung abfangen                      */
  if (!CheckLimits(iAzimuth, iZenith))
  {
    /* Außerhalb der Grenzen                              */
    sSetpoint.bValid = false;
    return false;
  }
  
  /* Sollwert übernehmen                                  */
  sSetpoint.iAzimuth = iAzimuth;
  sSetpoint.iZenith = iZenith;
  sSetpoint.bValid = true;
  return true;
}


/*!****************************************************************************
 * @brief
 * Modul initialisieren
 *
 * @date 30.12.2019
 ******************************************************************************/
void Tracking_Init(void)
{
  bTrackingActive = false;
}

/*!****************************************************************************
 * @brief
 * Pollingroutine für 1s-Task
 *
 * Prüfung, ob Ausrichtung abgeschlossen wurde und evtl. Nachführung starten
 *
 * @date  30.12.2019
 ******************************************************************************/
void Tracking_Task1s(void)
{
  if (bTrackingActive)
  {
    if (sSetpoint.bValid)
    {
      if (Motor_IsTurnReached() && Motor_IsTiltReached())
      { 
        /* Sollwert erreicht                                */
        Motor_Cmd(false);
      }
    }
    else
    {
      /* Sollwert ungültig - Motor anhalten                 */
      Motor_Cmd(false);
    }
  }
}

/*!****************************************************************************
 * @brief
 * Pollingroutine für Wakeup-Task
 *
 * Startet die Ausrichtung neu
 *
 * @date  30.12.2019
 ******************************************************************************/
void Tracking_TaskWakeup(void)
{
  if (bTrackingActive)
  {    
    /* Neue Sollposition berechnen                        */
    if (Tracking_CalcSetpoint())
    {      
      /* Neue Sollposition anfahren                       */
      printf("Track: %d, %d\r\n", sSetpoint.iAzimuth, sSetpoint.iZenith);
      
      Motor_SetTurn(sSetpoint.iAzimuth);
      Motor_SetTilt(sSetpoint.iZenith);
      if (!Motor_IsTiltReached() || !Motor_IsTurnReached())
      {
        Motor_Cmd(true);
      }
    }
    else
    {
      /* Sollwert ungültig - Motor anhalten               */
      Motor_Cmd(false);
    }
  }
}

/*!****************************************************************************
 * @brief
 * Aktivierungszustand des Moduls ändern
 *
 * @param[in] bEnable   Neuer Aktivierungszustand
 *
 * @date  30.12.2019
 ******************************************************************************/
void Tracking_Cmd(bool bEnable)
{
  if (Motor_IsHomingActive())
  {
    Motor_Cmd(true);
  }
  else
  {
    bTrackingActive = bEnable;
  }
}

/*!****************************************************************************
 * @brief
 * Aktuellen Aktivierungszustand auslesen
 *
 * @return  bool    Aktivierungszustand des Moduls
 *
 * @date  30.12.2019
 ******************************************************************************/
bool Tracking_IsEnabled(void)
{
  return bTrackingActive;
}

/*!****************************************************************************
 * @brief
 * Sollwertbegrenzung festlegen
 *
 * Begrenzung wird deaktiviert, wenn Min==Max.
 *
 * @param[in] iMinAzm   Minimalwert Azimuth in 0.1°
 * @param[in] iMaxAzm   Maximalwert Azimuth in 0.1°
 * @param[in] iMinZen   Minimalwert Zenith in 0.1°
 * @param[in] iMaxZen   Maximalwert Zenith in 0.1°
 * 
 * @date 30.12.2019
 ******************************************************************************/
void Tracking_SetLimits(int iMinAzm, int iMaxAzm, int iMinZen, int iMaxZen)
{
  sLimAzimuth.bActive = (iMinAzm != iMaxAzm);
  if (sLimAzimuth.bActive)
  {
    sLimAzimuth.iMin = iMinAzm;
    sLimAzimuth.iMax = iMaxAzm;
  }
  
  sLimZenithUser.bActive = (iMinZen != iMaxZen);
  if (sLimZenithUser.bActive)
  {
    sLimZenithUser.iMin = iMinZen;
    sLimZenithUser.iMax = iMaxZen;
  }
}