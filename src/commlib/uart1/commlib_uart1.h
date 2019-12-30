/*!****************************************************************************
 * @file
 * commlib_uart1.h
 *
 * Handler für serielle Kommunikation über die USART1-Schnittstelle.
 *
 * @date  26.10.2019
 ******************************************************************************/

#ifndef COMMLIB_UART1_H_
#define COMMLIB_UART1_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>


/*- Symbolische Konstanten ---------------------------------------------------*/
/*! Pufferlänge                                                               */
#define COMMLIB_UART1_MAX_BUF  92


/*- Globale Variablen --------------------------------------------------------*/
/*! Sendepuffer                                                               */
extern volatile uint8_t aucUart1TxBuf[COMMLIB_UART1_MAX_BUF];

/*! Empfangspuffer                                                            */
extern volatile uint8_t aucUart1RxBuf[COMMLIB_UART1_MAX_BUF];


/*- Funktionsdeklarationen ---------------------------------------------------*/
void UART1_Init(void);
void UART1_DeInit(void);

void UART1_Send(uint8_t ucLength);
void UART1_SendUntil(char cEndMarker, uint8_t ucMaxLength);
void UART1_Receive(uint8_t ucLength);
void UART1_ReceiveUntil(char cEndMarker, uint8_t ucMaxLength);
uint8_t UART1_GetRxCount(void);

bool UART1_IsRxReady(void);
bool UART1_IsTxReady(void);

void UART1_FlushRx(void);
void UART1_FlushTx(void);

void UART1_SetEchoMode(bool bEnable);

#endif /* COMMLIB_UART1_H_ */