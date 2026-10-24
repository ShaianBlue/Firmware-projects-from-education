/*
 * Timer1.h
 *
 * Created: 08-04-2021 14:26:38
 *  Author: Kristian
 */ 


#ifndef TIMER1_H_
#define TIMER1_H_
// initialiser timer 1 med et interrupt
void inittimer1(unsigned int compareValue);
void inittimer3(unsigned int compareValue);
void inittimer1Nointe(unsigned int compareValue);
void resetttimer1(unsigned int compareValue);

#endif /* TIMER1_H_ */