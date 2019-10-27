/*- Headerdateien ------------------------------------------------------------*/
#include "stm8l15x.h"
#include "commlib_i2c.h"
#include "commlib_i2c_interrupt.h"


/*- Typdefinitionen ----------------------------------------------------------*/
/*!****************************************************************************
 * @brief
 * Empfangs/Sendemodus
 *
 * @date  22.10.2019
 ******************************************************************************/
typedef enum tag_I2CMaster_Int_Mode
{
  I2CMaster_Int_Mode_IDLE,
  I2CMaster_Int_Mode_TRANSMIT,
  I2CMaster_Int_Mode_RECEIVE,
  I2CMaster_Int_Mode_ERROR
} I2CMaster_Int_Mode;


/*- Globale Variablen (für Interruptverarbeitung) ----------------------------*/
/*! Slaveadresse                                                              */
volatile uint8_t ucSlaveAddr;

/*! Sendepuffer                                                               */
volatile uint8_t aucTxBuf[COMMLIB_I2C_MAX_BUF];

/*! Länge der zu sendenden Daten                                              */
volatile uint8_t ucTxLen;
volatile uint8_t ucTxCtr;

/*! Empfangspuffer                                                            */
volatile uint8_t aucRxBuf[COMMLIB_I2C_MAX_BUF];

/*! Länge der zu empfangenden Daten                                           */
volatile uint8_t ucRxLen;
volatile uint8_t ucRxCtr;

volatile uint8_t ucIsrDebug;

/*! Sende/Empfangsmodus                                                       */
volatile I2CMaster_Int_Mode eMode;


/*!****************************************************************************
 * @brief
 * Interrupts für I2C-Schnittstelle initialisieren 
 *
 * @date  22.10.2019
 ******************************************************************************/
void I2CMaster_Int_Init(void)
{
  I2CMaster_Int_Flush();
  I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_ERR, ENABLE);
}

/*!****************************************************************************
 * @brief
 * Empfangspuffer leeren
 *
 * @date  22.10.2019
 ******************************************************************************/
void I2CMaster_Int_ClearRx(void)
{
  uint8_t ucIndex;
  for (ucIndex = 0; ucIndex < COMMLIB_I2C_MAX_BUF; ++ucIndex)
  {
    aucRxBuf[ucIndex] = 0;
  }
}

/*!****************************************************************************
 * @brief
 * Pufferinhalte leeren und States zurücksetzen
 *
 * @date  22.10.2019
 ******************************************************************************/
void I2CMaster_Int_Flush(void)
{
  uint8_t ucIndex;
  
  for (ucIndex = 0; ucIndex < COMMLIB_I2C_MAX_BUF; ++ucIndex)
  {
    aucRxBuf[ucIndex] = 0;
    aucTxBuf[ucIndex] = 0;
  }
  
  ucRxCtr = 0;
  ucTxCtr = 0;
  ucSlaveAddr = 0;
  
  eMode = I2CMaster_Int_Mode_IDLE;
}

/*!****************************************************************************
 * @brief
 * Interrupts für I2C-Schnittstelle deaktivieren
 *
 * @date  22.10.2019
 ******************************************************************************/
void I2CMaster_Int_DeInit(void)
{
  I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_ERR, DISABLE);
}

/*!**************************************************************************** 
 * @brief
 * Sendevorgang einreihen 
 *
 * @param[in] ucSlave     Slave-Adresse
 * @param[in] ucDataLen   Anzahl der zu sendenden Bytes
 *
 * @date  22.10.2019
 ******************************************************************************/
void I2CMaster_Int_QueueTransmit(uint8_t ucSlave, uint8_t ucDataLen)
{
  ucSlaveAddr = ucSlave << 1;
  ucTxLen = ucDataLen;
  ucTxCtr = 0;
  eMode = I2CMaster_Int_Mode_TRANSMIT;
  ucIsrDebug = 0;
  
  I2C_GenerateSTART(I2C1, ENABLE);
}

/*!****************************************************************************
 * @brief
 * Empfangsvorgang einreihen
 *
 * @param[in] ucSlave     Slave-Adresse
 * @param[in] ucDataLen   Anzahl der zu empfangenden Bytes
 *
 * @date  22.10.2019
 ******************************************************************************/
void I2CMaster_Int_QueueReceive(uint8_t ucSlave, uint8_t ucDataLen)
{
  ucSlaveAddr = ucSlave << 1;
  ucRxLen = ucDataLen;
  ucRxCtr = 0;
  eMode = I2CMaster_Int_Mode_RECEIVE;
  ucIsrDebug = 0;
  
  I2C_GenerateSTART(I2C1, ENABLE);
}

/*!****************************************************************************
 * @brief
 * Statusabfrage, ob Schnittstelle für neue Daten bereit ist
 *
 * @date  22.10.2019
 ******************************************************************************/
bool I2CMaster_Int_IsReady(void)
{
  return (eMode == I2CMaster_Int_Mode_IDLE);
}

/*!****************************************************************************
 * @brief
 * Statusabfrage, ob Schnittstelle im Fehlerzustand ist
 *
 * @date  22.10.2019
 ******************************************************************************/
bool I2CMaster_Int_IsError(void)
{
  return (eMode == I2CMaster_Int_Mode_ERROR);
}

