/*
 * ADC.c
 *
 * Created: 15-04-2021 15:16:36
 *  Author: M. Shaian Latif og Ludvig Skjøtt Mogensen
 */ 

#include <avr/io.h>

#include "ADC.h"

void init_adc() {
	ADCSRA |=(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);					//Intern klok på 125 kHz
	ADCSRA |=(1<<ADEN)|(1<<ADSC);								//Aktiverer ADC og starte konverteringen
	ADMUX |=(1<<REFS0);											//Sætter ekstern kapacitor ved AREF pin
}

void channel_select(char channel) {
	if(channel <= 7) {
		ADMUX |= channel;										//Vælger ADC-kanal 0-7 (Single-ended)
		DIDR0 =(1<<channel);									//Markerer kanal til deaktivering
		DIDR0 =~DIDR0;											//Inverterer hele deaktiveringsregister
		DIDR2 =0xff;											//Slukker de resterende deaktiveringsregister
	}
	else if(channel > 7 && channel <= 15) {
		ADMUX |= channel + 24;									//Vælger ADC-kanal 8-15 (Single-ended)
		DIDR0 =0xff;											//Slukker de resterende deaktiveringsregister
		DIDR0 =(1<<(channel-8));								//Markerer kanal til deaktivering
		DIDR2 =~DIDR0;											//Inverterer hele deaktiveringsregister
	}
}

unsigned int get_sample() {
	ADCSRA |=(1<<ADSC);											//ADC starter sampling
	while(!(ADCSRA &(1<<ADIF)));								//Venter til interrupt flaget bliver lav
	return (ADCL+ (ADCH<<8));									//Returnere 10 bit sampleværdi
}