/*!****************************************************************************
 * @file 
 * sensorlib_bme280.h
 *
 * Ansteuerung f�r den Wettersensor BME280 von Bosch Sensortec
 *
 * @date  28.10.2019
 * @date  31.10.2019  Update-Routine, Strukturtyp f�r Sensordaten
 ******************************************************************************/

#ifndef SENSORLIB_BME280_H_
#define SENSORLIB_BME280_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>


/*- Symbolische Konstanten ---------------------------------------------------*/
/*! Standardadresse f�r BME280 ohne AD0-Pin                                   */
#define BME280_DEFAULT_ADDR 0x76


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Sensordaten- und Konfigurationsstruktur f�r BME280
 *
 * @date  28.10.2019
 ******************************************************************************/
typedef struct tag_BME280_Sensor {
  /*! I2C Slave-Adresse                                   */
  uint8_t ucSlaveAddr;
  
  /*! Kalibrierungsdaten                                  */
  struct
  {
    /*! Kalibrierungsdaten f�r Temperaturmessnug          */
    uint16_t uiDigT1;
    int16_t iDigT2;
    int16_t iDigT3;
    
    /*! Kalibrierungdaten f�r Druckmessung                */
    uint16_t uiDigP1;
    int16_t iDigP2;
    int16_t iDigP3;
    int16_t iDigP4;
    int16_t iDigP5;
    int16_t iDigP6;
    int16_t iDigP7;
    int16_t iDigP8;
    int16_t iDigP9;
    
    /*! Kalibrierungsdaten f�r Luftfeuchtemessung         */
    uint8_t ucDigH1;
    int16_t iDigH2;
    uint8_t ucDigH3;
    int16_t iDigH4;
    int16_t iDigH5;
    int8_t cDigH6;
  } sCalib;
  
  /*! Rohdaten                                            */
  struct
  {
    /*! Kompensationswert f�r Lufttemperatur              */
    int32_t lTfine;
    
    /*! Rohdaten f�r Temperatur                           */
    uint32_t ulRawTemp;
    
    /*! Rohdaten f�r Luftdruck                            */
    uint32_t ulRawPress;
    
    /*! Rohdaten f�r Luftfeuchte                          */
    uint16_t uiRawHum;
  } sRaw;
  
  /*! Kompensierte Messwerte                              */
  struct
  {    
    /*! Lufttemperatur in 0.01�C                          */
    int16_t iTemperature;
    
    /*! Luftdruck in 0.01 hPa                             */
    uint32_t ulPressure;
    
    /*! Luftfeuchtigkeit in 22Q10 %RH                     */
    uint32_t ulHumidity;
  } sMeasure;
} BME280_Sensor;


/*- Funktionsdeklarationen ---------------------------------------------------*/
void BME280_Init(BME280_Sensor* pSensor, uint8_t ucSlaveAddr);
void BME280_Update(BME280_Sensor* pSensor);

#endif /* SENSORLIB_BME280_H_ */
