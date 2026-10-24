/*
 * oscilloscope.c
 *
 * Created: 11-06-2021 10:31:06
 * Author : Kristian
 */ 

#define F_CPU 16000000UL
#define BAUD 115200
#define MYUBRRF F_CPU/8/BAUD-1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/io.h>
#include "ADCconverter.h"
#include "Uart.h"
#include "SPI.h"
#include "Timer1.h"
#include "Check.h"

#define size 1007

// globale variable
unsigned char MesCS2 = 0x00;
unsigned char MesCS = 0x00;
volatile char RXflag = 0;
/*unsigned char Z16Flag = 0;*/
unsigned char LRCFlag =0;
char SyncFlag = 0;
char RecieveBufferLW[11] = {'\0'};
char SendBufferLW[11] = {0x55, 0xAA };
volatile unsigned char adcflag=0;
unsigned char res=0;
unsigned int count=0;
unsigned int count2=0;
char bufsend2[size]={0x55,0xAA,'\0'}; 
char bufsend[size]={0x55,0xAA,'\0'};
unsigned char ReadToggle=0;
unsigned char SendToggle =0;
unsigned int RecLenInterrup=0;


typedef enum											// laver typen så der kan bruges navne på casene i stedet for tal.
{measure, Recievecentral, init,generator}
state_t;
state_t state=init;


ISR(USART1_RX_vect)										// Læser en string fra terminal
{
  	resetttimer1(1000);									// Reset timer1 så man sikrer at vi ikke senere fanger den midt i en tælling
  	UCSR1B&=0b11011111;									// Slår transmit interrupt fra.
  	count=0;
  	count2=0;
	static int i = 0;
	
	RecieveBufferLW[i++] = UDR1;

	if (i == 11 || (i==9 &&RecieveBufferLW[4]==0x01))
	{
		RXflag = 1;
		i=0;
	}
	
}

ISR(ADC_vect){
	res=ADCH;  // læser den konverterede værdi ind i variablen res.
	if (ReadToggle==0){
		bufsend2[count2+5]=res;				// læser resultatet ind efter de 5 første opsætnings bytes.
		MesCS2 = MesCS2 ^ res;				// MesCS2 startværdi er en xor med de 5 første bytes og her xores det med 
		count2++;
		adcflag=1;
	}
	else if (ReadToggle==1)
	{
		bufsend[count+5] = res;				// læser resultatet ind efter de 5 første opsætnings bytes.
		MesCS = MesCS ^ res;				// MesCS startværdi er en xor med de 5 første bytes og her xores det med 
		count++;
		adcflag=1;
	}
}


ISR(TIMER1_COMPA_vect){
	ADCSRA|=(1<<ADSC); // starter samplingen.
}

ISR(USART1_UDRE_vect){
	static int k=0;
	static int v=0;
	static int n=0;
	if (SendToggle==0)
	{
		UDR1=bufsend2[n++];

		if (n>=(RecLenInterrup))
		{
			n=0;
			count2=0;
			UCSR1B&=0b11011111;			// Slukker for interrupt servicen
	}
	}
	else if (SendToggle==1) 
	{
		
		UDR1=bufsend[k++];
		if (k>=(RecLenInterrup))
			{
			k=0;
			count=0;
			UCSR1B&=0b11011111;			// Slukker for interrupt servicen
			}
	}
	else  {
		UDR1=SendBufferLW[v];
		v++;
		if (v==(11))
		{
			v=0;
			UCSR1B&=0b11011111;		// Slukker for interrupt servicen
		}
	}
}


