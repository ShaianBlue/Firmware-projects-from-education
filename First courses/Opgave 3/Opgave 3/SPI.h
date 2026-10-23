/*
 * main.c
 *
 * Created: 2/25/2021 3:32:02 PM
 *  Author: shaian
 */ 
#ifndef SPI_H_
#define SPI_H_

extern void master_init(void);

extern void slave_init(void);

extern void putcSPI_master(unsigned char cx);

extern unsigned char getcSPI_master(void);

#endif /* SPI_H_ */