/*!****************************************************************************
 * @file
 * sensorlib_qmc5883.h
 *
 * Ansteuerung und Auswertung f�r das QMC5883 Magnetometer
 * 
 * @date  31.10.2019
 ******************************************************************************/

#ifndef SENSORLIB_QMC5883_H_
#define SENSORLIB_QMC5883_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>


/*- Symbolische Konstanten ---------------------------------------------------*/
/*! Standardadresse f�r QMC5883                                               */
#define QMC5883_DEFAULT_ADDR 0x0D


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Sensordaten- und Konfigurationsstruktur f�r QMC5883
 *
 * @date  31.10.2019
 ******************************************************************************/
typedef struct tag_QMC5883_Sensor {
  /*! I2C Slaveadresse                                    */
  uint8_t ucSlaveAddr;
  
  /*! Kalibrierungsdaten                                  */
  struct {
    /*! Referenztemperatur in 0.1�C                       */
    int16_t iRefTemp;
    
    /*! X-Kompensation                                    */
    float fXComp;
    
    /*! Y-Kompensation                                    */
    float fYComp;
    
    int16_t iXMin;
    int16_t iXMax;
    int16_t iYMin;
    int16_t iYMax;
    
    float fXGain;
    float fYGain;
    
    /*! Anzahl der Messpunkte f�r die Kompensation        */
    uint16_t uiNumComp;
  } sCalib;
  
  /*! Kalibrierung aktiv                                  */
  bool bCalActive;
  
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
    /*! Azimuth in 0.1�                                   */
    uint16_t uiAzimuth;
    
    /*! Temperatur in 0.01�C                              */
    int16_t iTemperature;
  } sMeasure;
} QMC5883_Sensor;


/*- Funktionsprototypen ------------------------------------------------------*/
void QMC5883_Init(QMC5883_Sensor* pSensor, uint8_t ucSlaveAddr);
void QMC5883_Update(QMC5883_Sensor* pSensor);
void QMC5883_SetRefTemp(QMC5883_Sensor* pSensor, int16_t iRefTemp);

void QMC5883_StartCal(QMC5883_Sensor* pSensor);

#endif /* SENSORLIB_QMC5883_H_ */