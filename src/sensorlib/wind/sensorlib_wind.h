/*!****************************************************************************
 * @file
 * sensorlib_wind.h
 *
 * Auswertung der Windfahne und des Anemometers
 *
 * @date 31.10.2019
 ******************************************************************************/

#ifndef SENSORLIB_WIND_H_
#define SENSORLIB_WIND_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Windrichtung
 *
 * Ermittlung nicht als kontinuierliche Größe möglich, da die Richtung an der
 * Windfahne durch Magnetschalter in Sektoren eingeteilt ist.
 *
 * @date 31.10.2019
 ******************************************************************************/
typedef enum tag_Wind_Direction
{
  Wind_Direction_N,
  Wind_Direction_NNE,
  Wind_Direction_NE,
  Wind_Direction_ENE,
  Wind_Direction_E,
  Wind_Direction_ESE,
  Wind_Direction_SE,
  Wind_Direction_SSE,
  Wind_Direction_S,
  Wind_Direction_SSW,
  Wind_Direction_SW,
  Wind_Direction_WSW,
  Wind_Direction_W,
  Wind_Direction_WNW,
  Wind_Direction_NW,
  Wind_Direction_NNW
} Wind_Direction;

/*!****************************************************************************
 * @brief
 * Windsensor-Struktur
 *
 * @date  31.10.2019
 ******************************************************************************/
typedef struct tag_Wind_Sensor
{
  /*! Kalibrierungsdaten                                  */
  struct
  {
    /*! Timer-Abfrageintervall                            */
    uint16_t uiPollInterval;
  } sCalib;
  
  /*! Rohdaten                                            */
  struct
  {
    bool bRawDataUpdate;
    uint16_t uiRawVelocity;
    uint16_t uiRawDirection;
  } sRaw;
  
  /*! Umgerechnete Messwerte                              */
  struct
  {
    /*! Windgeschwindigkeit in m/s                        */
    uint16_t uiVelocity;
    
    /*! Windrichtung als Himmelsrichtung                  */
    Wind_Direction eDirection;
  } sMeasure;
} Wind_Sensor;


/*- Funktionsprototypen ------------------------------------------------------*/
void Wind_Init(Wind_Sensor* pSensor, uint16_t uiPollInterval);
void Wind_Update(Wind_Sensor* pSensor);

#endif /* SENSORLIB_WIND_H_ */