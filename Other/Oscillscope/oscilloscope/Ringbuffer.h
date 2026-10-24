/*
 * Ringbuffer.h
 *
 * Created: 08-06-2021 12:50:23
 *  Author: Kristian
 */ 


#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_
#define BUFSIZE 1007
extern char buf[BUFSIZE];
extern  char *pIn, *pOut, *pEnd;
char full;
extern unsigned int counter;

void ringbuf_init();
int ringbuf_put(char c);
char buf_read(char *pc);




#endif /* RINGBUFFER_H_ */