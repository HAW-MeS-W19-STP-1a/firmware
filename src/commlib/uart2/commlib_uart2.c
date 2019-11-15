/*!****************************************************************************
 * @file
 * commlib_uart2.h
 *
 * Umleitung der stdio-Bibliotheksfunktionen auf die USART2-Schnittstelle
 *
 * @date  26.10.2019
 ******************************************************************************/

/*- Headerdateien ------------------------------------------------------------*/
#include <stdbool.h>
#include "stm8l15x.h"
#include "commlib_uart2.h"


/*!****************************************************************************
 * @brief
 * Modul initialisieren 
 *
 * @note  Nur Senderichtung wird unterstützt
 *
 * @date  26.10.2019
 ******************************************************************************/
void UART2_Init(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_USART2, ENABLE);
  USART_Init(USART2, 1000000, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, USART_Mode_Tx);
  USART_Cmd(USART2, ENABLE);
}

/*!****************************************************************************
 * @brief
 * Modul deaktivieren 
 *
 * @date  26.10.2019
 ******************************************************************************/
void UART2_DeInit(void)
{
  USART_Cmd(USART2, DISABLE);
  USART_DeInit(USART2);
  CLK_PeripheralClockConfig(CLK_Peripheral_USART2, DISABLE);
}

/*!****************************************************************************
 * @brief
 * STDIN-Umleitung
 *
 * @note  Nicht unterstützt
 *
 * @return  char    Immer '\0'.
 *
 * @date  26.10.2019
 ******************************************************************************/
char getchar(void)
{
  return '\0';
}

/*!****************************************************************************
 * @brief
 * STDOUT-Umleitung
 *
 * @param[in] c   Auszugebendes Zeichen
 * @return  char  = c
 *
 * @date  26.10.2019
 ******************************************************************************/
char putchar(char c)
{
  while (!(USART_GetFlagStatus(USART2, USART_FLAG_TXE)));
  USART_SendData8(USART2, c);
  return (c);
}