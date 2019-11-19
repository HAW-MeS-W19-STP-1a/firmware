#ifndef USERLIB_BLINKSEQUENCER_H_
#define USERLIB_BLINKSEQUENCER_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>


/*- Typdefinitionen ----------------------------------------------------------*/
typedef enum tag_Blink_Led
{
  Blink_Led_SYS = 0,
  Blink_Led_MOT,
  Blink_Led_BT,
  Blink_MaxLed
} Blink_Led;


/*- Funktionsprototypen ------------------------------------------------------*/
void Blink_Init(void);
void Blink_SetPattern(Blink_Led eLed, uint16_t uiPattern);
bool Blink_Ready(Blink_Led eLed);
void Blink_Poll(void);

#endif /* USERLIB_BLINKSEQUENCER_H_ */