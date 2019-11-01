/*!****************************************************************************
 * @file 
 * sensorlib_bme280_internal.c
 *
 * Interne Datenverarbeitung und Kommunikation für den BME280 Sensor von Bosch
 * Sensortec.
 *
 * @date  31.10.2019
 ******************************************************************************/

/*- Headerdateien ------------------------------------------------------------*/
#include "commlib_i2c.h"
#include "powerlib.h"
#include "sensorlib_bme280_internal.h"


/*- Symbolische Konstanten ---------------------------------------------------*/
/*! Oberer Temperaturgrenzwert in 0.01°C                                      */
#define SENSORLIB_BME280_LIMIT_TEMPMAX  8500L

/*! Unterer Temperaturgrenzwert in 0.01°C                                     */
#define SENSORLIB_BME280_LIMIT_TEMPMIN  (-4000L)

/*! Oberer Luftdruckgrenzwert in 0.01 hPa                                     */
#define SENSORLIB_BME280_LIMIT_PRESSMAX 110000UL

/*! Unterer Luftdruckgrenzwert in 0.01 hPa                                    */
#define SENSORLIB_BME280_LIMIT_PRESSMIN 30000UL

/*! Oberer Luftfeuchtegrenzwert in 22Q10-Darstellung                          */
#define SENSORLIB_BME280_LIMIT_HUMMAX   102400UL


/*- Lokale Funktionsprototypen -----------------------------------------------*/
/*!****************************************************************************
 * @brief
 * BME280-Register lesen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @param[in] eRegister Registeradresse
 * @return    uint8_t   Wert im Register
 *
 * @date  31.10.2019
 ******************************************************************************/
static @inline uint8_t BME280_ReadRegister(BME280_Sensor* pSensor, BME280_Register eRegister)
{
  return I2CMaster_RegisterRead(pSensor->ucSlaveAddr, eRegister);
}

/*!****************************************************************************
 * @brief
 * Wert in BME280-Register schreiben
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @param[in] eRegister Registeradresse
 * @param[in] ucValue   Wert
 *
 * @date  31.10.2019
 ******************************************************************************/
static @inline void BME280_WriteRegister(BME280_Sensor* pSensor, BME280_Register eRegister, uint8_t ucValue)
{
  I2CMaster_RegisterWrite(pSensor->ucSlaveAddr, eRegister, ucValue);
}


/*!****************************************************************************
 * @brief
 * Sensor Chip-ID Register auslesen. Für BME280 ist hier die Kennung "0x60"
 * eingetragen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    uint8_t   0x60, wenn ein BME280-Sensor angeschlossen ist
 *
 * @date  31.10.2019
 ******************************************************************************/
uint8_t BME280_GetChipID(BME280_Sensor* pSensor)
{
  return BME280_ReadRegister(pSensor, BME280_Register_ID);
}

/*!****************************************************************************
 * @brief
 * Umsetznungsmodus auslesen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    BME280_Mode Umsetzungsmodus
 *
 * @date  31.10.2019
 ******************************************************************************/
BME280_Mode BME280_GetMode(BME280_Sensor* pSensor)
{
  return BME280_ReadRegister(pSensor, BME280_Register_CTRL_MEAS) & 0x03;
}

/*!****************************************************************************
 * @brief
 * Standby-Dauer auslesen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    BME280_StandbyTime  Standby-Dauer
 * 
 * @date  31.10.2019
 ******************************************************************************/
BME280_StandbyTime BME280_GetStandbyTime(BME280_Sensor* pSensor)
{
  return (BME280_ReadRegister(pSensor, BME280_Register_CONFIG) >> 5) & 0x07;
}

/*!****************************************************************************
 * @brief
 * IIR-Filterkonstante auslesen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    BME280_Filter IIC-Filterkonfiguration
 *
 * @date  31.10.2019
 ******************************************************************************/ 
BME280_Filter BME280_GetFilter(BME280_Sensor* pSensor)
{
  return (BME280_ReadRegister(pSensor, BME280_Register_CONFIG) >> 2) & 0x07;
}

/*!****************************************************************************
 * @brief
 * Oversampling für Luftfeuchtemessung auslesen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return BME280_Oversampling  Oversampling-Konfiguration
 *
 * @date  31.10.2019
 ******************************************************************************/
