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
#include <stdio.h>


/*- Symbolische Konstanten ---------------------------------------------------*/
/*! Anzahl der Tabellenzeilen für Windrichtungsermittlung                     */
#define WIND_DIRLUT_NUM 16


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Tabellenzeile zur Ermittlung der Windrichtung über ADC
 *
 * @date  07.11.2019
 ******************************************************************************/
typedef struct tag_Wind_DirLutRow {
  /*! Untergrenze für ADC-Messwert                        */
  uint16_t uiAdcMin;
  
  /*! Obergrenze für ADC-Messwert                         */
  uint16_t uiAdcMax;
  
  /*! Zugeordneter Richtungswert                          */
  Wind_Direction eDir;
} Wind_DirLutRow;


/*- Modulglobale Variablen ---------------------------------------------------*/
/*! Lookup-Tabelle für Windrichtungsermittlung aus ADC-Messwerten             */
static const Wind_DirLutRow asWindLUT[WIND_DIRLUT_NUM] = {
  {2970, 3010, Wind_Direction_N},
  {1567, 1607, Wind_Direction_NNE},
  {1773, 1813, Wind_Direction_NE},
  { 320,  360, Wind_Direction_ENE},
  { 360,  400, Wind_Direction_E},
  { 245,  285, Wind_Direction_ESE},
  { 728,  768, Wind_Direction_SE},
  { 496,  536, Wind_Direction_SSE},
  {1120, 1160, Wind_Direction_S},
  { 957,  997, Wind_Direction_SSW},
  {2390, 2430, Wind_Direction_SW},
  {2280, 2320, Wind_Direction_WSW}, 
  {3549, 3589, Wind_Direction_W},
  {3117, 3157, Wind_Direction_WNW},
  {3340, 3380, Wind_Direction_NW},
  {2660, 2700, Wind_Direction_NNW}
};


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
  pSensor->sRaw.auiRawVelocity[pSensor->sRaw.ucHead] = TIM3_GetCounter();
  pSensor->sRaw.ucHead += 1;
  while(pSensor->sRaw.ucHead >= NUM_WIND_AVG)
  {
    pSensor->sRaw.ucHead -= NUM_WIND_AVG;
  }
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
 * Mittlere Windgeschwindigkeit von PPS in m/s umrechnen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    uint16_t  Windgeschwindigkeit in m/s
 *
 * @date  31.10.2019
 * @date  19.12.2019  Mittelwertbildung
 ******************************************************************************/
uint16_t Wind_CalcAvgVelocity(Wind_Sensor* pSensor)
{ 
  uint32_t ulVel = 0;
  int iOffset;
  for (iOffset = 0; iOffset < NUM_WIND_AVG; ++iOffset)
  {
    uint8_t ucIndex = pSensor->sRaw.ucHead + iOffset;
    while (ucIndex >= NUM_WIND_AVG)
    {
      ucIndex -= NUM_WIND_AVG;
    }
    ulVel += ((uint32_t)pSensor->sRaw.auiRawVelocity[ucIndex] * 553) >> 6;
  }
  
  return (ulVel >> NUM_WIND_AVG_BITS);
}

/*!****************************************************************************
 * @brief
 * Maximale Windgeschwindigkeit von PPS in m/s umrechnen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    uint16_t  Windgeschwindigkeit in m/s
 *
 * @date  19.12.2019
 ******************************************************************************/
uint16_t Wind_CalcMaxVelocity(Wind_Sensor* pSensor)
{ 
  uint16_t uiVel = 0;
  int iOffset;
  for (iOffset = 0; iOffset < NUM_WIND_AVG; ++iOffset)
  {
    uint16_t uiThisVel;
    uint8_t ucIndex = pSensor->sRaw.ucHead + iOffset;
    while (ucIndex >= NUM_WIND_AVG)
    {
      ucIndex -= NUM_WIND_AVG;
    }
    uiThisVel = ((uint32_t)pSensor->sRaw.auiRawVelocity[ucIndex] * 553) >> 6;
    if (uiThisVel > uiVel)
    {
      uiVel = uiThisVel;
    }
  }
  
  return uiVel;
}

/*!****************************************************************************
 * @brief
 * Windrichtung über Lookup-Tabelle aus Analogwert ermitteln
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    Wind_Direction  Windrichtung
 *
 * @date  31.10.2019
 * @date  07.11.2019  LUT hinzugefügt
 ******************************************************************************/
Wind_Direction Wind_CalcDirection(Wind_Sensor* pSensor)
{
  uint8_t ucIndex;
  Wind_Direction eDir;
  uint16_t uiRaw = pSensor->sRaw.uiRawDirection;
  for (ucIndex = 0; ucIndex < WIND_DIRLUT_NUM; ++ucIndex)
  {
    if ((uiRaw >= asWindLUT[ucIndex].uiAdcMin) && (uiRaw <= asWindLUT[ucIndex].uiAdcMax))
    {
      eDir = asWindLUT[ucIndex].eDir;
      break;
    }
  }
  
  printf("\tWIND -- %d - %d\r\n", (int)eDir, (int)uiRaw);
  
  /* Eintrag nicht in LUT gefunden - Standardwert "N"     */
  return eDir;
}