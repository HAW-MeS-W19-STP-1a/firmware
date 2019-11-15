#ifndef APP_SENSORS_H_
#define APP_SENSORS_H_

/*- Headerdateien ------------------------------------------------------------*/
#include "sensorlib.h"


/*- Globale Variablen --------------------------------------------------------*/
extern BME280_Sensor sSensorBME280;
extern Wind_Sensor sSensorWind;
extern CPUTemp_Sensor sSensorCPUTemp;
extern QMC5883_Sensor sSensorQMC5883;
extern MPU6050_Sensor sSensorMPU6050;

#endif /* APP_SENSORS_H_ */