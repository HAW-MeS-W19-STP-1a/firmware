#ifndef COMMLIB_I2C_INTERRUPT_H_
#define COMMLIB_I2C_INTERRUPT_H_

#include <stdint.h>

extern volatile uint8_t aucTxBuf[];
extern volatile uint8_t aucRxBuf[];

void I2CMaster_Int_Init(void);
void I2CMaster_Int_Flush(void);
void I2CMaster_Int_ClearRx(void);
void I2CMaster_Int_DeInit(void);

void I2CMaster_Int_QueueTransmit(uint8_t ucSlaveAddr, uint8_t ucDataLen);
void I2CMaster_Int_QueueReceive(uint8_t ucSlaveAddr, uint8_t ucDataLen);

bool I2CMaster_Int_IsReady(void);
bool I2CMaster_Int_IsError(void);

#endif /* COMMLIB_I2C_INTERRUPT_H_ */