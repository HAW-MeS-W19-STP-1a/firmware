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
  {2986, 3301, Wind_Direction_N},
  {1543, 1705, Wind_Direction_NNE},
  {1753, 1938, Wind_Direction_NE},
  { 318,  352, Wind_Direction_ENE},
  { 354,  391, Wind_Direction_E},
  { 250,  277, Wind_Direction_ESE},
  { 702,  776, Wind_Direction_SE},
  { 481,  531, Wind_Direction_SSE},
  {1092, 1207, Wind_Direction_S},
  { 930, 1028, Wind_Direction_SSW},
  {2395, 2647, Wind_Direction_SW},
  {2278, 2518, Wind_Direction_WSW}, 
  {3592, 3970, Wind_Direction_W},
  {3145, 3476, Wind_Direction_WNW},
  {3372, 3727, Wind_Direction_NW},
  {2671, 2952, Wind_Direction_NNW}
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
 * @date  07.11.2019  LUT hinzugefügt
 ******************************************************************************/
Wind_Direction Wind_CalcDirection(Wind_Sensor* pSensor)
{
  uint8_t ucIndex;
  uint16_t uiRaw = pSensor->sRaw.uiRawDirection;
  for (ucIndex = 0; ucIndex < WIND_DIRLUT_NUM; ++ucIndex)
  {
    if ((uiRaw >= asWindLUT[ucIndex].uiAdcMin) && (uiRaw <= asWindLUT[ucIndex].uiAdcMax))
    {
      return asWindLUT[ucIndex].eDir;
    }
  }
  
  /* Eintrag nicht in LUT gefunden - Standardwert "N"     */
  return Wind_Direction_N;
}