BME280_Oversampling BME280_GetHumOversamp(BME280_Sensor* pSensor)
{
  return BME280_ReadRegister(pSensor, BME280_Register_CTRL_HUM) & 0x07;
}

/*!****************************************************************************
 * @brief
 * Oversampling für Temperaturmessung auslesen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return BME280_Oversampling  Oversampling-Konfiguration
 *
 * @date  31.10.2019
 ******************************************************************************/
BME280_Oversampling BME280_GetTempOversamp(BME280_Sensor* pSensor)
{
  return (BME280_ReadRegister(pSensor, BME280_Register_CTRL_MEAS) >> 5) & 0x07;
}

/*!****************************************************************************
 * @brief
 * Oversampling für Luftdruckmessung auslesen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return BME280_Oversampling  Oversampling-Konfiguration
 *
 * @date  31.10.2019
 ******************************************************************************/
BME280_Oversampling BME208_GetPressOversamp(BME280_Sensor* pSensor)
{
  return (BME280_ReadRegister(pSensor, BME280_Register_CTRL_MEAS) >> 2) & 0x07;
}

/*!****************************************************************************
 * @brief
 * Prüfen, ob Messung noch läuft
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    bool      true, wenn Messung noch läuft
 *
 * @date  31.10.2019
 ******************************************************************************/
bool BME280_IsMeasuring(BME280_Sensor* pSensor)
{
  register uint8_t ucStatus = BME280_ReadRegister(pSensor, BME280_Register_STATUS);
  return ((ucStatus & BME280_StatusFlag_MEASURING) == BME280_StatusFlag_MEASURING);
}

/*!****************************************************************************
 * @brief
 * Prüfen, ob interner Speicher noch geschrieben wird
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @return    bool      true, wenn Speicher geschrieben wird
 *
 * @date  31.10.2019
 ******************************************************************************/
bool BME280_IsNvmUpdating(BME280_Sensor* pSensor)
{
  register uint8_t ucStatus = BME280_ReadRegister(pSensor, BME280_Register_STATUS);
  return ((ucStatus & BME280_StatusFlag_IMUPDATE) == BME280_StatusFlag_IMUPDATE);
}

/*!****************************************************************************
 * @brief
 * Umsetzungsmodus setzen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @param[in] eMode     Umsetzungsmodus
 *
 * @date  31.10.2019
 ******************************************************************************/
void BME280_SetMode(BME280_Sensor* pSensor, BME280_Mode eMode)
{
  uint8_t ucVal = BME280_ReadRegister(pSensor, BME280_Register_CTRL_MEAS);
  ucVal = (ucVal & ~0x3) | eMode;
  BME280_WriteRegister(pSensor, BME280_Register_CTRL_MEAS, ucVal);
}

/*!****************************************************************************
 * @brief
 * Filterkonstante und Standby-Zeit setzen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @param[in] eFilter   IIR-Filterkonstante
 * @param[in] eTime     Standby-Zeit
 *
 * @date  31.10.2019
 ******************************************************************************/
void BME280_SetFilterStandby(BME280_Sensor* pSensor, BME280_Filter eFilter, BME280_StandbyTime eTime)
{
  uint8_t ucVal = (eFilter << 2) | (eTime << 5);
  BME280_WriteRegister(pSensor, BME280_Register_CONFIG, ucVal);
}

/*!****************************************************************************
 * @brief
 * Oversampling für die drei Kanäle setzen
 *
 * @param[in] *pSensor  Sensor-Struktur
 * @param[in] eHum      Oversampling für Luftfeuchtemessung
 * @param[in] eTemp     Oversampling für Temperaturmessung
 * @param[in] ePress    Oversampling für Luftdruckmessung
 *
 * @date  31.10.2019
 ******************************************************************************/
void BME280_SetOversamp(BME280_Sensor* pSensor, BME280_Oversampling eHum, BME280_Oversampling eTemp, BME280_Oversampling ePress)
{
  uint8_t ucVal;
  
  BME280_Mode eOldMode = BME280_GetMode(pSensor);
  ucVal = eOldMode | (ePress << 2) | (eTemp << 5);
  
  BME280_WriteRegister(pSensor, BME280_Register_CTRL_HUM, eHum);
  BME280_WriteRegister(pSensor, BME280_Register_CTRL_MEAS, ucVal);
}

