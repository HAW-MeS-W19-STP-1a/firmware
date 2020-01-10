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
#include "motorlib.h"
#include <stdio.h>


/*- Makros -------------------------------------------------------------------*/
#define abs(x)  ((x < 0) ? (-x) : (x))


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
  pSensor->sCalib.fXComp = -1140.0;
  pSensor->sCalib.fYComp = -830.0;
  pSensor->sCalib.fXGain = 0.8921;
  pSensor->sCalib.fYGain = 0.7746;
  pSensor->sCalib.uiNumComp = 0;
  
  /* Slaveadresse abspeichern                             */
  pSensor->ucSlaveAddr = ucSlaveAddr;
  
  /* Sensor ID prüfen                                     */
  QMC5883_SoftReset(pSensor);
  while (QMC5883_GetChipID(pSensor) != 0xFF) { Power_Wait(); }
  
  QMC5883_SetSRST(pSensor, 0x01);
  QMC5883_Configure(pSensor, false, false, QMC5883_Oversampling_512, QMC5883_Range_2G, QMC5883_DataRate_10Hz);
  QMC5883_SetMode(pSensor, QMC5883_Mode_CONT);
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
  /*
  QMC5883_SetSRST(pSensor, 0x01);
  QMC5883_Configure(pSensor, false, false, QMC5883_Oversampling_512, QMC5883_Range_2G, QMC5883_DataRate_10Hz);
  QMC5883_SetMode(pSensor, QMC5883_Mode_CONT);*/
  while (!QMC5883_IsDataReady(pSensor));
  QMC5883_GetSensorData(pSensor);
  pSensor->sMeasure.uiAzimuth = QMC5883_CalcAzimuth(pSensor);
  pSensor->sMeasure.iTemperature = QMC5883_CalcTemperature(pSensor);
  /*QMC5883_SoftReset(pSensor);*/
  
  if (pSensor->bCalActive)
  {
    QMC5883_HandleCalData(pSensor);
    if (Motor_IsTurnReached())
    {
      pSensor->bCalActive = false;
      pSensor->sCalib.fXComp = pSensor->sCalib.fXComp / (float)pSensor->sCalib.uiNumComp;
      pSensor->sCalib.fYComp = pSensor->sCalib.fYComp / (float)pSensor->sCalib.uiNumComp;
      pSensor->sCalib.fXGain = 2.0 / (float)abs(pSensor->sCalib.iXMax - pSensor->sCalib.iXMin);
      pSensor->sCalib.fYGain = 2.0 / (float)abs(pSensor->sCalib.iYMax - pSensor->sCalib.iYMin);
      printf("CAL: %d, %d, %d, %d, %d, %d, %d, %d\r\n", (int)pSensor->sCalib.fXComp, (int)pSensor->sCalib.fYComp, pSensor->sCalib.iXMax, pSensor->sCalib.iXMin, pSensor->sCalib.iYMax, pSensor->sCalib.iYMin, (int)pSensor->sCalib.fXGain * 10000, (int)pSensor->sCalib.fYGain * 10000);
    }
  }
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

void QMC5883_StartCal(QMC5883_Sensor* pSensor)
{
  pSensor->bCalActive = true;
  pSensor->sCalib.fXComp = 0.0;
  pSensor->sCalib.fYComp = 0.0;
  pSensor->sCalib.iXMax = -32768;
  pSensor->sCalib.iXMin = 32767;
  pSensor->sCalib.iYMax = -32768;
  pSensor->sCalib.iXMin = 32767;
  pSensor->sCalib.uiNumComp = 0;
}