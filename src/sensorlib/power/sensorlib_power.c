/*- Headerdateien ------------------------------------------------------------*/
#include "sensorlib_power.h"
#include "sensorlib_power_internal.h"


void Power_Init(Power_Sensor* pSensor, \
  ADC_Channel_TypeDef eChCurr, ADC_Channel_TypeDef eChVolt, \
  GPIO_TypeDef* pCurrPort, GPIO_Pin_TypeDef eCurrPin, \
  GPIO_TypeDef* pVoltPort, GPIO_Pin_TypeDef eVoltPin)
{
  /* Struktur initialisieren                              */
  uint8_t ucIndex;
  for (ucIndex = 0; ucIndex < sizeof(*pSensor); ++ucIndex)
  {
    *((uint8_t*)pSensor + ucIndex) = 0;
  }
  
  pSensor->sChannel.eCurr = eChCurr;
  pSensor->sChannel.eVolt = eChVolt;
  
  /* GPIO initialisieren                                  */
  GPIO_Init(pCurrPort, eCurrPin, GPIO_Mode_In_FL_No_IT);
  GPIO_Init(pVoltPort, eVoltPin, GPIO_Mode_In_FL_No_IT);
}

void Power_SetCurrRef(Power_Sensor* pSensor, uint16_t uiOffset, int16_t iSlope)
{
  pSensor->sCalib.uiCurrZeroOffset = uiOffset;
  pSensor->sCalib.iCurrSlope = iSlope;
}

void Power_SetVoltRef(Power_Sensor* pSensor, uint16_t uiSlope)
{
  pSensor->sCalib.uiVoltSlope = uiSlope;
}

void Power_Update(Power_Sensor* pSensor)
{
  Power_GetAnalogVal(pSensor);
  
  pSensor->sMeasure.iCurr = Power_CalcCurr(pSensor);
  pSensor->sMeasure.uiVolt = Power_CalcVolt(pSensor);
}