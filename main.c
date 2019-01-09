#include "stm32f10x.h"

volatile uint32_t time_ms = 0;

void SysTick_Handler(){
	if(time_ms)
		time_ms--;	
}

void delay_ms(int t){
	time_ms = t;
	while(time_ms);
}

void send_char( char c ){
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2, c);
}

void send_string( const char* s){
	while(*s)
		send_char(*s++);
}

void send_int( int i){
	char buffer[10];
	sprintf(buffer, "%d", i);
	send_string(buffer);
}

int main( void ){
	
	//podlaczenie portu GPIOA oraz GPIOC do linii zegara takutjacego 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);
	
	//deklaracja obiektu typu GPIO
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio); //ustawienie GPIO na stany domyslne
	
	//przygotowanie moudulu GPIO 
	//PIN9C (wyjscie)
	gpio.GPIO_Pin = GPIO_Pin_9;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &gpio);
	
	//PIN2A (funckja alternatywna TX) (wyjscie)
	gpio.GPIO_Pin = GPIO_Pin_2;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio);
	
	//podlaczenie funkcji alterantywnych do zegara taktujacego
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	//podlaczenie ADC do zegara taktujacego
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//dzielnik czestotliwosc zegara dla ADC
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	//deklaracja obiektu typu ADC
	ADC_InitTypeDef adc; 
	ADC_StructInit(&adc); //ustawienie ADC na stany domyslne
	
	//porzgotowanie modulu ADC
	adc.ADC_ContinuousConvMode = ENABLE;
	adc.ADC_NbrOfChannel = 1;
	adc.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_Init(ADC1, &adc);
	
	//wybor portu
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_71Cycles5);
	
	//uruchomienie ADC
	ADC_Cmd(ADC1, ENABLE);
	
	//kalibracja ADC
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	//rozpoczecie pomiarów ADC
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	//podlaczenie USART do linii zegara taktujacego
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//deklaracja obiektu typu USART
	USART_InitTypeDef usart;
	USART_StructInit(&usart); //ustawienie USART na stany domyslne
	
	//przygotowanie modulu USART
	usart.USART_BaudRate = 9600;
	USART_Init(USART2, &usart);
	USART_Cmd(USART2, ENABLE);
	
	//okeslenie co ile cykli zegara ma byc wywolane przerwanie 
	//oraz wykonana fukncja SysTick_Handler
	SysTick_Config(SystemCoreClock / 1000);
	
	while(1){
		send_string("Wartosc ADC wynosi: ");
		send_int(ADC_GetConversionValue(ADC1));
		send_string("\n\r");
		
		delay_ms(100);
		
		if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE)){
			char c = USART_ReceiveData(USART2);
			switch(c){
				case 'q' : GPIO_SetBits(GPIOC, GPIO_Pin_9); break;
				case 'a' : GPIO_ResetBits(GPIOC, GPIO_Pin_9); break;
				default: GPIO_ResetBits(GPIOC, GPIO_Pin_9); break;
			}
		}
	}
}