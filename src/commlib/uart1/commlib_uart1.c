/*!****************************************************************************
 * @file
 * commlib_uart1.c
 *
 * Handler für serielle Kommunikation über die USART1-Schnittstelle.
 *
 * @date  26.10.2019
 ******************************************************************************/
 
/*- Headerdateien ------------------------------------------------------------*/
#include "stm8l15x.h"
#include "commlib_uart1.h"


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Interne Zustandskodierung für den Übertragungsmodus
 *
 * @date 26.10.2019
 ******************************************************************************/
typedef enum tag_USART1_Mode {
  /*! Schnittstelle nicht aktiv                           */
  UART1_Mode_IDLE,
  
  /*! Feste Anzahl an Zeichen senden/empfangen            */
  UART1_Mode_LENGTH,
  
  /*! Bis zum Marker-Zeichen senden/empfangen             */
  UART1_Mode_CHAR
} UART1_Mode;


/*- Globale Variablen --------------------------------------------------------*/
/*! Sendepuffer                                                               */
volatile uint8_t aucUart1TxBuf[COMMLIB_UART1_MAX_BUF];

/*! Empfangspuffer                                                            */
volatile uint8_t aucUart1RxBuf[COMMLIB_UART1_MAX_BUF];

/*! Zähler für gesendete Zeichen                                              */
volatile uint8_t ucUart1TxCtr;

/*! Maximale Anzahl zu sendender Zeichen                                      */
volatile uint8_t ucUart1TxLen;

/*! Marker-Zeichen für Ende der Sendedaten                                    */
volatile char cUart1TxEndChar;

/*! Zähler für empfangene Zeichen                                             */
volatile uint8_t ucUart1RxCtr;

/*! Maximale Anzahl zu empfangender Zeichen                                   */
volatile uint8_t ucUart1RxLen;

/*! Marker-Zeichen für Ende der Empfangsdaten                                 */
volatile char cUart1RxEndChar;

/*! Zustandsspeicher für Empfansmodus                                         */
volatile UART1_Mode eUart1RxMode;

/*! Zustandsspeicher für Sendemodus                                           */
volatile UART1_Mode eUart1TxMode;


/*!****************************************************************************
 * @brief
 * Modulinitialisierung
 *
 * @date  26.10.2019
 ******************************************************************************/
void UART1_Init(void)
{
  /* Peripheriemodul initialisieren                       */
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
  USART_Init(USART1, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, USART_Mode_Rx | USART_Mode_Tx);
  USART_ClearFlag(USART1, USART_FLAG_FE);
  USART_Cmd(USART1, ENABLE);
  
  /* Interne Zustandsvariablen initialisieren             */
  ucUart1TxCtr = 0;
  ucUart1TxLen = 0;
  cUart1TxEndChar = '\0';
  ucUart1RxCtr = 0;
  ucUart1RxLen = 0;
  cUart1RxEndChar = '\0';
  eUart1TxMode = UART1_Mode_IDLE;
  eUart1RxMode = UART1_Mode_IDLE;
}

/*!****************************************************************************
 * @brief
 * Modul deaktivieren
 *
 * @date  26.10.2019
 ******************************************************************************/
void UART1_DeInit(void)
{
  USART_Cmd(USART1, DISABLE);
  USART_ITConfig(USART1, USART_IT_RXNE | USART_IT_TXE, DISABLE);
  USART_DeInit(USART1);
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1, DISABLE);
}

/*!****************************************************************************
 * @brief
 * Daten aus dem Sendepuffer übertragen
 *
 * @param[in] ucLength    Anzahl der zu sendenden Bytes
 *
 * @date  26.10.2019
 ******************************************************************************/
