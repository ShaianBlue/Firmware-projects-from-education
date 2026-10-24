/*
 * Timer1.c
 *
 * Created: 08-04-2021 14:25:54
 *  Author: Kristian
 */ 

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#define F_CPU 16000000UL
#include <util/delay.h>

// initialiser timer 1
void inittimer1(unsigned int compareValue)
{
	TCCR1B|=(1<<WGM12)|(1<<CS11)|(1<<CS10);				// Sætter timeren til CTC mode og laver en prescale på 64
	OCR1A=compareValue;
	TIMSK1 |=(1<<OCIE1A);								// laver et intrrupt når TCNT1 = ORC1A værdi.
}
void inittimer1Nointe(unsigned int compareValue)
{
	TCCR1B|=(1<<WGM12)|(1<<CS11)|(1<<CS10);				// Sætter timeren til CTC mode og laver en prescale på 64
	OCR1A=compareValue;
	
}
void inittimer3(unsigned int compareValue)
{
	TCCR3B|=(1<<WGM32)|(1<<CS32);				// Sætter timeren til CTC mode og laver en prescale på 64
	OCR3A=compareValue;
	TIMSK3 |=(1<<OCIE3A);								// laver et intrrupt når TCNT1 = ORC1A værdi.
}

void resetttimer1(unsigned int compareValue)
{
	TCCR1B&=~(1<<CS10);				// Sætter timeren til CTC mode og laver en prescale på 64
	TCCR1B&=~(1<<WGM12);
	TCCR1B&=~(1<<CS11);
	OCR1A=compareValue;
	TIMSK1 &=~(1<<OCIE1A);								// laver et intrrupt når TCNT1 = ORC1A værdi.
}