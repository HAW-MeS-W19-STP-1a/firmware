/*!****************************************************************************
 * @file
 * motorlib.c
 *
 * Ansteuerung des Motortreibers und Einlesen der Endlagenschalter
 *
 * @date  16.11.2019
 ******************************************************************************/
 
/*- Headerdateien ------------------------------------------------------------*/
#include "stm8l15x.h"
#include "BlinkSequencer.h"
#include "motorlib.h"


/*- Symbolische Konstanten ---------------------------------------------------*/
/*! Bitmaske für Endlagenschalter A-Richtung                                  */
#define MOT_LIM_A 0x08

/*! Bitmaske für Endlagenschalter B-Richtung                                  */
#define MOT_LIM_B 0x10


/*- Typdefinitionen ----------------------------------------------------------*/
static int16_t abs(int16_t i)
{
  return (i < 0) ? -i : i;
}

/*!****************************************************************************
 * @brief
 * Optionen für die Drehrichtungsvorgabe
 *
 * 17.11.2019
 ******************************************************************************/
typedef enum {
  /*! Bewegung anhalten                                   */
  Motor_Direction_Stop = 0,
  
  /*! Drehung in B-Richtung                               */
  Motor_Direction_B = 0x01,
  
  /*! Drehung in A-Richtung                               */
  Motor_Direction_A = 0x10
} Motor_Direction;


/*- Modulglobale Variablen ---------------------------------------------------*/
/*! Aktuelle Position des Turms in 0.1°                                       */
volatile int16_t iTurnAct;

/*! Sollposition des Turms in 0.1°                                            */
volatile int16_t iTurnSet;

/*! Aktuelle Position des Panels in 0.1°                                      */
volatile int16_t iTiltAct;

/*! Sollposition des Panels in 0.1°                                           */
volatile int16_t iTiltSet;

/*! Freigabesignal                                                            */
volatile bool bMotorEnable;

/*! Nothalt-Signal                                                            */
volatile bool bMotorStop;

/*! Referenzfahrt aktiv                                                       */
volatile bool bHomingActive;

/*! Endlagenschalter erkannt                                                  */
volatile uint8_t ucLimitHit;


/*- Lokale Funktionen --------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Panelansteuerung Bewegungsrichtung festlegen
 *
 * @param[in] eDir  Drehrichtung
 *
 * @date 17.11.2019
*******************************************************************************/ 
static void Motor_CmdTilt(Motor_Direction eDir)
{
  /* Freigabe prüfen                                      */
  if (bMotorEnable && !bMotorStop)
  {
    /* Soll-Drehrichtung auswerten                        */
    switch (eDir)
    {
      case Motor_Direction_A:
        /* Endlagenschalter für Drehrichtung A auswerten  */
        if (ucLimitHit & MOT_LIM_A)
        {
          GPIO_WriteBit(GPIOF, GPIO_Pin_7, true);
          GPIO_WriteBit(GPIOF, GPIO_Pin_6, false);
        }
        break;
        
      case Motor_Direction_B:
        /* Endlagenschalter für Drehrichtung B auswerten  */
        if (ucLimitHit & MOT_LIM_B)
        {
          GPIO_WriteBit(GPIOF, GPIO_Pin_7, false);
          GPIO_WriteBit(GPIOF, GPIO_Pin_6, true);
        }
        break;
        
      case Motor_Direction_Stop:
      default:
        /* Bewegung stoppen                               */
        GPIO_WriteBit(GPIOF, GPIO_Pin_7, false);
        GPIO_WriteBit(GPIOF, GPIO_Pin_6, false);
    }
  }
}

/*!****************************************************************************
 * @brief
 * Turmansteuerung Bewegungsrichtung festlege
 *
 * @param[in] eDir  Drehrichtung
 *
 * @date  17.11.2019
 ******************************************************************************/
static void Motor_CmdTurn(Motor_Direction eDir)
{
  /* Freigabe prüfen                                      */
  if (bMotorEnable && !bMotorStop)
  {
    /* Soll-Drehrichtung auswerten                        */
    switch (eDir)
    {
      case Motor_Direction_A:
        /* Drehrichtung A starten                         */
        GPIO_WriteBit(GPIOF, GPIO_Pin_5, true);
        GPIO_WriteBit(GPIOF, GPIO_Pin_4, false);
        break;
        
      case Motor_Direction_B:
        /* Drehrichtung B starten                         */
        GPIO_WriteBit(GPIOF, GPIO_Pin_5, false);
        GPIO_WriteBit(GPIOF, GPIO_Pin_4, true);
        break;
      
      case Motor_Direction_Stop:
      default:
        /* Bewegung stoppen                               */
        GPIO_WriteBit(GPIOF, GPIO_Pin_5, false);
        GPIO_WriteBit(GPIOF, GPIO_Pin_4, false);
        break;
    }
  }
}

