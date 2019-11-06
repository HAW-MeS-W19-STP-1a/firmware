/*!****************************************************************************
 * @file
 * sensorlib_qmc5883_internal.h
 *
 * Interne Funktionen zur Auswertung und Kommunikation mit dem QMC5883 Magneto-
 * meter
 *
 * @date  31.10.2019
 ******************************************************************************/
 
/*- Headerdateien ------------------------------------------------------------*/
#include <math.h>
#include "stm8l15x.h"
#include "commlib.h"
#include "sensorlib_qmc5883_internal.h"


/*- Lokale Funktionen --------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * QMC5883 Register lesen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @param[in] eReg      Registeradresse
 * @return    uint8_t   Wert
 *
 * @date  31.10.2019
 ******************************************************************************/
static @inline uint8_t QMC5883_ReadRegister(QMC5883_Sensor* pSensor, QMC5883_Register eReg)
{
  return I2CMaster_RegisterRead(pSensor->ucSlaveAddr, eReg);
}

/*!****************************************************************************
 * @brief
 * QMC5883 Register schreiben
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @param[in] eReg      Registeradresse
 * @param[in] ucVal     Wert
 *
 * @date  31.10.2019
 ******************************************************************************/
static @inline void QMC5883_WriteRegister(QMC5883_Sensor* pSensor, QMC5883_Register eReg, uint8_t ucVal)
{
  I2CMaster_RegisterWrite(pSensor->ucSlaveAddr, eReg, ucVal);
}


/*!****************************************************************************
 * @brief
 * Chip-ID auslesen 
 *
 * Üblicherweise 0xFF
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    uint8_tu  Sensor Chip-ID
 *
 * @date  31.10.2019
 ******************************************************************************/
uint8_t QMC5883_GetChipID(QMC5883_Sensor* pSensor)
{
  return QMC5883_ReadRegister(pSensor, QMC5883_Register_ID);
}

/*!****************************************************************************
 * @brief
 * Oversampling-Konfiguration lesen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    QMC5883_Oversampling  Konfigurationswert
 *
 * @date  31.10.2019
 ******************************************************************************/
QMC5883_Oversampling QMC5883_GetOversampling(QMC5883_Sensor* pSensor)
{
  return QMC5883_ReadRegister(pSensor, QMC5883_Register_CR1) >> 6;
}

/*!****************************************************************************
 * @brief
 * Messbereich-Konfiguration lesen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    QMC5883_Range Konfigurationswert
 *
 * @date  31.10.2019
 ******************************************************************************/
QMC5883_Range QMC5883_GetRange(QMC5883_Sensor* pSensor)
{
  return (QMC5883_ReadRegister(pSensor, QMC5883_Register_CR1) >> 4) & 0x03;
}

/*!****************************************************************************
 * @brief
 * Konfigurierte Datenrate auslesen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    QMC5883_DataRate  Konfigurationswert
 *
 * @date  31.10.2019
 ******************************************************************************/
QMC5883_DataRate QMC5883_GetDataRate(QMC5883_Sensor* pSensor)
{
  return (QMC5883_ReadRegister(pSensor, QMC5883_Register_CR1) >> 2) & 0x03;
}

/*!****************************************************************************
 * @brief
 * Betriebsmodus auslesen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    QMC5883_Mode  Betriebsmodus
 *
 * @date  31.10.2019
 ******************************************************************************/
QMC5883_Mode QMC5883_GetMode(QMC5883_Sensor* pSensor)
{
  return QMC5883_ReadRegister(pSensor, QMC5883_Register_CR1) & 0x03;
}

/*!****************************************************************************
 * @brief
 * Registerinhalt "SET/RESET"-Register auslesen
 *
 * @param[in] *pSensor  Sensor-Strukutur
 * @return    uint8_t   Registerwert
 *
 * @date  31.10.2019
 ******************************************************************************/
uint8_t QMC5883_GetSRST(QMC5883_Sensor* pSensor)
{
  return QMC5883_ReadRegister(pSensor, QMC5883_Register_SRST);
}

/*!****************************************************************************
 * @brief
 * Prüfen, ob Messwerte zum Ablesen bereitstehen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    bool      true, wenn Daten zum Lesen bereitstehen
 *
 * @date  31.10.2019
 ******************************************************************************/
uint8_t QMC5883_GetStatus(QMC5883_Sensor* pSensor)
{
  return QMC5883_ReadRegister(pSensor, QMC5883_Register_STATUS);
}

/*!****************************************************************************
 * @brief
 * Prüfen, ob Messwerte zum Ablesen bereitstehen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    bool      true, wenn Daten zum Lesen bereitstehen
 *
 * @date  31.10.2019
 ******************************************************************************/
bool QMC5883_IsDataReady(QMC5883_Sensor* pSensor)
{
  uint8_t ucValue =  QMC5883_ReadRegister(pSensor, QMC5883_Register_STATUS);
  return (ucValue & QMC5883_StatusFlag_DRDY) == QMC5883_StatusFlag_DRDY;
}

/*!****************************************************************************
 * @brief
 * Prüfen, ob Overflow-Flag gesetzt ist
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    bool      true, wenn Overflow-Flag gesetzt ist
 *
 * @date  31.10.2019
 ******************************************************************************/
bool QMC5883_IsOverflow(QMC5883_Sensor* pSensor)
{
  uint8_t ucValue =  QMC5883_ReadRegister(pSensor, QMC5883_Register_STATUS);
  return (ucValue & QMC5883_StatusFlag_OVL) == QMC5883_StatusFlag_OVL;
}

