/*
 * Timer.h
 *
 * Created: 27-03-2021 08:28:21
 *  Author: M. Shaian Latif og Ludvig Skjøtt Mogensen
 */ 


#ifndef TIMER_H_
#define TIMER_H_

extern void timer3_CTC_init(unsigned int CV);

extern void timer1_CTC_init(unsigned int CV);

extern void timer0_fastPWM_init(unsigned int CV);

extern void timer2_phasecorrect_init(unsigned int CV);

extern void timer1_phfrPWM_init(unsigned int CV, unsigned int DC);

#endif /* TIMER_H_ */