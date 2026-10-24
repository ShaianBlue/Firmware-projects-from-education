/*
 * Uart.h
 *
 * Created: 08-04-2021 12:56:34
 *  Author: Kristian
 */ 


#ifndef UART_H_
#define UART_H_

void UART0_INIT(unsigned int ubrr);
void UART0int_init(unsigned int ubrr);
char recieveUART0(void);
void transmitUART0(char tx);
void putsUSART0(char *ptr);
void UART1int_init(unsigned int ubrr);

#endif /* UART_H_ */