/*- Headerdateien ------------------------------------------------------------*/
#include "sensorlib_power_internal.h"
#include "powerlib.h"


void Power_GetAnalogVal(Power_Sensor* pSensor)
{
  /* Strommessung                                         */
  ADC_ChannelCmd(ADC1, pSensor->sChannel.eCurr, ENABLE);
  ADC_SoftwareStartConv(ADC1);
  while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) { Power_Wait(); }
  pSensor->sRaw.uiRawCurr = ADC_GetConversionValue(ADC1);
  ADC_ChannelCmd(ADC1, pSensor->sChannel.eCurr, DISABLE);
  
  /* Spannungsmessung                                     */
  ADC_ChannelCmd(ADC1, pSensor->sChannel.eVolt, ENABLE);
  ADC_SoftwareStartConv(ADC1);
  while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) { Power_Wait(); }
  pSensor->sRaw.uiRawVolt = ADC_GetConversionValue(ADC1);
  ADC_ChannelCmd(ADC1, pSensor->sChannel.eVolt, DISABLE);
}

int16_t Power_CalcCurr(Power_Sensor* pSensor)
{
  int16_t iRawZeroComp;
  iRawZeroComp = pSensor->sRaw.uiRawCurr - pSensor->sCalib.uiCurrZeroOffset;
  return ((int32_t)iRawZeroComp * pSensor->sCalib.iCurrSlope) >> 10;
}

uint16_t Power_CalcVolt(Power_Sensor* pSensor)
{
  return ((uint32_t)pSensor->sRaw.uiRawVolt * pSensor->sCalib.uiVoltSlope) >> 10;
}