/*!****************************************************************************
 * @brief
 * Referenzfahrt der Panelansteuerung
 *
 * Regelungsroutinen werden im 100ms-Task innerhalb des Timer-Interrupts aus-
 * geführt
 *
 * @date  17.11.2019
 ******************************************************************************/
static void Motor_HomingTask(void)
{
  /* Freigabe prüfen                                      */
  if (bMotorEnable && !bMotorStop)
  {
    /* Aktuellen Zustand der Endlagenschalter auswerten   *
     * und mit eventuellem Ergebnis aus ISR verodern      */
    uint8_t ucLimitState = (~GPIO_ReadInputData(GPIOB) & 0x18) | ucLimitHit;
    
    /* Endlagenschalterzustand auswerten                  */
    if (ucLimitState)
    {
      /* Referenzfahrt beendet                            */
      bHomingActive = false;
      Motor_CmdTilt(Motor_Direction_Stop);
      
      if (!(ucLimitState & MOT_LIM_B))
      {
        /* Endlagenschalter am B-Ende ausgelöst           */
        iTiltAct = 0;
      }
      else
      {
        /* Endlagenschalter am A-Ende ausgelöst           */
        iTiltAct = 900;
      }
    }
    else
    {
      /* Noch kein Endlagenschalter getroffen             *
       * Drehrichtung A starten                           */
      Motor_CmdTilt(Motor_Direction_B);
    }
  }
  else
  {
    /* Freigabe zurückgenommen - Bewegung anhalten        */
    Motor_CmdTilt(Motor_Direction_Stop);
  }
}

/*!****************************************************************************
 * @brief
 * Dreipunktregler für Panel- und Turmausrichtung
 *
 * Regelungsroutinen werden im 100ms-Task innerhalb des Timer-Interrupts aus-
 * geführt
 *
 * @date  19.11.2019
 ******************************************************************************/
static void Motor_ControlTask(void)
{
  register int16_t iDelta;
  
  /* Freigabesignal auswerten                             */ 
  if (bMotorEnable && !bMotorStop)
  {
    /* Dreipunktregler für Panelausrichtung               */
    iDelta = iTiltSet - iTiltAct;
    if (iDelta > MOTORLIB_MIN_ANGLE)
    {
      /* Nach oben drehen                                 */
      Motor_CmdTilt(Motor_Direction_A);
      iTiltAct += MOTORLIB_TILT_SPD;
    }
    else if (iDelta < -MOTORLIB_MIN_ANGLE)
    {
      /* Nach unten drehen                                */
      Motor_CmdTilt(Motor_Direction_B);
      iTiltAct -= MOTORLIB_TILT_SPD;
    }
    else
    {
      /* Winkeldifferenz zu klein                         */
      Motor_CmdTilt(Motor_Direction_Stop);
    }
    
    /* Dreipunktregler für Turmausrichtung                *
     * Bestimmung der optimalen Drehrichtung für den      *
     * kleinsten Drehwinkel                               */
    iDelta = iTurnSet - iTurnAct;
    if (iDelta > MOTORLIB_MIN_ANGLE)
    {
      /* Drehrichtung A (rechts)                          */
      Motor_CmdTurn(Motor_Direction_A);
      iTurnAct += MOTORLIB_TURN_SPD;
    }
    else if (iDelta < -MOTORLIB_MIN_ANGLE)
    {
      /* Drehrichtung B (links)                           */
      Motor_CmdTurn(Motor_Direction_B);
      iTurnAct -= MOTORLIB_TURN_SPD;
    }
    else
    {
      /* Winkeldifferenz zu klein                         */
      Motor_CmdTurn(Motor_Direction_Stop);
    }
  }
  else
  {
    /* Freigabe zurückgenommen - Bewegung anhalten        */
    Motor_CmdTilt(Motor_Direction_Stop);
  }
}


/*!****************************************************************************
 * @brief
 * Motor-Modul initialisieren
 *
 * @date  17.11.2019
 ******************************************************************************/
