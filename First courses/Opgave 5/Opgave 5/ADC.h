/*
 * ADC.h
 *
 * Created: 15-04-2021 15:30:17
 *  Author: M. Shaian Latif og Ludvig Skjøtt Mogensen
 */ 


#ifndef ADC_H_
#define ADC_H_

extern void init_adc();

extern void channel_select(char channel);

extern unsigned int get_sample();

#endif /* ADC_H_ */