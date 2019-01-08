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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	
	gpio.GPIO_Pin = GPIO_Pin_9;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &gpio);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	ADC_InitTypeDef adc;
	ADC_StructInit(&adc);
	
	adc.ADC_ContinuousConvMode = ENABLE;
	adc.ADC_NbrOfChannel = 1;
	adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_Init(ADC1, &adc);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_71Cycles5);
	
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		
	SysTick_Config(SystemCoreClock / 1000);
	
	while(1){
		GPIO_SetBits(GPIOC, GPIO_Pin_9);
		delay_ms(ADC_GetConversionValue(ADC1));
		GPIO_ResetBits(GPIOC, GPIO_Pin_9);
		delay_ms(ADC_GetConversionValue(ADC1));
	}
}