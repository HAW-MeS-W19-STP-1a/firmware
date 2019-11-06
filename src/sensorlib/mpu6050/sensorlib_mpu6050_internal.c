/*!****************************************************************************
 * @file
 * sensorlib_mpu6050_internal.c
 *
 * Interne Funktionen zur Ansteuerung des MPU6050 Gyro/Accelerometers
 *
 * @date  06.11.2019
 ******************************************************************************/

/*- Headerdateien ------------------------------------------------------------*/
#include <math.h>
#include "stm8l15x.h"
#include "commlib.h"
#include "sensorlib_mpu6050.h"
#include "sensorlib_mpu6050_internal.h"


/*- Lokale Funktionen --------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * MPU6050 Register lesen
 * 
 * @param[in]   *pSensor  Sensor-Struktur
 * @param[in]   eReg      Registeradresse
 * @return      uint8_t   Registerwert
 * 
 * @date  06.11.2019
 ******************************************************************************/
static @inline uint8_t MPU6050_ReadRegister(MPU6050_Sensor* pSensor, MPU6050_Register eReg)
{
  return I2CMaster_RegisterRead(pSensor->ucSlaveAddr, eReg);
}

/*!****************************************************************************
 * @brief
 * MPU6050 Register schreiben
 *
 * @param[in]   *pSensor  Sensor-Struktur
 * @param[in]   eReg      Registeradresse
 * @param[in]   ucVal     Registerwert
 *
 * @date  06.11.2019
 ******************************************************************************/
static @inline void MPU6050_WriteRegister(MPU6050_Sensor* pSensor, MPU6050_Register eReg, uint8_t ucVal)
{
  I2CMaster_RegisterWrite(pSensor->ucSlaveAddr, eReg, ucVal);
}


/*!****************************************************************************
 * @brief
 * Chip-ID lesen
 *
 * MPU6050 liefert "0x68" als Chip-ID
 *
 * @param[in]   *pSensor  Sensor-Struktur
 * @return      uint8_t   Chip-ID
 * 
 * @date  06.11.2019
 ******************************************************************************/
uint16_t MPU6050_GetChipID(MPU6050_Sensor* pSensor)
{
  uint8_t ucVal = MPU6050_ReadRegister(pSensor, MPU6050_Register_ID);
  return (ucVal & 0x7E);
}

/*!****************************************************************************
 * @brief
 * Statusregister lesen
 *
 * @param[in]   *pSensor  Sensor-Struktur
 * @return      MPU6050_Flag  Statusflags
 *
 * @date  06.11.2019
 ******************************************************************************/
MPU6050_Flag MPU6050_GetStatus(MPU6050_Sensor* pSensor)
{
  return MPU6050_ReadRegister(pSensor, MPU6050_Register_INT_STATUS);
}

/*!****************************************************************************
 * @brief
 * Prüft, ob das Data Ready Flag gesetzt ist
 *
 * Hierfür muss das Flag zuvor im INT_ENABLE Register aktiviert worden sein
 *
 * @param[in]   *pSensor  Sensor-Struktur
 * @return      bool      True, wenn das Flag gesetzt ist
 *
 * @date  06.11.2019
 ******************************************************************************/ 
bool MPU6050_IsDataReady(MPU6050_Sensor* pSensor)
{
  return ((MPU6050_GetStatus(pSensor) & MPU6050_Flag_DRDY) == MPU6050_Flag_DRDY);
}

/*!****************************************************************************
 * @brief
 * Samplerate-Divider für Gyro-Messungen setzen
 *
 * @param[in]   *pSensor  Sensor-Struktur
 * @param[in]   ucDiv     Divider-Wert
 *
 * @date  06.11.2019
 ******************************************************************************/
void MPU6050_SetSampleDiv(MPU6050_Sensor* pSensor, uint8_t ucDiv)
{
  MPU6050_WriteRegister(pSensor, MPU6050_Register_SMPLRT_DIV, ucDiv);
}

