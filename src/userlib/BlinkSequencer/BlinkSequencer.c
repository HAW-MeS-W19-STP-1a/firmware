#include "stm8l15x.h"
#include "io_map.h"
#include "BlinkSequencer.h"

/*- Typdefinitionen ----------------------------------------------------------*/
typedef struct tag_Blink_LedGpio
{
  GPIO_TypeDef* pGpio;
  uint8_t ucPin;
} Blink_LedGpio;


/*- Modulglobale Variablen ---------------------------------------------------*/
volatile Blink_LedGpio asBlinkGPIO[Blink_MaxLed] = 
{
  {LED_PORT, LED_SYS_PIN},  /* SYS LED */
  {LED_PORT, LED_MOT_PIN},  /* MOT LED */
  {LED_PORT, LED_BT_PIN}    /* BT LED */
};

volatile uint16_t uiBlinkPattern[Blink_MaxLed];
volatile uint16_t uiNextBlinkPattern[Blink_MaxLed];
volatile uint8_t ucPatternIndex;


void Blink_Init(void)
{
  uint8_t ucIndex;
  
  for (ucIndex = 0; ucIndex < Blink_MaxLed; ++ucIndex)
  {
    GPIO_Init(asBlinkGPIO[ucIndex].pGpio, asBlinkGPIO[ucIndex].ucPin, GPIO_Mode_Out_PP_Low_Slow);
    uiBlinkPattern[ucIndex] = 0;
    uiNextBlinkPattern[ucIndex] = 0;
  }

  ucPatternIndex = 0;
}

void Blink_SetPattern(Blink_Led eLed, uint16_t uiPattern)
{
  uiNextBlinkPattern[eLed] = uiPattern;
}

bool Blink_Ready(Blink_Led eLed)
{
  return (uiBlinkPattern[eLed] == uiNextBlinkPattern[eLed]);
}

void Blink_Poll(void)
{
  uint8_t ucIndex;
  for (ucIndex = 0; ucIndex < Blink_MaxLed; ++ucIndex)
  {
    BitAction bBitState = (uiBlinkPattern[ucIndex] & (1 << ucPatternIndex)) == (1 << ucPatternIndex);
    GPIO_WriteBit(asBlinkGPIO[ucIndex].pGpio, asBlinkGPIO[ucIndex].ucPin, bBitState);
  }
  
  ++ucPatternIndex;
  if (ucPatternIndex > 15)
  {
    ucPatternIndex = 0;
    for (ucIndex = 0; ucIndex < Blink_MaxLed; ++ucIndex)
    {
      uiBlinkPattern[ucIndex] = uiNextBlinkPattern[ucIndex];
    }
  }
}