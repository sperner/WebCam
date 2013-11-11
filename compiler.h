#ifndef __COMPILER_H__
#define __COMPILER_H__

#ifdef __IMAGECRAFT__


// choose your AVR device here
#include <iom128v.h>

#include <macros.h>

#define outp(val, reg)  (reg = val)
#define inp(reg)        (reg)

#define cli()           CLI()
#define sei()           SEI()
#define cbi(reg, bit)   (reg &= ~BIT(bit))
#define sbi(reg, bit)   (reg |= BIT(bit))

#define SIGNAL(x)       void x(void)  

#define nop() NOP()

#define _BV(x)	   (1<<x)


#else /* --- GCC --- */

#include <avr/interrupt.h>
#include <avr/io.h>
/*
// atmega 644 anfang
#define TCCR0	TCCR0A
#define TIMSK	TIMSK0
#define UBRRH UBRR0H
#define UBRRL UBRR0L 
#define UDR UDR0

#define UCSRA UCSR0A
#define UDRE UDRE0
#define RXC RXC0

#define UCSRB UCSR0B
#define RXEN RXEN0
#define TXEN TXEN0
#define RXCIE RXCIE0

#define UCSRC UCSR0C
#define URSEL 
#define UCSZ0 UCSZ00
#define UCSZ1 UCSZ01
#define UCSRC_SELECT 1 
#define U2X   U2X0
*/
//#define UCSRC_SELECT (1 << URSEL)
//#define UBRRH UBRR0H
//#define UBRRL UBRR0L 

#define nop() asm volatile("nop\n\t"::);
#define cbi(reg, bit)   (reg &= ~_BV(bit))
#define sbi(reg, bit)   (reg |= _BV(bit))

#endif /* Compiler Used */



#endif /* __COMPILER_H__ */

