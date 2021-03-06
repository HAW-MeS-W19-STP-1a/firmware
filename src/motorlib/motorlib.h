/*!****************************************************************************
 * @file
 * motorlib.h
 *
 * Ansteuerung des Motortreibers und Einlesen der Endlagenschalter
 *
 * @date  16.11.2019
 ******************************************************************************/
 
#ifndef MOTORLIB_H_
#define MOTORLIB_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>


/*- Symbolische Konstanten ---------------------------------------------------*/
/*! Turm-Drehgeschwindigkeit in 0.1�/100ms                                    */
#define MOTORLIB_TURN_SPD     5

/*! Panel-Drehgeschwindigket in 0.1�/100ms                                    */
#define MOTORLIB_TILT_SPD     30

/*! Mindest-Drehwinkel in 0.1�                                                */
#define MOTORLIB_MIN_ANGLE    50


/*- Funktionsprototypen ------------------------------------------------------*/
void Motor_Init(void);
void Motor_Task100ms(void);
void Motor_Cmd(bool bEnable);

void Motor_SetTurn(int16_t iSetpoint);
void Motor_SetTurnRef(int16_t iActval);
void Motor_SetTilt(int16_t iSetpoint);
void Motor_SetTiltRef(int16_t iActval);

int16_t Motor_GetTurn(void);
int16_t Motor_GetTilt(void);

bool Motor_IsTurnReached(void);
bool Motor_IsTiltReached(void);

bool Motor_IsHomingActive(void);

#endif /* MOTORLIB_H_ */