/*!****************************************************************************
 * @file
 * sensorlib_wind.h
 *
 * Auswertung der Windfahne und des Anemometers
 *
 * @date 31.10.2019
 ******************************************************************************/
 
/*- Headerdateien ------------------------------------------------------------*/
#include "stm8l15x.h"
#include "sensorlib_wind_internal.h"
#include "sensorlib_wind.h"


/*!****************************************************************************
 * @brief
 * Windsensoren initialisieren
 *
 * @param[inout]  *pSensor  Sensor-Struktur
 *
 * @date  31.10.2019
 ******************************************************************************/
void Wind_Init(Wind_Sensor* pSensor, uint16_t uiPollInterval)
{
  /* Struktur initialisieren                              */
  uint8_t ucIndex;
  for (ucIndex = 0; ucIndex < sizeof(*pSensor); ++ucIndex)
  {
    *((uint8_t*)pSensor + ucIndex) = 0;
  }
  
  /* Timer 3 als Pulszähler konfigurieren                 */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);
  TIM3_TIxExternalClockConfig(TIM3_TIxExternalCLK1Source_TI1, TIM3_ICPolarity_Falling, 0);
  TIM3_CounterModeConfig(TIM3_CounterMode_Up);
  TIM3_SetCounter(0);
  TIM3_Cmd(ENABLE);
}

/*!****************************************************************************
 * @brief
 * Rohdaten einlesen und Messwerte umrechnen
 *
 * @param[inout]  *pSensor  Sensor-Struktur
 *
 * @date  31.10.2019
 ******************************************************************************/
void Wind_Update(Wind_Sensor* pSensor)
{
  Wind_GetPulseCount(pSensor);
  Wind_GetAnalogVal(pSensor);
  pSensor->sMeasure.uiVelocity = Wind_CalcVelocity(pSensor);
  pSensor->sMeasure.eDirection = Wind_CalcDirection(pSensor);
}