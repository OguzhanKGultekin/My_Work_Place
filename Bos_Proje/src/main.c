#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_conf.h"
#include "main.h"
#include "delay.h"

int main(void) {
	
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);

	
	while (1) {

			STM_EVAL_LEDToggle(LED3);
			delay_ms(100);
		  STM_EVAL_LEDToggle(LED4);
			delay_ms(100);
		  STM_EVAL_LEDToggle(LED5);
			delay_ms(100);
		  STM_EVAL_LEDToggle(LED6);
			delay_ms(100);
	}
}
