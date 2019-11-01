/*!****************************************************************************
 * @file
 * commlib_uart3.h
 *
 * Handler für serielle Kommunikation über die USART3-Schnittstelle.
 *
 * @date  26.10.2019
 ******************************************************************************/
 
#ifndef COMMLIB_UART3_H_
#define COMMLIB_UART3_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>


/*- Symbolische Konstanten ---------------------------------------------------*/
/*! Sendepufferlänge                                                          */
#define COMMLIB_UART3TX_MAX_BUF   32

/*! Empfangspufferlänge für NMEA 0183 ('$' + 79 chars + "\r\n")               */
#define COMMLIB_UART3RX_MAX_BUF   82


/*- Globale Variablen --------------------------------------------------------*/
/*! Sendepuffer                                                               */
extern volatile uint8_t aucUart3TxBuf[COMMLIB_UART3TX_MAX_BUF];

/*! Empfangspuffer                                                            */
extern volatile uint8_t aucUart3RxBuf[COMMLIB_UART3RX_MAX_BUF];


/*- Funktionsdeklarationen ---------------------------------------------------*/
void UART3_Init(void);
void UART3_DeInit(void);

void UART3_Send(uint8_t ucLength);
void UART3_SendUntil(char cEndMarker, uint8_t ucMaxLength);
void UART3_Receive(uint8_t ucLength);
void UART3_ReceiveUntil(char cEndMarker, uint8_t ucMaxLength);
void UART3_ReceiveUntilTrig(char cStartMarker, char cEndMarker, uint8_t ucMaxLength);
uint8_t UART3_GetRxCount(void);

bool UART3_IsRxReady(void);
bool UART3_IsTxReady(void);

void UART3_FlushRx(void);
void UART3_FlushTx(void);

#endif /* COMMLIB_UART3_H_ */