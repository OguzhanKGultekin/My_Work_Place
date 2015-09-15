#include "MPU6050.h"
#include "calibration.h"
#include "main.h"

int16_t AX;
int16_t AY;
int16_t AZ;
 
int16_t GX;
int16_t GY;
int16_t GZ;

int16_t ax_offset=0,ay_offset=0,az_offset=0,gx_offset=0,gy_offset=0,gz_offset=0;

int i;
char str_calibration[23];

void calibrate_gyro() {
	
	//MPUsetXGyroOffset(0);
	//MPUsetYGyroOffset(0);
	//MPUsetZGyroOffset(0);
	
	for(i=0; i<2000; i++){
		MPUgetMotion6(&AX,&AY,&AZ,&GX,&GY,&GZ);
		
		if(i>999) {
			
			gx_offset = gx_offset + GX; 
			gy_offset = gy_offset + GY; 
			gz_offset = gz_offset + GZ; 
		}	
	}
	
	gx_offset = gx_offset / 1000;
	gy_offset = gy_offset / 1000;
	gz_offset = gz_offset / 1000;
	
	sprintf(str_calibration, "#%d:%d:%d\n\r", gx_offset,gy_offset,gz_offset);
	USART_puts(USART2,str_calibration);
	
	//MPUsetXGyroOffset(gx_offset);
	//MPUsetYGyroOffset(gy_offset);
	//MPUsetZGyroOffset(gz_offset);
}
