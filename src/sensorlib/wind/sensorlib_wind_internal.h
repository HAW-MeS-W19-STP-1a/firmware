/*!****************************************************************************
 * @file
 * sensorlib_wind_internal.h
 *
 * Interne Funktionen zur Auswertung des Windsensors
 *
 * @date  31.10.2019
 ******************************************************************************/

#ifndef SENSORLIB_WIND_INTERNAL_H_
#define SENSORLIB_WIND_INTERNAL_H_

/*- Headerdateien ------------------------------------------------------------*/
#include "sensorlib_wind.h"


/*- Funktionsprototypen ------------------------------------------------------*/
void Wind_GetPulseCount(Wind_Sensor* pSensor);
void Wind_GetAnalogVal(Wind_Sensor* pSensor);

uint16_t Wind_CalcVelocity(Wind_Sensor* pSensor);
Wind_Direction Wind_CalcDirection(Wind_Sensor* pSensor);

#endif /* SENSORLIB_WIND_INTERNAL_H_ */