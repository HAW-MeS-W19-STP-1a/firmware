/*- Headerdateien ------------------------------------------------------------*/
#include "stm8l15x.h"
#include "commlib_uart3.h"


/*- Typdefinitionen ----------------------------------------------------------*/
typedef enum tag_USART3_Mode {
  UART3_Mode_IDLE,
  UART3_Mode_LENGTH,
  UART3_Mode_CHAR
} UART3_Mode;


/*- Globale Variablen --------------------------------------------------------*/
volatile uint8_t aucUart3TxBuf[COMMLIB_UART3_MAX_BUF];
volatile uint8_t aucUart3RxBuf[COMMLIB_UART3_MAX_BUF];
volatile uint8_t ucUart3TxCtr;
volatile uint8_t ucUart3TxLen;
volatile char cUart3TxEndChar;
volatile uint8_t ucUart3RxCtr;
volatile uint8_t ucUart3RxLen;
volatile char cUart3RxEndChar;
volatile UART3_Mode eUart3RxMode;
volatile UART3_Mode eUart3TxMode;


void UART3_Init(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_USART3, ENABLE);
  USART_Init(USART3, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, USART_Mode_Rx | USART_Mode_Tx);
  USART_ClearFlag(USART3, USART_FLAG_FE);
  USART_Cmd(USART3, ENABLE);
  
  ucUart3TxCtr = 0;
  ucUart3TxLen = 0;
  cUart3TxEndChar = '\0';
  ucUart3RxCtr = 0;
  ucUart3RxLen = 0;
  cUart3RxEndChar = '\0';
  eUart3TxMode = UART3_Mode_IDLE;
  eUart3RxMode = UART3_Mode_IDLE;
}

void UART3_DeInit(void)
{
  USART_Cmd(USART3, DISABLE);
  USART_ITConfig(USART3, USART_IT_RXNE | USART_IT_TXE, DISABLE);
  USART_DeInit(USART3);
  CLK_PeripheralClockConfig(CLK_Peripheral_USART3, DISABLE);
}

void UART3_Send(uint8_t ucLength)
{
  eUart3TxMode = UART3_Mode_LENGTH;
  cUart3TxEndChar = '\0';
  ucUart3TxCtr = 1;
  ucUart3TxLen = ucLength;
  USART_SendData8(USART3, aucUart3TxBuf[0]);
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

void UART3_SendUntil(char cEndMarker, uint8_t ucMaxLength)
{
  eUart3TxMode = UART3_Mode_CHAR;
  cUart3TxEndChar = cEndMarker;
  ucUart3TxCtr = 1;
  ucUart3TxLen = ucMaxLength;
  USART_SendData8(USART3, aucUart3TxBuf[0]);
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

void UART3_Receive(uint8_t ucLength)
{
  eUart3RxMode = UART3_Mode_LENGTH;
  cUart3RxEndChar = '\0';
  ucUart3RxCtr = 0;
  ucUart3RxLen = ucLength;
  USART_ReceiveData8(USART3);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

void UART3_ReceiveUntil(char cEndMarker, uint8_t ucMaxLength)
{
  eUart3RxMode = UART3_Mode_CHAR;
  cUart3RxEndChar = cEndMarker;
  ucUart3RxCtr = 0;
  ucUart3RxLen = ucMaxLength;
  USART_ReceiveData8(USART3);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

uint8_t UART3_GetRxCount(void)
{
  return ucUart3RxCtr;
}

bool UART3_IsRxReady(void)
{
  return (eUart3RxMode == UART3_Mode_IDLE);
}

bool UART3_IsTxReady(void)
{
  return (eUart3TxMode == UART3_Mode_IDLE);
}

void UART3_FlushRx(void)
{
  uint8_t ucIndex;
  for (ucIndex = 0; ucIndex < COMMLIB_UART3_MAX_BUF; ++ucIndex)
  {
    aucUart3RxBuf[ucIndex] = 0;
  }
}

void UART3_FlushTx(void)
{
  uint8_t ucIndex;
  for (ucIndex = 0; ucIndex < COMMLIB_UART3_MAX_BUF; ++ucIndex)
  {
    aucUart3TxBuf[ucIndex] = 0;
  }
}

@far @interrupt void UART3_TxInterruptHandler(void)
{
  switch (eUart3TxMode)
  {
    case UART3_Mode_CHAR:
      if ((ucUart3TxCtr < ucUart3TxLen) && ((char)aucUart3TxBuf[ucUart3TxCtr] != cUart3TxEndChar))
      {
        USART_SendData8(USART3, aucUart3TxBuf[ucUart3TxCtr]);
        ++ucUart3TxCtr;
      }
      else
      {
        eUart3TxMode = UART3_Mode_IDLE;
        USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
      }
      break;
          
    case UART3_Mode_LENGTH:
      if (ucUart3TxCtr < ucUart3TxLen)
      {
        USART_SendData8(USART3, aucUart3TxBuf[ucUart3TxCtr]);
        ++ucUart3TxCtr;
      }
      else
      {
        eUart3TxMode = UART3_Mode_IDLE;
        USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
      }
      break;
    
    case UART3_Mode_IDLE:
    default:
      USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
  }
}

@far @interrupt void UART3_RxInterruptHandler(void)
{
  uint8_t ucRxData = USART_ReceiveData8(USART3);
  
  switch (eUart3RxMode)
  {
    case UART3_Mode_CHAR:
      if ((ucUart3RxCtr < ucUart3RxLen) && ((char)ucRxData != cUart3RxEndChar))
      {
        aucUart3RxBuf[ucUart3RxCtr] = ucRxData;
        ++ucUart3RxCtr;
        
        if (ucUart3RxCtr == ucUart3RxLen)
        {
          eUart3RxMode = UART3_Mode_IDLE;
          USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
        }
      }
      else
      {
        eUart3RxMode = UART3_Mode_IDLE;
        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
      }
      break;
    
    case UART3_Mode_LENGTH:
      if (ucUart3RxCtr < ucUart3RxLen)
      {
        aucUart3RxBuf[ucUart3RxCtr] = USART_ReceiveData8(USART3);
        ++ucUart3RxCtr;
        
        if (ucUart3RxCtr == ucUart3RxLen)
        {
          eUart3RxMode = UART3_Mode_IDLE;
          USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
        }
      }
      else
      {
        eUart3RxMode = UART3_Mode_IDLE;
        USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
      }
      break;
    
    case UART3_Mode_IDLE:
    default:
      USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
  }
}