/*!****************************************************************************
 * @file 
 * sensorlib_bme280.c
 *
 * Ansteuerung für den Wettersensor BME280 von Bosch Sensortec
 *
 * @date  28.10.2019
 ******************************************************************************/

/*- Headerdateien ------------------------------------------------------------*/
#include "commlib_i2c.h"
#include "powerlib.h"
#include "sensorlib_bme280.h"


/*- Typdefinitionen ----------------------------------------------------------*/
typedef enum tag_BME280_StandbyTime {
  BME280_StandbyTime_0_5ms = 0,
  BME280_StandbyTime_62_5ms = 1,
  BME280_StandbyTime_125ms = 2,
  BME280_StandbyTime_250ms = 3,
  BME280_StandbyTime_500ms = 4,
  BME280_StandbyTime_1s = 5,
  BME280_StandbyTime_10ms = 6,
  BME280_StandbyTime_20ms = 7
} BME280_StandbyTime;

typedef enum tag_BME280_Filter {
  BME280_Filter_OFF = 0,
  BME280_Filter_2 = 1,
  BME280_Filter_4 = 2,
  BME280_Filter_8 = 3,
  BME280_Filter_16 = 4
} BME280_Filter;

typedef enum tag_BME280_Oversampling {
  BME280_Oversampling_OFF = 0,
  BME280_Oversampling_1 = 1,
  BME280_Oversampling_2 = 2,
  BME280_Oversampling_4 = 3,
  BME280_Oversampling_8 = 4,
  BME280_Oversampling_16 = 5
} BME280_Oversampling;

typedef enum tag_BME280_Mode {
  BME280_Mode_SLEEP = 0,
  BME280_Mode_FORCED = 1,
  BME280_Mode_NORMAL = 3
} BME280_Mode;

typedef enum tag_BME280_Register {
  BME280_Register_DIG_T1_LSB = 0x88,
  BME280_Register_DIG_T1_MSB = 0x89,
  BME280_Register_DIG_T2_LSB = 0x8A,
  BME280_Register_DIG_T2_MSB = 0x8B,
  BME280_Register_DIG_T3_LSB = 0x8C,
  BME280_Register_DIG_T3_MSB = 0x8D,
  BME280_Register_DIG_P1_LSB = 0x8E,
  BME280_Register_DIG_P1_MSB = 0x8F,
  BME280_Register_DIG_P2_LSB = 0x90,
  BME280_Register_DIG_P2_MSB = 0x91,
  BME280_Register_DIG_P3_LSB = 0x92,
  BME280_Register_DIG_P3_MSB = 0x93,
  BME280_Register_DIG_P4_LSB = 0x94,
  BME280_Register_DIG_P4_MSB = 0x95,
  BME280_Register_DIG_P5_LSB = 0x96,
  BME280_Register_DIG_P5_MSB = 0x97,
  BME280_Register_DIG_P6_LSB = 0x98,
  BME280_Register_DIG_P6_MSB = 0x99,
  BME280_Register_DIG_P7_LSB = 0x9A,
  BME280_Register_DIG_P7_MSB = 0x9B,
  BME280_Register_DIG_P8_LSB = 0x9C,
  BME280_Register_DIG_P8_MSB = 0x9D,
  BME280_Register_DIG_P9_LSB = 0x9E,
  BME280_Register_DIG_P9_MSB = 0x9F,
  BME280_Register_DIG_H1 = 0xA1,
  BME280_Register_ID = 0xD0,
  BME280_Register_RESET = 0xE0,
  BME280_Register_DIG_H2_LSB = 0xE1,
  BME280_Register_DIG_H2_MSB = 0xE2,
  BME280_Register_DIG_H3 = 0xE3,
  BME280_Register_DIG_H4_LSB = 0xE5,
  BME280_Register_DIG_H4_MSB = 0xE4,
  BME280_Register_DIG_H5_LSB = 0xE5,
  BME280_Register_DIG_H5_MSB = 0xE6,
  BME280_Register_DIG_H6 = 0xE7,
  BME280_Register_CTRL_HUM = 0xF2,
  BME280_Register_STATUS = 0xF3,
  BME280_Register_CTRL_MEAS = 0xF4,
  BME280_Register_CONFIG = 0xF5,
  BME280_Register_PRESS_MSB = 0xF7,
  BME280_Register_PRESS_LSB = 0xF8,
  BME280_Register_PRESS_XSB = 0xF9,
  BME280_Register_TEMP_MSB = 0xFA,
  BME280_Register_TEMP_LSB = 0xFB,
  BME280_Register_TEMP_XSB = 0xFC,
  BME280_Register_HUM_MSB = 0xFD,
  BME280_Register_HUM_LSB = 0xFE
} BME280_Register;

