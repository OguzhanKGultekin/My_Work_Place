#include "i2c.h"
#include "HAL_I2C.h" 
#include "delay.h" 

void Init_I2C(void){

  I2C_InitTypeDef  I2C_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable I2C and GPIO clocks */
  RCC_APB1PeriphClockCmd(I2C_RCC_Periph, ENABLE);
  RCC_AHB1PeriphClockCmd(I2C_RCC_Port, ENABLE);
  /* Configure I2C pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  I2C_SCL_Pin | I2C_SDA_Pin; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType=GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
  GPIO_Init(I2C_Port, &GPIO_InitStructure); 
      
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);

  I2C_DeInit(I2C1);

  /* I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x68<<1; 
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;

  /* Apply I2C configuration after enabling it */
  I2C_Init(I2C1, &I2C_InitStructure);
  /* I2C Peripheral Enable */  
  I2C_Cmd(I2C1, ENABLE);
}

void I2C_readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data){
	
	I2C_readBytes(devAddr, regAddr, 1, data);
}

void I2C_readBytes(uint8_t slaveAddr, uint8_t readAddr, uint8_t NumByteToRead, uint8_t *pBuffer) {

/* While the bus is busy */
  while(I2C_GetFlagStatus(I2C, I2C_FLAG_BUSY));
	
  /* Send START condition */
  I2C_GenerateSTART(I2C, ENABLE);

  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_MODE_SELECT));

  /* Send MPU6050 address for write */
  I2C_Send7bitAddress(I2C, slaveAddr, I2C_Direction_Transmitter); 

  /* Test on EV6 and clear it *///takiliyor
  while(!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
		
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C, ENABLE);

  /* Send the MPU6050's internal address to write to */
  I2C_SendData(I2C, readAddr);
	
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* Send STRAT condition a second time */
  I2C_GenerateSTART(I2C, ENABLE);

  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_MODE_SELECT));

  /* Send MPU6050 address for read */
  I2C_Send7bitAddress(I2C, slaveAddr, I2C_Direction_Receiver);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
  /* While there is data to be read */
  while(NumByteToRead)
  {
    if(NumByteToRead == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(I2C, DISABLE);

      /* Send STOP Condition */
      I2C_GenerateSTOP(I2C, ENABLE);
    }
			
    /* Test on EV7 and clear it */
    if(I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
    {
      /* Read a byte from the MPU6050 */
      *pBuffer = I2C_ReceiveData(I2C);

      /* Point to the next location where the byte read will be saved */
      pBuffer++;

      /* Decrement the read bytes counter */
      NumByteToRead--;
    }
  }

  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(I2C, ENABLE);
}

void I2C_readBit(uint8_t address, uint8_t regAddr, uint8_t bitNum, uint8_t *data) {
	
    uint8_t tmp;
    I2C_readByte(address, regAddr, &tmp);  
    *data = tmp & (1 << bitNum);
}

void I2C_readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data)
{
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    uint8_t mask,tmp;
    I2C_readByte(devAddr, regAddr, &tmp); 
    mask = ((1 << length) - 1) << (bitStart - length + 1);
    tmp &= mask;
    tmp >>= (bitStart - length + 1);
		*data = tmp;
}

void I2C_writeBytes(uint8_t slaveAddr, uint8_t writeAddr, uint8_t length, uint8_t* data){
	
  int i;
  
  for(i=0; i<=length; i++){	
		I2C_writeByte(slaveAddr, writeAddr, data[i]);
		writeAddr++;
  }
}

void I2C_writeByte(uint8_t slaveAddr, uint8_t writeAddr, uint8_t data){

  /* Send START condition */
  I2C_GenerateSTART(I2C, ENABLE);

  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_MODE_SELECT));

  /* Send MPU6050 address for write */
  I2C_Send7bitAddress(I2C, slaveAddr, I2C_Direction_Transmitter);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  /* Send the MPU6050's internal address to write to */
  I2C_SendData(I2C, writeAddr);

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
  /* Send the byte to be written */
  I2C_SendData(I2C, data);

  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  /* Send STOP condition */
  I2C_GenerateSTOP(I2C, ENABLE);
}

void I2C_writeBit(uint8_t slaveAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data)
{
    uint8_t tmp;
    I2C_readByte(slaveAddr, regAddr, &tmp);  
    tmp = (data != 0) ? (tmp | (1 << bitNum)) : (tmp & ~(1 << bitNum));
    I2C_writeByte(slaveAddr,regAddr,tmp); 
}

void I2C_writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data)
{
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t tmp,mask;
    I2C_readByte(devAddr, regAddr, &tmp);
    mask = ((1 << length) - 1) << (bitStart - length + 1);
    data <<= (bitStart - length + 1); // shift data into correct position
    data &= mask; // zero all non-important bits in data
    tmp &= ~(mask); // zero all important bits in existing byte
    tmp |= data; // combine data with existing byte
    I2C_writeByte(devAddr, regAddr, tmp);	
}

void I2C_writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data) {
     I2C_writeWords(devAddr, regAddr, 1, &data);
}

void I2C_writeWords(uint8_t slaveAddr, uint8_t writeAddr, uint8_t length, uint16_t* data){
	
	int i;
	/* Send START condition */
	I2C_GenerateSTART(I2C, ENABLE);

	/* Test on EV5 and clear it */
	while(!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_MODE_SELECT));

	/* Send MPU6050 address for write */
	I2C_Send7bitAddress(I2C, slaveAddr, I2C_Direction_Transmitter);

	/* Test on EV6 and clear it */
	while(!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));	
	
	/* Send the MPU6050's internal address to write to */
	I2C_SendData(I2C, writeAddr);

	/* Test on EV8 and clear it */
	while(!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		
	/* Send the byte to be written */
	for(i=0; i<length*2; i++){
		
		I2C_SendData(I2C1, (uint8_t)(data[i++] >> 8));
		//delay_ms(25);

		I2C_SendData(I2C1, (uint8_t)data[i]);
		//delay_ms(25);

		
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}
	
	/* Send STOP condition */
	I2C_GenerateSTOP(I2C, ENABLE);
	//delay_ms(25);
}
