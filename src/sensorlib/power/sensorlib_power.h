#ifndef SENSORLIB_POWER_H_
#define SENSORLIB_POWER_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include "stm8l15x.h"


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Strukturdefinition f�r Strom- und Spannungsmessung
 *
 * @date 06.01.2020
 ******************************************************************************/
typedef struct tag_Power_Sensor {
  /*! Kalibrierungsdaten                                  */
  struct
  {
    /*! Nullpunktabgleich f�r die Strommessung als Rohw.  */
    uint16_t uiCurrZeroOffset;
    
    /*! Kennliniensteigung f�r den Strom in 1mA/lsb       */
    int16_t iCurrSlope;
    
    /*! Kennliniensteigung f�r die Spannung in 1mV/lsb    */
    uint16_t uiVoltSlope;
  } sCalib;
  
  /*! Konfiguration der ADC-Kan�le                        */
  struct
  {
    /*! Kanal f�r die Strommessung                        */
    ADC_Channel_TypeDef eCurr;
    
    /*! Kanal f�r die Spannungsmessung                    */
    ADC_Channel_TypeDef eVolt;
  } sChannel;
  
  /*! Rohdaten                                            */
  struct 
  {
    /*! ADC Rohwert f�r die Spannungsmessung              */
    uint16_t uiRawVolt;
    
    /*! ADC Rohwert f�r die Strommessung                  */
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