/*!****************************************************************************
 * @brief
 * Tiefpassfilter-Konfiguration setzen
 *
 * @param[in]   *pSensor  Sensor-Struktur
 * @param[in]   ucDlpfCfg Konfigurationswert
 *
 * @date  06.11.2019
 ******************************************************************************/
void MPU6050_SetDlpfConfig(MPU6050_Sensor* pSensor, uint8_t ucDlpfCfg)
{
  MPU6050_WriteRegister(pSensor, MPU6050_Register_CONFIG, ucDlpfCfg & 0x7);
}

/*!****************************************************************************
 * @brief
 * Messbereich und Auflösung für Drehratenmessung konfigurieren
 *
 * @param[in]   *pSensor  Sensor-Struktur
 * @param[in]   eRange    Messbereichs-Konfiguration
 *
 * @date  06.11.2019
 ******************************************************************************/
void MPU6050_SetGyroConfig(MPU6050_Sensor* pSensor, MPU6050_GyroRange eRange)
{
  MPU6050_WriteRegister(pSensor, MPU6050_Register_GYRO_CONFIG, eRange << 3);
}

/*!****************************************************************************
 * @brief
 * Messbereich und Auflösung für Beschleunigungsmessung konfigurieren
 *
 * @param[in]   *pSensor  Sensor-Struktur
 * @param[in]   eRange    Messbereichs-Konfiguration
 *
 * @date  06.11.2019
 ******************************************************************************/
void MPU6050_SetAccelConfig(MPU6050_Sensor* pSensor, MPU6050_AccelRange eRange)
{
  MPU6050_WriteRegister(pSensor, MPU6050_Register_ACCEL_CONFIG, (eRange & 0x3) << 3);
}

/*!****************************************************************************
 * @brief
 * Interrupts konfigurieren
 *
 * @param[in]   *pSensor  Sensor-Struktur
 * @param[in]   eFlag     Bitfeld Interruptflags
 *
 * @date  06.11.2019
 ******************************************************************************/
void MPU6050_SetInt(MPU6050_Sensor* pSensor, MPU6050_Flag eFlag)
{
  MPU6050_WriteRegister(pSensor, MPU6050_Register_INT_ENABLE, eFlag);
}

/*!****************************************************************************
 * @brief
 * Einzelne Signalpfade zurücksetzen
 *
 * @param[in]   *pSensor  Sensor-Struktur
 * @param[in]   bGyro     Gyro zurücksetzen
 * @param[in]   bAccel    Accelerometer zurücksetzen
 * @param[in]   bTemp     Temperatursensor zurücksetzen 
 *
 * @date  06.11.2019
 ******************************************************************************/
void MPU6050_ResetSignalPath(MPU6050_Sensor* pSensor, bool bGyro, bool bAccel, bool bTemp)
{
  MPU6050_WriteRegister(pSensor, MPU6050_Register_SIGNAL_PATH_RESET, (bGyro << 2) | (bAccel << 1) | (bTemp << 0));
}

/*!****************************************************************************
 * @brief
 * Alle Sensoren zurücksetzen
 *
 * @param[in]   *pSensor  Sensor-Struktur
 *
 * @date  06.11.2019
 ******************************************************************************/
void MPU6050_ResetSignalPathAll(MPU6050_Sensor* pSensor)
{
  MPU6050_WriteRegister(pSensor, MPU6050_Register_USER_CTRL, 0x01);
}

/*!****************************************************************************
 * @brief
 * Sensormodus konfigurieren
 *
 * @param[in]   *pSensor  Sensor-Struktur
 * @param[in]   eMode     Messmodus
 * @param[in]   eSrc      Taktquelle
 * @param[in]   bDisableTS  Temperatursensor deaktivieren
 * @param[in]   eGyroMode Deaktivierung einzelner Gyro-Kanäle
 * @param[in]   eAccelMode  Deaktivierung einzelner Accelerometer-Kanäle
 *
 * @date  06.11.2019
 ******************************************************************************/
