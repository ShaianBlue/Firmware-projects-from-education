/*
 * Opgave 5.c
 *
 * Created: 15-04-2021 14:44:03
 * Author : M. Shaian Latif og Ludvig Skjøtt Mogensen
 */ 

//Alle inkluderede filer
#include <avr/io.h>											//AVR pakken
#include <avr/interrupt.h>									//AVR interrupt pakken
#include <stdio.h>											//Standard I/O pakken
#include <stdlib.h>											//Standard Library pakken

//Alle inkluderede selvlavede filer
#include "ADC.h"											//ADC pakken
#include "Timer.h"											//Timer pakken
#include "UART.h"											//UART pakken
#include "I2C.h"											//I2C pakken
#include "ssd1306.h"										//OLED display pakken

//Alle globale flagvariabler
volatile char flag_ADC = 0;									//Flag for ADC interrupt
volatile char flag_CTC = 0;									//Flag for CTC Timer interrupt
volatile char flag_RX = 0;									//Flag for UART Receive interrupt

//Alle globale arrays
char resADC[4] = {'\0'};									//Array for spænding
char buffer[4] = {'\0'};									//Array for UART Receive

//Tilstande til tilstandsmaskinen
unsigned char state;										//Variabel for tilstande
enum state {
	listen,													//Tilstand for valg af andre tilstande
	PWM_limit_min,											//Tiltsand for valg af minimumsværdi for PWM
	PWM_limit_max,											//Tilstand for valg af maksimumsværdi for PWM
	PWM_signal												//Tilstand for opdatering af PWM
};

//ISR for ADC
ISR(ADC_vect) {
	flag_ADC = 1;											//Sætter ADC flag høj
}

//ISR for CTC Timer
ISR(TIMER3_COMPA_vect) {
	ADCSRA |=(1<<ADSC);										//Starter konverteringen
	flag_CTC = 1;											//Sætter CTC flag høj
}

//ISR for USART Receive
ISR(USART0_RX_vect) {
	//flag_RX = 1;											//Med alternativ rute (LF og CR fjernes fra terminal vindue), skal der kun sættes RX flag høj
	static int i = 0;										//En variabel for arrayelement defineres
	buffer[i++] = UDR0;										//Stringen skaffes fra UDR0 en ad gangen
	//Tjekker om at der er nået maksimal værdi (uden NULL)
	if(i == 3) {
		flag_RX = 1;										//Sætter RX flag høj
		i = 0;												//Arrayelementet resettes til 0
	}
}

void voltage_frac(int sample) {
	resADC[0] = (10 * sample / 2046) + 0x30;				//Finder spændingen og viser dette i ASCII
	int frac = (10 * sample) % 2046;						//Finder resten for første ciffer
	resADC[1] = '.';										//Sætter en kommaseperator på
	resADC[2] = 10 * frac / 2046 + 0x30;					//Finder den anden ciffer som ASCII
	frac = (10 * frac) % 2046;								//Finder resten for anden ciffer
	resADC[3] = (10 * frac) / 2046 + 0x30;					//Finder den tredje ciffer som ASCII
	frac = (10 * frac) % 2046;								//Finder resten for tredje ciffer
	if(frac >= 1023){
		resADC[3]++;										//Tallet bliver større, hvis resten fra tredje ciffer er større en 5
		if(resADC[3] == (0x30 + 10)) {
			resADC[3] = 0x30;
			resADC[2]++;									//Tallet bliver større, hvis resten fra anden ciffer er større en 5
			if(resADC[2] == (0x30 + 10)) {
				resADC[2] = 0x30;
				resADC[0]++;								//Tallet bliver større, hvis resten fra første ciffer er større en 5
			}
		}
	}
}

