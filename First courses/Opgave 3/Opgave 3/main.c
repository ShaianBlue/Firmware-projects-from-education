/*
 * opg1_ny_E20.c
 *
 * Created: 01-09-2020 14:34:13
 * Author : Shaian og Ludvig
 */ 
#include <xc.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include "I2C.h"												//Inkluderer I2C driver
#include "ssd1306.h"											//Inkluderer display driver
#include "SPI.h"												//Inkluderer SPI operationer

int master_SPI(void) {
	int res = 0;												//Definerer resultattet
	char msb = 0, lsb = 0;										//Definerer MSB og LSB værdier fra slaven
	PORTB &=~ (1 << PB0);										//Deaktiverer slaven
	SPDR = 0;													//Sender 0 fra master til slave
	msb = getcSPI_master();										//Får værdi fra slave (MSB)
	SPDR = 0;													//Sender 0 fra master til slave
	lsb = getcSPI_master();										//Får værdi fra slave (LSB)
	res = ((msb << 8) + lsb);									//Indsætter vores resultat
	PORTB |= (1 << PB0);										//Lader SS! være høj
	return res;													//Returnerer resultatet
}

int main(void) {
	_i2c_address = 0X78;										//Skriver addresse til I2C interface
	
	I2C_Init();													//Initialiserer I2C interface til display
	InitializeDisplay();										//Initialoserer display
	
	print_fonts();												//For at teste og ekskluderer clear_display();
	char text[]="Temperaturen er:";								//Deklareret string
	clear_display();											//Gør display klar
	
	master_init();												//Lader mikrocontrolleren være en master
	char temp[] = {0};											//Sætter array, som temperauren indsættes i
	int MasterConvert;											//En variabel, som temperaturen indlæses i
	
	while (1) {
		MasterConvert = (master_SPI() >> 3) * 0.25;				//Indlæser temperaturen
		
		sprintf(temp, "%d", MasterConvert);						//Indsætter temperaturen i temp
		
		sendStrXY(text,0,0);									//Afspiller en string på display
		
		sendStrXY(temp,1,2);									//Afspiller en char variabel på display
	}
}