typedef enum tag_BME290_StatusFlag {
  BME280_StatusFlag_MEASURING = (1 << 3),
  BME280_StatusFlag_IMUPDATE = (1 << 0)
} BME280_StatusFlag;


/*!****************************************************************************
 * @brief
 * Sensor initialisieren
 * 
 * @param[inout]  *pSensor    Zeiger auf Sensor-Struct
 * @param[in]     ucSlaveAddr I2C Slave-Adresse des Sensors (default 0x76)
 *
 * @date  28.10.2019
 ******************************************************************************/
void BME280_Init(BME280_Sensor* pSensor, uint8_t ucSlaveAddr)
{
  uint8_t ucIndex;
  uint8_t ucSensorID;
  
  /* Struktur initialisieren                              */
  for (ucIndex = 0; ucIndex < sizeof(*pSensor); ++ucIndex)
  {
    *((uint8_t*)pSensor + ucIndex) = 0;
  }
  
  pSensor->ucSlaveAddr = ucSlaveAddr;
  
  while (I2CMaster_RegisterRead(ucSlaveAddr, BME280_Register_ID) != 0x60);
  I2CMaster_RegisterWrite(ucSlaveAddr, BME280_Register_RESET, 0xB6);
  while (I2CMaster_RegisterRead(ucSlaveAddr, BME280_Register_ID) != 0x60);
  while (I2CMaster_RegisterRead(ucSlaveAddr, BME280_Register_ID) != 0x60);
  
  /* Kalibrierungsdaten für Temperatursensor auslesen     */
  for (ucIndex = 0; ucIndex < 3; ++ucIndex)
  {
    pSensor->uiDigT[ucIndex] = 
      I2CMaster_RegisterRead(ucSlaveAddr, BME280_Register_DIG_T1_LSB + (ucIndex << 1)) |
      (I2CMaster_RegisterRead(ucSlaveAddr, BME280_Register_DIG_T1_MSB + (ucIndex << 1)) << 8);
  }
  
  /* Kalibrierungsdaten für Luftdrucksensor auslesen      */
  for (ucIndex = 0; ucIndex < 9; ++ucIndex)
  {
    pSensor->uiDigP[ucIndex] =
      I2CMaster_RegisterRead(ucSlaveAddr, BME280_Register_DIG_P1_LSB + (ucIndex << 1)) +
      (I2CMaster_RegisterRead(ucSlaveAddr, BME280_Register_DIG_P1_MSB + (ucIndex << 1)) << 8);
  }
  
  /* Kalibrierungsdaten für Luftfeuchte auslesen          */
  pSensor->uiDigH[0] = I2CMaster_RegisterRead(ucSlaveAddr, BME280_Register_DIG_H1);
  pSensor->uiDigH[1] = I2CMaster_RegisterRead(ucSlaveAddr, BME280_Register_DIG_H2_LSB) + 
                      (I2CMaster_RegisterRead(ucSlaveAddr, BME280_Register_DIG_H2_MSB) << 8);
  pSensor->uiDigH[2] = I2CMaster_RegisterRead(ucSlaveAddr, BME280_Register_DIG_H3);
  pSensor->uiDigH[3] = (I2CMaster_RegisterRead(ucSlaveAddr, BME280_Register_DIG_H4_LSB) & 0x0F) +
                      (I2CMaster_RegisterRead(ucSlaveAddr, BME280_Register_DIG_H4_MSB) << 4);
  pSensor->uiDigH[4] = ((I2CMaster_RegisterRead(ucSlaveAddr, BME280_Register_DIG_H5_LSB) >> 4) & 0x0F) +
                      (I2CMaster_RegisterRead(ucSlaveAddr, BME280_Register_DIG_H5_MSB) << 4);
  pSensor->uiDigH[5] = I2CMaster_RegisterRead(ucSlaveAddr, BME280_Register_DIG_H6);
}

/*!****************************************************************************
 * @brief
 * Lufttemperatur in 0.01°C ermitteln
 *
 * @param[inout]  *pSensor  Sensordaten-Struktur
 * @return  int16_t Lufttemperatur in 0.01°C
 *
 * @date  28.10.2019
 ******************************************************************************/
