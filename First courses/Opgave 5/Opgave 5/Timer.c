/*
 * Timer.c
 *
 * Created: 25-03-2021 15:25:47
 * Author : M. Shaian Latif og Ludvig Skjøtt Mogensen
 */ 

#include <avr/io.h>

#include "Timer.h"

void timer3_CTC_init(unsigned int CV) {
	TCCR3B |=(1<<CS32)|(1<<CS30)|(1<<WGM32);		//Prescaling sættes til 1024 og CTC mode aktiveres med en afhængig TOP
	OCR3A = CV;										//Bestemmer TOP-værdien for timeren
	TIMSK3 |=(1<<OCIE3A);							//Interrupt sættes til, når TCNT når TOP-værdien
}

void timer0_fastPWM_init(unsigned int CV) {
	DDRB |=(1<<DDB7);								//Sætter en output til OC0A
	TCCR0A =(1<<COM0A1)|(1<<WGM01)|(1<<WGM00);		//Vælger Fast PWM og rydder OC0A ved compare match
	TCCR0B |=(1<<CS01);								//Prescaling sættes til 8
	TCNT0 = 0;										//Tvinger counteren til at starte i 0
	OCR0A = CV;										//Sætter duty cycle
// 	TIMSK0 |=(1<<OCIE0A);							//Interrupt sættes til, når TCNT når TOP-værdien
}

void timer2_phasecorrect_init(unsigned int CV) {
	DDRB |=(1<<DDB4);								//Sætter en output til OC2A
	TCCR2A |=(1<<COM2A1)|(1<<WGM20);				//Vælger PWM Phase Correct og rydder OC2A ved compare match
	TCCR2B =(1<<CS21);								//Prescaling sættes til 8
	TCNT2 = 0;										//Tvinger counteren til at starte i 0
	OCR2A = CV;										//Sætter duty cycle
// 	TIMSK0 |=(1<<OCIE2A);							//Interrupt sættes til, når TCNT når TOP-værdien
}

void timer1_phfrPWM_init(unsigned int CV, unsigned int DC) {
	DDRB |= (1<<PB5);								//Sætter en output til OC1A
	TCCR1A |= (1<<COM1A1);							//Ryd OC1A ved compare match
	TCCR1B |=(1<<CS11)|(1<<WGM13);					//Prescaling sættes til 8 og PWM Phase and Frequency Correct mode aktiveres med en afhængig TOP og duty cycle
	ICR1 = CV;										//Bestemmer TOP-værdien for timeren
	OCR1A = DC;										//Bestemmer duty cycle
// 	TIMSK1 |=(1<<OCIE1A);							//Interrupt sættes til, når TCNT når TOP-værdien
}