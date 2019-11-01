/*!****************************************************************************
 * @file 
 * sensorlib_bme280_internal.h
 *
 * Interne Datenverarbeitung und Kommunikation für den BME280 Sensor von Bosch
 * Sensortec.
 *
 * @date  31.10.2019
 ******************************************************************************/

#ifndef SENSORLIB_BME280_INTERNAL_H_
#define SENSORLIB_BME280_INTERNAL_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "sensorlib_bme280.h"


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Optionen für Standby Time
 *
 * @date  31.10.2019
 ******************************************************************************/
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

/*!****************************************************************************
 * @brief
 * Optionen für die IIR-Filterkonstante
 *
 * @date  31.10.2019
 ******************************************************************************/
typedef enum tag_BME280_Filter {
  BME280_Filter_OFF = 0,
  BME280_Filter_2 = 1,
  BME280_Filter_4 = 2,
  BME280_Filter_8 = 3,
  BME280_Filter_16 = 4
} BME280_Filter;

/*!****************************************************************************
 * @brief
 * Optionen für die Oversampling-Konfiguration
 *
 * @date  31.10.2019
 ******************************************************************************/
typedef enum tag_BME280_Oversampling {
  BME280_Oversampling_OFF = 0,
  BME280_Oversampling_1 = 1,
  BME280_Oversampling_2 = 2,
  BME280_Oversampling_4 = 3,
  BME280_Oversampling_8 = 4,
  BME280_Oversampling_16 = 5
} BME280_Oversampling;

/*!****************************************************************************
 * @brief
 * Optionen für den Umsetzungsmodus
 *
 * @date  31.10.2019
 ******************************************************************************/
typedef enum tag_BME280_Mode {
  BME280_Mode_SLEEP = 0,
  BME280_Mode_FORCED = 1,
  BME280_Mode_NORMAL = 3
} BME280_Mode;

/*!****************************************************************************
 * @brief
 * BME280 Register
 *
 * @date  31.10.2019
 ******************************************************************************/
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
  BME280_Register_DIG_H4_MSB = 0xE4,
  BME280_Register_DIG_H4_LSB = 0xE5,
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

/*!****************************************************************************
 * @brief
 * Status-Flags
 *
 * @date  31.10.2019
 ******************************************************************************/
typedef enum tag_BME290_StatusFlag {
  BME280_StatusFlag_MEASURING = (1 << 3),
  BME280_StatusFlag_IMUPDATE = (1 << 0)
} BME280_StatusFlag;


/*- Funktionsprototypen ------------------------------------------------------*/
uint8_t BME280_GetChipID(BME280_Sensor* pSensor);
BME280_Mode BME280_GetMode(BME280_Sensor* pSensor);
BME280_StandbyTime BME280_GetStandbyTime(BME280_Sensor* pSensor);
BME280_Filter BME280_GetFilter(BME280_Sensor* pSensor);
BME280_Oversampling BME280_GetHumOversamp(BME280_Sensor* pSensor);
BME280_Oversampling BME280_GetTempOversamp(BME280_Sensor* pSensor);
BME280_Oversampling BME208_GetPressOversamp(BME280_Sensor* pSensor);
bool BME280_IsMeasuring(BME280_Sensor* pSensor);
bool BME280_IsNvmUpdating(BME280_Sensor* pSensor);

void BME280_SetMode(BME280_Sensor* pSensor, BME280_Mode eMode);
void BME280_SetFilterStandby(BME280_Sensor* pSensor, BME280_Filter eFilter, BME280_StandbyTime eTime);
void BME280_SetOversamp(BME280_Sensor* pSensor, BME280_Oversampling eHum, BME280_Oversampling eTemp, BME280_Oversampling ePress);

void BME280_SoftReset(BME280_Sensor* pSensor);
void BME280_GetCalData(BME280_Sensor* pSensor);
void BME280_GetSensorData(BME280_Sensor* pSensor);

int16_t BME280_CalcTemp(BME280_Sensor* pSensor);
uint32_t BME280_CalcPress(BME280_Sensor* pSensor);
uint32_t BME280_CalcHum(BME280_Sensor* pSensor);

#endif /* SENSORLIB_BME280_INTERNAL_H_ */