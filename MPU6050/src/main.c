/*

TX - PA2
RX - PA3

SCL - PB7 -> SDA(MPU6050)
SDA - PB8 -> SCL(MPU6050)

*/

#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_conf.h"
#include "main.h"
#include "delay.h"

#include "MPU6050.h"
#include "i2c.h"
#include "calibration.h"

#include <stdio.h>
#include <stdlib.h>

#define PI 3.14f

char str_main[50];

int16_t AccX;
int16_t AccY;
int16_t AccZ;
 
int16_t GyrX;
int16_t GyrY;
int16_t GyrZ;

float ax,ay,az,gx,gy,gz;
float R;
float Pitch,Roll;
float compAnglePitch, compAngleRoll;
float dt = 1/191.0f;

int main(void) {
	
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);

	Init_USART2();
  Init_I2C();	
	
	MPU6050(0x68<<1);
	MPUinitialize();
	
	if( MPUtestConnection()== SUCCESS) {
		STM_EVAL_LEDOn(LED3);
	}
	
	MPUsetFullScaleGyroRange(3);
	
	//MPUsetXGyroOffset(0); 
	//MPUsetYGyroOffset(0);
	//MPUsetZGyroOffset(0);

	//calibrate_gyro();

	while (1) {

		MPUgetMotion6(&AccX,&AccY,&AccZ,&GyrX,&GyrY,&GyrZ);
		
		ax = (float) AccX * (1/16384.0f);
 		ay = (float) AccY * (1/16384.0f);
		az = (float) AccZ * (1/16384.0f);
		
		R = sqrt(ax*ax + ay*ay + az*az);
		ax = ax/R;
		ay = ay/R;
		az = az/R;
		
		gx = (float) GyrX * (1/131.0f);		//(PI/180.0f)
		gy = (float) GyrY * (1/131.0f);
		gz = (float) GyrZ * (1/131.0f);
		
		//sprintf(str_main,"   #: %d : %d : %d : %d : %d : %d :\n\r",AccX,AccY,AccZ,GyrX,GyrY,GyrZ);
		//sprintf(str_main,"   #: %.2f : %.2f : %.2f : %.2f : %.2f : %.2f :\n\r",ax,ay,az,gx,gy,gz);
    //USART_puts(USART2,str_main);
		
		Pitch = atan2(ax, sqrt(ay*ay + az*az)) * (180/PI);
	  Roll =  atan2(ay, sqrt(ax*ax + az*az)) * (180/PI);
		//sprintf(str_main," # : %.2f : %.2f : %.2f :\n\r", Pitch,Roll,0.00f);
    //USART_puts(USART2,str_main);
		
		//Complimentary filter
		compAnglePitch = (0.93 * (compAnglePitch + (gx * dt))) + (0.07 * Pitch);
    compAngleRoll = (0.93 * (compAngleRoll + (gy * dt))) + (0.07 * Roll);
		//sprintf(str_main," # : %.2f : %.2f : %.2f :\n\r", compAnglePitch,compAngleRoll,0.00f);
    //USART_puts(USART2,str_main);
		
		sprintf(str_main,"#:%.2f:%.2f:%.2f\n", compAngleRoll, compAnglePitch, 0.00f);
		USART_puts(USART2,str_main);

		STM_EVAL_LEDToggle(LED6);
	}
}

void Init_USART2(){
	
	GPIO_InitTypeDef GPIO_InitStruct; 
	USART_InitTypeDef USART_InitStruct; 
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; 			
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;			
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;			
	GPIO_Init(GPIOA, &GPIO_InitStruct);					
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	USART_InitStruct.USART_BaudRate = 115200;				
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;	
	USART_InitStruct.USART_Parity = USART_Parity_No;		
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; 
	USART_Init(USART2, &USART_InitStruct);					
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			 
	NVIC_Init(&NVIC_InitStructure);						

	USART_Cmd(USART2, ENABLE);
}

void USART_puts(USART_TypeDef* USARTx, volatile char *s){

	while(*s){
		// wait until data register is empty
		while( !(USARTx->SR & 0x00000040) ); 
		USART_SendData(USARTx, *s);
		*s++;
	}
}
