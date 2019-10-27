/*!****************************************************************************
 * @file
 * commlib_i2c.c
 *
 * Handler für die I2C-Kommunikation im Master-Mode
 *
 * @date  16.10.2019
 * @date  22.10.2019
 ******************************************************************************/
 
/*- Headerdateien ------------------------------------------------------------*/
#include "stm8l15x.h"
#include "powerlib.h"
#include "commlib_i2c_interrupt.h"
#include "commlib_i2c.h"


/*!**************************************************************************** 
 * @brief
 * I2C Master Schnittstelle initialisieren
 *
 * @date  22.10.2019
 ******************************************************************************/
void I2CMaster_Init(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_I2C1, ENABLE);
  I2C_Init(
    I2C1, 
    100000,
    0xA0,
    I2C_Mode_I2C,
    I2C_DutyCycle_16_9,
    I2C_Ack_Enable,
    I2C_AcknowledgedAddress_7bit
  );
  I2C_StretchClockCmd(I2C1, ENABLE);
  I2CMaster_Int_Init();
  I2C_Cmd(I2C1, ENABLE);
}

/*!****************************************************************************
 * @brief
 * I2C Master Schnittstelle deaktiveren
 *
 * @date  22.10.2019
 ******************************************************************************/
void I2CMaster_DeInit(void)
{
  I2C_Cmd(I2C1, DISABLE);
  I2CMaster_Int_DeInit();
  I2C_DeInit(I2C1);
  CLK_PeripheralClockConfig(CLK_Peripheral_I2C1, DISABLE);
}

const I2CMaster_RxData_TypeDef* I2CMaster_GetRx(void)
{
  return (const I2CMaster_RxData_TypeDef*)aucRxBuf;
}

I2CMaster_TxData_TypeDef* I2CMaster_GetTx(void)
{
  return (I2CMaster_TxData_TypeDef*)aucTxBuf;
}

/*!****************************************************************************
 * @brief
 * Registerschreibauftrag erzeugen und senden (blocking)
 *
 * @param[in] ucSlaveAddr   Slave-Adresse
 * @param[in] ucRegister    Register/Offset
 * @param[in] ucValue       Registerwert
 *
 * @date  22.10.2019
 ******************************************************************************/
void I2CMaster_RegisterWrite(uint8_t ucSlaveAddr, uint8_t ucRegister, uint8_t ucValue)
{
  //while (!I2CMaster_Int_IsReady() && !I2CMaster_Int_IsError());
  if (!I2CMaster_Int_IsReady())
  {
    I2C_GenerateSTOP(I2C1, ENABLE);
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF)) { Power_Wait(); }
  }
    
  /* Datensatz zusammenstellen */
  I2CMaster_GetTx()->ucRegister = ucRegister;
  I2CMaster_GetTx()->aucData[0] = ucValue;
  I2CMaster_Int_QueueTransmit(ucSlaveAddr, 2);
  
  while (!I2CMaster_Int_IsReady() && !I2CMaster_Int_IsError()) { Power_Wait(); }
}

/*!****************************************************************************
 * @brief
 * Registerleseauftrag erzeugen und senden
 *
 * @param[in] ucSlaveAddr   Slave-Adresse
 * @param[in] ucRegister    Register/Offset
 * 
 * @date  22.10.2019
 ******************************************************************************/
uint8_t I2CMaster_RegisterRead(uint8_t ucSlaveAddr, uint8_t ucRegister)
{
  //while (!I2CMaster_Int_IsReady() && !I2CMaster_Int_IsError());
  if (!I2CMaster_Int_IsReady())
  {
    I2C_GenerateSTOP(I2C1, ENABLE);
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF)) { Power_Wait(); }
  }
  
  /* Registeradresse senden                               */
  I2CMaster_Int_ClearRx();
  I2CMaster_GetTx()->ucRegister = ucRegister;
  I2CMaster_Int_QueueTransmit(ucSlaveAddr, 1);
  while (!I2CMaster_Int_IsReady() && !I2CMaster_Int_IsError()) { Power_Wait(); }
  if (I2CMaster_Int_IsError())
  {
    return 0xFF;
  }
  
  /* Registerinhalt lesen                                 */
  I2CMaster_Int_QueueReceive(ucSlaveAddr, 1);
  while (!I2CMaster_Int_IsReady() && !I2CMaster_Int_IsError()) { Power_Wait(); }
  if (I2CMaster_Int_IsError())
  {
    return 0xFF;
  }
  
  return I2CMaster_GetRx()->aucData[0];
}