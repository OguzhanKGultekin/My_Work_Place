#ifndef __I2C_H
#define __I2C_H

#include "stm32f4xx.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_gpio.h"

extern void Init_I2C(void);

extern void I2C_readByte(uint8_t address, uint8_t readAddr, uint8_t *data);
extern void I2C_readBytes(uint8_t slaveAddr, uint8_t readAddr, uint8_t NumByteToRead, uint8_t *pBuffer);

extern void I2C_readBit(uint8_t address, uint8_t regAddr, uint8_t bitNum, uint8_t *data);
extern void I2C_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);

extern void I2C_writeByte(uint8_t address, uint8_t writeAddr, uint8_t data);
//extern void I2C_writeBytes(uint8_t Address, uint16_t addr, uint16_t NumByteToWrite, uint8_t* pBuffer);
extern void I2C_writeBytes(uint8_t slaveAddr, uint8_t writeAddr, uint8_t length, uint8_t* data);

extern void I2C_writeBit(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
extern void I2C_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);

extern void I2C_writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);
extern void I2C_writeWords(uint8_t slaveAddr, uint8_t writeAddr, uint8_t length, uint16_t* data);

#endif /* __I2C_H */