void Motor_Init(void)
{
  bMotorEnable = false;
  bMotorStop = false;
  bHomingActive = true;
  
  ucLimitHit = 0;
  
  iTurnAct = 0;
  iTurnSet = 0;
  iTiltAct = 0;
  iTiltSet = 0;
}

/*!****************************************************************************
 * @brief
 * Bewegung freigeben
 *
 * @date  17.11.2019
 ******************************************************************************/
void Motor_Cmd(bool bEnable)
{  
  bMotorEnable = bEnable;
  if (bEnable)
  {
    Blink_SetPattern(Blink_Led_MOT, 0x0001);
  }
  else
  {
    Blink_SetPattern(Blink_Led_MOT, 0x0000);
  }
}

/*!****************************************************************************
 * @brief
 * Sollwertvorgabe Turmausrichtung
 *
 * @param[in] iSetpoint   Winkel in 0.1°
 *
 * @date  17.11.2019
 ******************************************************************************/
void Motor_SetTurn(int16_t iSetpoint)
{
  iTurnSet = iSetpoint;
}

/*!****************************************************************************
 * @brief
 * Istwert-Driftkompensation Turmausrichtung
 *
 * @param[in] iActval     Winkel in 0.1°
 *
 * @date  17.11.2019
 ******************************************************************************/
void Motor_SetTurnRef(int16_t iActval)
{
  iTurnAct = iActval;
}

/*!****************************************************************************
 * @brief
 * Sollwertvorgabe Panelausrichtung
 *
 * @param[in] iSetpoint   Winkel in 0.1°
 *
 * @date  17.11.2019
 ******************************************************************************/
void Motor_SetTilt(int16_t iSetpoint)
{
  iTiltSet = iSetpoint;
}

/*!****************************************************************************
 * @brief
 * Istwert-Driftkompensation Panelausrichtung
 *
 * @param[in] iActval   Winkel in 0.1°
 *
 * @date  17.11.2019
 ******************************************************************************/
void Motor_SetTiltRef(int16_t iActval)
{
  iTiltAct = iActval;
  bHomingActive = false;
}

/*!****************************************************************************
 * @brief
 * 100ms-Task für die Lageregelung
 *
 * Aufgerufen im 100ms-Timer
 *
 * @date  19.11.2019
 ******************************************************************************/
void Motor_Task100ms(void)
{
  if (bHomingActive)
  {
    /* Referenzfahrt aktiv                                */
    Motor_HomingTask();
  }
  else
  {
    /* Lageregelung                                       */
    Motor_ControlTask();
  }
}

/*!****************************************************************************
 * @brief
 * Interrupthandler für Endlagenschalter
 *
 * Behandelt die Auslösung von Endlagenschaltern oder dem Nothalt
 *
 * @date  17.11.2019
 ******************************************************************************/
@far @interrupt void Motor_LimitInterruptHandler(void)
{
  register uint8_t ucInBuf = ~GPIO_ReadInputData(GPIOB);
  
  EXTI_ClearITPendingBit(EXTI_IT_PortB);
  
  if (ucInBuf & MOT_LIM_A)
  {
    /* Limit Switch A ausgelöst                           */
    GPIO_WriteBit(GPIOF, GPIO_Pin_7, false);
  }

  if (ucInBuf & MOT_LIM_B)
  {
    /* Limit Switch B ausgelöst                           */
    GPIO_WriteBit(GPIOF, GPIO_Pin_6, false);
  }
  
  if (ucInBuf & 0x01)
  {
    /* Nothalt ausgelöst                                  */
    GPIO_ResetBits(GPIOF, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
    GPIO_ResetBits(GPIOD, GPIO_Pin_4);
    bMotorEnable = false;
    bMotorStop = true;
    Blink_SetPattern(Blink_Led_MOT, 0x5555);
  }
  
  /* Endlagenschalterposition speichern                   */
  ucLimitHit = ucInBuf & 0x18;
}

int16_t Motor_GetTurn(void)
{
  return iTurnAct;
}

int16_t Motor_GetTilt(void)
{
  return iTiltAct;
}

bool Motor_IsTurnReached(void)
{
  int16_t iDelta = iTurnSet - iTurnAct;
  return (abs(iDelta) <= MOTORLIB_MIN_ANGLE);
}

bool Motor_IsTiltReached(void)
{
  int16_t iDelta = iTiltSet - iTiltAct;
  return (abs(iDelta) <= MOTORLIB_MIN_ANGLE);
}