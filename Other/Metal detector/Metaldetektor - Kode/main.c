/*
 * Metaldetektor - Kode.c
 *
 * Created: 04-01-2022 20:43:34
 * Author : Muhammad Shaian Latif
 */ 

//Alle inkluderede filer
#include <avr/io.h>																		//AVR pakken
#include <avr/interrupt.h>																//AVR interrupt pakken
#include <stdio.h>																		//Standard I/O pakken
#include <math.h>																		//Matematik pakken
#include <stdlib.h>																		//Standard bibliotek

//Alle inkluderede selvlavede filer
#include "ADC.h"																		//ADC pakken
#include "Timer.h"																		//Timer pakken
#include "I2C.h"																		//I2C pakken
#include "ssd1306.h"																	//OLED display pakken

//Tilstande til tilstandsmaskinen
unsigned char state;																	//Variabel for tilstande
enum state {
	DFT,																				//Tilstand for fourier
	Display,																			//Tilstand for display
	Sample,																				//Tilstand for sampling
	Wait																				//Tilstand for at vente
};

//Alle macroer
#define N 64

//Alle globale variabler
volatile char count = 0;																//Variabel for optælling
volatile char byte = 0;																	//Variabel for elemtenter
volatile short int buffer[N] = {'\0'};													//Array for indlæsning af data

//ISR for CTC Timer
ISR(TIMER3_COMPA_vect) {
	count += 1;																			//Tæller til fire
	if(count == 4) {
		count = 0;
		PORTG ^= (1<<PG5);																//PG5 toggles
		//Tilstand for at vente
		if(state == Wait && PORTG == (1<<PG5)) {
			state = Sample;																//Vi gå til tilstanden for sampling
		}



	}
	//Tilstand for sampling
	if(state == Sample) {
		ADCSRA |=(1<<ADSC);																//Starter konverteringen
		buffer[byte++] = (ADCL + (ADCH<<8));											//Indlæs værdierne for signalet
		if(byte == N) {																	//Hvis den er fyldt op
			byte = 0;
			state = DFT;																//Vi gå til tilstanden for fourier
		}
		else {
			state = Sample;																//Vi gå til tilstanden for sampling
		}
	}
}

int main(void) {
	//Lokale variabler og array
	float Real = 0;																		//Variabel for realdel
	float Imag = 0;																		//Variabel for imaginærdel
	float Ampl = 0;																		//Variabel for amplitude
	float Phase = 0;																	//Variabel for fase
	float FilAmpl = 0;																	//Variabel for amplitude filter
	float FilPhase = 0;																	//Variabel for fase filter
	char tekstAmpl[5] = {'\0'};															//Amplitude string til display
	char tekstPhase[5] = {'\0'};														//Fase string til display
	//Aktivering af enheder
    _i2c_address = 0X78;																//Sætter adresse til I2C
    I2C_Init();																			//Forbinder OLED display via I2C
    InitializeDisplay();																//Initialiserer OLED display
    print_fonts();																		//Sætter skriften til OLED display
    clear_display();																	//Rydder indholdet i OLED display
    init_adc();																			//Initialiserer ADC-enheden
    channel_select(1);																	//Vælger den kanal, som ikke skal deaktiveres
    timer3_CTC_init(124);																//Initialiserer CTC Timer for 8 kHz
	//Andre
	sei();																				//Tænder for interrupts
	DDRG |=(1<<PG5);																	//PG5 bruges som output
	state = Wait;																		//Vi gå til tilstanden for at vente
	while (1) {
		//Tilstandsmaskine
		switch (state) {
			//Tilstand for fourier
			case DFT:
				Real = 0;
				Imag = 0;
				//Vi finder DFT, men kun for 2 kHz-signalet
				for (int i = 0; i < N; i++) {
					if(i % 4 == 0) {
						Real += buffer[i];												//Real
					}
					if(i % 4 == 1) {
						Imag += buffer[i];												//Imaginær
					}
					if(i % 4 == 2) {
						Real -= buffer[i];												//Real
					}
					if(i % 4 == 3) {
						Imag -= buffer[i];												//Imaginær
					}
				}
				Ampl = sqrt(pow(Real, 2) + pow(Imag, 2));								//Amplitude
				Phase = atan2(Imag, Real) * 180 / M_PI;									//Fase
				state = Display;														//Vi gå til tilstanden for display
				break;
			//Tilstand for display
			case Display:
				FilAmpl = 0.9 * FilAmpl + 0.1 * Ampl;									//Filtreret amplitude
				FilPhase = 0.9 * FilPhase + 0.1 * Phase;								//Filtreret fase
				_delay_ms(100);															//Vi venter på 100 milisekunder
				dtostrf(FilAmpl, 4, 1, tekstAmpl);										//Amplituden vises som en string
				sendStrXY("Amplitude:", 1, 0);											//Sender stringen for amplitude til OLED display
				sendStrXY(tekstAmpl, 3, 10);
				dtostrf(FilPhase, 4, 1, tekstPhase);									//Fasen vises som en string
				sendStrXY("Fase:", 5, 0);												//Sender stringen for fase til OLED display
				sendStrXY(tekstPhase, 7, 10);
				state = Wait;															//Vi gå til tilstanden for at vente
				break;
		}
    }
}
