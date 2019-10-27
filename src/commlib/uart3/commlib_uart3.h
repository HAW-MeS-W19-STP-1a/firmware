#ifndef COMMLIB_UART3_H_
#define COMMLIB_UART3_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>


/*- Symbolische Konstanten ---------------------------------------------------*/
#define COMMLIB_UART3_MAX_BUF  64


/*- Globale Variablen --------------------------------------------------------*/
extern volatile uint8_t aucUart3TxBuf[COMMLIB_UART3_MAX_BUF];
extern volatile uint8_t aucUart3RxBuf[COMMLIB_UART3_MAX_BUF];


/*- Funktionsdeklarationen ---------------------------------------------------*/
void UART3_Init(void);
void UART3_DeInit(void);

void UART3_Send(uint8_t ucLength);
void UART3_SendUntil(char cEndMarker, uint8_t ucMaxLength);
void UART3_Receive(uint8_t ucLength);
void UART3_ReceiveUntil(char cEndMarker, uint8_t ucMaxLength);
uint8_t UART3_GetRxCount(void);

bool UART3_IsRxReady(void);
bool UART3_IsTxReady(void);

void UART3_FlushRx(void);
void UART3_FlushTx(void);

#endif /* COMMLIB_UART3_H_ */