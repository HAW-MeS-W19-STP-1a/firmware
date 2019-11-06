/*!****************************************************************************
 * @file
 * sensorlib_mpu6050.h
 *
 * Ansteuerung und Auswertung für das MPU6050 Gyro/Accelerometer
 *
 * @date 06.11.2019
 ******************************************************************************/
 
#ifndef SENSORLIB_MPU6050_H_
#define SENSORLIB_MPU6050_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>


/*- Symbolische Konstanten ---------------------------------------------------*/
/*! Standardadresse für MPU6050 ohne AD0                                      */
#define MPU6050_DEFAULT_ADDR  0x68


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Konfigurations- und Sensordatenstruktur für MPU6050
 *
 * @date  06.11.2019
 ******************************************************************************/
typedef struct tag_MPU6050_Sensor {
  /*! I2C Slaveadresse                                    */
  uint8_t ucSlaveAddr;
  
  /*! Temperaturmessungen aktiv                           */
  bool bMeasureTemp;
  
  /*! Rohdaten                                            */
  struct {
    /*! Rohwert für X-Beschleunigung                      */
    int16_t iRawX;
    
    /*! Rohwert für Y-Beschleunigung                      */
    int16_t iRawY;
    
    /*! Rohwert für Z-Beschleunigung                      */
    int16_t iRawZ;
    
    /*! Rohwert für Temperatur                            */
    int16_t iRawTemp;
  } sRaw;
  
  /*! Umgerechnete Messwerte                              */
  struct {
    /*! Einstellwinkel in 0.1°                            */
    struct {
      int iXZ;
      int iYZ;
    } sAngle;
    
    /*! Temperaturmesswert in 0.01°C                      */
    int16_t iTemperature;
  } sMeasure;
} MPU6050_Sensor;


/*- Funktionsprototypen ------------------------------------------------------*/
void MPU6050_Init(MPU6050_Sensor* pSensor, uint8_t ucSlaveAddr, bool bMeasureTemp);
void MPU6050_Update(MPU6050_Sensor* pSensor);

#endif /* SENSORLIB_MPU6050_H_ */