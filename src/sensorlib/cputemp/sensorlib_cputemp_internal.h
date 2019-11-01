/*!****************************************************************************
 * @file
 * sensorlib_cputemp_internal.h
 *
 * Interne Funktionen zur Auswertung des On-Die Temperatursensors
 *
 * @date  31.10.2019
 ******************************************************************************/

#ifndef SENSORLIB_CPUTEMP_INTERNAL_H_
#define SENSORLIB_CPUTEMP_INTERNAL_H_

/*- Headerdateien ------------------------------------------------------------*/
#include "sensorlib_cputemp.h"


/*- Funktionsprototypen ------------------------------------------------------*/
void CPUTemp_GetSensorData(CPUTemp_Sensor* pSensor);
int8_t CPUTemp_CalcTemperature(CPUTemp_Sensor* pSensor);

#endif /* SENSORLIB_CPUTEMP_INTERNAL_H_ */