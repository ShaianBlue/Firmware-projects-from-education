/*
 * Uart.c
 *
 * Created: 08-04-2021 12:54:17
 *  Author: Kristian
 */ 
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#define F_CPU 16000000UL
#include <util/delay.h>

void UART0_init(unsigned int ubrr)							// UART uden interrupt 
{
	UCSR0A=(1<<U2X0);										// full duplex
	

	UCSR0B|=(1<<RXEN0)|(1<<TXEN0);							// nu enables recieve and transmit.
	UCSR0C|=(1<<UCSZ00)|(1<<UCSZ01);						// framen sættes nu op til 8 data bit uden parity

	UBRR0H=(unsigned char)(ubrr>>8);						 
	UBRR0L =(unsigned char)ubrr;
}

void UART0int_init(unsigned int ubrr)						// UART med interrupt
{
	UCSR0A=(1<<U2X0);										// full duplex
	UCSR0B|=(1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);				// Recieve & transmit enable.  (1<<RXCIE0) enabler interrupt
	UCSR0C|=(1<<UCSZ00)|(1<<UCSZ01);						// framen sættes nu op til 8 data bit uden parity

	UBRR0H=(unsigned char)(ubrr>>8);
	UBRR0L =(unsigned char)ubrr;
}

char recieveUART0(void)										// henter information (1 bit af gangen).
{									
	while(!(UCSR0A& (1<<RXC0)));							// Venter på at en hel byte er modtaget					
	return UDR0;
}

void transmitUART0(char tx)									// sender information (1 byte af gangen).
{
	while(!(UCSR0A&(1<<UDRE0)));							// Venter på at en hel byte er sendt.
	UDR0 = tx;
}

void putsUSART0(char *ptr)									// Sender string til terminal
{
	 while(*ptr)
	{
	 	transmitUART0(*ptr);								
	 	ptr++;
	}
}
void UART1int_init(unsigned int ubrr)						// UART med interrupt
{
	UCSR1A=(1<<U2X1);										// full duplex
	UCSR1B|=(1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1);				// Recieve & transmit enable.  (1<<RXCIE0) enabler interrupt
	UCSR1C|=(1<<UCSZ10)|(1<<UCSZ11);						// framen sættes nu op til 8 data bit uden parity

	UBRR1H=(unsigned char)(ubrr>>8);
	UBRR1L =(unsigned char)ubrr;
}

char recieveUART1(void)										// henter information (1 bit af gangen).
{
	while(!(UCSR1A& (1<<RXC1)));							// Venter på at en hel byte er modtaget
	return UDR1;
}

void transmitUART1(char tx)									// sender information (1 bit af gangen).
{
	while(!(UCSR1A&(1<<UDRE1)));							// Venter på at en hel byte er sendt.
	UDR1 = tx;
}

void putsUSART1(char *ptr)									// Sender string til terminal
{
	while(*ptr)
	{
		transmitUART1(*ptr);
		ptr++;
	}
}
