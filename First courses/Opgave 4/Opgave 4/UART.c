/*
 * main.c
 *
 * Created: 3/11/2021 4:02:22 PM
 *  Author: M. Shaian Latif og Ludvig Skjøtt Mogensen
 */ 
#define BAUD 19200										//Baudrate
#define MYUBRRF F_CPU/8/BAUD-1							//Full duplex
#define MYUBRRH F_CPU/16/BAUD-1							//Half duplex
#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "UART.h"

void uart0_init(unsigned int ubbr) {
	UCSR0A=(1<<U2X0);									//Full duplex - Aktiverer afsending og modtagning
	UCSR0B |=(1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);			//1 start bit, 8 data bit, ingen slutbit, aktiverer interrupt
	UCSR0C|=(1<<UCSZ00)|(1<<UCSZ01);					//Baudrate værdi med 16 bit
	UBRR0H = (unsigned char) (ubbr>>8);					//Sætter de 8 mest betydende ciffre for baudrate
	UBRR0L = (unsigned char)ubbr;						//Sætter de 8 mindst betydende ciffre for baudrate
}

char getchUSART0 (void) {								//Modtager en byte
	while(!(UCSR0A & (1<<RXC0)));						//Vent indtil karakter er modtaeget
	return UDR0;
}

void putchUSART0 (char tx) {							//Send en byte
	while(!(UCSR0A & (1<<UDRE0)));						//Vent til databyte bliver tømt
	UDR0 = tx;
}

void putsUSART0 (char *ptr) {							//Send en string
	while(*ptr) {										//Mens man stadig er i stringen
		putchUSART0(*ptr);								//Behandl hver element som en char byte
		ptr++;											//Går videre til næste element
	}
}

char getsUSART0(unsigned char *ptr, int max) {			//Modtag en string
	char cx;
	char i = 0;
	while((cx = getchUSART0()) != 0x0D && i < max) {	//Mens den modtagne byte ikke er en carriage
		*ptr = cx;										//Indsæt den modtagne byte som en string element
		putchUSART0(cx);								//Send elementet
		if(cx == 8){									//Hvis der er backspace
			putchUSART0(32);							//Sat et mellemrum
			putchUSART0(8);								//Sat en backspace
			ptr--;										//Går et element tilbage
		}
		else {
			ptr++;										//Går et element frem
			i++;										//Går et element nummer frem
		}
	}
	*ptr = 0;											//Stringen afluttes
	return i;											//Resturnerer element nummer
}