/*!****************************************************************************
 * @brief
 * Soft-Reset des Sensors durchführen
 *
 * @param[in] *pSensor  Sensor-Struktur
 *
 * @date  31.10.2019
 ******************************************************************************/
void BME280_SoftReset(BME280_Sensor* pSensor)
{
  BME280_WriteRegister(pSensor, BME280_Register_RESET, 0xB6);
  while (BME280_GetChipID(pSensor) != 0x60) { Power_Wait(); }
  while (BME280_IsNvmUpdating(pSensor)) { Power_Wait(); }
}

/*!****************************************************************************
 * @brief
 * Kalibrierungsdaten auslesen
 *
 * @param[inout]  *pSensor  Sensor-Struktur
 *
 * @date  31.10.2019
 ******************************************************************************/
void BME280_GetCalData(BME280_Sensor* pSensor)
{
  #define BME280_CONCAT_BYTES(msb,lsb) (((uint16_t)msb << 8) | (uint16_t)lsb)
  
  uint8_t ucIndex;
  uint8_t aucData[26];
  int16_t dig_H4_lsb;
  int16_t dig_H4_msb;
  int16_t dig_H5_lsb;
  int16_t dig_H5_msb;
  
  /* Temperatur- und Luftdruck-Kalibrierungswerte lesen   */
  for (ucIndex = 0; ucIndex < 26; ++ucIndex)
  {
    aucData[ucIndex] = BME280_ReadRegister(pSensor, BME280_Register_DIG_T1_LSB + ucIndex);
  }
  pSensor->sCalib.uiDigT1 = BME280_CONCAT_BYTES(aucData[1], aucData[0]);
  pSensor->sCalib.iDigT2 = (int16_t)BME280_CONCAT_BYTES(aucData[3], aucData[2]);
  pSensor->sCalib.iDigT3 = (int16_t)BME280_CONCAT_BYTES(aucData[5], aucData[4]);
  pSensor->sCalib.uiDigP1 = BME280_CONCAT_BYTES(aucData[7], aucData[6]);
  pSensor->sCalib.iDigP2 = (int16_t)BME280_CONCAT_BYTES(aucData[9], aucData[8]);
  pSensor->sCalib.iDigP3 = (int16_t)BME280_CONCAT_BYTES(aucData[11], aucData[10]);
  pSensor->sCalib.iDigP4 = (int16_t)BME280_CONCAT_BYTES(aucData[13], aucData[12]);
  pSensor->sCalib.iDigP5 = (int16_t)BME280_CONCAT_BYTES(aucData[15], aucData[14]);
  pSensor->sCalib.iDigP6 = (int16_t)BME280_CONCAT_BYTES(aucData[17], aucData[16]);
  pSensor->sCalib.iDigP7 = (int16_t)BME280_CONCAT_BYTES(aucData[19], aucData[18]);
  pSensor->sCalib.iDigP8 = (int16_t)BME280_CONCAT_BYTES(aucData[21], aucData[20]);
  pSensor->sCalib.iDigP9 = (int16_t)BME280_CONCAT_BYTES(aucData[23], aucData[22]);
  pSensor->sCalib.ucDigH1 = aucData[25];
  
  /* Luftfeuchtigkeits-Kalibrierungswerte lesen           */
  for (ucIndex = 0; ucIndex < 7; ++ucIndex)
  {
    aucData[ucIndex] = BME280_ReadRegister(pSensor, BME280_Register_DIG_H2_LSB + ucIndex);
  }
  pSensor->sCalib.iDigH2 = (int16_t)BME280_CONCAT_BYTES(aucData[1], aucData[0]);
  pSensor->sCalib.ucDigH3 = aucData[2];
  dig_H4_msb = (int16_t)(int8_t)aucData[3] * 16;
  dig_H4_lsb = (int16_t)(aucData[4] & 0x0F);
  pSensor->sCalib.iDigH4 = dig_H4_msb | dig_H4_lsb;
  dig_H5_msb = (int16_t)(int8_t)aucData[5] * 16;
  dig_H5_lsb = (int16_t)(aucData[4] >> 4);
  pSensor->sCalib.iDigH5 = dig_H5_msb | dig_H5_lsb;
  pSensor->sCalib.cDigH6 = (int8_t)aucData[6];
}

