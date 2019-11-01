/*!****************************************************************************
 * @file
 * sensorlib_qmc5883.h
 *
 * Ansteuerung und Auswertung für den QMC5883 Beschleunigungssensor
 * 
 * @date  31.10.2019
 ******************************************************************************/

#ifndef SENSORLIB_QMC5883_H_
#define SENSORLIB_QMC5883_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>


/*- Symbolische Konstanten ---------------------------------------------------*/
#define QMC5883_DEFAULT_ADDR 0x0D


/*- Typdefinitionen ----------------------------------------------------------*/
typedef struct tag_QMC5883_Sensor {
  uint8_t ucSlaveAddr;
  
  struct {
    int16_t iRefTemp;
  } sCalib;
  
  struct {
    int16_t iRawX;
    int16_t iRawY;
    int16_t iRawZ;
    int16_t iRawTemp;
  } sRaw;
  
  struct {
    int16_t iPlaneAngle;
    int16_t iTemperature;
  } sMeasure;
} QMC5883_Sensor;


/*- Funktionsprototypen ------------------------------------------------------*/
void QMC5883_Init(QMC5883_Sensor* pSensor, uint8_t ucSlaveAddr);
void QMC5883_Update(QMC5883_Sensor* pSensor);
void QMC5883_SetRefTemp(QMC5883_Sensor* pSensor, int16_t iRefTemp);

#endif /* SENSORLIB_QMC5883_H_ */