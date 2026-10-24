/*
 * Digital PID.c
 *
 * Created: 03-06-2022 09:16:47
 * Author : Muhammad Shaian "den utrolige" Latif
 */ 

//Alle inkluderede filer
#include <avr/io.h>																		//AVR pakken
#include <avr/interrupt.h>																//AVR interrupt pakken
#include <stdio.h>																		//Standard I/O pakken

//Alle inkluderede selvlavede filer
#include "ADC.h"																		//ADC pakken
#include "Timer.h"																		//Timer pakken

//Alle macroer
#define KP 40//25.7534																	//Proportionalitetskonstant
#define KI 197.4425																		//Integrationskonstant
#define KD 2.0443																		//Differentiationskonstant
#define Fs 1000																			//Samplingsfrekvensen
#define limit 280000																	//Begrænsning
#define R 960								//15 * 5 / 16 * 1024 / 5 					//Referencesignalet
#define TOP 100																			//Topværdi for PWM

//Alle globale variabler
volatile int Ym = 0;																	//Variabel for indlæsning af data i indgangssignalet
volatile char flag_sample = 0;															//Variabel for ny sample

//ISR for Timer og ADC
ISR(TIMER3_COMPA_vect) {
	ADCSRA |=(1<<ADSC);																	//Starter konverteringen
	Ym = (ADCL + (ADCH<<8));															//Indlæs værdierne for indgangssignalet
	flag_sample = 1;																	//Der er taget en ny sample
}

int main(void) {
	//Lokale variabler
	float E = 0;																		//Fejlsignalet
	float P = 0;																		//Udgangssignal for proportionalitet
	float IE = 0;																		//Integration
	float I = 0;																		//Udgangssignal for integration
	float DE = 0;																		//Differentiation
	float D = 0;																		//Udgangssignal for differentiation
	float U = 0;																		//Udgangssignal
	float E_old = 0;																	//Gamle værdi for fejlsignalet
	int duty = 0;																		//Værdi for duty cycle
	//Aktivering af enheder
	init_adc();																			//Initialiserer ADC-enheden
	channel_select(1);																	//Vælger den kanal, som ikke skal deaktiveres
    timer3_CTC_init(124);					//F_CPU / (2 * Fs * 64) - 1					//Initialiserer CTC Timer for samplingsfrekvensen
	//Andre
	sei();																				//T�nder for interrupts
	//L�kken, som aldrig stopper
    while (1) {
		if(flag_sample == 1) {
			flag_sample = 0;															//Der ventes for ny sample
			E = R - Ym;																	//Finder fejlsignalet
			P = KP * E;																	//Finder værdi for udgangssignalet af proportionalitet
			IE = IE + E;																//Finder værdi for integration
			I = KI / Fs * IE;															//Finder værdi for udgangssignalet af integration
			if (I > limit) {
				I = limit;																//Lader værdi for udgangssignalet af integration være øvre grænse
			}
			else if (I < -limit) {
				I = -limit;																//Lader værdi for udgangssignalet af integration være nedre grænse
			}
			DE = E - E_old;																//Finder værdi for differentiation
			D = KD * Fs * DE;															//Finder værdi for udgangssignalet af differentiation
			U = P + I + D;																//Finder værdi for udgangssignalet
			if (U > limit) {
				U = limit;																//Lader værdi for udgangssignalet være øvre grænse
			}
			else if (U < 0) {
				U = 0;																	//Lader værdi for udgangssignalet være nedre grænse
			}
			E_old = E;																	//Gemmer den gamle fejlsignal
			duty = U / limit * TOP;														//Finder værdi for duty cycle i PID
			timer1_phfrPWM_init(TOP, duty);												//Initialiserer PWM Timer, vurderes ud fra begrænsning
		}
    }
}
