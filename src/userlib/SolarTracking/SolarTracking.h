#ifndef SOLARTRACKING_H_
#define SOLARTRACKING_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>


/*- Funktionsprototypen ------------------------------------------------------*/
void Tracking_Init(void);
void Tracking_Task1s(void);
void Tracking_TaskWakeup(void);

void Tracking_Cmd(bool bEnable);
bool Tracking_IsEnabled(void);
void Tracking_SetLimits(int iMinAzm, int iMaxAzm, int iMinZen, int iMaxZen);

#endif /* SOLARTRACKING_H_ */