/*!****************************************************************************
 * @brief
 * Sensor-Rohdaten lesen 
 *
 * @param[inout]  *pSensor  Sensor-Struktur
 *
 * @date  31.10.2019
 ******************************************************************************/
void BME280_GetSensorData(BME280_Sensor* pSensor)
{
  pSensor->sRaw.ulRawPress = ((uint32_t)BME280_ReadRegister(pSensor, BME280_Register_PRESS_MSB) << 12) |
                             ((uint32_t)BME280_ReadRegister(pSensor, BME280_Register_PRESS_LSB) << 4) |
                             ((uint32_t)BME280_ReadRegister(pSensor, BME280_Register_PRESS_XSB) >> 4);

  pSensor->sRaw.ulRawTemp = ((uint32_t)BME280_ReadRegister(pSensor, BME280_Register_TEMP_MSB) << 12) |
                            ((uint32_t)BME280_ReadRegister(pSensor, BME280_Register_TEMP_LSB) << 4) |
                            ((uint32_t)BME280_ReadRegister(pSensor, BME280_Register_TEMP_XSB) >> 4);

  pSensor->sRaw.uiRawHum = ((uint16_t)BME280_ReadRegister(pSensor, BME280_Register_HUM_MSB) << 8) |
                           ((uint16_t)BME280_ReadRegister(pSensor, BME280_Register_HUM_LSB));
}

/*!****************************************************************************
 * @brief
 * Lufttemperatur in 0.01°C ermitteln
 *
 * Siehe https://github.com/BoschSensortec/BME280_driver 
 *
 * @param[inout]  *pSensor  Sensordaten-Struktur
 * @return  int16_t Lufttemperatur in 0.01°C
 *
 * @date  28.10.2019
 ******************************************************************************/
int16_t BME280_CalcTemp(BME280_Sensor* pSensor)
{
  int32_t var1, var2, temperature;
  int32_t lAdcT = pSensor->sRaw.ulRawTemp;
  
  var1 = (int32_t)((lAdcT >> 3) - ((int32_t)pSensor->sCalib.uiDigT1 << 1));
  var1 = (var1 * ((int32_t)pSensor->sCalib.iDigT2)) >> 11;
  var2 = (int32_t)((lAdcT >> 4) - ((int32_t)pSensor->sCalib.uiDigT1));
  var2 = (((var2 * var2) >> 12) * ((int32_t)pSensor->sCalib.iDigT3)) >> 14;
  pSensor->sRaw.lTfine = var1 + var2;
  temperature = (pSensor->sRaw.lTfine * 5 + 128) >> 8;
  if (temperature < SENSORLIB_BME280_LIMIT_TEMPMIN)
  {
      temperature = SENSORLIB_BME280_LIMIT_TEMPMIN;
  }
  else if (temperature > SENSORLIB_BME280_LIMIT_TEMPMAX)
  {
      temperature = SENSORLIB_BME280_LIMIT_TEMPMAX;
  }

  return (int16_t)temperature;
}

/*!****************************************************************************
 * @brief
 * Luftdruck in 0.01 hPa ermitteln
 *
 * Siehe https://github.com/BoschSensortec/BME280_driver
 *
 * @note Braucht Lufttemperatur in lTfine zur Kompensation 
 *
 * @param[in] *pSensor  Sensordaten-Struktur
 * @return  uint32_t  Luftdruck in 1 Pa
 *
 * @date  28.10.2019
 ******************************************************************************/
