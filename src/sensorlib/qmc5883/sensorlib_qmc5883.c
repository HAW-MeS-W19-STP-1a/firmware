/*!****************************************************************************
 * @file
 * sensorlib_qmc5883.c
 *
 * Ansteuerung und Auswertung für das QMC5883 Magnetometer
 * 
 * @date  31.10.2019
 ******************************************************************************/

/*- Headerdateien ------------------------------------------------------------*/
#include "powerlib.h"
#include "sensorlib_qmc5883_internal.h"
#include "sensorlib_qmc5883.h"


/*!****************************************************************************
 * @brief
 * Sensor und Struktur initialisieren
 *
 * @param[inout]  *pSensor    Sensor-Struktur
 * @param[in]     ucSlaveAddr I2C Slave-Adresse
 *
 * @date  31.10.2019
 ******************************************************************************/
void QMC5883_Init(QMC5883_Sensor* pSensor, uint8_t ucSlaveAddr)
{
  /* Struktur initialisieren                              */
  uint8_t ucIndex = 0;
  for (ucIndex = 0; ucIndex < sizeof(*pSensor); ++ucIndex)
  {
    *((uint8_t*)pSensor + ucIndex) = 0;
  }
  
  /* Slaveadresse abspeichern                             */
  pSensor->ucSlaveAddr = ucSlaveAddr;
  
  /* Sensor ID prüfen                                     */
  QMC5883_SoftReset(pSensor);
  while (QMC5883_GetChipID(pSensor) != 0xFF) { Power_Wait(); }
}

/*!****************************************************************************
 * @brief
 * Rohdaten vom Sensor einlesen und umrechnen
 *
 * @param[inout]  *pSensor  Sensor-Struktur
 *
 * @date  31.10.2019
 * @date  01.10.2019  Bugfix: alte Statusflags vor dem Neustart zurücksetzen
 ******************************************************************************/
void QMC5883_Update(QMC5883_Sensor* pSensor)
{
  QMC5883_SetSRST(pSensor, 0x01);
  QMC5883_Configure(pSensor, false, false, QMC5883_Oversampling_64, QMC5883_Range_8G, QMC5883_DataRate_10Hz);
  QMC5883_SetMode(pSensor, QMC5883_Mode_CONT);
  while (!QMC5883_IsDataReady(pSensor));
  QMC5883_GetSensorData(pSensor);
  pSensor->sMeasure.uiAzimuth = QMC5883_CalcAzimuth(pSensor);
  pSensor->sMeasure.iTemperature = QMC5883_CalcTemperature(pSensor);
  QMC5883_SoftReset(pSensor);
}

/*!****************************************************************************
 * @brief
 * Referenzwert für Temperaturermittelung setzen
 *
 * Der interne Temperaturfühler ist nur auf eine Steigung kalibriert. Der Off-
 * set muss manuell angerechnet werden.
 * Nach dem Aufruf von QMC5883_SetRefTemp muss ein QMC5883_Update durchgeführt
 * werden, um den neuen Offset zu berücksichtigen.
 *
 * @param[inout]  *pSensor  Sensor-Struktur
 * @param[in]     iRefTemp  Referenz-Temperatur in 0.01°C
 *
 * @date  31.10.2019
 ******************************************************************************/
void QMC5883_SetRefTemp(QMC5883_Sensor* pSensor, int16_t iRefTemp)
{
  pSensor->sCalib.iRefTemp = iRefTemp;
}