int16_t BME280_GetTemperature(BME280_Sensor* pSensor)
{
  int32_t lAdcT, lVar1, lVar2, lT;
  
  /* Sensor für einzelne Messung starten                  */
  I2CMaster_RegisterWrite(pSensor->ucSlaveAddr, BME280_Register_CTRL_MEAS, BME280_Oversampling_1 << 5);
  I2CMaster_RegisterWrite(pSensor->ucSlaveAddr, BME280_Register_CTRL_MEAS, (BME280_Oversampling_1 << 5) | BME280_Mode_FORCED);
  
  /* Auf Ergebnis warten (~12ms)                          *
   * Sensor geht danach automatisch in SLEEP              */
  while ((I2CMaster_RegisterRead(pSensor->ucSlaveAddr, BME280_Register_STATUS) & BME280_StatusFlag_MEASURING) != 0);

  /* Rohwerte auslesen                                    */
  lAdcT = (uint32_t)I2CMaster_RegisterRead(pSensor->ucSlaveAddr, BME280_Register_TEMP_MSB) << 12;
  lAdcT |= (uint32_t)I2CMaster_RegisterRead(pSensor->ucSlaveAddr, BME280_Register_TEMP_LSB) << 4;
  lAdcT |= (I2CMaster_RegisterRead(pSensor->ucSlaveAddr, BME280_Register_TEMP_XSB) >> 4) & 0x0F;

  /* In Temperaturmesswert umrechnen                      *
   * siehe Datenblatt BME280, Abschnitt 4.2.3             */
  lVar1 = ((((lAdcT >> 3) - ((int32_t)pSensor->uiDigT[0]<<1))) * ((int32_t)pSensor->uiDigT[1])) >> 11;
  lVar2 = (((((lAdcT >> 4) - ((int32_t)pSensor->uiDigT[0])) * ((lAdcT>>4) - ((int32_t)pSensor->uiDigT[0]))) >> 12) * ((int32_t)pSensor->uiDigT[2])) >> 14;
  pSensor->lTfine = lVar1 + lVar2;
  lT = (pSensor->lTfine * 5 + 128) >> 8;
  
  /* Temperatur in 0.01°C                                 */
  return lT;
}

/*!****************************************************************************
 * @brief
 * Luftdruck in 0.01 hPa ermitteln
 *
 * @note Braucht Lufttemperatur in lTfine zur Kompensation 
 *
 * @param[in] *pSensor  Sensordaten-Struktur
 * @return  uint32_t  Luftdruck in 1 Pa
 *
 * @date  28.10.2019
 ******************************************************************************/
uint32_t BME280_GetPressure(BME280_Sensor* pSensor)
{
  int32_t lVar1, lVar2, lAdcP;
  uint32_t ulTempP;
  
  /* Sensor für einzelne Messung starten                  */
  I2CMaster_RegisterWrite(pSensor->ucSlaveAddr, BME280_Register_CTRL_MEAS, BME280_Oversampling_1 << 3);
  I2CMaster_RegisterWrite(pSensor->ucSlaveAddr, BME280_Register_CTRL_MEAS, (BME280_Oversampling_1 << 3) | BME280_Mode_FORCED);
  
  /* Auf Ergebnis warten (~12ms)                          *
   * Sensor geht danach automatisch in SLEEP              */
  while ((I2CMaster_RegisterRead(pSensor->ucSlaveAddr, BME280_Register_STATUS) & BME280_StatusFlag_MEASURING) != 0);
  
  /* Rohwerte auslesen                                    */
  lAdcP = (uint32_t)I2CMaster_RegisterRead(pSensor->ucSlaveAddr, BME280_Register_PRESS_MSB) << 12;
  lAdcP |= (uint32_t)I2CMaster_RegisterRead(pSensor->ucSlaveAddr, BME280_Register_PRESS_LSB) << 4;
  lAdcP |= (I2CMaster_RegisterRead(pSensor->ucSlaveAddr, BME280_Register_PRESS_XSB) >> 4) & 0x0F;
  
  /* Luftdruck (QFE) in Pa berechnen                      *
   * siehe Datenblatt BME280, Abschnitt 8.2               */
  lVar1 = (((int32_t)pSensor->lTfine) >> 1) - (int32_t)64000;
  lVar2 = (((lVar1 >> 2) * (lVar1 >> 2)) >> 11) * ((int32_t)pSensor->uiDigP[5]);
  lVar2 = lVar2 + ((lVar1 * ((int32_t)pSensor->uiDigP[4])) << 1);
  lVar2 = (lVar2 >> 2) + (((int32_t)pSensor->uiDigP[3]) << 16);
  lVar1 = (((pSensor->uiDigP[2] * (((lVar1 >> 2) * (lVar1 >> 2)) >> 13)) >> 3) + ((((int32_t)pSensor->uiDigP[1]) * lVar1) >> 1)) >> 18;
  lVar1 = ((((32768 + lVar1)) * ((int32_t)pSensor->uiDigP[0])) >> 15);
  if (lVar1 == 0)
  {
    return 0;
  }
  ulTempP = (((uint32_t)(((int32_t)1048576) - lAdcP) - (lVar2 >> 12))) * 3125;
  if (ulTempP < 0x80000000)
  {
    ulTempP = (ulTempP << 1) / ((uint32_t)lVar1);
  }
  else
  {
    ulTempP = (ulTempP / (uint32_t)lVar1) * 2;
  }
  lVar1 = (((int32_t)pSensor->uiDigP[8]) * ((int32_t)(((ulTempP >> 3) * (ulTempP >> 3)) >> 13))) >> 12;
  lVar2 = (((int32_t)(ulTempP >> 2)) * ((int32_t)pSensor->uiDigP[7])) >> 13;
  ulTempP = (uint32_t)((int32_t)ulTempP + ((lVar1 + lVar2 + pSensor->uiDigP[6]) >> 4));
  
  /* Luftdruck in Pa (0.01 hPa)                           */
  return ulTempP;
}

