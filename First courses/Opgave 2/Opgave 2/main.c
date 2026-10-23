/*
 * Hjemmeopgave 
 *
 * Due: 23-02-2021
 * Author : Muhammad Shaian Latif og Ludvig Skj?tt-Mogensen 
 * S: S205080 og S205060
 */  
#include <xc.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "I2C.h"						//Inkluderer library til i2c driver
#include "ssd1306.h"					//Inkluderer display driver
void init(){
	PORTK |= 0xFF;						//A8-A15 er aktiv høje
	PINE |= (1 << PE4);					//D2 er aktiv høj input
	EIMSK |= (1 << INT4);				//Interrupt til D2 er aktiv høj
}

volatile char flag = 0;					//Gloabal variabel, som fortæller om at knappen er trykket ned

ISR(INT4_vect) {						//Interrupt service routine
	flag = 1;
}

int main(void)
{  
	init();
  _i2c_address = 0X78;					//Skriver adresse til i2c interface
  
  I2C_Init();							//Initialiserer i2c interface til display
  InitializeDisplay();					//Initialiserer display
  
   int i = 0;
   print_fonts();						//Tester display
   char text[4] = {0};					//Array deklareres, med størrelsen af den ønskede string +1 (for at gøre plads i array til /0)
   clear_display();						//Bruges før egen skrift printes
  while (1)
  { 
	  if(flag == 1) {					//Når trykknappen er nede
		  _delay_ms(40);				//Venter i 40 ms
		  if(!(PINE & (1 << PE4))) {	//Tjekker om at knappen stadig er nede
			  text[i] = PINK;			//Skriver tekst til array
			  i++;						//Går til næste array element
			  flag = 0;					//Trykknappen er registreret som "oppe"
			  cli();					//Deaktiverer interrupts
			  _delay_ms(1000);			//Venter i 1 sekund
		  } else {
			  flag = 0;					//Hvis knappen ikke er nede, så vil flag sættes til 0
		  }
	  } else {							//Når trykknappen er oppe
		  sei();						//Aktiverer interrupts
	  }
	  if(i == 3) {						//Når man er endt i fjerde element
		  i = 0;						//Man starter forfra
	  }
	
	 sendStrXY(text,0,0);				//Stringen vises i OLED-displayet
	}
}