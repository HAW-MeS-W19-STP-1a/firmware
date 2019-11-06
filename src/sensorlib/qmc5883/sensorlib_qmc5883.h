/*!****************************************************************************
 * @file
 * sensorlib_qmc5883.h
 *
 * Ansteuerung und Auswertung für das QMC5883 Magnetometer
 * 
 * @date  31.10.2019
 ******************************************************************************/

#ifndef SENSORLIB_QMC5883_H_
#define SENSORLIB_QMC5883_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>


/*- Symbolische Konstanten ---------------------------------------------------*/
/*! Standardadresse für QMC5883                                               */
#define QMC5883_DEFAULT_ADDR 0x0D


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Sensordaten- und Konfigurationsstruktur für QMC5883
 *
 * @date  31.10.2019
 ******************************************************************************/
typedef struct tag_QMC5883_Sensor {
  /*! I2C Slaveadresse                                    */
  uint8_t ucSlaveAddr;
  
  /*! Kalibrierungsdaten                                  */
  struct {
    /*! Referenztemperatur in 0.1°C                       */
    int16_t iRefTemp;
  } sCalib;
  
  /*! Rohdaten                                            */
  struct {
    /*! Feld in X-Richtung                                */
    int16_t iRawX;
    
    /*! Feld in Y-Richtung                                */
    int16_t iRawY;
    
    /*! Feld in Z-Richtung                                */
    int16_t iRawZ;
    
    /*! Temperatur-Rohwert                                */
    int16_t iRawTemp;
  } sRaw;
  
  /*! Umgerechnete Messdaten                              */
  struct {
    /*! Azimuth in 0.1°                                   */
    uint16_t uiAzimuth;
    
    /*! Temperatur in 0.01°C                              */
    int16_t iTemperature;
  } sMeasure;
} QMC5883_Sensor;


/*- Funktionsprototypen ------------------------------------------------------*/
void QMC5883_Init(QMC5883_Sensor* pSensor, uint8_t ucSlaveAddr);
void QMC5883_Update(QMC5883_Sensor* pSensor);
void QMC5883_SetRefTemp(QMC5883_Sensor* pSensor, int16_t iRefTemp);

#endif /* SENSORLIB_QMC5883_H_ */