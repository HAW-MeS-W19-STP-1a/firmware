/*!****************************************************************************
 * @file
 * sensorlib_qmc5883_internal.h
 *
 * Interne Funktionen zur Auswertung und Kommunikation mit dem QMC5883 Magneto-
 * meter
 *
 * @date  31.10.2019
 ******************************************************************************/

#ifndef SENSORLIB_QMC5883_INTERNAL_H_
#define SENSORLIB_QMC5883_INTERNAL_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "sensorlib_qmc5883.h"


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * QMC5883-Register
 *
 * @date  31.10.2019
 ******************************************************************************/
typedef enum tag_QMC5883_Register {
  QMC5883_Register_X_LSB = 0x00,
  QMC5883_Register_X_MSB = 0x01,
  QMC5883_Register_Y_LSB = 0x02,
  QMC5883_Register_Y_MSB = 0x03,
  QMC5883_Register_Z_LSB = 0x04,
  QMC5883_Register_Z_MSB = 0x05,
  QMC5883_Register_STATUS = 0x06,
  QMC5883_Register_T_LSB = 0x07,
  QMC5883_Register_T_MSB = 0x08,
  QMC5883_Register_CR1 = 0x09,
  QMC5883_Register_CR2 = 0x0A,
  QMC5883_Register_SRST = 0x0B,
  QMC5883_Register_ID = 0x0D  
} QMC5883_Register;

/*!****************************************************************************
 * @brief
 * Status-Flags
 *
 * @date  31.10.2019
 ******************************************************************************/
typedef enum tag_QMC5883_StatusFlag {
  QMC5883_StatusFlag_DRDY = (1 << 0),
  QMC5883_StatusFlag_OVL = (1 << 1),
  QMC5883_StatusFlag_DOR = (1 << 2)
} QMC5883_StatusFlag;

/*!****************************************************************************
 * @brief
 * Sensor-Betriebsmodus
 *
 * @date  31.10.2019
 ******************************************************************************/
typedef enum tag_QMC5883_Mode {
  QMC5883_Mode_STBY = 0,
  QMC5883_Mode_CONT = 1
} QMC5883_Mode;

/*!****************************************************************************
 * @brief
 * Optionen für die Datenrate
 *
 * @date  31.10.2019
 ******************************************************************************/
typedef enum tag_QMC5883_DataRate {
  QMC5883_DataRate_10Hz = 0,
  QMC5883_DataRate_50Hz = 1,
  QMC5883_DataRate_100Hz = 2,
  QMC5883_DataRate_200Hz = 3
} QMC5883_DataRate;

/*!****************************************************************************
 * @brief
 * Optionen für den Messbereich (Gauss)
 *
 * @date  31.10.2019
 ******************************************************************************/
typedef enum tag_QMC5883_Range {
  QMC5883_Range_2G = 0,
  QMC5883_Range_8G = 1
} QMC5883_Range;

/*!****************************************************************************
 * @brief
 * Optionen für das Oversampling
 *
 * @date  31.10.2019
 ******************************************************************************/
typedef enum tag_QMC5883_Oversampling {
  QMC5883_Oversampling_512 = 0,
  QMC5883_Oversampling_256 = 1,
  QMC5883_Oversampling_128 = 2,
  QMC5883_Oversampling_64 = 3
} QMC5883_Oversampling;

/*!****************************************************************************
 * @brief
 * Konfigurations-Flags
 *
 * @date  31.10.2019
 ******************************************************************************/
typedef enum tag_QMC5883_ConfigFlags {
  QMC5883_ConfigFlags_INTENB = (1 << 0),
  QMC5883_ConfigFlags_ROLPNT = (1 << 6),
  QMC5883_ConfigFlags_SOFTRST = (1 << 7)
} QMC5883_ConfigFlags;


/*- Funktionsprototypen ------------------------------------------------------*/
uint8_t QMC5883_GetChipID(QMC5883_Sensor* pSensor);
QMC5883_Oversampling QMC5883_GetOversampling(QMC5883_Sensor* pSensor);
QMC5883_Range QMC5883_GetRange(QMC5883_Sensor* pSensor);
QMC5883_DataRate QMC5883_GetDataRate(QMC5883_Sensor* pSensor);
QMC5883_Mode QMC5883_GetMode(QMC5883_Sensor* pSensor);
uint8_t QMC5883_GetSRST(QMC5883_Sensor* pSensor);
uint8_t QMC5883_GetStatus(QMC5883_Sensor* pSensor);
bool QMC5883_IsDataReady(QMC5883_Sensor* pSensor);
bool QMC5883_IsOverflow(QMC5883_Sensor* pSensor);
bool QMC5883_IsDataSkip(QMC5883_Sensor* pSensor);

void QMC5883_SetSRST(QMC5883_Sensor* pSensor, uint8_t ucValue);
void QMC5883_Configure(QMC5883_Sensor* pSensor, bool bRolPnt, bool bIntEnb, QMC5883_Oversampling eOversample, QMC5883_Range eRange, QMC5883_DataRate eRate);
void QMC5883_SetMode(QMC5883_Sensor* pSensor, QMC5883_Mode eMode);

void QMC5883_SoftReset(QMC5883_Sensor* pSensor);
void QMC5883_GetSensorData(QMC5883_Sensor* pSensor);

uint16_t QMC5883_CalcAzimuth(QMC5883_Sensor* pSensor);
int16_t QMC5883_CalcTemperature(QMC5883_Sensor* pSensor);

#endif /* SENSORLIB_QMC5883_INTERNAL_H_ */