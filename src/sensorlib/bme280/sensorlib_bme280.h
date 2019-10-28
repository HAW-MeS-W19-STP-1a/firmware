/*!****************************************************************************
 * @file 
 * sensorlib_bme280.h
 *
 * Ansteuerung f�r den Wettersensor BME280 von Bosch Sensortec
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
 * Sensordaten- und Konfigurationsstruktur f�r BME280
 *
 * @date  28.10.2019
 ******************************************************************************/
typedef struct tag_BME280_Sensor {
  /*! I2C Slave-Adresse                                   */
  uint8_t ucSlaveAddr;
  
  /*! Kalibrierungsdaten f�r Temperaturmessnug            */
  uint16_t uiDigT[3];
  
  /*! Kalibrierungdaten f�r Druckmessung                  */
  uint16_t uiDigP[9];
  
  /*! Kalibrierungsdaten f�r Luftfeuchtemessung           */
  uint16_t uiDigH[6]; 
  
  /*! Kompensationswert f�r Lufttemperatur                */
  int32_t lTfine;
} BME280_Sensor;


/*- Funktionsdeklarationen ---------------------------------------------------*/
void BME280_Init(BME280_Sensor* pSensor, uint8_t ucSlaveAddr);

int16_t BME280_GetTemperature(BME280_Sensor* pSensor);
uint32_t BME280_GetPressure(BME280_Sensor* pSensor);
uint32_t BME280_GetHumidity(BME280_Sensor* pSensor);

#endif /* SENSORLIB_BME280_H_ */
