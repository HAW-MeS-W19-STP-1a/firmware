/*!****************************************************************************
 * @file 
 * sensorlib_bme280.h
 *
 * Ansteuerung für den Wettersensor BME280 von Bosch Sensortec
 *
 * @date  28.10.2019
 ******************************************************************************/

#ifndef SENSORLIB_BME280_H_
#define SENSORLIB_BME280_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Sensordaten- und Konfigurationsstruktur für BME280
 *
 * @date  28.10.2019
 ******************************************************************************/
typedef struct tag_BME280_Sensor {
  /*! I2C Slave-Adresse                                   */
  uint8_t ucSlaveAddr;
  
  /*! Kalibrierungsdaten für Temperaturmessnug            */
  uint16_t uiDigT[3];
  
  /*! Kalibrierungdaten für Druckmessung                  */
  uint16_t uiDigP[9];
  
  /*! Kalibrierungsdaten für Luftfeuchtemessung           */
  uint16_t uiDigH[6]; 
  
  /*! Kompensationswert für Lufttemperatur                */
  int32_t lTfine;
} BME280_Sensor;


/*- Funktionsdeklarationen ---------------------------------------------------*/
void BME280_Init(BME280_Sensor* pSensor, uint8_t ucSlaveAddr);

int16_t BME280_GetTemperature(BME280_Sensor* pSensor);
uint32_t BME280_GetPressure(BME280_Sensor* pSensor);
uint32_t BME280_GetHumidity(BME280_Sensor* pSensor);

#endif /* SENSORLIB_BME280_H_ */