int main(void) {
	//Initialisering og forberedelser
	_i2c_address = 0X78;									//Sætter adresse til I2C
	I2C_Init();												//Forbinder OLED display via I2C
	InitializeDisplay();									//Initialiserer OLED display
	print_fonts();											//Sætter skriften til OLED display
	clear_display();										//Rydder indholdet i OLED display
    init_adc();												//Initialiserer ADC-enheden
	channel_select(0);										//Vælger den kanal, som ikke skal deaktiveres
	uart0_init(MYUBRRF);									//Initialiserer UART0
	timer3_CTC_init(15624);									//Initialiserer CTC Timer
	sei();													//Aktiverer alle interrupts
	//Variabler
	int data = 0;											//Skriver ADC'ens værdi
	char byte = 1;											//Tilstandsvariablen, bruges til at skifte tilstande
	char min_lim = 0;										//Skriver minimumsværdi for PWM
	char max_lim = 0;										//Skriver maksimumsværdi for PWM
	//Stringe
	char volt[22] = {'\0'};									//Stringen for spændingen
	char duty[25] = {'\0'};									//Stringen for duty cycle
	//Koden for tilstandsmaskinen
    while (1) {
		switch(state) {
			//Tilstand for valg af andre tilstande
			case listen:
				//Vejledende besked til brugeren
				if (byte == 1) {
					putsUSART0("Tryk 'a' for at bestemmer grænsen, eller tryk 'b' for at justere PWM\n");
					byte = 0;								//Tilstandsvariablen nulstilles
				}
				//Udvælgelse af næste tilstand
				if (flag_RX == 1) {
					//byte = UDR0							//Med alternativ rute (LF og CR fjernes fra terminal vindue), nedenstående linje skal kommenteres
					byte = buffer[0];						//Karakteren skal være det første indtastede karakter
					flag_RX = 0;							//RX flag nulstilles
				}
				//Hvis man vil vælge PWM begrænsninger, som næste tilstand
				if (byte == 'a') {
					state = PWM_limit_min;					//Næste tilstand er minimumsgrænsen for PWM
				}
				//Hvis man vil vælge PWM opdatering, som næste tilstand
				else if (byte == 'b') {
					state = PWM_signal;						//Næste tilstand er PWM opdatering
					byte = 0;								//Tilstandsvariablen nulstilles
				}
				//Hvis intet er valgt
				else {
					state = listen;							//Forbliver i samme tilstand
				}
				break;
			//Tiltsand for valg af minimumsværdi for PWM
			case PWM_limit_min:
				//Vejledende besked til brugeren
				if (byte == 'a') {
					putsUSART0("Vælg mindste værdi, fra 0 til 8\n");
					byte = 0;								//Tilstandsvariablen nulstilles
				}
				//Udvælgelse af minmumsværdi for PWM
				if (flag_RX == 1) {
					//min_lim = UDR0						//Med alternativ rute (LF og CR fjernes fra terminal vindue), nedenstående linje skal kommenteres
					min_lim = buffer[0] - 0x30;				//Indtastet minimumsværdi, omsættes til kode
					byte = 1;								//Tilstandsvariablen sættes høj
					flag_RX = 0;							//RX flag nulstilles
				}
				//Hvis der er indtastet ugyldigt værdi
				if ((min_lim < 0 || min_lim > 8) && byte == 1) {
					putsUSART0("Ugyldigt input, indsæt venligst ny grænse, fra 0 til 8\n");
					byte = 0;								//Tilstandsvariablen nulstilles
				}
				//Hvis der er indtastet gyldigt værdi
				if (byte == 1) {
					state = PWM_limit_max;					//Næste tilstand bliver valg af andre tilstande
				}
				//Hvis intet er valgt
				else {
					state = PWM_limit_min;					//Forbliver i samme tilstand
				}
				break;
			//Tilstand for valg af maksimumsværdi for PWM
			case PWM_limit_max:
				//Vejledende besked til brugeren
				if (byte == 1) {
					putsUSART0("Vælg maksimale værdi, fra minimum til 9\n");
					byte = 0;								//Tilstandsvariablen nulstilles
				}
				//Udvælgelse af maksimumsværdi for PWM
				if (flag_RX == 1) {
					//max_lim = UDR0						//Med alternativ rute (LF og CR fjernes fra terminal vindue), nedenstående linje skal kommenteres
					max_lim = buffer[0] - 0x30;				//Indtastet maksimumsværdi, omsættes til kode
					byte = 1;								//Tilstandsvariablen sættes høj
					flag_RX = 0;							//RX flag nulstilles
				}
				//Hvis der er indtastet ugyldigt værdi
				if ((max_lim <= min_lim || max_lim > 9) && byte == 1) {
					putsUSART0("Ugyldigt input, indsæt venligst ny grænse, fra minimum til 9\n");
					byte = 0;								//Tilstandsvariablen nulstilles
				}
				//Hvis der er indtastet gyldigt værdi
				if (byte == 1) {
					state = listen;							//Næste tilstand bliver valg af andre tilstande
				}
				//Hvis intet er valgt
				else {
					state = PWM_limit_max;					//Forbliver i samme tilstand
				}
				break;
			//Tilstand for opdatering af PWM
			case PWM_signal:
				if (flag_ADC == 1 && flag_CTC == 1) {
					flag_CTC = 0;									//CTC Timer flag nulstilles
					flag_ADC = 0;									//ADC flag nulstilles
					data = (ADCL + (ADCH<<8));						//Indsætter konverteringen i data
					voltage_frac(data);								//Finder den konerterede spænding i analog
					sprintf(volt, "Spændingen er %s V\n", resADC);	//Indsætter spændingen i spændingsstringen
					putsUSART0(volt);								//Skriver spændingen
					//Tjekker om at spændingen er i de valgte grænser med 3.3 V, som TOP, som gøres med 675
					//Alternativ er at bruge 5 V, som TOP, som gøres med 1023
					if(data > (min_lim * (/*1023*/675 / 9)) && data < (max_lim * (/*1023*/675 / 9))) {
						timer1_phfrPWM_init(/*1023*/675, data);		//Opdaterer fase- og frekvenskorrekt PWM
						sprintf(duty, "Duty cyclen er ca. %c0%%\n", (10 * data / /*1023*/675) + 0x30);
						clear_display();							//Rydder indholdet i OLED display
						sendStrXY(duty,4,0);						//Sender duty cycle stringen til OLED display
						}
						//Hvis spændingen ikke er i de valgte grænser med 3.3 V, som TOP
						else {
						timer1_phfrPWM_init(/*1023*/675, 0);		//Slukker for fase- og frekvenskorrekt PWM
						clear_display();							//Rydder indholdet i OLED display
						sendStrXY("Er uden for grænsen",4,0);		//Giver besked til OLED display at spændingen er uden for grænsen
					}
				}
				//Hvis der bliver trykket på en knap i terminal vinduet
				if (flag_RX == 1) {
					byte = 1;										//Tilstandsvariablen sættes høj
					state = listen;									//Næste tilstand bliver valg af andre tilstande
					flag_RX = 0;									//RX flag nulstilles
				}
				//Hvis der ikke bliver trykket på en knap i terminal vinduet
				else {
					state = PWM_signal;								//Forbliver i samme tilstand
				}
				break;
			//Udvælger hvilken tilstand, der skal vises først
			default:
				state = listen;										//Standard tilstand bliver valg af andre tilstande
				byte = 1;											//Tilstandsvariablen sættes høj
				break;
		}
    }
}

