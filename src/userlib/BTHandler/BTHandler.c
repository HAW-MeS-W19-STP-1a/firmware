#include <stdio.h>
#include "stm8l15x.h"
#include "commlib.h"
#include "io_map.h"
#include "BlinkSequencer.h"


/*- Symbolische Konstanten ---------------------------------------------------*/
#define USERLIB_BTHANDLER_ADTIME  30
#define USERLIB_BTHANDLER_LED_OFF 0x0000
#define USERLIB_BTHANDLER_LED_ADV 0x0005
#define USERLIB_BTHANDLER_LED_ON  0xFFFF


/*- Typdefinitionen ----------------------------------------------------------*/
typedef enum tag_BTHandler_State
{
  BTHandler_State_OFF,
  BTHandler_State_ADVERTISE,
  BTHandler_State_CONNECTED
} BTHandler_State;


/*- Modulglobale Variablen ---------------------------------------------------*/
BTHandler_State eState;
uint8_t ucAdTimer;


void BTHandler_Init(void)
{
  GPIO_Init(USART2_BT_PORT, USART3_BT_TX_PIN, GPIO_Mode_In_FL_No_IT);      // Modul TXD an PC2
  GPIO_Init(USART2_BT_PORT, USART2_BT_RX_PIN, GPIO_Mode_Out_PP_High_Fast);  // Modul RXD an PC3
  GPIO_Init(BT_CMD_PORT, BT_CMD_PIN, GPIO_Mode_Out_PP_Low_Slow); // Modul "EN" (CMD) an PC4
  GPIO_Init(BT_STAT_PORT, BT_STAT_PIN, GPIO_Mode_In_FL_No_IT); // Modul "Status" an PD1
  GPIO_Init(BT_PWREN_PORT, BT_PWREN_PIN, GPIO_Mode_Out_PP_High_Slow); // Modul "PWREN" an PD3
  UART1_Init();
  
  printf("BTHandler ON\r\n");
  Blink_SetPattern(Blink_Led_BT, USERLIB_BTHANDLER_LED_ADV);
  ucAdTimer = USERLIB_BTHANDLER_ADTIME;
  eState = BTHandler_State_ADVERTISE;
}

void BTHandler_Poll(void)
{
  if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1))
  {
    /* Verbindungsstatus pr�fen                           */
    if (eState == BTHandler_State_ADVERTISE)
    {
      /* Verbindung auf Aktiv setzen                      */
      printf("BTHandler CONN\r\n");
      Blink_SetPattern(Blink_Led_BT, USERLIB_BTHANDLER_LED_ON);
      eState = BTHandler_State_CONNECTED;
      UART1_FlushRx();
      UART1_ReceiveUntil('\r', COMMLIB_UART1_MAX_BUF);
    }
  }
  else
  {
    if (eState == BTHandler_State_CONNECTED)
    {
      /* Verbindung abgebrochen                           */
      printf("BTHandler DISC\r\n");
      Blink_SetPattern(Blink_Led_BT, USERLIB_BTHANDLER_LED_ADV);
      ucAdTimer = USERLIB_BTHANDLER_ADTIME;
      eState = BTHandler_State_ADVERTISE;
    }
  }
}

void BTHandler_Task1s(void)
{
  if (eState == BTHandler_State_ADVERTISE)
  {
    if (ucAdTimer > 0)
    {
      /* Advertising aktiv                                */
      --ucAdTimer;
    }
    else
    {
      /* Advertising Timer abgelaufen ohne Verbindung     */
      printf("BTHandler OFF\r\n");
      GPIO_WriteBit(BT_PWREN_PORT, BT_PWREN_PIN, DISABLE);
      eState = BTHandler_State_OFF;
      Blink_SetPattern(Blink_Led_BT, USERLIB_BTHANDLER_LED_OFF);
    }
  }
}

void BTHandler_TakeWakeup(void)
{
  if (eState == BTHandler_State_OFF)
  {
    /* Advertising f�r einige Sekunden einschalten        */
    printf("BTHandler ON\r\n");
    GPIO_WriteBit(BT_PWREN_PORT, BT_PWREN_PIN, ENABLE);
    ucAdTimer = USERLIB_BTHANDLER_ADTIME;
    eState = BTHandler_State_ADVERTISE;   
    Blink_SetPattern(Blink_Led_BT, USERLIB_BTHANDLER_LED_ADV);
  }
}

bool BTHandler_IsActive(void)
{
  return (eState == BTHandler_State_CONNECTED);
}