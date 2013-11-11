#ifndef _DELAY_H_
#define _DELAY_H_

/* The AVR clock frequency in Hertz */
#define F_CPU 14745600   
//#define F_CPU 18432000
//#define F_CPU	11059200
void delay_ms(unsigned char ms);
void delay_us(unsigned char us);

#endif
 
