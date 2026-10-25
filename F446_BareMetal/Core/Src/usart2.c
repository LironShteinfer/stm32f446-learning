/*
 * usart2.c - minimal register-level USART2 driver (PA2/PA3, 9600 baud)
 */
#include <stdarg.h>
#include <stdio.h>
#include "stm32f446xx.h"
#include "usart2.h"


static char A_Buffer[URAT2_SIZE_OF_PRINT_BUFFER];

void printUSART(char *p_format, ...)
{
	va_list p_variables;
	va_start(p_variables, p_format);
	(void)vsprintf(A_Buffer, p_format, p_variables);
	va_end(p_variables);
	USART2_print(A_Buffer);
}

void USART2_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;// Enable GPIOA clock.
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Enable USART2 clock.

	// Configure GPIOA pins 2 and 3 as alternate function 7, which is USART2.
	GPIOA->MODER &= ~(0xF << 4);
	GPIOA->MODER |= 0x000000A0;
	GPIOA->AFR[0] |= 0x00007700;
	// AFR[[0] is the same as AFRL in the reference manual.


	USART2->BRR = 1666; // (System clock rate(16) + 10^6) / baud rate (9600)
	USART2->CR1 = 0x00002008; // Bit number 13 enable and bit num3 enable;

}

void USART2_print(const char *p_data, ...)
{
	while(*p_data != '\0')
	{
		while(!(USART2->SR & 0x00000080));
		USART2->DR = *p_data;
		p_data++;
	}
}

void USART2_printCharacter(char c)
{
	while(!(USART2->SR & 0x00000080)); // The TXE bit is bit 7 in the register ISR.
	USART2->DR = c;
}


