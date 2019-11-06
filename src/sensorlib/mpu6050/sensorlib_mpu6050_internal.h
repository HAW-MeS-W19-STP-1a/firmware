/*!****************************************************************************
 * @file
 * sensorlib_mpu6050_internal.h
 *
 * Interne Funktionen zur Ansteuerung des MPU6050 Gyro/Accelerometers
 *
 * @date  06.11.2019
 ******************************************************************************/
 
#ifndef SENSORLIB_MPU6050_INTERNAL_H_
#define SENSORLIB_MPU6050_INTERNAL_H_
 
/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "sensorlib_mpu6050.h"


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * MPU6050 Registeradressen  
 *
 * @date  06.11.2019
 ******************************************************************************/
typedef enum tag_MPU6050_Register {
  MPU6050_Register_SMPLRT_DIV = 0x19,
  MPU6050_Register_CONFIG = 0x1A,
  MPU6050_Register_GYRO_CONFIG = 0x1B,
  MPU6050_Register_ACCEL_CONFIG = 0x1C,
  MPU6050_Register_INT_ENABLE = 0x38,
  MPU6050_Register_INT_STATUS = 0x3A,
  MPU6050_Register_ACCEL_XOUT_H = 0x3B,
  MPU6050_Register_ACCEL_XOUT_L = 0x3C,
  MPU6050_Register_ACCEL_YOUT_H = 0x3D,
  MPU6050_Register_ACCEL_YOUT_L = 0x3E,
  MPU6050_Register_ACCEL_ZOUT_H = 0x3F,
  MPU6050_Register_ACCEL_ZOUT_L = 0x40,
  MPU6050_Register_TEMP_OUT_H = 0x41,
  MPU6050_Register_TEMP_OUT_L = 0x42,
  MPU6050_Register_SIGNAL_PATH_RESET = 0x68,
  MPU6050_Register_USER_CTRL = 0x6A,
  MPU6050_Register_PWR_MGMT_1 = 0x6B,
  MPU6050_Register_PWR_MGMT_2 = 0x6C,
  MPU6050_Register_ID = 0x75
} MPU6050_Register;

/*!****************************************************************************
 * @brief
 * Full-Scale Messbereichskonfiguration für Gyro
 *
 * @date  06.11.2019
 ******************************************************************************/
typedef enum tag_MPU6050_GyroRange {
  /*! +-250°/s                                            */
  MPU6050_GyroRange_250 = 0,
  
  /*! +-500°/s                                            */
  MPU6050_GyroRange_500 = 1,
  
  /*! +-1000°/s                                           */
  MPU6050_GyroRange_1000 = 2,
  
  /*! +-2000°/s                                           */
  MPU6050_GyroRange_2000 = 3
} MPU6050_GyroRange;

/*!****************************************************************************
 * @brief
 * Full-Scale Messbereichskonfiguration für Accelerometer
 *
 * @date 06.11.2019
 ******************************************************************************/
typedef enum tag_MPU6050_AccelRange {
  /*! +- 2g                                               */
  MPU6050_AccelRange_2g = 0,
  
  /*! +- 4g                                               */
  MPU6050_AccelRange_4g = 1,
  
  /*! +- 8g                                               */
  MPU6050_AccelRange_8g = 2,
  
  /*! +- 16g                                              */
  MPU6050_AccelRange_16g = 3
} MPU6050_AccelRange;

/*!****************************************************************************
 * @brief
 * Optionen für die interne Taktquelle
 *
 * @date  06.11.2019
 ******************************************************************************/
typedef enum tag_MPU6050_ClockSource {
  /*! Interner 8MHz Oszillator                            */
  MPU6050_ClockSource_INT_8MHz = 0,
  
  /*! PLL mit X-Gyro Referenz                             */
  MPU6050_ClockSource_GYRO_X = 1,
  
  /*! PLL mit Y-Gyro Referenz                             */
  MPU6050_ClockSource_GYRO_Y = 2,
  
  /*! PLL mit Z-Gyro Referenz                             */
  MPU6050_ClockSource_GYRO_Z = 3,
  
  /*! PLL mit externer 32.768kHz Referenz                 */
  MPU6050_ClockSource_EXT_32kHz = 4,
  
  /*! PLL mit externer 19.2MHz Referenz                   */
  MPU6050_ClockSource_EXT_19MHz = 5,
  
  /*! Taktgeber angehalten                                */
  MPU6050_ClockSource_STOP = 7
} MPU6050_ClockSource;

