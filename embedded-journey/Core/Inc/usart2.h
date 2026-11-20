#ifndef INC_USART2_H_
#define INC_USART2_H_

/* Size of the buffer used by printUSART(). Longer output is truncated. */
#define USART2_PRINT_BUFFER_SIZE 128

void USART2_init(void);
void USART2_print(const char *p_data);
void USART2_printCharacter(char c);
void printUSART(const char *p_format, ...);
char USART2_read(void);
void USART2_readString(char *buf, int maxLen);

#endif /* INC_USART2_H_ */
