/*
 * usart2.h - minimal register-level USART2 driver
 */

#ifndef INC_USART2_H_
#define INC_USART2_H_

#define URAT2_SIZE_OF_PRINT_BUFFER 128

void USART2_init(void);

void USART2_print(const char *p_data, ...);

void USART2_printCharacter(char c);

void printUSART(char *p_format, ...);


#endif /* INC_USART2_H_ */