/*!****************************************************************************
 * @brief
 * Optionen für den Aufzeichnungsmodus
 *
 * @date  06.11.2019
 ******************************************************************************/
typedef enum tag_MPU6050_Mode {
  /*! Kontinuierliche Messung                             */
  MPU6050_Mode_Cont = 0,
  
  /*! Sleep-Modus                                         */
  MPU6050_Mode_Sleep = (1 << 6),
  
  /*! Low-Power Mode mit 1.25Hz Intervall                 */
  MPU6050_Mode_Cycle_1Hz = (1 << 5) | 0,
  
  /*! Low-Power Mode mit 5Hz Intervall                    */
  MPU6050_Mode_Cycle_5Hz = (1 << 5) | 1,
  
  /*! Low-Power Mode mit 20Hz Intervall                   */
  MPU6050_Mode_Cycle_20Hz = (1 << 5) | 2,
  
  /*! Low-Power Mode mit 40Hz Intervall                   */
  MPU6050_Mode_Cycle_40Hz = (1 << 5) | 3
} MPU6050_Mode;

/*!****************************************************************************
 * @brief
 * Optionen für die Deaktivierung einzelner Kanäle von Gyro/Accelerometer
 *
 * @date  06.11.2019
 ******************************************************************************/
typedef enum tag_MPU6050_StbyMode {
  /*! Kein Kanal deaktiviert                              */
  MPU6050_StbyMode_NONE = 0,
  
  /*! Z-Kanal deaktiviert                                 */
  MPU6050_StbyMode_Z = 1 << 0,
  
  /*! Y-Kanal deaktiviert                                 */
  MPU6050_StbyMode_Y = 1 << 1,
  
  /*! X-Kanal deaktiviert                                 */
  MPU6050_StbyMode_X = 1 << 2,
  
  /*! Alle Kanäle deaktiviert                             */
  MPU6050_StbyMode_ALL = 7
} MPU6050_StbyMode;

/*!****************************************************************************
 * @brief
 * Interruptflags
 *
 * @date  06.11.2019
 ******************************************************************************/
typedef enum tag_MPU6050_Flag {
  /*! Data Ready Flag: Messung abgeschlossen              */
  MPU6050_Flag_DRDY = 1 << 0
} MPU6050_Flag;


/*- Funktionsprototypen ------------------------------------------------------*/
uint16_t MPU6050_GetChipID(MPU6050_Sensor* pSensor);
MPU6050_Flag MPU6050_GetStatus(MPU6050_Sensor* pSensor);
bool MPU6050_IsDataReady(MPU6050_Sensor* pSensor);

void MPU6050_SetSampleDiv(MPU6050_Sensor* pSensor, uint8_t ucDiv);
void MPU6050_SetDlpfConfig(MPU6050_Sensor* pSensor, uint8_t ucDlpfCfg);
void MPU6050_SetGyroConfig(MPU6050_Sensor* pSensor, MPU6050_GyroRange eRange);
void MPU6050_SetAccelConfig(MPU6050_Sensor* pSensor, MPU6050_AccelRange eRange);
void MPU6050_SetInt(MPU6050_Sensor* pSensor, MPU6050_Flag eFlag);
void MPU6050_SetMode(MPU6050_Sensor* pSensor, MPU6050_Mode eMode, MPU6050_ClockSource eSrc, bool bDisableTS, MPU6050_StbyMode eGyroMode, MPU6050_StbyMode eAccelMode);
void MPU6050_SetSleepMode(MPU6050_Sensor* pSensor, bool bEnable);

void MPU6050_ResetSignalPath(MPU6050_Sensor* pSensor, bool bGyro, bool bAccel, bool bTemp);
void MPU6050_ResetSignalPathAll(MPU6050_Sensor* pSensor);
void MPU6050_SoftReset(MPU6050_Sensor* pSensor);
void MPU6050_GetSensorData(MPU6050_Sensor* pSensor);

int16_t MPU6050_CalcAngle(MPU6050_Sensor* pSensor, bool bXY);
int16_t MPU6050_CalcTemp(MPU6050_Sensor* pSensor);

#endif /* SENSORLIB_MPU6050_INTERNAL_H_ */