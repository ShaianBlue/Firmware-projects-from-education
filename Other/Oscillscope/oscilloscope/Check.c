/*
 * Security_Check.c
 *
 * Created: 18-06-2021 13:30:27
 *  Author: Administrator
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>


char LRC8_check (char *LRC)
{
	char LRCFlag = 0;
	unsigned int o=0;
	int LRCcheck = LRC[0];
	while( o < 11 )
	{
		LRCcheck = LRCcheck ^ LRC[o+1];
		o++;
		if(LRCcheck == LRC[o])
		{
			LRCFlag = 1;
		}
	
	}
	o = 0;
	return LRCFlag;
}

char Sync_Check(char *Sync)
{
	int j = 0;
	char SyncFlag = 0;
	char SyncCont[2] = {'\0'};
	SyncCont[0] = 0x55, SyncCont[1] = 0xAA, SyncCont[2] = '\0';

	while (j < 2)
	{
		if (Sync[0] == SyncCont[0] && Sync[1] == SyncCont[1])
		{
			SyncFlag = 1;
		}
		j++;
	}
	j = 0;
	return SyncFlag;
}

char Z16_Check(char *Zchk)
{
	char Z16Flag = 0;
	int l=0;
	while(l < 11)
	{
		
		if (Zchk[l-1] == 0x00 && Zchk[l] == 0x00)
		{
			
			Z16Flag = 1;
			
		}
		l++;
	}
	l = 0;
	return Z16Flag;
}