void UART1_Send(uint8_t ucLength)
{
  eUart1TxMode = UART1_Mode_LENGTH;
  cUart1TxEndChar = '\0';
  ucUart1TxCtr = 1;
  ucUart1TxLen = ucLength;
  USART_SendData8(USART1, aucUart1TxBuf[0]);
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

/*!****************************************************************************
 * @brief
 * Daten aus dem Sendepuffer übertragen, bis Marker-Zeichen gefunden wird
 * 
 * @param[in] cEndMarker  Marker-Zeichen für Ende der Daten
 * @param[in] ucMaxLength Max. Länge der Sendedaten
 *
 * @date  26.10.2019
 ******************************************************************************/
void UART1_SendUntil(char cEndMarker, uint8_t ucMaxLength)
{
  eUart1TxMode = UART1_Mode_CHAR;
  cUart1TxEndChar = cEndMarker;
  ucUart1TxCtr = 1;
  ucUart1TxLen = ucMaxLength;
  USART_SendData8(USART1, aucUart1TxBuf[0]);
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

/*!****************************************************************************
 * @brief
 * Datenempfang in den Empfangspuffer starten
 *
 * @param[in] ucLength    Datenlänge, nach der die Übertragung abgeschlossen ist
 *
 * @date  26.10.2019
 ******************************************************************************/
void UART1_Receive(uint8_t ucLength)
{
  eUart1RxMode = UART1_Mode_LENGTH;
  cUart1RxEndChar = '\0';
  ucUart1RxCtr = 0;
  ucUart1RxLen = ucLength;
  USART_ReceiveData8(USART1);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

/*!****************************************************************************
 * @brief
 * Datenempfang starten. Einlesen, bis das Marker-Zeichen gefunden wurde
 *
 * @param[in] cEndMarker  Marker-Zeichen zum Beenden der Übertragung
 * @param[in] ucMaxLength Maximale Datenlänge
 *
 * @date  26.10.2019
 ******************************************************************************/
void UART1_ReceiveUntil(char cEndMarker, uint8_t ucMaxLength)
{
  eUart1RxMode = UART1_Mode_CHAR;
  cUart1RxEndChar = cEndMarker;
  ucUart1RxCtr = 0;
  ucUart1RxLen = ucMaxLength;
  USART_ReceiveData8(USART1);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

/*!****************************************************************************
 * @brief
 * Anzahl der empfangenen Bytes ermitteln
 *
 * @return  uint8_t   Anzahl der empfangenen Bytes im Puffer
 *
 * @date  26.10.2019
 ******************************************************************************/
uint8_t UART1_GetRxCount(void)
{
  return ucUart1RxCtr;
}

/*!****************************************************************************
 * @brief
 * Prüfen, ob der Datenempfang abgeschlossen wurde
 *
 * @return  bool      True, wenn keine Übertragung aktiv ist 
 *
 * @date  26.10.2019
 ******************************************************************************/
bool UART1_IsRxReady(void)
{
  return (eUart1RxMode == UART1_Mode_IDLE);
}

/*!****************************************************************************
 * @brief
 * Prüfen, ob der Sendevorgang abgeschlossen wurde
 *
 * @return  bool      True, wenn keine Übertragung aktiv ist
 *
 * @date  26.10.2019
 ******************************************************************************/
bool UART1_IsTxReady(void)
{
  return (eUart1TxMode == UART1_Mode_IDLE);
}

/*!****************************************************************************
 * @brief
 * Empfangspuffer leeren
 *
 * @date  26.10.2019
 ******************************************************************************/
void UART1_FlushRx(void)
{
  uint8_t ucIndex;
  for (ucIndex = 0; ucIndex < COMMLIB_UART1_MAX_BUF; ++ucIndex)
  {
    aucUart1RxBuf[ucIndex] = 0;
  }
  ucUart1RxCtr = 0;
  ucUart1RxLen = 0;
}

/*!****************************************************************************
 * @brief
 * Sendepuffer leeren
 *
 * @date  26.10.2019
 ******************************************************************************/
void UART1_FlushTx(void)
{
  uint8_t ucIndex;
  for (ucIndex = 0; ucIndex < COMMLIB_UART1_MAX_BUF; ++ucIndex)
  {
    aucUart1TxBuf[ucIndex] = 0;
  }
  ucUart1TxCtr = 0;
  ucUart1TxLen = 0;
}

/*!****************************************************************************
 * @brief
 * Interruptserviceroutine für Datenausgabe an der UART1-Schnittstelle
 *
 * @date  26.10.2019
 ******************************************************************************/
@far @interrupt void UART1_TxInterruptHandler(void)
{
  switch (eUart1TxMode)
  {
    case UART1_Mode_CHAR:
      if ((ucUart1TxCtr < ucUart1TxLen) && ((char)aucUart1TxBuf[ucUart1TxCtr] != cUart1TxEndChar))
      {
        USART_SendData8(USART1, aucUart1TxBuf[ucUart1TxCtr]);
        ++ucUart1TxCtr;
      }
      else
      {
        eUart1TxMode = UART1_Mode_IDLE;
        USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
      }
      break;
          
    case UART1_Mode_LENGTH:
      if (ucUart1TxCtr < ucUart1TxLen)
      {
        USART_SendData8(USART1, aucUart1TxBuf[ucUart1TxCtr]);
        ++ucUart1TxCtr;
      }
      else
      {
        eUart1TxMode = UART1_Mode_IDLE;
        USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
      }
      break;
    
    case UART1_Mode_IDLE:
    default:
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
  }
}

/*!****************************************************************************
 * @brief
 * Interruptserviceroutine für Datenempfang an der UART1-Schnittstelle
 *
 * @date  26.10.2019
 ******************************************************************************/
@far @interrupt void UART1_RxInterruptHandler(void)
{
  uint8_t ucRxData = USART_ReceiveData8(USART1);
  
  switch (eUart1RxMode)
  {
    case UART1_Mode_CHAR:
      if ((ucUart1RxCtr < ucUart1RxLen) && ((char)ucRxData != cUart1RxEndChar))
      {
        aucUart1RxBuf[ucUart1RxCtr] = ucRxData;
        ++ucUart1RxCtr;
        
        if (ucUart1RxCtr == ucUart1RxLen)
        {
          eUart1RxMode = UART1_Mode_IDLE;
          USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
        }
      }
      else
      {
        eUart1RxMode = UART1_Mode_IDLE;
        USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
      }
      break;
    
    case UART1_Mode_LENGTH:
      if (ucUart1RxCtr < ucUart1RxLen)
      {
        aucUart1RxBuf[ucUart1RxCtr] = USART_ReceiveData8(USART1);
        ++ucUart1RxCtr;
        
        if (ucUart1RxCtr == ucUart1RxLen)
        {
          eUart1RxMode = UART1_Mode_IDLE;
          USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
        }
      }
      else
      {
        eUart1RxMode = UART1_Mode_IDLE;
        USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
      }
      break;
    
    case UART1_Mode_IDLE:
    default:
      USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
  }
}