uint32_t BME280_CalcPress(BME280_Sensor* pSensor)
{
  int32_t var1;
  int32_t var2;
  int32_t var3;
  int32_t var4;
  uint32_t var5;
  uint32_t pressure;
  
  var1 = (((int32_t)pSensor->sRaw.lTfine) >> 1) - 64000L;
  var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)pSensor->sCalib.iDigP6);
  var2 = var2 + ((var1 * ((int32_t)pSensor->sCalib.iDigP5)) << 1);
  var2 = (var2 >> 2) + (((int32_t)pSensor->sCalib.iDigP4) << 16);
  var3 = (pSensor->sCalib.iDigP3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3;
  var4 = (((int32_t)pSensor->sCalib.iDigP2) * var1) >> 1;
  var1 = (var3 + var4) >> 18;
  var1 = (((32768 + var1)) * ((int32_t)pSensor->sCalib.uiDigP1)) >> 15;

  /* avoid exception caused by division by zero */
  if (var1)
  {
    var5 = (uint32_t)((uint32_t)1048576) - pSensor->sRaw.ulRawPress;
    pressure = ((uint32_t)(var5 - (uint32_t)(var2 >> 12))) * 3125;
    if (pressure < 0x80000000)
    {
      pressure = (pressure << 1) / ((uint32_t)var1);
    }
    else
    {
      pressure = (pressure / (uint32_t)var1) << 1;
    }
    var1 = (((int32_t)pSensor->sCalib.iDigP9) * ((int32_t)(((pressure >> 3) * (pressure >> 3)) >> 13))) >> 12;
    var2 = (((int32_t)(pressure >> 2)) * ((int32_t)pSensor->sCalib.iDigP8)) >> 13;
    pressure = (uint32_t)((int32_t)pressure + ((var1 + var2 + pSensor->sCalib.iDigP7) >> 4));
    if (pressure < SENSORLIB_BME280_LIMIT_PRESSMIN)
    {
      pressure = SENSORLIB_BME280_LIMIT_PRESSMIN;
    }
    else if (pressure > SENSORLIB_BME280_LIMIT_PRESSMAX)
    {
      pressure = SENSORLIB_BME280_LIMIT_PRESSMAX;
    }
  }
  else
  {
    pressure = SENSORLIB_BME280_LIMIT_PRESSMIN;
  }

  return pressure;
}

/*!****************************************************************************
 * @brief
 * Luftfeuchtigkeit in 22Q10 fixed point %RH ermitteln
 *
 * Siehe https://github.com/BoschSensortec/BME280_driver
 *
 * @note Braucht Lufttemperatur in lTfine zur Kompensation 
 *
 * @param[in] *pSensor  Sensordaten-Struktur
 * @return  uint32_t  Luftfeuchtigkeit in 1/1024 %RH
 *
 * @date 28.10.2019
 ******************************************************************************/
uint32_t BME280_CalcHum(BME280_Sensor* pSensor)
{
  int32_t var1;
  int32_t var2;
  int32_t var3;
  int32_t var4;
  int32_t var5;
  uint32_t humidity;

  var1 = pSensor->sRaw.lTfine - ((int32_t)76800);
  var2 = (int32_t)((uint32_t)pSensor->sRaw.uiRawHum << 14);
  var3 = (int32_t)(((int32_t)pSensor->sCalib.iDigH4) << 20);
  var4 = ((int32_t)pSensor->sCalib.iDigH5) * var1;
  var5 = (((var2 - var3) - var4) + (int32_t)16384) >> 15;
  var2 = (var1 * ((int32_t)pSensor->sCalib.cDigH6)) >> 10;
  var3 = (var1 * ((int32_t)pSensor->sCalib.ucDigH3)) >> 11;
  var4 = ((var2 * (var3 + (int32_t)32768)) >> 10) + (int32_t)2097152;
  var2 = ((var4 * ((int32_t)pSensor->sCalib.iDigH2)) + 8192) >> 14;
  var3 = var5 * var2;
  var4 = ((var3 >> 15) * (var3 >> 15)) >> 7;
  var5 = var3 - ((var4 * ((int32_t)pSensor->sCalib.ucDigH1)) >> 5);
  var5 = (var5 < 0 ? 0 : var5);
  var5 = (var5 > 419430400 ? 419430400 : var5);
  humidity = (uint32_t)(var5 >> 12);
  if (humidity > SENSORLIB_BME280_LIMIT_HUMMAX)
  {
      humidity = SENSORLIB_BME280_LIMIT_HUMMAX;
  }

  return humidity;
}
