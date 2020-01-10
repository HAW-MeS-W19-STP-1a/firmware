#ifndef SENSORLIB_POWER_H_
#define SENSORLIB_POWER_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include "stm8l15x.h"


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Strukturdefinition für Strom- und Spannungsmessung
 *
 * @date 06.01.2020
 ******************************************************************************/
typedef struct tag_Power_Sensor {
  /*! Kalibrierungsdaten                                  */
  struct
  {
    /*! Nullpunktabgleich für die Strommessung als Rohw.  */
    uint16_t uiCurrZeroOffset;
    
    /*! Kennliniensteigung für den Strom in 1mA/lsb       */
    int16_t iCurrSlope;
    
    /*! Kennliniensteigung für die Spannung in 1mV/lsb    */
    uint16_t uiVoltSlope;
  } sCalib;
  
  /*! Konfiguration der ADC-Kanäle                        */
  struct
  {
    /*! Kanal für die Strommessung                        */
    ADC_Channel_TypeDef eCurr;
    
    /*! Kanal für die Spannungsmessung                    */
    ADC_Channel_TypeDef eVolt;
  } sChannel;
  
  /*! Rohdaten                                            */
  struct 
  {
    /*! ADC Rohwert für die Spannungsmessung              */
    uint16_t uiRawVolt;
    
    /*! ADC Rohwert für die Strommessung                  */
    uint16_t uiRawCurr;
  } sRaw;
  
  /*! Umgerechnete Messwerte                              */
  struct {
    /*! Spannungsmesswert in 1mV                          */
    uint16_t uiVolt;
    
    /*! Strommesswert in 1mA                              */
    int16_t iCurr;
  } sMeasure;
} Power_Sensor;


/*- Funktionsprototypen ------------------------------------------------------*/
void Power_Init(Power_Sensor* pSensor, ADC_Channel_TypeDef eChCurr, ADC_Channel_TypeDef eChVolt, GPIO_TypeDef* pCurrPort, GPIO_Pin_TypeDef eCurrPin, GPIO_TypeDef* pVoltPort, GPIO_Pin_TypeDef eVoltPin);
void Power_SetCurrRef(Power_Sensor* pSensor, uint16_t uiOffset, int16_t iSlope);
void Power_SetVoltRef(Power_Sensor* pSensor, uint16_t uiSlope);
void Power_Update(Power_Sensor* pSensor);

#endif /* SENSORLIB_POWER_H_ */