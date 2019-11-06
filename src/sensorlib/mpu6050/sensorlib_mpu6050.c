/*!****************************************************************************
 * @file
 * sensorlib_mpu6050.c
 *
 * Ansteuerung und Auswertung für das MPU6050 Gyro/Accelerometer
 *
 * @date 06.11.2019
 ******************************************************************************/

/*- Headerdateien ------------------------------------------------------------*/
#include "powerlib.h"
#include "sensorlib_mpu6050_internal.h"
#include "sensorlib_mpu6050.h"


/*!****************************************************************************
 * @brief
 * Sensor und Struktur initialisieren
 *
 * @param[inout]  *pSensor  Sensor-Struktur
 * @param[in]     ucSlaveAddr I2C Slaveadresse
 * @param[in]     bMeasureTemp  Temperaturmessung aktivieren
 *
 * @date  06.11.2019
 ******************************************************************************/
void MPU6050_Init(MPU6050_Sensor* pSensor, uint8_t ucSlaveAddr, bool bMeasureTemp)
{
  uint8_t ucIndex;
  for (ucIndex = 0; ucIndex < sizeof(*pSensor); ++ucIndex)
  {
    *((uint8_t*)pSensor + ucIndex) = 0;
  }
  
  pSensor->ucSlaveAddr = ucSlaveAddr;
  pSensor->bMeasureTemp = bMeasureTemp;
    
  /* Sensor im Sleep-Modus lassen, bis Update() aufger.   */
  MPU6050_SetAccelConfig(pSensor, MPU6050_AccelRange_2g);
  MPU6050_SetMode(pSensor, MPU6050_Mode_Sleep, MPU6050_ClockSource_INT_8MHz, !bMeasureTemp, MPU6050_StbyMode_ALL, MPU6050_StbyMode_NONE);
  MPU6050_SetInt(pSensor, MPU6050_Flag_DRDY);
}

/*!****************************************************************************
 * @brief
 * Messung starten, Rohdaten einlesen und umrechnen
 *
 * @param[inout]  *pSensor  Sensor-Struktur
 *
 * @date  06.11.2019
 ******************************************************************************/
void MPU6050_Update(MPU6050_Sensor* pSensor)
{
  /* Sensor aufwecken und Daten einlesen                  */
  MPU6050_SetSleepMode(pSensor, false);
  while (!MPU6050_IsDataReady(pSensor)) { Power_Wait(); }
  MPU6050_GetSensorData(pSensor);
  
  /* Umrechnung                                           */
  pSensor->sMeasure.sAngle.iXZ = MPU6050_CalcAngle(pSensor, true);
  pSensor->sMeasure.sAngle.iYZ = MPU6050_CalcAngle(pSensor, false);
  if (pSensor->bMeasureTemp)
  {
    pSensor->sMeasure.iTemperature = MPU6050_CalcTemp(pSensor);
  }
  
  /* Sensor wieder in Sleep Mode versetzen                */
  MPU6050_SetSleepMode(pSensor, true);
}