/*!****************************************************************************
 * @file
 * commlib_uart3.c
 *
 * Handler für serielle Kommunikation über die USART3-Schnittstelle.
 *
 * @date  26.10.2019
 ******************************************************************************/

/*- Headerdateien ------------------------------------------------------------*/
#include "stm8l15x.h"
#include "commlib_uart3.h"


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Interne Zustandskodierung für den Übertragungsmodus
 *
 * @date 26.10.2019
 ******************************************************************************/
typedef enum tag_USART3_Mode {
  /*! Schnittstelle nicht aktiv                           */
  UART3_Mode_IDLE,
  
  /*! Feste Anzahl an Zeichen senden/empfangen            */
  UART3_Mode_LENGTH,
  
  UART3_Mode_LENGTH_TRIGGER,
  
  /*! Bis zum Marker-Zeichen senden/empfangen             */
  UART3_Mode_CHAR,
  
  UART3_Mode_CHAR_TRIGGER
} UART3_Mode;


/*- Globale Variablen --------------------------------------------------------*/
/*! Sendepuffer                                                               */
volatile uint8_t aucUart3TxBuf[COMMLIB_UART3TX_MAX_BUF];

/*! Empfangspuffer                                                            */
volatile uint8_t aucUart3RxBuf[COMMLIB_UART3RX_MAX_BUF];

/*! Zähler für gesendete Zeichen                                              */
volatile uint8_t ucUart3TxCtr;

/*! Maximale Anzahl zu sendender Zeichen                                      */
volatile uint8_t ucUart3TxLen;

/*! Marker-Zeichen für Ende der Sendedaten                                    */
volatile char cUart3TxEndChar;

/*! Zähler für empfangene Zeichen                                             */
volatile uint8_t ucUart3RxCtr;

/*! Maximale Anzahl zu empfangender Zeichen                                   */
volatile uint8_t ucUart3RxLen;

/*! Marker-Zeichen für Ende der Empfangsdaten                                 */
volatile char cUart3RxEndChar;

/*! Marker-Zeichen für Anfang der Empfangsdaten                               */
volatile char cUart3RxStartChar;

/*! Zustandsspeicher für Empfansmodus                                         */
volatile UART3_Mode eUart3RxMode;

/*! Zustandsspeicher für Sendemodus                                           */
volatile UART3_Mode eUart3TxMode;


/*!****************************************************************************
 * @brief
 * Modulinitialisierung
 *
 * @date  26.10.2019
 ******************************************************************************/
void UART3_Init(void)
{
  /* Peripheriemodul initialisieren                       */
  CLK_PeripheralClockConfig(CLK_Peripheral_USART3, ENABLE);
  USART_Init(USART3, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No, USART_Mode_Rx | USART_Mode_Tx);
  USART_ClearFlag(USART3, USART_FLAG_FE);
  USART_Cmd(USART3, ENABLE);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  
  /* Interne Zustandsvariablen initialisieren             */
  ucUart3TxCtr = 0;
  ucUart3TxLen = 0;
  cUart3TxEndChar = '\0';
  ucUart3RxCtr = 0;
  ucUart3RxLen = 0;
  cUart3RxEndChar = '\0';
  eUart3TxMode = UART3_Mode_IDLE;
  eUart3RxMode = UART3_Mode_IDLE;
}

/*!****************************************************************************
 * @brief
 * Modul deaktivieren
 *
 * @date  26.10.2019
 ******************************************************************************/
void UART3_DeInit(void)
{
  USART_Cmd(USART3, DISABLE);
  USART_ITConfig(USART3, USART_IT_RXNE | USART_IT_TXE, DISABLE);
  USART_DeInit(USART3);
  CLK_PeripheralClockConfig(CLK_Peripheral_USART3, DISABLE);
}

/*!****************************************************************************
 * @brief
 * Daten aus dem Sendepuffer übertragen
 *
 * @param[in] ucLength    Anzahl der zu sendenden Bytes
 *
 * @date  26.10.2019
 ******************************************************************************/