/*!****************************************************************************
 * @brief
 * I2C Interruptserviceroutine - FSM abarbeiten
 *
 * @date  22.10.2019
 ******************************************************************************/
@far @interrupt void I2CMaster_Int_I2CInterruptHandler(void)
{
  if (I2C_GetFlagStatus(I2C1, I2C_FLAG_AF))
  {
    /* ACK Fehler erkannt                              */
    I2C_ClearFlag(I2C1, I2C_FLAG_AF);
    
    if (eMode == I2CMaster_Int_Mode_RECEIVE)
    {
      I2C_SoftwareResetCmd(I2C1, ENABLE);
      eMode = I2CMaster_Int_Mode_ERROR;
      return;
    }
  }
  
  if (I2C_GetFlagStatus(I2C1, I2C_FLAG_OVR))
  {
    I2C_ClearFlag(I2C1, I2C_FLAG_OVR);
    
    if (eMode == I2CMaster_Int_Mode_TRANSMIT)
    {
      I2C_SendData(I2C1, aucTxBuf[ucTxCtr]);
    }
    else
    {
      (void)I2C_ReceiveData(I2C1);
    }
  }
  
  if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BERR))
  {
    I2C_ClearFlag(I2C1, I2C_FLAG_BERR);
  }
  
  switch (I2C_GetLastEvent(I2C1))
  {
    case I2C_EVENT_MASTER_MODE_SELECT:
    {
      /* EV5: Adresse in DR schreiben                     */
      ucIsrDebug = 0x50;
      I2C_Send7bitAddress(I2C1, ucSlaveAddr, (eMode == I2CMaster_Int_Mode_RECEIVE) ? I2C_Direction_Receiver : I2C_Direction_Transmitter);
    }
    break;
    
    case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
    {
      /* EV6_TX: - nichts zu tun -                        */
      /* EV8_1: Erstes Byte in DR schreiben               */
      ucIsrDebug = 0x81;
      I2C_SendData(I2C1, aucTxBuf[ucTxCtr]);
      ++ucTxCtr;
    }
    break;
    
    case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
    {
      /* EV8_2: Daten senden / STOP am Ende               */
      ucIsrDebug = 0x82;
      if (ucTxCtr < ucTxLen)
      {
        /* Weitere Daten ausstehend                       */
        I2C_SendData(I2C1, aucTxBuf[ucTxCtr]);
        ++ucTxCtr;
      }
      else
      {
        /* Daten gesendet - STOP senden                   */
        I2C_GenerateSTOP(I2C1, ENABLE);
        
        /* Fertig.                                        */
        eMode = I2CMaster_Int_Mode_IDLE;
      }
    }
    break;
    
    case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
    {
      /* EV6_RX: ACK je nach Empfangslänge setzen           */
      /*bool bSingleByte = (ucRxLen == 1);
      ucIsrDebug = 0x60;
      I2C_AcknowledgeConfig(I2C1, bSingleByte ? DISABLE : ENABLE);
      I2C_GenerateSTOP(I2C1, bSingleByte ? ENABLE : DISABLE);
      while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
      aucRxBuf[ucRxCtr] = I2C_ReceiveData(I2C1);
      eMode = I2CMaster_Int_Mode_IDLE;*/
      ucIsrDebug = 0x60;
      if (ucRxLen == 1)
      {
        I2C_AcknowledgeConfig(I2C1, DISABLE);
        I2C_GenerateSTOP(I2C1, ENABLE);
        while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
        aucRxBuf[ucRxCtr] = I2C_ReceiveData(I2C1);
        eMode = I2CMaster_Int_Mode_IDLE;
      }
    }
    break;
    
    case I2C_EVENT_MASTER_BYTE_RECEIVED:
    case I2C_EVENT_MASTER_BYTE_RECEIVED | I2C_FLAG_BTF:
    {
      /* EV7 bzw. EV7_1: Daten einlesen und ACK vorbereiten */
      if (ucRxCtr < ucRxLen)
      {
        ucIsrDebug = 0x70;
        if (ucRxCtr == (ucRxLen-2))
        {
          aucRxBuf[ucRxCtr] = I2C_ReceiveData(I2C1);
          ++ucRxCtr;
          /* Letztes Byte                                 */
          I2C_AcknowledgeConfig(I2C1, DISABLE);
          I2C_GenerateSTOP(I2C1, ENABLE);
          while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
          aucRxBuf[ucRxCtr] = I2C_ReceiveData(I2C1);
          ucIsrDebug = I2C1->SR1;
          ucIsrDebug = I2C_ReceiveData(I2C1);
          ucIsrDebug = I2C_ReceiveData(I2C1);
          eMode = I2CMaster_Int_Mode_IDLE;
        }
        else
        {
          aucRxBuf[ucRxCtr] = I2C_ReceiveData(I2C1);
          ++ucRxCtr;
        }
      }
      else
      {
        ucIsrDebug = 0x71;
        
        /* Trailing P verwerfen                           */
        (void)I2C_ReceiveData(I2C1);
        
        /* Fertig.                                        */
        eMode = I2CMaster_Int_Mode_IDLE;
      }
    }
    break;
    
    default:
      ;
  }
}