int main(void)
{
	unsigned char LRCnt=1;
	float SamRate = 0;
	unsigned char RunTog = 0;
    unsigned char DataTypeR = 0;		// 0x01 = BTN, 0x02 = SEND, 0x03 = START
	unsigned char ShapeFlag = 0;
	unsigned char AmpFlag = 0;
	unsigned char FreqFlag = 0;
    unsigned char Shape = 0;
    unsigned char Amp = 0;
    unsigned char Freq = 0;
    unsigned int RecLen = 0;
 	unsigned char RecLent1=0;
 	unsigned char RecLent2=0;
	unsigned char DataTypeS=0;
	unsigned int comparevalue=0;
	float timerequired=0;
		DDRC&=~(1<<PC6);	// sætter pin 31 op som et input.
		DDRC&=~(1<<PC5);

	     while (1) 
   {
		switch(state)
		{
		case init:
		sei();
		master_init();
		init_adc_timer8bit(2);
		UART0int_init(MYUBRRF);
		UART1int_init(MYUBRRF);
		state=Recievecentral;
		break;
		
		case Recievecentral:
	TIMSK1 &=~(1<<OCIE1A);			// Slår timer1 interrupt fra
		if (RXflag == 1)
		{
			SyncFlag=Sync_Check(RecieveBufferLW);
			LRCFlag=LRC8_check(RecieveBufferLW);
			RXflag = 0;
		}
		if(SyncFlag == 1 && LRCFlag == 1)
		{
			SyncFlag = 0;
			LRCFlag = 0;
			DataTypeR = RecieveBufferLW[4];
			if (DataTypeR==0x01)
			{
				state=generator;
				break;
			}
			else if (DataTypeR==2)
			{  
				SamRate = RecieveBufferLW[5] * 0x100 + RecieveBufferLW[6];
				RecLen = RecieveBufferLW[7] * 0x100 + RecieveBufferLW[8];
					RecLenInterrup=RecLen+7;
					RecLent1=(unsigned char)(RecLenInterrup>>8);
					RecLent2=(unsigned char)RecLenInterrup;
					MesCS = (((0xFF ^ RecLent1) ^ RecLent2) ^ 0x02);			// MeCS opsættes så de første 5 opsætnings bytes xores.
					MesCS2 = (((0xFF ^ RecLent1) ^ RecLent2) ^ 0x02);			// MeCS2 opsættes så de første 5 opsætnings bytes xores.
				state=measure;
				break;
			}
		}
 		state=Recievecentral;
 		break;
		// end of recievecentral
		
	
	case generator:
	TIMSK1 &=~(1<<OCIE1A);			// Slår timer1 interrupt fra
	SendToggle=3;
	SendBufferLW[2] = 0x00;										// Klargører starten af pakken der skal sendes
	SendBufferLW[3] = 0x0B;
	SendBufferLW[4] = 0x01;
	SendBufferLW[5] = DataTypeS;
	if(RecieveBufferLW[5] == 0x00)								// Enter trykket
	{
		if (DataTypeS==0)										// 0 = Shape
		{
			Shape=RecieveBufferLW[6];
			SendBufferLW[6] = Shape;
			SendBufferLW[7] = Amp;
			SendBufferLW[8] = Freq;
			SendBufferLW[9] = 0x00;
			SendBufferLW[10] = SendBufferLW[0];
		
		while(LRCnt < 10)										// Genererer LRC8 Checksummen
		{
			SendBufferLW[10] = SendBufferLW[10] ^ SendBufferLW[LRCnt];
			LRCnt++;
		}
		LRCnt = 1;
			UCSR1B|=0b00100000;									// Slår transmit interrupt til 
			state=Recievecentral;
			break;
		}
		else if (DataTypeS==1)									// 1 = Amplitude
		{
			Amp=RecieveBufferLW[6];
			SendBufferLW[6] = Shape;
			SendBufferLW[7] = Amp;
			SendBufferLW[8] = Freq;
			SendBufferLW[9] = 0x00;
		SendBufferLW[10] = SendBufferLW[0];
		while(LRCnt < 10)										// Genererer LRC checksum
		{
			SendBufferLW[10] = SendBufferLW[10] ^ SendBufferLW[LRCnt];
			LRCnt++;
		}
		LRCnt = 1;
		
		UCSR1B|=0b00100000;
			state=Recievecentral;
			break;
		}
		
		else if (DataTypeS==0x02)								// 2 = Frekvens
		{
			Freq=RecieveBufferLW[6];
			SendBufferLW[8]=Freq;
			SendBufferLW[9] = 0x00;
		SendBufferLW[10] = SendBufferLW[0];
		while(LRCnt < 10)										// Genererer LRC Checksum
		{
			SendBufferLW[10] = SendBufferLW[10] ^ SendBufferLW[LRCnt];
			LRCnt++;
		}
		LRCnt = 1;
		
		UCSR1B|=0b00100000;
			state=Recievecentral;
			break;
		}
	}
	
	if(RecieveBufferLW[5] == 0x01)								// Select trykket
	{
		DataTypeS++;											// Fortæller Labview hvilken variabel der ønskes opdateret
		SendBufferLW[5] = DataTypeS;
		SendBufferLW[6] = Shape;
		SendBufferLW[7] = Amp;
		SendBufferLW[8] = Freq;
		SendBufferLW[9] = 0x00;
		SendBufferLW[10] = SendBufferLW[0];
		while(LRCnt < 10)										// Genererer LRC Checksum
		{
			SendBufferLW[10] = SendBufferLW[10] ^ SendBufferLW[LRCnt];
			LRCnt++;
		}
		LRCnt = 1;
		UCSR1B|=0b00100000;				// Slår transmit interrupt til 
		if(DataTypeS>2)											// Der er kun 3 værdier der skal opdateres
		{
			DataTypeS = 0;
		}
		state=Recievecentral;
		break;
	}
	
	else if(RecieveBufferLW[5] == 0x02)							// Run / Stop trykket
	{ // SPI
		RunTog = RunTog ^ 1;
			 if(RunTog == 1)										// Run
			{
				putcSPI_master(0xAA);							// Sender Shape
				putcSPI_master(0x01);
				putcSPI_master(Shape);
				putcSPI_master(0xAB ^ Shape);
				ShapeFlag = 1;
		while((!(PINC & (1<<PC6) || PINC & (1<<PC5))));			// Venter på error eller done er modtaget
				
			if(ShapeFlag == 1 && PINC&(1<<PC6))					// Shape sendt og done modtaget
			{
				ShapeFlag = 0;
				putcSPI_master(0xAA);
				putcSPI_master(0x02);
				putcSPI_master(Amp);
				putcSPI_master(0xA8 ^ Amp);
				AmpFlag = 1;
			}
		while((!(PINC & (1<<PC6) || PINC & (1<<PC5))));			// Venter igen
			
			if(AmpFlag == 1 && PINC&(1<<PC6) )					// Amplitude sendt og done modtaget
			{
				AmpFlag = 0;
				putcSPI_master(0xAA);
				putcSPI_master(0x03);
				putcSPI_master(Freq);
				putcSPI_master(0xA9 ^ Freq);
				FreqFlag = 1;
				
			}
		while((!(PINC & (1<<PC6) || PINC & (1<<PC5))));			// Venter igen (Frekvens skal også tjekkes)
				FreqFlag = 0;
				
				if(PINC & (1<<PC5) && ShapeFlag == 1 )			// Shape sendt og error modtaget
					{
						
						putcSPI_master(0xAA);
						putcSPI_master(0x01);
						putcSPI_master(Shape);
						putcSPI_master(0xAB ^ Shape);
					}
					
				if(PINC & (1<<PC5) && AmpFlag == 1)				// Amplitude sendt og fejl modtaget
					{
						putcSPI_master(0xAA);
						putcSPI_master(0x02);
						putcSPI_master(Amp);
						putcSPI_master(0xA8 ^ Amp);
					}
								
				if (PINC & (1<<PC5) && FreqFlag == 1)			// Frekvens sendt og fejl modtaget
					{
						putcSPI_master(0xAA);
						putcSPI_master(0x03);
						putcSPI_master(Freq);
						putcSPI_master(0xA9 ^ Freq);

					}

				}			
					else if (RunTog == 0)						// Stop 
					{
							state = Recievecentral;
							break;
					}
			state = generator;
			break;
			
	}
		 if(RecieveBufferLW[5] == 0x03)						// Reset
		{
			SendBufferLW[5] = 0x00;
			SendBufferLW[6] = 0x00;
			SendBufferLW[7] = 0x00;
			SendBufferLW[8] = 0x00;
			Shape=0;
			Amp=0;
			Freq=0;
			DataTypeS=0;
			
		SendBufferLW[10] = SendBufferLW[0];						// Genererer LRC checksum
		while(LRCnt < 10)
		{
			SendBufferLW[10] = SendBufferLW[10] ^ SendBufferLW[LRCnt];
			LRCnt++;
		}
		LRCnt = 1;
			
			UCSR1B|=0b00100000;			// Slår transmit interrupt til 
			state=Recievecentral;
			break;

		}
										// end of generator
				
			case measure:
				if (RXflag==1)
				{
					state=Recievecentral;
					break;
				}
				
				timerequired=1/SamRate; // udregnet time required
				comparevalue=((16000000*timerequired)-64)/64;  // kan lave situationer hvor OCRNA er et komma tal, men ser ud til at fejlen i tiden er minimal. -udregnet comparevalue.
				bufsend[2]=RecLent1;
				bufsend[3]=RecLent2;
				bufsend[4]=0x02;
				bufsend2[2]=RecLent1;
				bufsend2[3]=RecLent2;
				bufsend2[4]=0x02;
				inittimer1(comparevalue);

			
 			
				if (adcflag==1){
					adcflag=0;

					if (count==RecLen)		// ligger under adcflag statement
					{
						SendToggle = 1;
						ReadToggle = 0;
						bufsend[count+5] = 0x00;		// sætter slutningen op
						bufsend[count+6] = MesCS;
						count = 0;
						UCSR1B|=0b00100000;
						MesCS = (((0xFF ^ RecLent1) ^ RecLent2) ^ 0x02);
						
		
						state=measure;
						break;
					}
						if (count2==RecLen)		// ligger under adcflag statement
						{
							SendToggle = 0;
							ReadToggle = 1;
							bufsend2[count2+5] = 0x00;		// sætter slutningen op
							bufsend2[count2+6] = MesCS2;
							count2 = 0;
							UCSR1B|=0b00100000;
							MesCS2 = (((0xFF ^ RecLent1) ^ RecLent2) ^ 0x02);
							state=measure;
							break;
						}
					state=measure;
					break;
					
				}
				state=measure;
				break;
									// end of measure
	}
    }
}

