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
#include "io_map.h"
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
  
  /* GPIO initialisieren                                  */
  GPIO_Init(TIM3_ANEM_IN_PORT, TIM3_ANEM_IN_PIN, GPIO_Mode_In_PU_No_IT);
  GPIO_Init(ADC1_WV_IN16_PORT, ADC1_WV_IN16_PIN, GPIO_Mode_In_FL_No_IT);

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
 * @date  19.12.2019  Nur Windgeschwindigkeit
 ******************************************************************************/
void Wind_UpdateSpd(Wind_Sensor* pSensor)
{
  Wind_GetPulseCount(pSensor);
  pSensor->sMeasure.uiAvgVelocity = Wind_CalcAvgVelocity(pSensor);
  pSensor->sMeasure.uiMaxVelocity = Wind_CalcMaxVelocity(pSensor);
}

/*!****************************************************************************
 * @brief
 * Rohdaten einlesen und Messwerte umrechnen
 *
 * @param[inout]  *pSensor  Sensor-Struktur
 *
 * @date  31.10.2019
 ******************************************************************************/
void Wind_UpdateDir(Wind_Sensor* pSensor)
{
  Wind_GetAnalogVal(pSensor);
  pSensor->sMeasure.eDirection = Wind_CalcDirection(pSensor);
}