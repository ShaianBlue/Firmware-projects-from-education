/*
 * Timer.c
 *
 * Created: 25-03-2021 15:25:47
 * Author : M. Shaian Latif og Ludvig Skjøtt Mogensen
 */ 

#include <avr/io.h>

#include "Timer.h"

void timer1_init(unsigned int CV) {
	TCCR1B |=(1<<CS11)|(1<<WGM12);					//Prescaling sættes til 8 og CTC mode aktiveres med en afhængig TOP
	OCR1A = CV;										//Bestemmer TOP-værdien for timeren
	TIMSK1 |=(1<<OCIE1A);							//Interrupt sættes til, når TCNT når TOP-værdien
}

/*													Timer
#include <avr/interrupt.h>

volatile int counter = 0;
volatile char flag = 0;

int main(void) {
	DDRB |= 0b10000000;
	timer1_init(1999);
	sei();
	while (1) {
		if(flag == 1) {
			PORTB ^=(1<<PB7);
			flag = 0;
		}
	}
}

ISR(TIMER1_COMPA_vect) {
	counter++;
	if(counter == 999) {
		flag = 1;
		counter = 0;
	}
}
*/