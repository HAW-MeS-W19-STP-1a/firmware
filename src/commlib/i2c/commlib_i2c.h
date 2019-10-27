/*!****************************************************************************
 * @file
 * commlib_i2c.h
 *
 * Handler für I2C-Kommunikation im Master Mode. Externe Schnittstellen
 *
 * @date  16.10.2019
 * @date  22.20.2019
 ******************************************************************************/

#ifndef COMMLIB_I2C_H_
#define COMMLIB_I2C_H_

/*- Headerdateien ------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>


/*- Symbolische Konstanten ---------------------------------------------------*/
#define COMMLIB_I2C_MAX_BUF   2


/*- Typdefinitionen ----------------------------------------------------------*/
typedef struct tag_I2CMaster_TxData_TypeDef {
  volatile uint8_t ucRegister;
  volatile uint8_t aucData[COMMLIB_I2C_MAX_BUF-1];
} I2CMaster_TxData_TypeDef;
typedef struct tag_I2CMaster_RxData_TypeDef {
  volatile uint8_t aucData[COMMLIB_I2C_MAX_BUF];
} I2CMaster_RxData_TypeDef;


/*- Funktionsdeklarationen ---------------------------------------------------*/
void I2CMaster_Init(void);
void I2CMaster_DeInit(void);

/*const I2CMaster_RxData_TypeDef* I2CMaster_GetRx(void);
I2CMaster_TxData_TypeDef* I2CMaster_GetTx(void);*/

void I2CMaster_RegisterWrite(uint8_t ucSlaveAddr, uint8_t ucRegister, uint8_t ucValue);
uint8_t I2CMaster_RegisterRead(uint8_t ucSlaveAddr, uint8_t ucRegister);

#endif /* COMMLIB_I2C_H_ */