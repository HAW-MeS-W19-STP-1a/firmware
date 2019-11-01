/*!****************************************************************************
 * @file
 * sensorlib_cputemp_internal.c
 *
 * Interne Funktionen zur Auswertung des On-Die Temperatursensors
 *
 * @date  31.10.2019
 ******************************************************************************/

/*- Headerdateien ------------------------------------------------------------*/
#include "stm8l15x.h"
#include "powerlib.h"
#include "sensorlib_cputemp.h"
#include "sensorlib_cputemp_internal.h"


/*!****************************************************************************
 * @brief
 * Temperatursensor-Rohwert mittels ADC einlesen
 *
 * @param[inout]  *pSensor  Sensor-Struktur
 *
 * @date  31.10.2019
 ******************************************************************************/
void CPUTemp_GetSensorData(CPUTemp_Sensor* pSensor)
{
  ADC_ChannelCmd(ADC1, ADC_Channel_TempSensor, ENABLE);
  ADC_SoftwareStartConv(ADC1);
  while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) { Power_Wait() };
  pSensor->sRaw.uiRawTemp = ADC_GetConversionValue(ADC1);
  ADC_ChannelCmd(ADC1, ADC_Channel_TempSensor, DISABLE);
}

/*!****************************************************************************
 * @brief
 * Roh-Messwert in Temperatur in 1°C umrechnen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    int8_t    Temperatur in 1°C
 *
 * @date  31.10.2019
 ******************************************************************************/
int8_t CPUTemp_CalcTemperature(CPUTemp_Sensor* pSensor)
{
  return (int16_t)((pSensor->sRaw.uiRawTemp * 15) >> 5) - 273;
}