void MPU6050_SetMode(MPU6050_Sensor* pSensor, MPU6050_Mode eMode, MPU6050_ClockSource eSrc, bool bDisableTS, MPU6050_StbyMode eGyroMode, MPU6050_StbyMode eAccelMode)
{
  uint8_t ucVal1, ucVal2;
  ucVal1 = (eSrc & 0x7) | (eMode & 0x60) | (bDisableTS << 3);
  ucVal2 = ((eMode & 0x3) << 6) | ((eAccelMode & 0x7) << 3) | (eGyroMode & 0x7);
  
  MPU6050_WriteRegister(pSensor, MPU6050_Register_PWR_MGMT_1, ucVal1);
  MPU6050_WriteRegister(pSensor, MPU6050_Register_PWR_MGMT_2, ucVal2);
}

/*!****************************************************************************
 * @brief
 * Sleep-Modus (de-)aktivieren, ohne restliche Konfiguration zu beeinflussen
 *
 * @param[in]   *pSensor  Sensor-Struktur
 * @param[in]   bEnable   Sleep-Modus aktivieren
 *
 * @date  06.11.2019
 ******************************************************************************/
void MPU6050_SetSleepMode(MPU6050_Sensor* pSensor, bool bEnable)
{
  uint8_t ucVal = MPU6050_ReadRegister(pSensor, MPU6050_Register_PWR_MGMT_1);
  ucVal = bEnable ? (ucVal | MPU6050_Mode_Sleep) : (ucVal & ~MPU6050_Mode_Sleep);
  MPU6050_WriteRegister(pSensor, MPU6050_Register_PWR_MGMT_1, ucVal);
}

/*!****************************************************************************
 * @brief
 * Device Reset durchführen
 *
 * @param[in]   *pSensor  Sensor-Struktur
 * 
 * @date  06.11.2019
 ******************************************************************************/
void MPU6050_SoftReset(MPU6050_Sensor* pSensor)
{
  MPU6050_WriteRegister(pSensor, MPU6050_Register_PWR_MGMT_1, 0x80);
}

/*!****************************************************************************
 * @brief
 * Sensor-Rohdaten einlesen
 *
 * @param[inout]  *pSensor  Sensor-Struktur
 *
 * @date  06.11.2019
 ******************************************************************************/
void MPU6050_GetSensorData(MPU6050_Sensor* pSensor)
{
  uint8_t aucBuf[14];
  uint8_t ucIndex;
  for (ucIndex = 0; ucIndex < 14; ++ucIndex)
  {
    aucBuf[ucIndex] = MPU6050_ReadRegister(pSensor, MPU6050_Register_ACCEL_XOUT_H + ucIndex);
  }
  
  pSensor->sRaw.iRawX = aucBuf[0] << 8 | aucBuf[1];
  pSensor->sRaw.iRawY = aucBuf[2] << 8 | aucBuf[3];
  pSensor->sRaw.iRawZ = aucBuf[4] << 8 | aucBuf[5];
  pSensor->sRaw.iRawTemp = aucBuf[6] << 8 | aucBuf[7];
}

/*!****************************************************************************
 * @brief
 * Einstellwinkel in 0.1° berechnen
 *
 * @param[in]   *pSensor  Sensor-Struktur
 * @param[in]   bXY       true=Winkel zwischen X,Z; false=Winkel zwischen Y,Z
 * @return      int16_t   Winkel in 0.1°
 *
 * @date  06.11.2019
 ******************************************************************************/
int16_t MPU6050_CalcAngle(MPU6050_Sensor* pSensor, bool bXY)
{
  int iAngle = atan2(pSensor->sRaw.iRawZ, bXY ? pSensor->sRaw.iRawX : pSensor->sRaw.iRawY) * 573;
  return iAngle;
}

/*!****************************************************************************
 * @brief
 * Temperatur in 0.01°C berechnen
 *
 * @param[in]   *pSensor  Sensor-Struktur
 * @return      int16_t   Temperatur in 0.01°C
 *
 * @date  06.11.2019
 ******************************************************************************/
int16_t MPU6050_CalcTemp(MPU6050_Sensor* pSensor)
{
  /* Offset und Sensitivity entspr. Datenblatt Abs. 6.4   */
  int iTemp = ((pSensor->sRaw.iRawTemp + 521) * 10) / 34 + 3500;
  return iTemp;
}