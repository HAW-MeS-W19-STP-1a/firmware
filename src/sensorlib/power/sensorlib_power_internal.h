#ifndef SENSORLIB_POWER_INTERNAL_H_
#define SENSORLIB_POWER_INTERNAL_H_

/*- Headerdateien ------------------------------------------------------------*/
#include "sensorlib_power.h"


/*- Funktionsprototypen ------------------------------------------------------*/
void Power_GetAnalogVal(Power_Sensor* pSensor);

int16_t Power_CalcCurr(Power_Sensor* pSensor);
uint16_t Power_CalcVolt(Power_Sensor* pSensor);

#endif