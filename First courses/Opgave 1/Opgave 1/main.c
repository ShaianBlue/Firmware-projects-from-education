/*
 * opg1_ny_E20.c
 *
 * Created: I denne uge
 * Author : Muhammad Shaian Latif
 */ 
#include <xc.h>
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include "I2C.h"  //include library for i2c driver
#include "ssd1306.h" //include display driver
void init(){
	PORTK|=0xFF;
	DDRG |=0b00100000;  //D4 as output
	PORTG |= 0b00100000; //D4 er aktiv høj i starten
}

int main(void)
{  
	init();
	
  _i2c_address = 0X78; // write address for i2c interface
  unsigned char PANTHER = PINK; //PINK is stored in PANTHER
  char Hex[2] = {0xF,0xF}; //array med ascii karakterer konverteret til hexadecimalværdier
  
  I2C_Init();  //initialize i2c interface to display
  InitializeDisplay(); //initialize  display
  
   print_fonts();  //for test and then exclude the  clear_display(); call
   //char text[]="en tekst string"; //string declared before use it in sendStrXY - 15 chars long incl spaces
   clear_display();   //use this before writing you own text
  while (1)
  {      
	  sprintf(Hex,"%2X", PANTHER	//Bitsene bliver sat i hex
	  //0xFF&PANTHER				//& giver den samme som inputtet.
	  //0xFF|PANTHER				//| giver det højeste værdi af inputtet.
	  //0xFF&~PANTHER				//&~ giver det højeste værdi, men trækket fra med den indtastede værdi
	  );
	 //sendCharXY('a',1,2);  //one char  - X is line number - from 0 -7 and Y number position of the char an the line - 15 chars 
	 
	 sendStrXY(Hex,0,0); //line 0  -print the line of text
	  
	  _delay_ms(1000); //forsinkelse i 1 s
	  PORTG ^= 0b00100000; //skift af D4
	  
	  }
}