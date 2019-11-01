/*!****************************************************************************
 * @file 
 * sensorlib_bme280.c
 *
 * Ansteuerung für den Wettersensor BME280 von Bosch Sensortec
 *
 * @date  28.10.2019
 * @date  31.10.2019  Update-Routine, Strukturtyp für Sensordaten
 ******************************************************************************/

/*- Headerdateien ------------------------------------------------------------*/
#include "commlib_i2c.h"
#include "sensorlib_bme280_internal.h"
#include "sensorlib_bme280.h"


/*!****************************************************************************
 * @brief
 * Sensor initialisieren
 * 
 * @param[inout]  *pSensor    Zeiger auf Sensor-Struct
 * @param[in]     ucSlaveAddr I2C Slave-Adresse des Sensors (default 0x76)
 *
 * @date  28.10.2019
 * @date  31.10.2019  Funktionalität ausgelagert in _internal-Modul
 ******************************************************************************/
void BME280_Init(BME280_Sensor* pSensor, uint8_t ucSlaveAddr)
{
  uint8_t ucIndex;
  
  /* Struktur initialisieren                              */
  for (ucIndex = 0; ucIndex < sizeof(*pSensor); ++ucIndex)
  {
    *((uint8_t*)pSensor + ucIndex) = 0;
  }
  
  /* I2C Slave-Adresse abspeichern                        */
  pSensor->ucSlaveAddr = ucSlaveAddr;

  /* Sensor initialisieren und Kalibrierungswerte lesen   */
  BME280_SoftReset(pSensor);  
  BME280_GetCalData(pSensor);
}

/*!****************************************************************************
 * @brief
 * Sensordaten abrufen und Rohdaten kompensieren
 *
 * @param[inout]  *pSensor  Sensor-Struktur
 *
 * @date  31.10.2019
 ******************************************************************************/
void BME280_Update(BME280_Sensor* pSensor)
{
  /* Sensor aus dem Sleep-Mode aufwecken und einzelne     *
   * Aufzeichnung der drei Messwerte starten              */
  BME280_SetMode(pSensor, BME280_Mode_SLEEP);
  BME280_SetOversamp(pSensor, BME280_Oversampling_1, BME280_Oversampling_1, BME280_Oversampling_1);
  BME280_SetMode(pSensor, BME280_Mode_FORCED);
  
  /* Auf Abschluss der Umsetzung warten                   */
  while (BME280_IsMeasuring(pSensor));
  
  /* Sensordaten auslesen und kompensierte Werte ber.     */
  BME280_GetSensorData(pSensor);
  pSensor->sMeasure.iTemperature = BME280_CalcTemp(pSensor);
  pSensor->sMeasure.ulPressure = BME280_CalcPress(pSensor);
  pSensor->sMeasure.ulHumidity = BME280_CalcHum(pSensor);
}