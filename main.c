#include "stm32f10x.h"

volatile uint32_t time_ms = 0;

void SysTick_Handler(){
	if(time_ms){
		time_ms--;
	}
}

void delay_ms(int t){
	time_ms = t;
	while(time_ms){};
}

int main( void ){
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
		
	SysTick_Config(SystemCoreClock / 1000);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	gpio.GPIO_Pin = GPIO_Pin_9;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &gpio);
	
	while(1){
		GPIO_SetBits(GPIOC, GPIO_Pin_9);
		delay_ms(1000);
		GPIO_ResetBits(GPIOC, GPIO_Pin_9);
		delay_ms(1000);
	}
}