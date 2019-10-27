/*- Headerdateien ------------------------------------------------------------*/
#include <stdbool.h>
#include "stm8l15x.h"
#include "commlib_uart2.h"


/*- Modulglobale Variablen ---------------------------------------------------*/
static bool bUart2IsInitialised = false;


void UART2_Init(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_USART2, ENABLE);
  USART_Init(USART2, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, USART_Mode_Tx);
  USART_Cmd(USART2, ENABLE);
}

void UART2_DeInit(void)
{
  USART_Cmd(USART2, DISABLE);
  USART_DeInit(USART2);
  CLK_PeripheralClockConfig(CLK_Peripheral_USART2, DISABLE);
}

char getchar(void)
{
  return '\0';
}

char putchar(char c)
{
  while (!(USART_GetFlagStatus(USART2, USART_FLAG_TXE)));
  USART_SendData8(USART2, c);
  return (c);
}