/*!****************************************************************************
 * @brief
 * Luftfeuchtigkeit in 22Q10 fixed point %RH ermitteln
 *
 * @note Braucht Lufttemperatur in lTfine zur Kompensation 
 *
 * @param[in] *pSensor  Sensordaten-Struktur
 * @return  uint32_t  Luftfeuchtigkeit in 1/1024 %RH
 *
 * @date 28.10.2019
 ******************************************************************************/
uint32_t BME280_GetHumidity(BME280_Sensor* pSensor)
{
  long lAdcH, lTemp;
  
  /* Sensor für einzelne Messung starten                  */
  I2CMaster_RegisterWrite(pSensor->ucSlaveAddr, BME280_Register_CTRL_HUM, BME280_Oversampling_1);
  I2CMaster_RegisterWrite(pSensor->ucSlaveAddr, BME280_Register_CTRL_MEAS, 0);
  I2CMaster_RegisterWrite(pSensor->ucSlaveAddr, BME280_Register_CTRL_MEAS, BME280_Mode_FORCED);
  
  /* Auf Ergebnis warten (~12ms)                          *
   * Sensor geht danach automatisch in SLEEP              */
  while ((I2CMaster_RegisterRead(pSensor->ucSlaveAddr, BME280_Register_STATUS) & BME280_StatusFlag_MEASURING) != 0);

  /* Rohwerte auslesen                                    */
  lAdcH = (uint32_t)I2CMaster_RegisterRead(pSensor->ucSlaveAddr, BME280_Register_HUM_MSB) << 8;
  lAdcH |= (uint32_t)I2CMaster_RegisterRead(pSensor->ucSlaveAddr, BME280_Register_HUM_LSB);
  
  /* Feuchtigkeit als 22Q10 Fixed-Point Integer           *
   * siehe Datenblatt BME280, Abschnitt 4.2.3             */
  return (uint32_t)(((((((((lAdcH<<14)-(((volatile int32_t)pSensor->uiDigH[3])<<20)-(((volatile int32_t)pSensor->uiDigH[4])*(pSensor->lTfine-76800L)))+((volatile int32_t)16384))>>15)*((((((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[5]))>>10)*((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[2]))>>11)+((volatile int32_t)32768)))>>10)+((volatile int32_t)2097152))*((volatile int32_t)pSensor->uiDigH[1])+8192)>>14))-((((((((((lAdcH<<14)-(((volatile int32_t)pSensor->uiDigH[3])<<20)-(((volatile int32_t)pSensor->uiDigH[4])*(pSensor->lTfine-76800L)))+((volatile int32_t)16384))>>15)*((((((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[5]))>>10)*((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[2]))>>11)+((volatile int32_t)32768)))>>10)+((volatile int32_t)2097152))*((volatile int32_t)pSensor->uiDigH[1])+8192)>>14))>>15)*((((((lAdcH<<14)-(((volatile int32_t)pSensor->uiDigH[3])<<20)-(((volatile int32_t)pSensor->uiDigH[4])*(pSensor->lTfine-76800L)))+((volatile int32_t)16384))>>15)*((((((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[5]))>>10)*((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[2]))>>11)+((volatile int32_t)32768)))>>10)+((volatile int32_t)2097152))*((volatile int32_t)pSensor->uiDigH[1])+8192)>>14))>>15))>>7)*((volatile int32_t)pSensor->uiDigH[0]))>>4))<0?0:((((((lAdcH<<14)-(((volatile int32_t)pSensor->uiDigH[3])<<20)-(((volatile int32_t)pSensor->uiDigH[4])*(pSensor->lTfine-76800L)))+((volatile int32_t)16384))>>15)*((((((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[5]))>>10)*((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[2]))>>11)+((volatile int32_t)32768)))>>10)+((volatile int32_t)2097152))*((volatile int32_t)pSensor->uiDigH[1])+8192)>>14))-((((((((((lAdcH<<14)-(((volatile int32_t)pSensor->uiDigH[3])<<20)-(((volatile int32_t)pSensor->uiDigH[4])*(pSensor->lTfine-76800L)))+((volatile int32_t)16384))>>15)*((((((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[5]))>>10)*((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[2]))>>11)+((volatile int32_t)32768)))>>10)+((volatile int32_t)2097152))*((volatile int32_t)pSensor->uiDigH[1])+8192)>>14))>>15)*((((((lAdcH<<14)-(((volatile int32_t)pSensor->uiDigH[3])<<20)-(((volatile int32_t)pSensor->uiDigH[4])*(pSensor->lTfine-76800L)))+((volatile int32_t)16384))>>15)*((((((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[5]))>>10)*((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[2]))>>11)+((volatile int32_t)32768)))>>10)+((volatile int32_t)2097152))*((volatile int32_t)pSensor->uiDigH[1])+8192)>>14))>>15))>>7)*((volatile int32_t)pSensor->uiDigH[0]))>>4)))>419430400?419430400:(((((((lAdcH<<14)-(((volatile int32_t)pSensor->uiDigH[3])<<20)-(((volatile int32_t)pSensor->uiDigH[4])*(pSensor->lTfine-76800L)))+((volatile int32_t)16384))>>15)*((((((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[5]))>>10)*((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[2]))>>11)+((volatile int32_t)32768)))>>10)+((volatile int32_t)2097152))*((volatile int32_t)pSensor->uiDigH[1])+8192)>>14))-((((((((((lAdcH<<14)-(((volatile int32_t)pSensor->uiDigH[3])<<20)-(((volatile int32_t)pSensor->uiDigH[4])*(pSensor->lTfine-76800L)))+((volatile int32_t)16384))>>15)*((((((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[5]))>>10)*((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[2]))>>11)+((volatile int32_t)32768)))>>10)+((volatile int32_t)2097152))*((volatile int32_t)pSensor->uiDigH[1])+8192)>>14))>>15)*((((((lAdcH<<14)-(((volatile int32_t)pSensor->uiDigH[3])<<20)-(((volatile int32_t)pSensor->uiDigH[4])*(pSensor->lTfine-76800L)))+((volatile int32_t)16384))>>15)*((((((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[5]))>>10)*((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[2]))>>11)+((volatile int32_t)32768)))>>10)+((volatile int32_t)2097152))*((volatile int32_t)pSensor->uiDigH[1])+8192)>>14))>>15))>>7)*((volatile int32_t)pSensor->uiDigH[0]))>>4))<0?0:((((((lAdcH<<14)-(((volatile int32_t)pSensor->uiDigH[3])<<20)-(((volatile int32_t)pSensor->uiDigH[4])*(pSensor->lTfine-76800L)))+((volatile int32_t)16384))>>15)*((((((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[5]))>>10)*((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[2]))>>11)+((volatile int32_t)32768)))>>10)+((volatile int32_t)2097152))*((volatile int32_t)pSensor->uiDigH[1])+8192)>>14))-((((((((((lAdcH<<14)-(((volatile int32_t)pSensor->uiDigH[3])<<20)-(((volatile int32_t)pSensor->uiDigH[4])*(pSensor->lTfine-76800L)))+((volatile int32_t)16384))>>15)*((((((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[5]))>>10)*((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[2]))>>11)+((volatile int32_t)32768)))>>10)+((volatile int32_t)2097152))*((volatile int32_t)pSensor->uiDigH[1])+8192)>>14))>>15)*((((((lAdcH<<14)-(((volatile int32_t)pSensor->uiDigH[3])<<20)-(((volatile int32_t)pSensor->uiDigH[4])*(pSensor->lTfine-76800L)))+((volatile int32_t)16384))>>15)*((((((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[5]))>>10)*((((pSensor->lTfine-76800L)*((volatile int32_t)pSensor->uiDigH[2]))>>11)+((volatile int32_t)32768)))>>10)+((volatile int32_t)2097152))*((volatile int32_t)pSensor->uiDigH[1])+8192)>>14))>>15))>>7)*((volatile int32_t)pSensor->uiDigH[0]))>>4))))>>12);
}