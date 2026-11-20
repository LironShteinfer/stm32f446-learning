/*
 * usart2.c - minimal register-level USART2 driver.
 * PA2 = TX, PA3 = RX (routed to the ST-LINK virtual COM port), 9600 baud, 8N1.
 * Assumes the system clock is the 16 MHz HSI with APB1 undivided.
 */

#include <stdarg.h>
#include <stdio.h>
#include "stm32f446xx.h"
#include "usart2.h"

#define USART2_PCLK_HZ   16000000U
#define USART2_BAUD      9600U

static char print_buffer[USART2_PRINT_BUFFER_SIZE];

/* printf-style output over USART2. vsnprintf truncates instead of
 * writing past the end of the buffer. */
void printUSART(const char *p_format, ...)
{
	va_list args;
	va_start(args, p_format);
	(void)vsnprintf(print_buffer, sizeof(print_buffer), p_format, args);
	va_end(args);
	USART2_print(print_buffer);
}

void USART2_init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;    /* GPIOA clock */
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;   /* USART2 clock */

	/* PA2 and PA3 in alternate-function mode, AF7 = USART2.
	 * AFR[0] is AFRL in the reference manual. */
	GPIOA->MODER  &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
	GPIOA->MODER  |=  (GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1);
	GPIOA->AFR[0] &= ~((0xFU << (2U * 4U)) | (0xFU << (3U * 4U)));
	GPIOA->AFR[0] |=  ((7U << (2U * 4U)) | (7U << (3U * 4U)));

	/* Oversampling by 16: BRR = PCLK / baud, rounded (16 MHz / 9600 = 1667 = 0x683) */
	USART2->BRR = (USART2_PCLK_HZ + USART2_BAUD / 2U) / USART2_BAUD;
	USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;   /* enable USART, TX and RX */;
}

char USART2_read(void)
{
	while (!(USART2->SR & USART_SR_RXNE));  /* wait for a received byte */
	return (char)USART2->DR;
}

/* Reads one line (up to maxLen - 1 characters) into buf.
 * Leading CR/LF are skipped, so terminals that send "\r\n" do not
 * produce an empty command after every line. */
void USART2_readString(char *buf, int maxLen)
{
	int i = 0;
	char c;

	do {
		c = USART2_read();
	} while (c == '\r' || c == '\n');

	while (i < maxLen - 1)
	{
		if (c == '\r' || c == '\n') break;
		buf[i++] = c;
		c = USART2_read();
	}
	buf[i] = '\0';
}

void USART2_print(const char *p_data)
{
	while (*p_data != '\0')
	{
		USART2_printCharacter(*p_data);
		p_data++;
	}
}

void USART2_printCharacter(char c)
{
	while (!(USART2->SR & USART_SR_TXE));   /* wait until the data register is empty */
	USART2->DR = (uint8_t)c;
}
