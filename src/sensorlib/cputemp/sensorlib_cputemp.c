/*!****************************************************************************
 * @file
 * sensorlib_cputemp.h
 *
 * Ansteuerung und Auswertung des On-Die Temperatursensors im Mikrocontroller
 *
 * @date  31.10.2019
 ******************************************************************************/
 
/*- Headerdateien ------------------------------------------------------------*/
#include "stm8l15x.h"
#include "sensorlib_cputemp_internal.h"
#include "sensorlib_cputemp.h"


/*!****************************************************************************
 * @brief
 * Sensor und Struktur initialisieren
 *
 * @param[inout]  *pSensor  Sensor-Struktur
 * 
 * @date  31.10.2019
 ******************************************************************************/
void CPUTemp_Init(CPUTemp_Sensor* pSensor)
{
  uint8_t ucIndex;
  for (ucIndex = 0; ucIndex < sizeof(*pSensor); ++ucIndex)
  {
    *((uint8_t*)pSensor + ucIndex) = 0;
  }
  
  /* Temperatursensor aktivieren                          */
  ADC_TempSensorCmd(ENABLE);
}

/*!****************************************************************************
 * @brief
 * Rohdaten einlesen und in Temperaturmesswert umrechnen
 *
 * @param[inout]  *pSensor  Sensor-Struktur
 *
 * @date  31.10.2019
 ******************************************************************************/
void CPUTemp_Update(CPUTemp_Sensor* pSensor)
{
  CPUTemp_GetSensorData(pSensor);
  pSensor->sMeasure.cTemp = CPUTemp_CalcTemperature(pSensor);
}