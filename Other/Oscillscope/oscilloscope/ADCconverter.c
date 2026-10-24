/*
 * ADCconverter.c
 *
 * Created: 22-04-2021 16:54:33
 *  Author: Kristian
 */ 
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
void init_adc_timer(char channel){ // free running mode
	ADCSRA|=(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2); // intern clock 125 kHz
	
	ADCSRA|=(1<<ADEN); // enabler converteren
	ADCSRA|=(1<<ADIE); // enabler interrupt.
	ADMUX=channel; // vælger hvilken analog pin der bruges.
	ADMUX|=(1<<REFS0);	// vælger reference spænding AVCC med externe kapacitorer.
	DIDR0 = (1<<channel); // sætter et 1 ind som forberedelse til at invertere fordi de bits er aktiv lave.
	DIDR0=~DIDR0;			// inverters så der kun er et 0 på den bit der ønskes at bruges.
	DIDR1=0xff;				// sættes alle til 1 for at spare på rammen fordi de bits også er aktiv lave.

}


void init_adc_timer8bit(char channel){ // free running mode
	ADCSRA|=(1<<ADPS2); // intern clock 1MHz
	
	ADCSRA|=(1<<ADEN); // enabler converteren
	ADCSRA|=(1<<ADIE); // enabler interrupt.
	ADMUX=channel; // vælger hvilken analog pin der bruges.
//	ADMUX|=(1<<REFS0);	// vælger reference spænding AVCC med externe kapacitorer. skal kommenteres ud.
	ADMUX|=(1<<ADLAR); // prøver at sætte til 8 bit.
	DIDR0 = (1<<channel); // sætter et 1 ind som forberedelse til at invertere fordi de bits er aktiv lave.
	DIDR0=~DIDR0;			// inverters så der kun er et 0 på den bit der ønskes at bruges.
	DIDR1=0xff;				// sættes alle til 1 for at spare på rammen fordi de bits også er aktiv lave.

}


