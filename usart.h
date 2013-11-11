#ifndef _USART_H_
#define _USART_H_

#include "delay.h"

#define UBRR_9600   F_CPU/16/9600UL
#define DBR_9600    0

#if F_CPU==18432000 || F_CPU==11059200 || F_CPU==3686400

#define UBRR_460800 F_CPU/8/460800UL
#define DBR_460800 1

#elif F_CPU==14745600 || F_CPU==7372800

#define UBRR_460800 F_CPU/16/460800UL
#define DBR_460800 0

#else

#error "Fehler bei der Auswahl der CPU-Taktfrequenz. Es sind nur Baudratenfrequenzen zulässig!"

#endif

#define SET_USART_9600() USART_Init((UBRR_9600)-1,DBR_9600)
#define SET_USART_460800() USART_Init((UBRR_460800)-1,DBR_460800)
#define USART_putchar(char) USART_Transmit(char)
#define USART_getchar() USART_Receive()


void USART_Init(unsigned int UBRR_N, unsigned char DBR);
void USART_Transmit(unsigned char data);
unsigned char USART_Receive(void);
void USART_Flush(void);
void USART_pstr_P(const char *addr);

#endif //ifndef
