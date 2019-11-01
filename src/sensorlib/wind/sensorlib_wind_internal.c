/*!****************************************************************************
 * @file
 * sensorlib_wind_internal.c
 *
 * Interne Funktionen zur Auswertung des Windsensors
 *
 * @date  31.10.2019
 ******************************************************************************/

/*- Headerdateien ------------------------------------------------------------*/
#include "stm8l15x.h"
#include "powerlib.h"
#include "sensorlib_wind.h"
#include "sensorlib_wind_internal.h"


/*!****************************************************************************
 * @brief
 * Anzahl der Pulse auslesen und Zähler zurücksetzen
 *
 * @param[inout]  *pSensor  Sensor-Struktur
 *
 * @date  31.10.2019
 ******************************************************************************/
void Wind_GetPulseCount(Wind_Sensor* pSensor)
{
  pSensor->sRaw.uiRawVelocity = TIM3_GetCounter();
  pSensor->sRaw.bRawDataUpdate = true;
  TIM3_SetCounter(0);
}

/*!****************************************************************************
 * @brief
 * Analogwert an AI16 für Windrichtung lesen
 *
 * @param[inout]  *pSensor  Sensor-Struktur
 *
 * @date  31.10.2019
 ******************************************************************************/
void Wind_GetAnalogVal(Wind_Sensor* pSensor)
{
  ADC_ChannelCmd(ADC1, ADC_Channel_16, ENABLE);
  ADC_SoftwareStartConv(ADC1);
  while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) { Power_Wait(); }
  pSensor->sRaw.uiRawDirection = ADC_GetConversionValue(ADC1);
  ADC_ChannelCmd(ADC1, ADC_Channel_16, DISABLE);
  
  pSensor->sRaw.bRawDataUpdate = true;
}

/*!****************************************************************************
 * @brief
 * Windgeschwindigkeit von PPS in m/s umrechnen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    uint16_t  Windgeschwindigkeit in m/s
 *
 * @date  31.10.2019
 ******************************************************************************/
uint16_t Wind_CalcVelocity(Wind_Sensor* pSensor)
{ 
  return ((uint32_t)pSensor->sRaw.uiRawVelocity * 553) >> 6;
}

/*!****************************************************************************
 * @brief
 * Windrichtung über Lookup-Tabelle aus Analogwert ermitteln
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    Wind_Direction  Windrichtung
 *
 * @date  31.10.2019
 ******************************************************************************/
Wind_Direction Wind_CalcDirection(Wind_Sensor* pSensor)
{
  // TODO: Spannung in Himmelsrichtung über Lookup-Tabelle mit Toleranz
  return 0;
}