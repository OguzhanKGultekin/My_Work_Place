/* Define to prevent recursive inclusion*/
#ifndef __HAL_I2C_H
#define __HAL_I2C_H

/* Includes */
#include "stm32f4xx.h"

#define I2C                  I2C1
#define I2C_RCC_Periph       RCC_APB1Periph_I2C1
#define I2C_Port             GPIOB
#define I2C_SCL_Pin          GPIO_Pin_7
#define I2C_SDA_Pin          GPIO_Pin_8
#define I2C_RCC_Port         RCC_AHB1Periph_GPIOB
#define I2C_Speed            100000 // 100kHz standard mode

#endif /* __HAL_I2C_H */