void UART3_Send(uint8_t ucLength)
{
  eUart3TxMode = UART3_Mode_LENGTH;
  cUart3TxEndChar = '\0';
  ucUart3TxCtr = 1;
  ucUart3TxLen = ucLength;
  USART_SendData8(USART3, aucUart3TxBuf[0]);
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
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
void UART3_SendUntil(char cEndMarker, uint8_t ucMaxLength)
{
  eUart3TxMode = UART3_Mode_CHAR;
  cUart3TxEndChar = cEndMarker;
  ucUart3TxCtr = 1;
  ucUart3TxLen = ucMaxLength;
  USART_SendData8(USART3, aucUart3TxBuf[0]);
  USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

/*!****************************************************************************
 * @brief
 * Datenempfang in den Empfangspuffer starten
 *
 * @param[in] ucLength    Datenlänge, nach der die Übertragung abgeschlossen ist
 *
 * @date  26.10.2019
 ******************************************************************************/
void UART3_Receive(uint8_t ucLength)
{
USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
  eUart3RxMode = UART3_Mode_LENGTH;
  cUart3RxEndChar = '\0';
  ucUart3RxCtr = 0;
  ucUart3RxLen = ucLength;
  USART_ReceiveData8(USART3);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
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
void UART3_ReceiveUntil(char cEndMarker, uint8_t ucMaxLength)
{
  USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
  eUart3RxMode = UART3_Mode_CHAR;
  cUart3RxEndChar = cEndMarker;
  ucUart3RxCtr = 0;
  ucUart3RxLen = ucMaxLength;
  USART_ReceiveData8(USART3);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

/*!****************************************************************************
 * @brief
 * Datenempfang starten. Einlesen, bis das Marker-Zeichen gefunden wurde. Emp-
 * fang bei Triggerzeichen starten
 *
 * @param[in] cEndMarker  Marker-Zeichen zum Beenden der Übertragung
 * @param[in] ucMaxLength Maximale Datenlänge
 *
 * @date  30.10.2019
 ******************************************************************************/
void UART3_ReceiveUntilTrig(char cStartMarker, char cEndMarker, uint8_t ucMaxLength)
{
  USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
  eUart3RxMode = UART3_Mode_CHAR_TRIGGER;
  cUart3RxStartChar = cStartMarker;
  cUart3RxEndChar = cEndMarker;
  ucUart3RxCtr = 0;
  ucUart3RxLen = ucMaxLength;
  USART_ReceiveData8(USART3);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

/*!****************************************************************************
 * @brief
 * Anzahl der empfangenen Bytes ermitteln
 *
 * @return  uint8_t   Anzahl der empfangenen Bytes im Puffer
 *
 * @date  26.10.2019
 ******************************************************************************/
uint8_t UART3_GetRxCount(void)
{
  return ucUart3RxCtr;
}

/*!****************************************************************************
 * @brief
 * Prüfen, ob der Datenempfang abgeschlossen wurde
 *
 * @return  bool      True, wenn keine Übertragung aktiv ist 
 *
 * @date  26.10.2019
 ******************************************************************************/
bool UART3_IsRxReady(void)
{
  return (eUart3RxMode == UART3_Mode_IDLE);
}

/*!****************************************************************************
 * @brief
 * Prüfen, ob der Sendevorgang abgeschlossen wurde
 *
 * @return  bool      True, wenn keine Übertragung aktiv ist
 *
 * @date  26.10.2019
 ******************************************************************************/
bool UART3_IsTxReady(void)
{
  return (eUart3TxMode == UART3_Mode_IDLE);
}

/*!****************************************************************************
 * @brief
 * Empfangspuffer leeren
 *
 * @date  26.10.2019
 ******************************************************************************/
void UART3_FlushRx(void)
{
  uint8_t ucIndex;
  for (ucIndex = 0; ucIndex < COMMLIB_UART3RX_MAX_BUF; ++ucIndex)
  {
    aucUart3RxBuf[ucIndex] = 0;
  }
  ucUart3RxCtr = 0;
  ucUart3RxLen = 0;
}

/*!****************************************************************************
 * @brief
 * Sendepuffer leeren
 *
 * @date  26.10.2019
 ******************************************************************************/
void UART3_FlushTx(void)
{
  uint8_t ucIndex;
  for (ucIndex = 0; ucIndex < COMMLIB_UART3TX_MAX_BUF; ++ucIndex)
  {
    aucUart3TxBuf[ucIndex] = 0;
  }
  ucUart3TxCtr = 0;
  ucUart3TxLen = 0;
}

/*!****************************************************************************
 * @brief
 * Interruptserviceroutine für Datenausgabe an der UART3-Schnittstelle
 *
 * @date  26.10.2019
 ******************************************************************************/
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

/*!****************************************************************************
 * @brief
 * Interruptserviceroutine für Datenempfang an der UART3-Schnittstelle
 *
 * @date  26.10.2019
 * @date  30.10.2019    Trigger hinzugefügt
 ******************************************************************************/
@far @interrupt void UART3_RxInterruptHandler(void)
{
  uint8_t ucRxData = USART_ReceiveData8(USART3);
  
  switch (eUart3RxMode)
  {
    case UART3_Mode_CHAR_TRIGGER:
      if (ucRxData == cUart3RxStartChar)
      {
        aucUart3RxBuf[ucUart3RxCtr] = ucRxData;
        ++ucUart3RxCtr;
        eUart3RxMode = UART3_Mode_CHAR;
      }
      break;
      
    case UART3_Mode_LENGTH_TRIGGER:
      if (ucRxData == cUart3RxStartChar)
      {
        aucUart3RxBuf[ucUart3RxCtr] = ucRxData;
        ++ucUart3RxCtr;
        eUart3RxMode = UART3_Mode_LENGTH;
      }
      break;
    
    case UART3_Mode_CHAR:
      if (ucUart3RxCtr < ucUart3RxLen)
      {
        aucUart3RxBuf[ucUart3RxCtr] = ucRxData;
        ++ucUart3RxCtr;
      }
      if (((char)ucRxData == cUart3RxEndChar) || (ucUart3RxCtr >= ucUart3RxLen))
      {
        eUart3RxMode = UART3_Mode_IDLE;
      }
      break;
      
    case UART3_Mode_LENGTH:
      if (ucUart3RxCtr < ucUart3RxLen)
      {
        aucUart3RxBuf[ucUart3RxCtr] = ucRxData;
        ++ucUart3RxCtr;
      }
      if (ucUart3RxCtr >= ucUart3RxLen)
      {
        eUart3RxMode = UART3_Mode_IDLE;
      }
      break;
      
    default:
      ;
  }
}