/*!****************************************************************************
 * @brief
 * Prüfen, ob DataSkip-Flag gesetzt ist
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    bool      true, wenn DataSkip-Flag gesetzt ist
 *
 * @date  31.10.2019
 ******************************************************************************/
bool QMC5883_IsDataSkip(QMC5883_Sensor* pSensor)
{
  uint8_t ucValue =  QMC5883_ReadRegister(pSensor, QMC5883_Register_STATUS);
  return (ucValue & QMC5883_StatusFlag_DOR) == QMC5883_StatusFlag_DOR;
}

/*!****************************************************************************
 * @brief
 * SET/RESET-Register setzen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @param[in] ucValue   Registerwert
 *
 * @date  31.10.2019
 ******************************************************************************/
void QMC5883_SetSRST(QMC5883_Sensor* pSensor, uint8_t ucValue)
{
  QMC5883_WriteRegister(pSensor, QMC5883_Register_SRST, ucValue);
}

/*!****************************************************************************
 * @brief
 * Sensor konfigurieren
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @param[in] bRolPnt   Registeradresse automatisch inkrementieren
 * @param[in] bIntEnb   INT-Pin aktivieren
 * @param[in] eOversample Oversampling
 * @param[in] eRagne    Messbereich
 * @param[in] eRate     Datenrate
 * 
 * @date  31.10.2019
 ******************************************************************************/
void QMC5883_Configure(QMC5883_Sensor* pSensor, bool bRolPnt, bool bIntEnb, QMC5883_Oversampling eOversample, QMC5883_Range eRange, QMC5883_DataRate eRate)
{
  QMC5883_Mode eMode = QMC5883_GetMode(pSensor);
  QMC5883_WriteRegister(pSensor, QMC5883_Register_CR1, eMode | (eRate << 2) | (eRange << 4) | (eOversample << 6));
  QMC5883_WriteRegister(pSensor, QMC5883_Register_CR2, bIntEnb | (bRolPnt << 6));
}

/*!****************************************************************************
 * @brief
 * Sensor Betriebsmodus setzen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @param[in] eMode     Betriebsmodus
 *
 * @date  31.10.2019
 ******************************************************************************/
void QMC5883_SetMode(QMC5883_Sensor* pSensor, QMC5883_Mode eMode)
{
  uint8_t ucVal = QMC5883_ReadRegister(pSensor, QMC5883_Register_CR1);
  ucVal = (ucVal & ~0x3) | eMode;
  QMC5883_WriteRegister(pSensor, QMC5883_Register_CR1, ucVal);
}

/*!****************************************************************************
 * @brief
 * Soft-Reset durchführen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * 
 * @date  31.10.2019
 ******************************************************************************/
void QMC5883_SoftReset(QMC5883_Sensor* pSensor)
{
  QMC5883_WriteRegister(pSensor, QMC5883_Register_CR2, 1 << 7);
}

/*!****************************************************************************
 * @brief
 * Rohdaten vom Sensor einlesen
 *
 * @param[inout]  *pSensor  Sensor-Struktur
 *
 * @date  31.10.2019
 ******************************************************************************/
void QMC5883_GetSensorData(QMC5883_Sensor* pSensor)
{
  pSensor->sRaw.iRawX = QMC5883_ReadRegister(pSensor, QMC5883_Register_X_LSB) |
                        (QMC5883_ReadRegister(pSensor, QMC5883_Register_X_MSB) << 8);
  pSensor->sRaw.iRawY = QMC5883_ReadRegister(pSensor, QMC5883_Register_Y_LSB) |
                        (QMC5883_ReadRegister(pSensor, QMC5883_Register_Y_MSB) << 8);
  pSensor->sRaw.iRawZ = QMC5883_ReadRegister(pSensor, QMC5883_Register_Z_LSB) |
                        (QMC5883_ReadRegister(pSensor, QMC5883_Register_Z_MSB) << 8);
  pSensor->sRaw.iRawTemp = QMC5883_ReadRegister(pSensor, QMC5883_Register_T_LSB) |
                           (QMC5883_ReadRegister(pSensor, QMC5883_Register_T_MSB) << 8);
}

/*!****************************************************************************
 * @brief
 * Kompassrichtung auf x/y-Ebene berechnen
 *
 * Berechnet die Kompassrichtung mittels atan2 aus den X- und Y-Vektoren der
 * Magnetfeldstärke. Um genau zu sein, muss die Z-Achse senkrecht zum Boden
 * stehen, da ohne Beschleunigungssensor keine Pitch Kompensation möglich ist.
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    uint16_t  Richtung 0° ... 359.9° in 0.1° Auflösung
 *
 * @date  31.10.2019
 * @date  01.11.2019
 ******************************************************************************/
uint16_t QMC5883_CalcAzimuth(QMC5883_Sensor* pSensor)
{
  int iAzimuth = atan2(pSensor->sRaw.iRawY, pSensor->sRaw.iRawX) * 573;
  return (uint16_t)(iAzimuth < 0 ? iAzimuth + 3600 : iAzimuth);
}

/*!****************************************************************************
 * @brief
 * Temperatur ermitteln
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    uint16_t  Temperatur in 0.01°C
 *
 * @date  31.10.2019
 ******************************************************************************/
int16_t QMC5883_CalcTemperature(QMC5883_Sensor* pSensor)
{
  return pSensor->sRaw.iRawTemp + pSensor->sCalib.iRefTemp;
}
