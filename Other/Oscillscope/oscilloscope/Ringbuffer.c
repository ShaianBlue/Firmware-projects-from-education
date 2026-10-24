/*
 * Ringbuffer.c
 *
 * Created: 08-06-2021 12:43:59
 *  Author: Kristian
 */ 
#include "Ringbuffer.h"

char buf[BUFSIZE];
char *pIn, *pOut, *pEnd;
char full;
unsigned int counter;

void ringbuf_init(){
	pIn = pOut = buf;
	pEnd = &buf[BUFSIZE];
	full=0;
	counter=0;
}

int ringbuf_put(char c){
	if (pIn==pOut && full)
	
	return 0;
	
	*pIn++=c;
	if (pIn>=pEnd)
	
	pIn=buf;
	
	if (pIn==pOut)
	
	full=1;
	
	counter++;
	return 1;
}

char buf_read(char *pc){
	counter--;
	if (pIn==pOut && !full)
	return 0; // buffer tom
	*pc= *pOut++;  // finder den næste char der skal returneres.
	if (pOut>=pEnd) // har vi nået enden
	pOut=buf;
	full=0;
	return *pc; // den der har data.
}