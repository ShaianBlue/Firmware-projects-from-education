/*
 * main.c
 *
 * Created: 2/25/2021 3:32:02 PM
 *  Author: shaian
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include "SPI.h"

void master_init(void) {									//MOSI
	DDRB |=(1<<DDB2)|(1<<DDB1)|(1<<DDB0);					//Lader masterfunktioner være høj
	SPCR |=(1<<SPE)|(1<<MSTR);								//Aktiverer SPI, sætter master og lader klokken starte højt
	SPCR |=(1<<SPR1)/*|(1<<SPIE)*/;							//Lader klokken være på 500 kHz
	SPSR |=(1<<SPI2X);
}

void slave_init(void) {										//MISO
	DDRB |= (1 << DDB3);									//Lader slavefunktioner være høj
	SPCR |= (1 << SPE) ;									//Aktiverer SPI og lader klokken starte højt
	PORTB |= (1 << PB0);									//Lader SS! være høj
}

void putcSPI_master(unsigned char cx) {						//Afsending af data
	PORTB &=~(1<<PB0);										//Lader SS! være lav
	SPDR =cx;												//Indsætter værdi for data registeret
	while(!(SPSR & (1<<SPIF)));								//Tjekker om at slave har modtaget dataen
	PORTB |=(1<<PB0);										//Lader SS! være høj
}

unsigned char getcSPI_master(void) {						//Modtagning af data
	while(!(SPSR & (1 << SPIF)));							//Tjekker om at slave har modtaget dataen
	return SPDR;											//Returnerer dataregisterens indhold
}