/*!****************************************************************************
 * @file
 * sensorlib_cputemp.h
 *
 * Ansteuerung und Auswertung des On-Die Temperatursensors im Mikrocontroller
 *
 * @date  31.10.2019
 ******************************************************************************/

#ifndef SENSORLIB_CPUTEMP_H_
#define SENSORLIB_CPUTEMP_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Sensor-Struktur für den CPU-Temperatursensor
 *
 * @date  31.10.2019
 ******************************************************************************/
typedef struct tag_CPUTemp_Sensor
{
  /*! Rohdaten                                            */
  struct
  {
    /*! Temperatur-Rohwert                                */
    uint16_t uiRawTemp;
  } sRaw;
  
  /*! Umgerechnete Messwerte                              */
  struct
  {
    /*! Prozessortemperatur in 1°C                        */
    int8_t cTemp;
  } sMeasure;
} CPUTemp_Sensor;


/*- Funktionsdeklarationen ---------------------------------------------------*/
void CPUTemp_Init(CPUTemp_Sensor* pSensor);
void CPUTemp_Update(CPUTemp_Sensor* pSensor);

#endif /* SENSORLIB_CPUTEMP_H_ */