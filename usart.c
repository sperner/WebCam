#include <avr/io.h>
#include <avr/pgmspace.h>
#include "compiler.h"
#include "usart.h"

extern volatile unsigned char usart_timeout;


void USART_Init(unsigned int UBRR_N, unsigned char DBR) {

 UBRRH=UBRR_N>>8;
 UBRRL=UBRR_N&255;
 UCSRB=(1<<RXEN)|(1<<TXEN);
// UCSRC=(1<<URSEL)|(3<<UCSZ0);
 if (DBR) UCSRA|=(1<<U2X);
 else UCSRA&=(unsigned char)~(1<<U2X);
}


void USART_Transmit(unsigned char data) {

 while(!(UCSRA&(1<<UDRE)));
 UDR=data;

}


unsigned char USART_Receive(void) {

 usart_timeout=180;
 while(!(UCSRA&(1<<RXC))) if(!usart_timeout) return 0;

 return UDR;

}


void USART_Flush(void) {

unsigned char dummy;

 while(UCSRA&(1<<RXC)) dummy=UDR;

}


void USART_pstr_P(const char *addr)
{
 char c;
 while((c=pgm_read_byte(addr++)))
  USART_putchar(c);
}
