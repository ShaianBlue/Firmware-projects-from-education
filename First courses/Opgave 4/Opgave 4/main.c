/*
 * main.c
 *
 * Created: 17-03-2021 16:24:31
 *  Author: M. Shaian Latif og Ludvig Skjøtt Mogensen
 */ 

#define MAX 11

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "UART.h"
#include "Timer.h"

char buffer[MAX] = {0};													//Array for klokken
volatile char flag_reset = 0;											//Flag for Reset interrupt
volatile char flag_rx = 0;												//Flag for UART Receive Interrupt
volatile int count = 0;													//Optæller, som får timeren til at tælle
volatile char flag_cn = 0;												//Flag for Timer/Counter Interrupt

//Ekstern interrupt
ISR(INT4_vect) {
	flag_reset = 1;														//Når interrupt er aktiveret sættes flaget til høj
}

//UART interrupt
ISR(USART0_RX_vect) {
	static int i = 0;													//En variabel for arrayelement defineres
	buffer[i++] = UDR0;													//Stringen skaffes fra UDR0 en ad gangen
	if(i == MAX - 1) {													//Tjekker om at elementet er nået til den maksimale værdi
		flag_rx = 1;													//Flaget sættes høj
		i = 0;															//Arrayelementet resettes til 0
	}
}

//Timer interrupt
ISR(TIMER1_COMPA_vect) {
	count++;															//Variablen tælles op med en
	if(count == 999) {													//Når tælleren er nået til 999
		flag_cn = 1;													//Flaget sættes høj
		count = 0;														//Tælleren resettes
	}
}

//Opdatering af tiden
void tid (char *klok) {
	int sec = (*(klok + 6) - 0x30) * 10 + *(klok + 7) - 0x30;			//Definerer sekunder
	int min = (*(klok + 3) - 0x30) * 10 + *(klok + 4) - 0x30;			//Definerer minutter
	int hour = (*klok - 0x30) * 10 + *(klok + 1) - 0x30;				//Definerer timer
	if(sec > 60 || min > 59 || hour > 23) {								//Hvis der bliver angivet en ugyldig tid
		putsUSART0("Ugyldig tid, systemet genstartes\n");				//Der bliver vist at dette er ugyldigt
		flag_reset = 1;													//Programmet aflsuttes pga fejl
	}
	sec++;																//Øger sekunder
	if(sec == 60) {														//Hvis sekunder er lig med 60
		if (min == 59) {												//Hvis minutter er lig med 59
			if (hour == 23) {											//Hvis timer er lig med 23
				sec = 0;												//Nulstiller sekunder
				min = 0;												//Nulstiller minutter
				hour = 0;												//Nulstiller timer
			}
			else {														//Hvis timer ikke er lig med 23
				sec = 0;												//Sekunder nulstilles
				min = 0;												//Minutter nulstilles
				hour++;													//Øger timer
			}
		}
		else {															//Hvis minutter ikke er lig med 59
			sec = 0;													//Nulstiller sekunder
			min++;														//Øger minutter
		}
	}
	*klok = (char) hour / 10 - (hour % 10) / 10 + 0x30;					//Sætter anden ciffer til time
	*(klok + 1) = (char) hour % 10 + 0x30;								//Sætter første ciffer til time
	*(klok + 3) = (char) min / 10 - (min % 10) / 10 + 0x30;				//Sætter anden ciffer til minutter
	*(klok + 4) = (char) min % 10 + 0x30;								//Sætter første ciffer til minutter
	*(klok + 6) = (char) sec / 10 - (sec % 10) / 10 + 0x30;				//Sætter anden ciffer til sekunder
	*(klok + 7) = (char) sec % 10 + 0x30;								//Sætter første ciffer til sekunder
}

int main (void) {
	uart0_init(MYUBRRF);												//Sætter UART'ens indre klok til at være 19200
	timer1_init(1999);													//Sætter timeren til at tælle 1 ms
	PORTE |=(1<<PE4);													//Pull-up på D2
	EIMSK |=(1<<INT4);													//Ekstern interrupt til D2
	EICRB |=(1<<ISC41)|(0<<ISC40);										//Opfangning af faldende kant
 	sei();																//Aktiverer global interrupt
	putsUSART0("\nSkriv tid, som hh:mm:ss\n");							//Minder folk om at hvilken format tiden skal sættes som
	while(1) {
		if(flag_rx == 1) {												//Hvis der er modtaget noget
			flag_rx = 0;												//Flaget sættes tilbage til 0
			while(flag_rx != 1 && flag_reset != 1) {					//Mens flagene ikke er 1
				if(flag_cn == 1) {										//Hvis der er gået 1 sekund
					putsUSART0("\nAktuelt klokkeslæt er ");				//Skriv første del af stringen
					putsUSART0(buffer);									//Skriv tiden
					tid(buffer);										//Opdaterer tiden
					flag_cn = 0;										//Lad flaget være lav
				}
			}
		}
		if(flag_reset == 1) {											//Hvis systemet er resettet
			flag_reset = 0;												//Flaget sættes tilbage til 0
			buffer[MAX - 1] = '\0';										//Arrayen resettes
			putsUSART0("\nSkriv tid, som hh:mm:ss\n");					//Minder folk om at hvilken format tiden skal sættes som
		}
	}
}
