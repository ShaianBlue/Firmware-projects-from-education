/*
 * UART.h
 *
 * Created: 17-03-2021 16:21:46
 *  Author: M. Shaian Latif og Ludvig Skjøtt Mogensen
 */ 


#ifndef UART_H_
#define UART_H_

#define BAUD 19200

#define MYUBRRF F_CPU/8/BAUD-1

#define MYUBRRH F_CPU/16/BAUD-1

#define F_CPU 16000000UL

extern void uart0_init(unsigned int ubbr);

extern char getchUSART0 (void);

extern void putchUSART0 (char tx);

extern void putsUSART0 (char *ptr);

extern char getsUSART0(unsigned char *ptr, int max);

#endif /* UART_H_ */